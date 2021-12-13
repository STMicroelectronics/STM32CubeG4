/**
  ******************************************************************************
  * @file    BSP/Src/sd.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the SD Driver in the
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

/** @addtogroup STM32G4xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SD_CARD_PRESENCE_POLLING_MODE        0
#define SD_CARD_PRESENCE_INTERRUPT_MODE      1

#define SD_CARD_PRESENCE_VALIDATION_MODE      SD_CARD_PRESENCE_INTERRUPT_MODE

#define BLOCK_START_ADDR         0     /* Block start address      */
#define NUM_OF_BLOCKS            5    /* Total number of blocks   */
#define BLOCKSIZE                SD_BLOCKSIZE
#define BUFFER_WORDS_SIZE        ((BLOCKSIZE * NUM_OF_BLOCKS) >> 2) /* Total data size in bytes */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t aTxBuffer[BUFFER_WORDS_SIZE];
uint32_t aRxBuffer[BUFFER_WORDS_SIZE];
__IO uint32_t SDWriteStatus = 0, SDReadStatus = 0, SDDetectStatus = 0, SDDetectIT = 0;

extern __IO uint32_t UserButtonPressed;

/* Private function prototypes -----------------------------------------------*/
static void SD_SetHint(void);
static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset);
static uint8_t Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint16_t BufferLength);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  SD Demo
  * @param  None
  * @retval None
  */
int32_t SD_demo (void)
{
  int32_t result = 0;
  int32_t SD_state = BSP_ERROR_NONE;
  static uint8_t prev_status = 2;  /* Undefined state */

  SD_SetHint();

  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  SD_state = BSP_SD_Init(0);

#if (SD_CARD_PRESENCE_VALIDATION_MODE == SD_CARD_PRESENCE_INTERRUPT_MODE)
  BSP_SD_DetectITConfig(0);
#endif
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_LIGHTBLUE);

  if (SD_state != BSP_ERROR_NONE)
  {
    SD_state = BSP_SD_IsDetected(0);
    if(SD_state == SD_NOT_PRESENT)
    {
      printf ("\r\nSD shall be inserted before running test");
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
      UTIL_LCD_DisplayStringAt(20, 200, (uint8_t *)"Insert SD card", LEFT_MODE);
    }
    else
    {
      printf ("\r\nSD Initialization : FAIL.");
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
      UTIL_LCD_DisplayStringAt(20, 120, (uint8_t *)"SD card init failed", LEFT_MODE);
    }
    printf ("\r\nSD Test Aborted.");
    result --;
  }
  else
  {
    printf ("\r\nSD Initialization : OK.");
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
    UTIL_LCD_DisplayStringAt(20, 120, (uint8_t *)"SD card init OK    ", LEFT_MODE);
    prev_status = SD_PRESENT;

      /* Fill the buffer to write */
      Fill_Buffer(aTxBuffer, BUFFER_WORDS_SIZE, 0x22FF);
      SD_state = BSP_SD_WriteBlocks(0, aTxBuffer, BLOCK_START_ADDR, NUM_OF_BLOCKS);
      HAL_Delay(500);

      /* Wait until SD cards are ready to use for new operation */
      while((BSP_SD_GetCardState(0) != SD_TRANSFER_OK))
      {
      }

      if (SD_state != BSP_ERROR_NONE)
      {
        printf ("\r\nSD WRITE : FAILED.");
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
        UTIL_LCD_DisplayStringAt(20, 140, (uint8_t *)"SD card write failed", LEFT_MODE);
        result --;
      }
      else
      {
        printf ("\r\nSD WRITE : OK.");
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
        UTIL_LCD_DisplayStringAt(20, 140, (uint8_t *)"SD card write OK    ", LEFT_MODE);

        SD_state = BSP_SD_ReadBlocks(0, aRxBuffer, BLOCK_START_ADDR, NUM_OF_BLOCKS);
        HAL_Delay(500);

        /* Wait until SD card is ready to use for new operation */
        while(BSP_SD_GetCardState(0) != SD_TRANSFER_OK)
        {
        }

        if (SD_state != BSP_ERROR_NONE)
        {
          printf ("\r\nSD READ : FAILED.");
          UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
          UTIL_LCD_DisplayStringAt(20, 160, (uint8_t *)"SD card read failed", LEFT_MODE);
          result --;
        }
        else
        {
          printf ("\r\nSD READ : OK.");
          UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
          UTIL_LCD_DisplayStringAt(20, 160, (uint8_t *)"SD card read OK     ", LEFT_MODE);
          if (Buffercmp(aTxBuffer, aRxBuffer, BUFFER_WORDS_SIZE) > 0)
          {
            printf ("\r\nSD COMPARE : FAILED.");
            result --;
          }
          else
          {
            printf ("\r\nSD COMPARE : OK.");
          }
          SD_state = BSP_SD_Erase(0, BLOCK_START_ADDR, (BLOCK_START_ADDR + (BLOCKSIZE * NUM_OF_BLOCKS)));
          HAL_Delay(500);

          if (SD_state != BSP_ERROR_NONE)
          {
            printf ("\r\nSD ERASE : FAILED.");
            UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
            UTIL_LCD_DisplayStringAt(20, 180, (uint8_t *)"SD card erase failed", LEFT_MODE);
            result --;
          }
          else
          {
            printf ("\r\nSD ERASE : OK.");
            UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
            UTIL_LCD_DisplayStringAt(20, 180, (uint8_t *)"SD card erase OK    ", LEFT_MODE);
          }
        }
      }
  }

  printf ("\r\nSD Test done.");
  printf ("\r\nSD can be removed.\n");
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_DisplayStringAt(20, 200, (uint8_t *)"SD card test Done     ", LEFT_MODE);

  while (1)
  {
#if (SD_CARD_PRESENCE_VALIDATION_MODE == SD_CARD_PRESENCE_INTERRUPT_MODE)
    if(SDDetectIT != 0)
    {
#endif
      /* Check if the SD card is plugged in the slot */
      if (SDDetectStatus != SD_PRESENT)
      {
        if(prev_status != SD_NOT_PRESENT)
        {
          BSP_SD_Init(0);
          printf ("\r\nSD Not Connected");
          UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
          UTIL_LCD_DisplayStringAt(20, 200, (uint8_t *)"SD card not connected", LEFT_MODE);
          prev_status = SD_NOT_PRESENT;
        }
      }
      else if (prev_status != SD_PRESENT)
      {
        printf ("\r\nSD Connected");
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
        UTIL_LCD_DisplayStringAt(20, 200, (uint8_t *)"SD card connected   ", LEFT_MODE);
        prev_status = SD_PRESENT;
      }
#if (SD_CARD_PRESENCE_VALIDATION_MODE == SD_CARD_PRESENCE_INTERRUPT_MODE)
      SDDetectIT = 0;
    }
#endif

  if (UserButtonPressed != 0)
  {
    /* Add delay to avoid rebound and reset it status */
    HAL_Delay(500);
    UserButtonPressed = 0;

    return result;
  }
 }
}

/**
  * @brief  Display SD Demo Hint
  * @param  None
  * @retval None
  */
static void SD_SetHint(void)
{
  uint32_t x_size, y_size;
  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  printf("TEST OF SD card\n");

  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Set LCD Demo description */
  BSP_LCD_FillRect(0, 0, 0, x_size, 80, UTIL_LCD_COLOR_BLUE );
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"SD", CENTER_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, 30, (uint8_t *)"This example shows how to write", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 45, (uint8_t *)"and read data on the microSD and also", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"how to detect the presence of the card", CENTER_MODE);

   /* Set the LCD Text Color */
  UTIL_LCD_DrawRect(10, 90, x_size - 20, y_size- 100, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DrawRect(11, 91, x_size - 22, y_size- 102, UTIL_LCD_COLOR_BLUE);
}

/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the buffer to fill
  * @param  uwBufferLenght: size of the buffer to fill
  * @param  uwOffset: first value to fill on the buffer
  * @retval None
  */
static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
  uint32_t tmpIndex = 0;

  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
  }
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 1: pBuffer identical to pBuffer1
  *         0: pBuffer differs from pBuffer1
  */
static uint8_t Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/**
  * @brief Tx Transfer completed callback
  * @param  Instance     SD Instance
  * @retval None
  */
void BSP_SD_WriteCpltCallback(uint32_t Instance)
{
  SDWriteStatus = 1;
}

/**
  * @brief Rx Transfer completed callback
  * @param  Instance     SD Instance
  * @retval None
  */
void BSP_SD_ReadCpltCallback(uint32_t Instance)
{
  SDReadStatus = 1;
}

/**
  * @brief  BSP SD Callback.
  * @param  Instance SD Instance
  * @param  Status   Pin status
  * @retval None.
  */
void BSP_SD_DetectCallback(uint32_t Instance, uint32_t Status)
{
  SDDetectIT = 1;
  SDDetectStatus = Status;
}

/**
  * @}
  */

/**
  * @}
  */

