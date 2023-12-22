/**
  ******************************************************************************
  * @file    wave_player.h
  * @author  MCD Application Team
  * @brief   WAVE player implementation
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "stm32g4xx_hal.h"
#include "app_audio.h"
#include "wave_player.h"
#include "stm32g474e_eval.h"
#include "stm32g474e_eval_lcd.h"
#include "stm32g474e_eval_audio.h"
#include "utils.h"
#include "ff.h"
#include "k_config.h"
#include "k_module.h"
#include "k_menu.h"
#include "k_storage.h"
#include "k_window.h"
#include "k_widgets.h"

/* Private typedef ----------------------------------------------------------*/

/* Private constants ----------------------------------------------------------*/
/* WAVE player states */
enum {
  WAVEPLAYER_INIT,
  WAVEPLAYER_SELECT_TITLE,
  WAVEPLAYER_PLAYING,
  WAVEPLAYER_STOPPED,
  WAVEPLAYER_PAUSED,
  WAVEPLAYER_WAIT_SELECTION,
  WAVEPLAYER_PLAYING_WAITEVENT,
  WAVEPLAYER_EXIT,
  WAVEPLAYER_END
};

/* Playlist table dimensionning */
#define MAX_TITLES            10
#define MAX_TITLE_NAME_LENGTH 16

/* User messages */
#define MSG_TITLE_SELECTION   "SEL: Play - UP/DOWN: Select - RIGHT: Quit"
#define MSG_PLAYBACK_PLAYING  "  SEL: Pause - DOWN: Stop - RIGHT: Quit  "
#define MSG_PLAYBACK_PAUSED   "  SEL: Play - DOWN: Stop - RIGHT: Quit   "
#define MSG_PLAYBACK_STOPPED  "         SEL: Play - RIGHT: Quit         "

/* Audio ramp up step */
#define STEP 1

/* Private function prototypes -----------------------------------------------*/
static int32_t WavePlayer_PlaybackInit(uint32_t SampleRate, uint16_t BitPerSample, uint32_t NbrChannels);
static void     WavePlayer_PlaybackDeInit(void);

static void     WavePlayer_PlaybackStart(uint8_t sel);
static void     WavePlayer_PlaybackStop(void);
static void     WavePlayer_PlaybackPause(void);
static void     WavePlayer_PlaybackResume(void);

static void     WavePlayer_PlaybackPlay(void);

static uint32_t WavePlayer_JumpToWaveData(FIL * file, WAV_HeaderTypeDef * WAV_header);
static void     WavePlayer_Prepare(FIL* fp, uint32_t * pBuffer, uint32_t Size);
static uint8_t  WavePlayer_InitializePlayList(void);
static void     WavePlayer_DisplayTitleSelectionScreen(void);
static void     WavePlayer_DisplayPlayList(uint8_t Select, uint8_t NbTitles);
static void     WavePlayer_DisplayPlaybackScreen(uint8_t Select);
static void     WavePlayer_DisplayBottomMessage(char * msg);
static uint8_t  WavePlayer_IsValidWAVHeader(WAV_HeaderTypeDef* WAV_header);

static void     PlaybackStateUpdate(char * PlaybackState);

/* Private Variables ---------------------------------------------------------*/
/* Used to exit application */
static __IO uint8_t UserEvent=0;
static __IO uint8_t UserEntry=0;

/* Play list */
static char PlayList[MAX_TITLES][MAX_TITLE_NAME_LENGTH] = {0};

/* Wave file information */
static FILINFO WaveFileInfo;

/* Default volume */
__IO uint32_t uwVolume = 35;

static RCC_ClkInitTypeDef RCC_ClkInitStructSaved = {0};
static uint32_t FLatencySaved;
/* Private typedef -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void WavePlayer_Demo(void)
{
  uint8_t nof_titles = 0;
  uint8_t application_state = WAVEPLAYER_INIT;
  uint8_t sel = 1;

  do
  {
    switch(application_state)
    {
    case WAVEPLAYER_INIT:
      nof_titles = WavePlayer_InitializePlayList();
      if (nof_titles == 0)
      {
        kWindow_Popup("PLAY LIST EMPTY", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE , "Push JoyStick\nto exit.\n", UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE);
        UserEvent  = JOY_NONE;
        UserEntry = 0;
        while(UserEvent == JOY_NONE);
        application_state = WAVEPLAYER_EXIT;
      }
      else
      {
        application_state = WAVEPLAYER_SELECT_TITLE;
      }
      break;

    case WAVEPLAYER_SELECT_TITLE:
      WavePlayer_DisplayTitleSelectionScreen();
      WavePlayer_DisplayPlayList(sel, nof_titles);
      WavePlayer_DisplayBottomMessage(MSG_TITLE_SELECTION);
      application_state = WAVEPLAYER_WAIT_SELECTION;
      break;

    /* Wait for an user event */
    case WAVEPLAYER_WAIT_SELECTION:
      /* User action init */
      UserEvent        = JOY_NONE;
      UserEntry       = 0;
      while(UserEvent == JOY_NONE);

      /* treatment of the user request */
      switch(UserEvent)
      {
      case JOY_DOWN:
        if (sel == nof_titles)
        {
          sel = 1;
        }
        else
        {
          sel ++;
        }
        WavePlayer_DisplayPlayList(sel, nof_titles);
        break;

      case JOY_UP:
        if (sel == 1)
        {
          sel = nof_titles;
        }
        else
        {
          sel --;
        }
        WavePlayer_DisplayPlayList(sel, nof_titles);
      break;

      case JOY_SEL:
        WavePlayer_DisplayPlaybackScreen(sel);
        WavePlayer_DisplayBottomMessage(MSG_PLAYBACK_PLAYING);
        PlaybackStateUpdate("PLAYING");
        WavePlayer_PlaybackStart(sel);
        application_state = WAVEPLAYER_PLAYING;
        break;

      case JOY_LEFT:
        /* don't care */
        break;

      case JOY_RIGHT:
        application_state = WAVEPLAYER_EXIT;
        break;
      }
      /* Reset the Userevent */
      UserEvent = JOY_NONE;
      UserEntry = 0;
      break;

    case WAVEPLAYER_PLAYING:
      {
        /* Update playing information */
        if (WaveFile.fptr < (WaveFileInfo.fsize - (2 * BUFFER_HALF_SIZE_BYTE)))
        {
          if (DMA_AudioOutComplete != 0)
          {
            WavePlayer_PlaybackPlay();
          }
        }
        else
        {
          kWidgets_ProgressBarUpdate(hProgressBar, WaveFileInfo.fsize);
          WavePlayer_PlaybackStop();
          WavePlayer_DisplayBottomMessage(MSG_PLAYBACK_STOPPED);
          PlaybackStateUpdate("  STOPPED  ");
          application_state = WAVEPLAYER_STOPPED;
        }

        switch (UserEvent)
        {
        case JOY_SEL:
          WavePlayer_PlaybackPause();
          WavePlayer_DisplayBottomMessage(MSG_PLAYBACK_PAUSED);
          PlaybackStateUpdate("  PAUSED  ");
          application_state = WAVEPLAYER_PAUSED;
          break;

        case JOY_DOWN:
          WavePlayer_PlaybackStop();
          WavePlayer_DisplayBottomMessage(MSG_PLAYBACK_STOPPED);
          PlaybackStateUpdate("  STOPPED  ");
          application_state = WAVEPLAYER_STOPPED;
          break;

        case JOY_RIGHT:
          WavePlayer_PlaybackStop();
          BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL);
          application_state = WAVEPLAYER_SELECT_TITLE;
          break;
        }
        UserEvent = JOY_NONE;
        UserEntry = 0;
        break;
      }

    case WAVEPLAYER_PAUSED:
      {
        if(1 == UserEntry)
        {
          switch (UserEvent)
          {
          case JOY_SEL:
            WavePlayer_PlaybackResume();
            WavePlayer_DisplayBottomMessage(MSG_PLAYBACK_PLAYING);
            PlaybackStateUpdate("PLAYING");
            application_state = WAVEPLAYER_PLAYING;
            break;

          case JOY_RIGHT:
            WavePlayer_PlaybackStop();
            BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL);
            application_state = WAVEPLAYER_SELECT_TITLE;
            break;

          case JOY_DOWN:
            WavePlayer_PlaybackStop();
            WavePlayer_DisplayBottomMessage(MSG_PLAYBACK_STOPPED);
            PlaybackStateUpdate("  STOPPED  ");
            application_state = WAVEPLAYER_STOPPED;
            break;

          default:
            break;
          }
          /* Reset the user event */
          UserEvent = JOY_NONE;
          UserEntry = 0;
        }
        break;
      }

    case WAVEPLAYER_STOPPED:
      switch (UserEvent)
      {
      case JOY_SEL:
        kWidgets_ProgressBarReset(hProgressBar);
        WavePlayer_PlaybackStart(sel);
        WavePlayer_DisplayBottomMessage(MSG_PLAYBACK_PLAYING);
        PlaybackStateUpdate("PLAYING");
        application_state = WAVEPLAYER_PLAYING;
        UserEvent = JOY_NONE;
        UserEntry = 0;
        break;

      case JOY_RIGHT:
        BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL);
        application_state = WAVEPLAYER_SELECT_TITLE;
        UserEvent = JOY_NONE;
        UserEntry = 0;
        break;

      default:
        /* do noting */
        UserEvent = JOY_NONE;
        UserEntry = 0;
        break;
      }

      break;

    case WAVEPLAYER_EXIT :
      if (hProgressBar != NULL)
      {
        kWidgets_ProgressBarDestroy(hProgressBar);
        hProgressBar = NULL;
      }
      application_state = WAVEPLAYER_END;
      break;
    }
  } while(application_state != WAVEPLAYER_END);
}

/**
  * @brief  Get User action
  * @param  sel User selection (JOY_SEL,...)
  * @note   This example is the only way to get user information.
  * @retval None
  */
void WavePlayer_UserAction(uint8_t sel)
{
    UserEvent = sel;
    UserEntry = 1;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the hardware resources used by the audio player
  * @param  SampleRate   sample rate to configure
  * @param  BitPerSample nb of bit per sample to configure
  * @param  NbrChannels  1 or 2 channels to configure
  * @retval BSP status
  */
static int32_t WavePlayer_PlaybackInit(uint32_t SampleRate, uint16_t BitPerSample, uint32_t NbrChannels)
{
  BSP_AUDIO_Init_t AudioPlayInit;
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  AudioPlayInit.Device = AUDIO_OUT_DEVICE_HEADPHONE;
  AudioPlayInit.ChannelsNbr = NbrChannels;
  AudioPlayInit.SampleRate = SampleRate;
  AudioPlayInit.BitsPerSample = BitPerSample;
  AudioPlayInit.Volume = uwVolume;

  if ( BSP_AUDIO_OUT_Init(0, &AudioPlayInit) == BSP_ERROR_NONE)
  {
      /* need to decrease SPI2 clock */
      HAL_RCC_GetClockConfig(&RCC_ClkInitStructSaved, &FLatencySaved);
      /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
         clocks dividers */
      RCC_ClkInitStruct.ClockType           = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
                                               RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
      RCC_ClkInitStruct.SYSCLKSource        = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.AHBCLKDivider       = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.APB1CLKDivider      = RCC_HCLK_DIV2;
      RCC_ClkInitStruct.APB2CLKDivider      = RCC_HCLK_DIV1;

      if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_8) != HAL_OK)
      {
        /* Initialization Error */
        return BSP_ERROR_CLOCK_FAILURE;
      }
  }
  else
  {
    return BSP_ERROR_NO_INIT;
  }
  return BSP_ERROR_NONE;
}

/**
  * @brief  Releases the hardware resources used by the audio player
  * @retval None
  */
static void WavePlayer_PlaybackDeInit(void)
{
  BSP_AUDIO_OUT_DeInit(0);

  /* reset SPI2 clock */
  HAL_RCC_ClockConfig(&RCC_ClkInitStructSaved, FLatencySaved);
}

/**
  * @brief  Starts audio playback
  * @param  Select   item to play
  * @retval None
  */
static void WavePlayer_PlaybackStart(uint8_t Select)
{
  WAV_HeaderTypeDef wav_header;
  uint32_t          bytes_read;
  char              file_name[MAX_TITLE_NAME_LENGTH+5];

  sprintf(file_name, "USER/%s", PlayList[Select-1]);

  if (f_stat (file_name, &WaveFileInfo) != FR_OK)
  {
    while(1);
  }

  if (f_open(&WaveFile, file_name, FA_READ) != FR_OK)
  {
    while(1);
  }

  if (f_read(&WaveFile, (void*)&wav_header, WAV_HEADER_SIZE, (UINT*)&bytes_read) != FR_OK)
  {
    while(1);
  }

  if (WavePlayer_IsValidWAVHeader(&wav_header))
  {
    /* Initialize hardware resources */
    WavePlayer_PlaybackInit(wav_header.SampleRate, wav_header.BitPerSample, wav_header.NbrChannels);

    /* Jump to wave data */
    WavePlayer_JumpToWaveData(&WaveFile, &wav_header);

    /* Prepare Buffer */
    WavePlayer_Prepare(&WaveFile, Buffer, BUFFER_HALF_SIZE_WORD * 2);

    /* Play buffer */
    BSP_AUDIO_OUT_Play(0, (uint8_t*)Buffer, BUFFER_HALF_SIZE_BYTE * 2);

    /* Set DMA flag */
    DMA_AudioOutComplete = STARTED;
  }
}

/**
  * @brief  Stops audio playback
  * @retval None
  */
static void WavePlayer_PlaybackStop(void)
{
  /* Stop Audio output */
  BSP_AUDIO_OUT_Stop(0);
  WavePlayer_PlaybackDeInit();

  /* Close Wave file */
  if (f_close(&WaveFile) != FR_OK)
  {
    while(1);
  }
}

/**
  * @brief  Pauses audio playback
  * @retval None
  */
static void WavePlayer_PlaybackPause(void)
{
  /* Pause audio output */
    BSP_AUDIO_OUT_Pause(0);
}

/**
  * @brief  Resumes audio playback
  * @retval None
  */
static void WavePlayer_PlaybackResume(void)
{
  /* Resume audio output */
    BSP_AUDIO_OUT_Resume(0);
}

/**
  * @brief  Plays the item sending samples to the codec
  * @retval None
  */
static void WavePlayer_PlaybackPlay(void)
{
  uint32_t   bytes_read;
  uint8_t   *pBuffer = NULL;

  if (DMA_AudioOutComplete == COMPLETE)
  {
    /* Play buffer 1st half, prepare 2nd half */
    pBuffer = (uint8_t*)Buffer + BUFFER_HALF_SIZE_BYTE;
  }
  else if (DMA_AudioOutComplete == HALF_COMPLETE)
  {
    pBuffer = (uint8_t*)Buffer;
  }

  /* Prepare next half buffer */
  if (f_read(&WaveFile, (void*)pBuffer, BUFFER_HALF_SIZE_BYTE, (UINT*)&bytes_read) != FR_OK)
  {
    while(1);
  }

  /* Reset DMA flag */
  DMA_AudioOutComplete = STARTED;

  /* Update progress bar*/
  kWidgets_ProgressBarUpdate(hProgressBar, WaveFile.fptr);
}

/**
  * @brief  Fill in playback buffer from audio file
  * @param  fp       audio file pointer
  * @param  pBuffer  audio samples buffer
  * @param  Size     audio samples buffer size (in words)
  * @retval None
  */
static void WavePlayer_Prepare(FIL* fp, uint32_t * pBuffer, uint32_t Size)
{
  uint32_t   i = 0;
  uint16_t sample = 0;

  /* Ramp-up audio reduce pop noise when audio playback is started. Valid for 1 channel as well as 2 channels */
  for (i = 0 ; i < Size ; i++)
  {
    (*pBuffer) = (sample << 16) | sample;
    pBuffer++;

    if (sample < (0x7FFF - STEP))
    {
      sample +=  STEP;
    }
  }
}

/**
  * @brief  Initializes the playlist
  * @retval nof_titles  Number of titles
  */
static uint8_t WavePlayer_InitializePlayList(void)
{
  uint8_t nof_titles = 0;

  if (kStorage_GetDirectoryFiles((uint8_t *)"USER", KSTORAGE_FINDFIRST, (uint8_t *)PlayList[nof_titles], (uint8_t *)"wav") == KSTORAGE_NOERROR)
  {
    nof_titles++;

    /* Get next titles (if any) */
    while (kStorage_GetDirectoryFiles((uint8_t *)"USER", KSTORAGE_FINDNEXT, (uint8_t *)PlayList[nof_titles], (uint8_t *)"wav") == KSTORAGE_NOERROR)
    {
      nof_titles++;
    }
  }
  return nof_titles;
}

/**
  * @brief  Displays the title selection screen
  * @retval None
  */
static void WavePlayer_DisplayTitleSelectionScreen(void)
{
  uint16_t x      = 140;
  uint16_t y      = 35;
  uint16_t width  = 180;
  uint16_t height = 160;
  uint32_t pXSize;

  /* Black screen */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Display Title */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
  BSP_LCD_GetXSize(LCD_INSTANCE, &pXSize);
  UTIL_LCD_FillRect(0, LINE(0), pXSize, Font24.Height, UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAt(0, LINE(0), (uint8_t *)"Wave Player", CENTER_MODE);

  /* Display playback Icon */
  kStorage_OpenFileDrawBMP(35, 46, (uint8_t *)"STFILES/Music2.bmp");

  /* Display playlist list box */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_FillRect(x, y, width, height, UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_FillRect(x + 3, y + 3, width - 6, height - 6, UTIL_LCD_COLOR_WHITE);
}

/**
  * @brief  Displays the play list
  * @param  Select    actual selection
  * @param  NbTitles  total number of titles
  * @retval None
  */
static void WavePlayer_DisplayPlayList(uint8_t Select, uint8_t NbTitles)
{
  uint8_t i;

  UTIL_LCD_SetFont(&Font16);

  for (i = 0; i < NbTitles; i++)
  {
    if (i == Select - 1)
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    }
    else
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
    }

    UTIL_LCD_DisplayStringAt(145, 40 + Font16.Height*i, (uint8_t *)PlayList[i], LEFT_MODE);
  }

  UTIL_LCD_SetFont(&Font24);
}

/**
  * @brief  Displays the playback screen
  * @param  Select title file name
  * @retval None
  */
static void WavePlayer_DisplayPlaybackScreen(uint8_t Select)
{
  char                  file_name[MAX_TITLE_NAME_LENGTH+5];
  FILINFO               fno;
  uint32_t pXSize;

  BSP_LCD_GetXSize(LCD_INSTANCE, &pXSize);
  kWidgetsProgressBar_t progress_bar =
  {
    .textColor       = UTIL_LCD_COLOR_ST_PINK,
    .backgroungColor = UTIL_LCD_COLOR_WHITE,
    .xpos            = 10,
    .ypos            = 150,
    .width           = pXSize - 20,
    .height          = 6,
  };

  /* Black screen */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Display Title */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_FillRect(0, LINE(0), pXSize, Font24.Height, UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"Wave Player", CENTER_MODE);

  /* Display playback Icon */
  kStorage_OpenFileDrawBMP(35, 46, (uint8_t *)"STFILES/Music2.bmp");

  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_DisplayStringAt(35, 46, (uint8_t *)PlayList[Select-1], CENTER_MODE);

  /* Retrieve file size */
  sprintf(file_name, "USER/%s", PlayList[Select-1]);

  if (f_stat (file_name, &fno) != FR_OK)
  {
    while(1);
  }

  /* Display playback progress bar */
  hProgressBar = kWidgets_ProgressBarCreate(&progress_bar, fno.fsize);
}

/**
  * @brief  Displays the message at the bottom of the screen
  * @param  msg  bottom message
  * @retval None
  */
static void WavePlayer_DisplayBottomMessage(char * msg)
{
  uint32_t pXSize;

  BSP_LCD_GetXSize(LCD_INSTANCE, &pXSize);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_FillRect(0, LINE(19), pXSize, Font20.Height, UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAt(0, LINE(19), (uint8_t *)msg, CENTER_MODE);
  UTIL_LCD_SetFont(&Font24);
}

/**
  * @brief  Indicates whether or not the wav header of the audio file is valid
  * @param  WAV_header  WAV header
  * @retval 1 if the wav header is valid, 0 otherwise
  */
static uint8_t WavePlayer_IsValidWAVHeader(WAV_HeaderTypeDef * WAV_header)
{
  uint8_t status = 1;

  if (   (strncmp(WAV_header->RIFF, "RIFF", SUB_CHUNK_ID_LENGHT) != 0)
      || (strncmp(WAV_header->WAVE, "WAVE", SUB_CHUNK_ID_LENGHT) != 0)
      || (WAV_header->AudioFormat != 1))
  {
    status = 0;
  }
  return status;
}

/**
  * @brief  Move the file pointer to the first audio sample
  * @param  fp             audio file pointer
  * @param  WAV_header     WAV header
  * @retval AudioDataSize  size of the raw audio data
  */
static uint32_t WavePlayer_JumpToWaveData(FIL * fp, WAV_HeaderTypeDef * WAV_header)
{
  uint32_t bytes_read;
  char     sub_chunk_id[SUB_CHUNK_ID_LENGHT];
  uint32_t sub_chunk_size;
  uint32_t audio_data_size = 0;
  FSIZE_t  offset = 0;

  /* Move file pointer after fmt chunk */
  offset = FMT_CHK_POS + SUB_CHUNK_ID_LENGHT +  sizeof(uint32_t) + WAV_header->SubChunk1Size;
  f_lseek(fp, offset);

  /* Read the next sub-chunk id */
  f_read(fp, (void *)sub_chunk_id, SUB_CHUNK_ID_LENGHT, (UINT*)&bytes_read);
  offset += bytes_read;

  /* Skip next sub-chunk if not a 'data' sub-chunk */
  if (strncmp(sub_chunk_id, "data", SUB_CHUNK_ID_LENGHT) == 0)
  {
    f_read(fp, (void *)&audio_data_size, sizeof(uint32_t), (UINT*)&bytes_read);
  }
  else
  {
    f_read(fp, (void *)&sub_chunk_size, sizeof(uint32_t), (UINT*)&bytes_read);
    offset += (bytes_read + sub_chunk_size);
    f_lseek(fp, offset);

    /* Read the next sub-chunk id */
    f_read(fp, (void *)sub_chunk_id, SUB_CHUNK_ID_LENGHT, (UINT*)&bytes_read);

    if (strncmp(sub_chunk_id, "data", SUB_CHUNK_ID_LENGHT) == 0)
    {
      f_read(fp, (void *)&audio_data_size, sizeof(uint32_t), (UINT*)&bytes_read);;
    }
  }

  return audio_data_size;
}

/**
  * @brief  Conversion complete callback in non-blocking mode for Channel1
  * @param  Interface Audio out instance.
  * @retval None
  */
void BSP_AUDIO_OUT_TransferComplete_CallBack(uint32_t Interface)
{
  DMA_AudioOutComplete = COMPLETE;
}

/**
  * @brief  Conversion half complete callback in non-blocking mode for Channel1
  * @param  Interface Audio out instance.
  * @retval None
  */
void BSP_AUDIO_OUT_HalfTransfer_CallBack(uint32_t Interface)
{
  DMA_AudioOutComplete = HALF_COMPLETE;
}

/**
  * @brief  Update the displayed playback state
  * @param  PlaybackState  playback state
  * @retval None
  */
static void PlaybackStateUpdate(char * PlaybackState)
{
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_DisplayStringAt(0, 120, (uint8_t *)PlaybackState, CENTER_MODE);
}



