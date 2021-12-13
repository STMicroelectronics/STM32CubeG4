/**
  ******************************************************************************
  * @file    BSP/Src/audio_rec.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the audio feature in the
  *          STM32G474E EVAL driver
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
#include "main.h"
#include <stdio.h>
#include "string.h"

/** @addtogroup STM32G4xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

typedef enum
{
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF = 1,
  BUFFER_OFFSET_FULL = 2,
}BUFFER_StateTypeDef;

extern AUDIO_ErrorTypeDef AUDIO_Start(uint32_t audio_start_address, uint32_t audio_file_size);

#define AUDIO_BLOCK_SIZE   ((uint32_t)0xFFFE)
#define AUDIO_NB_BLOCKS    ((uint32_t)4)

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint16_t  internal_buffer[AUDIO_BLOCK_SIZE];

/* Global variables ---------------------------------------------------------*/
uint32_t  audio_rec_buffer_state;
BSP_AUDIO_Init_t AnalogInInit;
 BSP_AUDIO_Init_t  AudioInInit;
 BSP_AUDIO_Init_t AudioOutInit;
uint32_t AudioBufferOffset;
uint32_t InputVolume = 0, InputDevice, BitPerSample, ChannelNbr;

/* Private function prototypes -----------------------------------------------*/
static void AudioRec_SetHint(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Audio Play demo
  * @param  None
  * @retval None
  */
int32_t AudioRec_demo (void)
{
  uint32_t  block_number;
  uint32_t y_size;

  BSP_LCD_GetYSize(0, &y_size);

  AudioRec_SetHint();

  AudioOutInit.Device = AUDIO_OUT_DEVICE_HEADPHONE;
  AudioOutInit.ChannelsNbr = 2;
  AudioOutInit.SampleRate = AUDIO_FREQUENCY_16K;
  AudioOutInit.BitsPerSample = AUDIO_RESOLUTION_32B;
  AudioOutInit.Volume = 100;

  AudioInInit.Device = AUDIO_IN_DEVICE_ANALOG_MIC;
  AudioInInit.ChannelsNbr = 2;
  AudioInInit.SampleRate = AUDIO_FREQUENCY_16K;
  AudioInInit.BitsPerSample = AUDIO_RESOLUTION_16B;
  AudioInInit.Volume = 30;

  /* Initialize Audio Recorder */
  if (BSP_AUDIO_IN_Init(0, &AudioInInit) == BSP_ERROR_NONE)
  {
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
    UTIL_LCD_DisplayStringAt(0, y_size - 95, (uint8_t *)"  AUDIO RECORD INIT OK  ", CENTER_MODE);
  }
  else
  {
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    UTIL_LCD_DisplayStringAt(0, y_size - 95, (uint8_t *)"  AUDIO RECORD INIT FAIL", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, y_size - 80, (uint8_t *)" Try to reset board ", CENTER_MODE);
  }

  AudioBufferOffset = BUFFER_OFFSET_NONE;

  /* Display the state on the screen */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DisplayStringAt(0, y_size - 80, (uint8_t *)"       RECORDING...     ", CENTER_MODE);

  BSP_AUDIO_IN_GetVolume(0, &InputVolume);
  BSP_AUDIO_IN_GetDevice(0, &InputDevice);
  BSP_AUDIO_IN_GetChannelsNbr(0, &ChannelNbr);
  BSP_AUDIO_IN_GetBitsPerSample(0, &BitPerSample);

  BSP_AUDIO_IN_SetVolume(0, 80);
  BSP_AUDIO_IN_SetDevice(0, AUDIO_IN_DEVICE_ANALOG_MIC);
  BSP_AUDIO_IN_SetChannelsNbr(0, 1);
  BSP_AUDIO_IN_SetBitsPerSample(0, AUDIO_RESOLUTION_32B);

  BSP_AUDIO_IN_GetVolume(0, &InputVolume);
  BSP_AUDIO_IN_GetDevice(0, &InputDevice);
  BSP_AUDIO_IN_GetChannelsNbr(0, &ChannelNbr);
  BSP_AUDIO_IN_GetBitsPerSample(0, &BitPerSample);

  /* Start Recording: Size in number of bytes */
  BSP_AUDIO_IN_Record(0, (uint8_t*)&internal_buffer, 2*AUDIO_BLOCK_SIZE);

  for (block_number = 0; block_number < AUDIO_NB_BLOCKS; block_number++)
  {
    /* Wait end of half block recording */
    while(AudioBufferOffset != BUFFER_OFFSET_HALF)
    {
      if (CheckForUserInput() > 0)
      {
        ButtonState = 0;
        /* Stop Recorder before close Test */
        BSP_AUDIO_IN_Stop(0);
        return 0;
      }
    }
    AudioBufferOffset = BUFFER_OFFSET_NONE;
    /* Copy recorded 1st half block in SDRAM */
    memcpy((uint32_t *)(AUDIO_REC_START_ADDR + (block_number * AUDIO_BLOCK_SIZE * 2)),
           internal_buffer,
           AUDIO_BLOCK_SIZE);

    /* Wait end of one block recording */
    while(AudioBufferOffset != BUFFER_OFFSET_FULL)
    {
      if (CheckForUserInput() > 0)
      {
        ButtonState = 0;
        /* Stop Recorder before close Test */
        BSP_AUDIO_IN_Stop(0);
        return 0;
      }
    }
    AudioBufferOffset = BUFFER_OFFSET_NONE;
    /* Copy recorded 2nd half block in SDRAM */
    memcpy((uint32_t *)(AUDIO_REC_START_ADDR + (block_number * AUDIO_BLOCK_SIZE * 2) + (AUDIO_BLOCK_SIZE)),
           (uint16_t *)(&internal_buffer[AUDIO_BLOCK_SIZE/2]),
           AUDIO_BLOCK_SIZE);
  }

  /* Stop recorder */
  BSP_AUDIO_IN_Stop(0);
  BSP_AUDIO_IN_DeInit(0);

  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DisplayStringAt(0, y_size - 65, (uint8_t *)"RECORDING DONE, START PLAYBACK...", CENTER_MODE);

  /* -----------Start Playback -------------- */
  /* Initialize audio IN at REC_FREQ*/
  BSP_AUDIO_OUT_Init(0, &AudioOutInit);

  /* Play the recorded buffer*/
  AUDIO_Start(AUDIO_REC_START_ADDR, AUDIO_BLOCK_SIZE * AUDIO_NB_BLOCKS * 2);
  UTIL_LCD_DisplayStringAt(0, y_size - 40, (uint8_t *)"PLAYBACK DONE", CENTER_MODE);

  while (1)
  {
    AUDIO_Process();

    if (CheckForUserInput() > 0)
    {
      ButtonState = 0;
      /* Stop Player before close Test */
      BSP_AUDIO_OUT_Stop(0);
      BSP_AUDIO_OUT_DeInit(0);
      return 0;
    }
  }
}

/**
  * @brief  Audio Play demo
  * @param  None
  * @retval None
  */
int32_t AudioRecAnalog_demo (void)
{
  uint32_t  block_number;
  uint32_t y_size;

  BSP_LCD_GetYSize(0, &y_size);

  AudioRec_SetHint();
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"AUDIO RECORD ANALOG", CENTER_MODE);
  UTIL_LCD_SetFont(&Font12);

  AudioOutInit.Device = AUDIO_OUT_DEVICE_HEADPHONE;
  AudioOutInit.ChannelsNbr = 2;
  AudioOutInit.SampleRate = AUDIO_FREQUENCY_48K;
  AudioOutInit.BitsPerSample = AUDIO_RESOLUTION_16B;
  AudioOutInit.Volume = 95;

  AnalogInInit.Device = AUDIO_IN_DEVICE_ANALOG_MIC;
  AnalogInInit.ChannelsNbr = 1;
  AnalogInInit.SampleRate = AUDIO_FREQUENCY_48K;
  AnalogInInit.BitsPerSample = AUDIO_RESOLUTION_16B;
  AnalogInInit.Volume = 100;

  /* Initialize Audio Recorder */
  if (BSP_AUDIO_IN_Init(0, &AnalogInInit) == BSP_ERROR_NONE)
  {
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
    UTIL_LCD_DisplayStringAt(0, y_size - 95, (uint8_t *)"  AUDIO RECORD INIT OK  ", CENTER_MODE);
  }
  else
  {
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    UTIL_LCD_DisplayStringAt(0, y_size - 95, (uint8_t *)"  AUDIO RECORD INIT FAIL", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, y_size - 80, (uint8_t *)" Try to reset board ", CENTER_MODE);
  }

  AudioBufferOffset = BUFFER_OFFSET_NONE;

  /* Display the state on the screen */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DisplayStringAt(0, y_size - 80, (uint8_t *)"       RECORDING...     ", CENTER_MODE);


  /* Start Recording */
  BSP_AUDIO_IN_Record(0, (uint8_t*)internal_buffer, 2*AUDIO_BLOCK_SIZE);

  for (block_number = 0; block_number < AUDIO_NB_BLOCKS; block_number++)
  {
    /* Wait end of half block recording */
    while(AudioBufferOffset != BUFFER_OFFSET_HALF)
    {
      if (CheckForUserInput() > 0)
      {
        ButtonState = 0;
        /* Stop Recorder before close Test */
        BSP_AUDIO_IN_Stop(0);
        return 0;
      }
    }
    AudioBufferOffset = BUFFER_OFFSET_NONE;
    /* Copy recorded 1st half block in SDRAM */
    memcpy((uint32_t *)(AUDIO_REC_START_ADDR + (block_number * AUDIO_BLOCK_SIZE * 2)),
           internal_buffer,
           AUDIO_BLOCK_SIZE);

    /* Wait end of one block recording */
    while(AudioBufferOffset != BUFFER_OFFSET_FULL)
    {
      if (CheckForUserInput() > 0)
      {
        ButtonState = 0;
        /* Stop Recorder before close Test */
        BSP_AUDIO_IN_Stop(0);
        return 0;
      }
    }
    AudioBufferOffset = BUFFER_OFFSET_NONE;
    /* Copy recorded 2nd half block in SDRAM */
    memcpy((uint32_t *)(AUDIO_REC_START_ADDR + (block_number * AUDIO_BLOCK_SIZE * 2) + (AUDIO_BLOCK_SIZE)),
           (uint16_t *)(&internal_buffer[AUDIO_BLOCK_SIZE/2]),
           AUDIO_BLOCK_SIZE);
  }

  /* Stop recorder */
  BSP_AUDIO_IN_Stop(0);
  BSP_AUDIO_IN_DeInit(0);

  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DisplayStringAt(0, y_size - 65, (uint8_t *)"RECORDING DONE, START PLAYBACK...", CENTER_MODE);

  /* -----------Start Playback -------------- */
  /* Initialize audio IN at REC_FREQ*/
  BSP_AUDIO_OUT_Init(0, &AudioOutInit);

  /* Play the recorded buffer*/
  AUDIO_Start(AUDIO_REC_START_ADDR, AUDIO_BLOCK_SIZE * AUDIO_NB_BLOCKS * 2);
  UTIL_LCD_DisplayStringAt(0, y_size - 40, (uint8_t *)"PLAYBACK DONE", CENTER_MODE);

  while (1)
  {
    AUDIO_Process();

    if (CheckForUserInput() > 0)
    {
      ButtonState = 0;
      /* Stop Player before close Test */
      BSP_AUDIO_OUT_Stop(0);
      BSP_AUDIO_OUT_DeInit(0);
      return 0;
    }
  }
}

/**
  * @brief  Display Audio Record demo hint
  * @param  None
  * @retval None
  */
static void AudioRec_SetHint(void)
{
  uint32_t x_size, y_size;

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Set Audio Demo description */
  BSP_LCD_FillRect(0, 0, 0, x_size, 90, LCD_COLOR_RGB565_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"AUDIO RECORD EXAMPLE", CENTER_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, 30, (uint8_t *)"Press User button for next menu", CENTER_MODE);

  /* Set the LCD Text Color */
  UTIL_LCD_DrawRect(10, 100, x_size - 20, y_size - 110, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DrawRect(11, 101, x_size - 22, y_size - 112, UTIL_LCD_COLOR_BLUE);

}

/*------------------------------------------------------------------------------
       Callbacks implementation:
           the callbacks API are defined __weak in the stm32746g_discovery_audio.c file
           and their implementation should be done the user code if they are needed.
           Below some examples of callback implementations.
  ----------------------------------------------------------------------------*/

/**
  * @brief  Audio IN Error callback function.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance)
{
  /* This function is called when an Interrupt due to transfer error on or peripheral
     error occurs. */
  /* Display message on the LCD screen */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_RED);
  UTIL_LCD_DisplayStringAt(0, LINE(14), (uint8_t *)"       DMA  ERROR     ", CENTER_MODE);

  /* Stop the program with an infinite loop */
  while (BSP_PB_GetState(BUTTON_USER) != RESET)
  {
    return;
  }
  /* could also generate a system reset to recover from the error */
  /* .... */
}



/**
  * @}
  */

/**
  * @}
  */


