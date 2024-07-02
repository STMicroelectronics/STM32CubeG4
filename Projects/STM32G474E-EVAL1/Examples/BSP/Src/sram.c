/**
  ******************************************************************************
  * @file    BSP/Src/sram.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the SRAM Driver in the 
  *          STM32G474E EVAL1 driver
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
#define BUFFER_SIZE         ((uint32_t)0x1000)
#define WRITE_READ_ADDR     ((uint32_t)0x0800)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t sram_aTxBuffer[BUFFER_SIZE];
uint16_t sram_aRxBuffer[BUFFER_SIZE];
uint8_t ubSramWrite = 0, ubSramRead = 0, ubSramInit = 0;

/* Private function prototypes -----------------------------------------------*/
static void SRAM_SetHint(void);
static void SRAM_polling(void);
static void SRAM_DMA(void);
static void Fill_Buffer(uint16_t *pBuffer, uint32_t uwBufferLength, uint32_t uwOffset);
static uint8_t Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);

/* Private functions ---------------------------------------------------------*/

/**
* @brief  SRAM tests
* @param  None
* @retval None
*/
int32_t SRAM_demo(void)
{
  SRAM_polling();
  SRAM_DMA();
  return 0;
}

/**
  * @brief  SRAM polling access
  * @param  None
  * @retval None
  */
void SRAM_polling(void)
{
  SRAM_SetHint();

  /*##-1- Configure the SRAM device ##########################################*/
  /* SRAM device configuration */
  if(BSP_SRAM_Init(0) != BSP_ERROR_NONE)
  {
    ubSramInit++;
  }

  /*##-2- SRAM memory read/write access ######################################*/
  /* Fill the buffer to write */
  Fill_Buffer(sram_aTxBuffer, BUFFER_SIZE, 0xC20F);

  /* Write data to the SRAM memory */
  if(HAL_SRAM_Write_16b(&hsram, (uint32_t *)(SRAM_DEVICE_ADDR + WRITE_READ_ADDR), sram_aTxBuffer, BUFFER_SIZE) != BSP_ERROR_NONE)
  {
    ubSramWrite++;
  }

  /* Read back data from the SRAM memory */
  if(HAL_SRAM_Read_16b(&hsram, (uint32_t *)(SRAM_DEVICE_ADDR + WRITE_READ_ADDR), sram_aRxBuffer, BUFFER_SIZE) != BSP_ERROR_NONE)
  {
    ubSramRead++;
  }

  BSP_SRAM_DeInit(0);

  /*##-3- Checking data integrity ############################################*/
  if(ubSramInit != 0)
  {
    UTIL_LCD_DisplayStringAt(20, 100, (uint8_t *)"SRAM Initialization : FAILED.", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(20, 100, (uint8_t *)"SRAM Initialization : OK.", LEFT_MODE);
  }
  if(ubSramWrite != 0)
  {
    UTIL_LCD_DisplayStringAt(20, 115, (uint8_t *)"SRAM WRITE : FAILED.", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(20, 115, (uint8_t *)"SRAM WRITE : OK.", LEFT_MODE);
  }
  if(ubSramRead != 0)
  {
    UTIL_LCD_DisplayStringAt(20, 130, (uint8_t *)"SRAM READ : FAILED.", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(20, 130, (uint8_t *)"SRAM READ : OK.", LEFT_MODE);
  }

  if(Buffercmp(sram_aRxBuffer, sram_aTxBuffer, BUFFER_SIZE) > 0)
  {
    UTIL_LCD_DisplayStringAt(20, 145, (uint8_t *)"SRAM COMPARE : FAILED.", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(20, 145, (uint8_t *)"SRAM Test : OK.", LEFT_MODE);
  }

  while (1)
  {
    if(CheckForUserInput() > 0)
    {
      ButtonState = 0;
      return;
    }
  }
}

/**
  * @brief  SRAM DMA
  * @param  None
  * @retval None
  */
void SRAM_DMA(void)
{
  SRAM_SetHint();

  /*##-1- Configure the SRAM device ##########################################*/
  /* SRAM device configuration */
  if(BSP_SRAM_Init(0) != BSP_ERROR_NONE)
  {
    ubSramInit++;
  }

  /*##-2- SRAM memory read/write access ######################################*/
  /* Fill the buffer to write */
  Fill_Buffer(sram_aTxBuffer, BUFFER_SIZE, 0xC20F);

  /* Write data to the SRAM memory */
  if(HAL_SRAM_Write_DMA(&hsram, (uint32_t *)(SRAM_DEVICE_ADDR + WRITE_READ_ADDR), (uint32_t *)sram_aTxBuffer, BUFFER_SIZE) != BSP_ERROR_NONE)
  {
    ubSramWrite++;
  }

  /* Wait SRAM controller is READY */
  while (HAL_SRAM_GetState(&hsram) != HAL_SRAM_STATE_READY) {}

  /* Read back data from the SRAM memory */
  if(HAL_SRAM_Read_DMA(&hsram, (uint32_t *)(SRAM_DEVICE_ADDR + WRITE_READ_ADDR), (uint32_t *)sram_aRxBuffer, BUFFER_SIZE) != BSP_ERROR_NONE)
  {
    ubSramRead++;
  }

  BSP_SRAM_DeInit(0);

  /*##-3- Checking data integrity ############################################*/
  if(ubSramInit != 0)
  {
    UTIL_LCD_DisplayStringAt(20, 100, (uint8_t *)"SRAM Initialization (DMA): FAILED.", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(20, 100, (uint8_t *)"SRAM Initialization (DMA): OK.", LEFT_MODE);
  }
  if(ubSramWrite != 0)
  {
    UTIL_LCD_DisplayStringAt(20, 115, (uint8_t *)"SRAM WRITE (DMA): FAILED.", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(20, 115, (uint8_t *)"SRAM WRITE (DMA): OK.", LEFT_MODE);
  }
  if(ubSramRead != 0)
  {
    UTIL_LCD_DisplayStringAt(20, 130, (uint8_t *)"SRAM READ (DMA): FAILED.", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(20, 130, (uint8_t *)"SRAM READ (DMA): OK.", LEFT_MODE);
  }

  if(Buffercmp(sram_aRxBuffer, sram_aTxBuffer, BUFFER_SIZE) > 0)
  {
    UTIL_LCD_DisplayStringAt(20, 145, (uint8_t *)"SRAM COMPARE (DMA): FAILED.", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(20, 145, (uint8_t *)"SRAM Test (DMA): OK.", LEFT_MODE);
  }

  while (1)
  {
    if(CheckForUserInput() > 0)
    {
      ButtonState = 0;
      return;
    }
  }
}

/**
  * @brief  Display SRAM Demo Hint
  * @param  None
  * @retval None
  */
static void SRAM_SetHint(void)
{
  uint32_t x_size, y_size;

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Set LCD Demo description */
  UTIL_LCD_FillRect(0, 0, x_size, 80, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"SRAM", CENTER_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, 30, (uint8_t *)"This example tests", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 45, (uint8_t *)"read/write access on SRAM", CENTER_MODE);

   /* Set the LCD Text Color */
  UTIL_LCD_DrawRect(10, 90, x_size - 20, y_size- 100, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DrawRect(11, 91, x_size - 22, y_size- 102, UTIL_LCD_COLOR_BLUE);

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
}

/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the buffer to fill
  * @param  uwBufferLength: size of the buffer to fill
  * @param  uwOffset: first value to fill on the buffer
  * @retval None
  */
static void Fill_Buffer(uint16_t *pBuffer, uint32_t uwBufferLength, uint32_t uwOffset)
{
  uint32_t tmpindex = 0;

  /* Put in global buffer different values */
  for (tmpindex = 0; tmpindex < uwBufferLength; tmpindex++ )
  {
    pBuffer[tmpindex] = tmpindex + uwOffset;
  }
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 1: pBuffer identical to pBuffer1
  *         0: pBuffer differs from pBuffer1
  */
static uint8_t Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength)
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
  * @}
  */

/**
  * @}
  */

