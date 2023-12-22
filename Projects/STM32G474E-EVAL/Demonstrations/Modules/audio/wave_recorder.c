/**
  ******************************************************************************
  * @file    wave_recorder.c
  * @author  MCD Application Team
  * @brief   WAVE recorder implementation
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
#include "stm32g4xx_hal.h"
#include "app_audio.h"
#include "wave_recorder.h"
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
  WAVERECORDER_INIT,
  WAVERECORDER_WAIT,
  WAVERECORDER_RECORDING,
  WAVERECORDER_STOPPED,
  WAVERECORDER_ERROR,
  WAVERECORDER_EXIT,
  WAVERECORDER_END
};

/* User messages */
#define MSG_RECORD_INIT      "SEL: Start Rec - RIGHT: Quit"
#define MSG_RECORD_ON_GOING  "DOWN: Stop - RIGHT: Quit   "
#define MSG_RECORD_STOPPED   "SEL: Restart Rec - RIGHT: Quit"

/* Record information */
#define MSG_FILE_NAME        (uint8_t *)"rec.wav"
#define MSG_SAMPLE_RATE      (uint8_t *)"44100 Hz"
#define MSG_NOF_CHANNELS     (uint8_t *)"Mono"
#define MSG_BITS_PER_SAMPLES (uint8_t *)"16 bits"

/* Sample rate (44 kHz)*/
#define SAMPLE_RATE         AUDIO_FREQUENCY_44K

/* Maximum data chunk size */
#define MAX_DATA_CHUNCK_SIZE  0x003A9800U

/* Position of the cksize field of the wave header */
#define CKSIZE_POS            4

/* position of the cksize field of the data chunk */
#define DATA_CKSIZE_POS       40

/* Private function prototypes -----------------------------------------------*/
static uint8_t RecordInit(void);
static uint8_t RecordDeInit(void);
static uint8_t RecordStart(void);
static uint8_t RecordStop(void);

static uint8_t CopyToSD(void);

static uint8_t WaveFileCreate(void);
static uint8_t WaveFileFinalize(void);

static void DisplayRecordHeaderScreen(void);
static void DisplayRecordStoppedScreen(void);
static void DisplayRecordOnGoingScreen(void);
static void DisplayBottomMessage(char * msg);
static void RecordStateUpdate(char * msg);

/* Private Variables ---------------------------------------------------------*/
/* Used to exit application */
static __IO uint8_t UserEvent=0;
static __IO uint8_t UserAction=0;

/* Actual size of the chunk that contains the sampled data */
uint32_t DataCkSize = 0;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void WaveRecorder_Demo(void)
{
  uint8_t application_state = WAVERECORDER_INIT;

  do
  {
    switch(application_state)
    {
    case WAVERECORDER_INIT:
      if (RecordInit() == 0)
      {
        DisplayRecordHeaderScreen();
        DisplayBottomMessage(MSG_RECORD_INIT);
        application_state = WAVERECORDER_WAIT;
      }
      else
      {
        application_state = WAVERECORDER_ERROR;
      }
      break;

    /* Wait for an user event */
    case WAVERECORDER_WAIT:
      /* User action init */
      UserEvent        = JOY_NONE;
      UserAction       = 0;
      while(UserEvent == JOY_NONE);

      /* treatment of the user request */
      switch(UserEvent)
      {
      case JOY_SEL:
        /* Clear audio buffers */
        memset(Buffer, 0, BUFFER_HALF_SIZE_BYTE *2 );

        /* Reset DMA_AudioInComplete status */
        DMA_AudioInComplete = STARTED;

        if (WaveFileCreate() == 0)
        {
          DisplayRecordOnGoingScreen();
          if (RecordStart() == 0)
          {
            application_state = WAVERECORDER_RECORDING;
          }
          else
          {
            application_state = WAVERECORDER_ERROR;
          }
        }
        else
        {
          application_state = WAVERECORDER_ERROR;
        }
        UserAction = 0;
        UserEvent = JOY_NONE;
        break;

      case JOY_RIGHT:
        application_state = WAVERECORDER_EXIT;
        break;

      default:
        /* don't care */
        break;
      }
      break;

    case WAVERECORDER_RECORDING:
      switch (UserEvent)
      {
      case JOY_NONE:
      case JOY_LEFT:
      case JOY_UP:
      case JOY_SEL:
        if (DataCkSize < MAX_DATA_CHUNCK_SIZE)
        {
          if ((DMA_AudioInComplete != STARTED))
          {
            if (CopyToSD() != 0)
            {
              application_state = WAVERECORDER_ERROR;
            }
          }
        }
        else
        {
          application_state = WAVERECORDER_EXIT;
        }
        UserAction = 0;
        break;

      case JOY_RIGHT:
        application_state = WAVERECORDER_EXIT;
        break;

      case JOY_DOWN:
        application_state = WAVERECORDER_STOPPED;
        break;
      }

      break;

    case WAVERECORDER_STOPPED:
      if (hProgressBar != NULL)
      {
        kWidgets_ProgressBarReset(hProgressBar);
      }
      WaveFileFinalize();
      RecordStop();
      RecordDeInit();
      DisplayRecordStoppedScreen();
      RecordInit();
      application_state = WAVERECORDER_WAIT;
      break;

    case WAVERECORDER_ERROR:
      RecordStop();
      RecordDeInit();
      f_close(&WaveFile);
      f_unlink (REC_FILE_NAME);
      application_state = WAVERECORDER_END;
      break;

    case WAVERECORDER_EXIT :
      if (hProgressBar != NULL)
      {
        kWidgets_ProgressBarDestroy(hProgressBar);
        hProgressBar = NULL;
      }
      WaveFileFinalize();
      RecordStop();
      RecordDeInit();
      application_state = WAVERECORDER_END;
      break;
    }
  } while(application_state != WAVERECORDER_END);
}

/**
  * @brief  Get User action
  * @param  sel   User selection (JOY_SEL,...)
  * @note   This example is the only way to get user information.
  * @retval None
  */
void WaveRecorder_UserAction(uint8_t sel)
{
  if (UserAction == 0)
  {
    UserEvent = sel;
    UserAction = 1;
  }
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the hardware resources used by the audio recorder
  * @retval None
  */
static uint8_t RecordInit(void)
{
  BSP_AUDIO_Init_t AudioRecInit;
  AudioRecInit.Device = AUDIO_IN_DEVICE_ANALOG_MIC;
  AudioRecInit.ChannelsNbr = 1;
  AudioRecInit.SampleRate = SAMPLE_RATE;
  AudioRecInit.BitsPerSample = AUDIO_RESOLUTION_16B;
  AudioRecInit.Volume = 100;

  return BSP_AUDIO_IN_Init(0, &AudioRecInit);
  /* with sample rate of AUDIO_FREQUENCY_22K, no need to decrease SPI2 clock **/
}

/**
  * @brief  Releases the hardware resources used by the audio recorder
  * @retval BSP status
  */
static uint8_t RecordDeInit(void)
{
  return (uint8_t)BSP_AUDIO_IN_DeInit(0);
}

/**
  * @brief  Starts audio recording
  * @retval BSP status
  */
static uint8_t RecordStart(void)
{
  /* Reset DMA_AudioInComplete status */
  DMA_AudioInComplete = STARTED;

  /* Start Recording */
  return (uint8_t)BSP_AUDIO_IN_Record(0, (uint8_t*)Buffer, BUFFER_HALF_SIZE_BYTE * 2);
}

/**
  * @brief  Stops audio recording
  * @retval BSP status
  */
static uint8_t RecordStop(void)
{
  return (uint8_t)BSP_AUDIO_IN_Stop(0);
}


uint32_t pBuffer2[BUFFER_HALF_SIZE_WORD];
/**
  * @brief  Copy audio buffer to SD card
  * @retval Error code
  */
static uint8_t CopyToSD(void)
{
  uint8_t    ret = 0;
  uint32_t   bytes_written;
  uint8_t   *pBuffer = NULL;

  /* Select the valid half buffer */
  if (DMA_AudioInComplete == HALF_COMPLETE)
  { /* Currently recording on 1st half buffer, so copy 2nd half to SD */
    pBuffer = (uint8_t*)Buffer;
  }
  else if (DMA_AudioInComplete == COMPLETE)
  { /* Currently recording on 2nd half buffer, so copy 1st half to SD */
    pBuffer = (uint8_t*)Buffer + BUFFER_HALF_SIZE_BYTE;
  }

  /* Copy audio buffer to SD card */
  if (f_write(&WaveFile, pBuffer, BUFFER_HALF_SIZE_BYTE, (UINT*)&bytes_written) != FR_OK )
  {
    ret++;
  }

  /* Update record progress bar */
  DataCkSize += bytes_written;
  kWidgets_ProgressBarUpdate(hProgressBar, DataCkSize);

  /* Reset DMA flag */
  DMA_AudioInComplete = STARTED;

  return ret;
}

/**
  * @brief  Create the record wave file
  * @retval Error code
  */
static uint8_t WaveFileCreate(void)
{
  uint8_t           ret = 0;
  uint32_t          bytes_written;
  uint8_t header_buffer[_MAX_SS] = {0};

  /* Delete previous wave file */
  f_unlink (REC_FILE_NAME);

  /* Create a new wave file */
  if (f_open(&WaveFile, REC_FILE_NAME, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
  {
    ret++;
  }

  /* Initialize the wave header */
  if (ret == 0)
  {
    /* write chunkID, must be 'RIFF'  ------------------------------------------*/
    header_buffer[0] = 'R';
    header_buffer[1] = 'I';
    header_buffer[2] = 'F';
    header_buffer[3] = 'F';

    /* Write the file length ---------------------------------------------------*/
    /* The sampling time 22050 Hz
    To record 30s we need 22050 x 30 x 2 = 0x00142FF8 */
    header_buffer[4] = 0xF8;
    header_buffer[5] = 0x2F;
    header_buffer[6] = 0x14;
    header_buffer[7] = 0x00;

    /* Write the file format, must be 'WAVE' -----------------------------------*/
    header_buffer[8]  = 'W';
    header_buffer[9]  = 'A';
    header_buffer[10] = 'V';
    header_buffer[11] = 'E';

    /* Write the format chunk, must be'fmt ' -----------------------------------*/
    header_buffer[12]  = 'f';
    header_buffer[13]  = 'm';
    header_buffer[14]  = 't';
    header_buffer[15]  = ' ';

    /* Write the length of the 'fmt' data, must be 0x10 ------------------------*/
    header_buffer[16]  = 0x10;
    header_buffer[17]  = 0x00;
    header_buffer[18]  = 0x00;
    header_buffer[19]  = 0x00;

    /* Write the audio format, must be 0x01 (PCM) ------------------------------*/
    header_buffer[20]  = 0x01;
    header_buffer[21]  = 0x00;

    /* Write the number of channels, must be 0x02 (Stereo) ---------------------*/
    header_buffer[22]  = 0x01;
    header_buffer[23]  = 0x00;

    /* Write the Sample Rate 22050 Hz ------------------------------------------*/
    header_buffer[24]  = (uint8_t)((SAMPLE_RATE & 0xFF));
    header_buffer[25]  = (uint8_t)((SAMPLE_RATE >> 8) & 0xFF);
    header_buffer[26]  = (uint8_t)((SAMPLE_RATE >> 16) & 0xFF);
    header_buffer[27]  = (uint8_t)((SAMPLE_RATE >> 24) & 0xFF);

    /* Write the Byte Rate: Sampling rate * BitsPerSample * NumChannel / 8
    = 22000 * 16 / 8
    =  22000 * 2 */
    header_buffer[28]  = (uint8_t)(((SAMPLE_RATE * 16/8) & 0xFF));
    header_buffer[29]  = (uint8_t)(((SAMPLE_RATE * 16/8) >> 8) & 0xFF);
    header_buffer[30]  = (uint8_t)(((SAMPLE_RATE * 16/8) >> 16) & 0xFF);
    header_buffer[31]  = (uint8_t)(((SAMPLE_RATE * 16/8) >> 24) & 0xFF);

    /* Write the block alignment -----------------------------------------------*/
    header_buffer[32]  = 0x04;
    header_buffer[33]  = 0x00;

    /* Write the number of bits per sample -------------------------------------*/
    header_buffer[34]  = 0x10;
    header_buffer[35]  = 0x00;

    /* Write the Data chunk, must be 'data' ------------------------------------*/
    header_buffer[36]  = 'd';
    header_buffer[37]  = 'a';
    header_buffer[38]  = 't';
    header_buffer[39]  = 'a';

    /* Write the number of sample data -----------------------------------------*/
    header_buffer[40]  = 0xF8;
    header_buffer[41]  = 0x2F;
    header_buffer[42]  = 0x14;
    header_buffer[43]  = 0x00;

    if (f_write(&WaveFile, &header_buffer, _MAX_SS, (UINT*)&bytes_written) != FR_OK)
    {
      f_close(&WaveFile);
      ret++;
    }
  }

  /* Initialize the data chunk */
  if (ret == 0)
  {
    /* Reset Data chunk size */
    DataCkSize = 0;

    if ((f_write(&WaveFile, "data", 4, (UINT*)&bytes_written) != FR_OK) ||
        (f_write(&WaveFile, &DataCkSize, sizeof(uint32_t), (UINT*)&bytes_written) != FR_OK))
    {
      f_close(&WaveFile);
      ret++;
    }
  }

  return ret;
}

/**
  * @brief  Finalize the record wave file
  * @retval Error code
  */
static uint8_t WaveFileFinalize(void)
{
  uint8_t  ret = 0;
  uint32_t bytes_written;
  uint32_t cksize;

  /* Update the cksize field of the wave header */
  cksize = 36 + DataCkSize;
  f_lseek(&WaveFile, CKSIZE_POS);
  f_write(&WaveFile, &cksize, sizeof(cksize), (UINT*)&bytes_written);

  /* Update the cksize field of the data chunk */
  f_lseek(&WaveFile, DATA_CKSIZE_POS);
  f_write(&WaveFile, &DataCkSize, sizeof(DataCkSize), (UINT*)&bytes_written);

  /* Close the wave file */
  f_close(&WaveFile);

  return ret;
}


/**
  * @brief  Conversion complete callback in non-blocking mode for Channel1
  * @param  Interface Audio out instance.
  * @retval None
  */
void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Interface)
{
  DMA_AudioInComplete = HALF_COMPLETE;
}

/**
  * @brief  Conversion half complete callback in non-blocking mode for Channel1
  * @param  Interface Audio out instance.
  * @retval None
  */
void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Interface)
{
  DMA_AudioInComplete = COMPLETE;
}

/**
  * @brief  Displays the record stopped screen
  * @param  None
  * @retval None
  */
static void DisplayRecordStoppedScreen(void)
{
  /* Display Record stopped Icon */
  kStorage_OpenFileDrawBMP(35, 46, (uint8_t *)"STFILES/RECC2.bmp");

  DisplayBottomMessage(MSG_RECORD_STOPPED);
  RecordStateUpdate(" RECORD STOPPED ");
}

/**
  * @brief  Displays the record on going screen
  * @param  None
  * @retval None
  */
static void DisplayRecordOnGoingScreen(void)
{
  /* Display Record on going Icon */
  kStorage_OpenFileDrawBMP(35, 46, (uint8_t *)"STFILES/RECC.bmp");

  DisplayBottomMessage(MSG_RECORD_ON_GOING);
  RecordStateUpdate(" RECORD ON GOING ");
}

/**
  * @brief  Displays the record header screen
  * @param  None
  * @retval None
  */
static void DisplayRecordHeaderScreen(void)
{
  uint32_t pXSize;

  BSP_LCD_GetXSize(LCD_INSTANCE, &pXSize);

  kWidgetsProgressBar_t progress_bar =
  {
    .textColor       = UTIL_LCD_COLOR_ST_PINK,
    .backgroungColor = UTIL_LCD_COLOR_WHITE,
    .xpos            = 10,
    .ypos            = 150,
    .width           = pXSize - 20,
    .height          = 5,
  };

  /* Black screen */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Display Title */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_FillRect(0, LINE(0), pXSize, Font24.Height, UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"Wave Recorder", CENTER_MODE);

  /* Display Record on going Icon */
  kStorage_OpenFileDrawBMP(35, 46, (uint8_t *)"STFILES/RECC2.bmp");

  RecordStateUpdate("'SEL' TO RECORD");

  /* Display record info */
  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_DisplayStringAt(150, 46,                     MSG_FILE_NAME,        LEFT_MODE);
  UTIL_LCD_DisplayStringAt(150, 46 + Font16.Height,     MSG_SAMPLE_RATE,      LEFT_MODE);
  UTIL_LCD_DisplayStringAt(150, 46 + 2 * Font16.Height, MSG_NOF_CHANNELS,     LEFT_MODE);
  UTIL_LCD_DisplayStringAt(150, 46 + 3 * Font16.Height, MSG_BITS_PER_SAMPLES, LEFT_MODE);

  /* Display playback progress bar */
  hProgressBar = kWidgets_ProgressBarCreate(&progress_bar, MAX_DATA_CHUNCK_SIZE);

}

/**
  * @brief  Displays the message at the bottom of the screen
  * @param  msg  bottom message
  * @retval None
  */
static void DisplayBottomMessage(char * msg)
{
  uint32_t pXSize;

  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
  BSP_LCD_GetXSize(LCD_INSTANCE, &pXSize);
  UTIL_LCD_FillRect(0, LINE(19), pXSize, Font20.Height,UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAt(0, LINE(19), (uint8_t *)msg, CENTER_MODE);
  UTIL_LCD_SetFont(&Font24);
}

/**
  * @brief  Update the displayed record state
  * @param  RecordState   record state
  * @retval None
  */
static void RecordStateUpdate(char * RecordState)
{
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_DisplayStringAt(0, 120, (uint8_t *)RecordState, CENTER_MODE);
}


