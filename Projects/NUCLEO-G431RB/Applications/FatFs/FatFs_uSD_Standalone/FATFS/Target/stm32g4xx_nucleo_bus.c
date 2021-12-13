/**
  ******************************************************************************
  * @file    stm32g4xx_nucleo_bus.c
  * @author  MCD Application Team
  * @brief   This file provides set of Bus IO functions to manage:
  *          - LCD and microSD available on Adafruit 1.8" TFT LCD
  *            shield (reference ID 802)
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
#include "stm32g4xx_nucleo_bus.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup ADAFRUIT_802
  * @{
  */

/** @addtogroup ADAFRUIT_802_BUS
  * @brief This file provides set of firmware functions to manage the bus operations
  *        for adafruit shield
  * @{
  */

/** @defgroup ADAFRUIT_802_BUS_Exported_Variables BUS Exported Variables
  * @{
  */
SPI_HandleTypeDef hSpiHandle;
/**
  * @}
  */

/** @defgroup ADAFRUIT_802_BUS_Private_Variables BUS Private Variables
  * @{
  */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
static uint32_t IsSpiMspCbValid = 0;
#endif
/**
  * @}
  */

/** @defgroup ADAFRUIT_802_BUS_Private_FunctionsPrototypes BUS Private Functions Prototypes
  * @{
  */

static void SPI_MspInit(SPI_HandleTypeDef *hspi);
static void SPI_MspDeInit(SPI_HandleTypeDef *hspi);
/**
  * @}
  */

/** @addtogroup ADAFRUIT_802_BUS_Private_Functions
  * @{
  */

/**
  * @brief  Initializes SPI HAL.
  * @retval None
  */
int32_t BSP_SPI1_Init(void)
{
  if(HAL_SPI_GetState(&hSpiHandle) == HAL_SPI_STATE_RESET)
  {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
    /* Init the SPI Msp */
    SPI_MspInit(&hSpiHandle);
#else
    if(IsSpiMspCbValid == 0U)
    {
      if(BSP_SPI1_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
      {
        return BSP_ERROR_MSP_FAILURE;
      }
    }
#endif

    if(MX_BSP_SPI1_Init(&hSpiHandle) != HAL_OK)
    {
      return BSP_ERROR_BUS_FAILURE;
    }
  }
  return BSP_ERROR_NONE;
}

/**
  * @brief  DeInitializes SPI HAL.
  * @retval None
  */
int32_t BSP_SPI1_DeInit(void)
{
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
  SPI_MspDeInit(&hSpiHandle);
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 0) */

  /* Init the SPI */
  if (HAL_SPI_DeInit(&hSpiHandle) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
  * @brief  MX SPI initialization.
  * @param  hspi SPI handle
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_BSP_SPI1_Init(SPI_HandleTypeDef *hspi)
{
  /* SPI Config */
  hspi->Instance               = NUCLEO_SPI;
  hspi->Init.BaudRatePrescaler = NUCLEO_SPI_BAUDRATEPRESCALER;
  hspi->Init.Direction         = SPI_DIRECTION_2LINES;
  hspi->Init.CLKPhase          = SPI_PHASE_2EDGE;
  hspi->Init.CLKPolarity       = SPI_POLARITY_HIGH;
  hspi->Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  hspi->Init.CRCPolynomial     = 7;
  hspi->Init.DataSize          = SPI_DATASIZE_8BIT;
  hspi->Init.FirstBit          = SPI_FIRSTBIT_MSB;
  hspi->Init.NSS               = SPI_NSS_SOFT;
  hspi->Init.TIMode            = SPI_TIMODE_DISABLE;
  hspi->Init.Mode              = SPI_MODE_MASTER;

  return HAL_SPI_Init(hspi);
}

/**
  * @brief  Send a byte to device
  * @param  pTxData pointer to TX buffer
  * @param  Length data length
  * @retval BSP status
  */
int32_t BSP_SPI1_Send(uint8_t *pTxData, uint32_t Length)
{
  int32_t ret = BSP_ERROR_NONE;

  if(HAL_SPI_Transmit(&hSpiHandle, pTxData, (uint16_t)Length, NUCLEO_SPI_TIMEOUT_MAX) != HAL_OK)
  {
    ret = BSP_ERROR_BUS_FAILURE;
  }

  return ret;
}

/**
  * @brief  Send a byte to device
  * @param  pRxData pointer to RX buffer
  * @param  Length data length
  * @retval BSP status
  */
int32_t BSP_SPI1_Recv(uint8_t *pRxData, uint32_t Length)
{
  int32_t ret = BSP_ERROR_NONE;

  if(HAL_SPI_Receive(&hSpiHandle, pRxData, (uint16_t)Length, NUCLEO_SPI_TIMEOUT_MAX) != HAL_OK)
  {
    ret = BSP_ERROR_BUS_FAILURE;
  }

  
  return ret;
}

/**
  * @brief  Send/Receive an amount of bytes
  * @param  pTxData pointer to TX buffer
  * @param  pRxData pointer to RX buffer
  * @param  Length data length
  * @retval BSP status
  */
int32_t BSP_SPI1_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint32_t Legnth)
{
  int32_t ret = BSP_ERROR_NONE;

  if(HAL_SPI_TransmitReceive(&hSpiHandle, pTxData, pRxData, (uint16_t)Legnth, NUCLEO_SPI_TIMEOUT_MAX) != HAL_OK)
  {
    ret = BSP_ERROR_BUS_FAILURE;
  }

  return ret;
}

/**
  * @brief  Delay function
  * @retval Tick value
  */
int32_t BSP_GetTick(void)
{
  return (int32_t)HAL_GetTick();
}

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
/**
  * @brief Register Default SPI Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_SPI1_RegisterDefaultMspCallbacks (void)
{
  int32_t ret = BSP_ERROR_NONE;

  __HAL_SPI_RESET_HANDLE_STATE(&hSpiHandle);

  /* Register default MspInit/MspDeInit Callback */
  if(HAL_SPI_RegisterCallback(&hSpiHandle, HAL_SPI_MSPINIT_CB_ID, SPI_MspInit) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else if(HAL_SPI_RegisterCallback(&hSpiHandle, HAL_SPI_MSPDEINIT_CB_ID, SPI_MspDeInit) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    IsSpiMspCbValid = 1U;
  }

  /* BSP status */
  return ret;
}

/**
  * @brief Register SPI Bus Msp Callback registering
  * @param Callbacks     pointer to SPI MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_SPI1_RegisterMspCallbacks (BSP_SPI1_Cb_t *Callback)
{
  int32_t ret = BSP_ERROR_NONE;

  __HAL_SPI_RESET_HANDLE_STATE(&hSpiHandle);

  /* Register MspInit/MspDeInit Callbacks */
  if(HAL_SPI_RegisterCallback(&hSpiHandle, HAL_SPI_MSPINIT_CB_ID, Callback->pMspSpiInitCb) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else if(HAL_SPI_RegisterCallback(&hSpiHandle, HAL_SPI_MSPDEINIT_CB_ID, Callback->pMspSpiDeInitCb) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    IsSpiMspCbValid = 1U;
  }

  /* BSP status */
  return ret;
}
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */

/** @addtogroup ADAFRUIT_802_BUS_Private_Functions BUS Private Functions
  * @{
  */

/**
  * @brief  Initializes SPI MSP.
  * @param  hspi SPI handler
  * @retval None
  */
static void SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*** Configure the SPI peripheral ***/
  /* Enable SPI clock */
  NUCLEO_SPI_CLK_ENABLE();

  /*** Configure the GPIOs ***/
  /* Enable GPIO clock */
  NUCLEO_SPI_SCK_GPIO_CLK_ENABLE();
  NUCLEO_SPI_MOSI_GPIO_CLK_ENABLE();
  NUCLEO_SPI_MISO_GPIO_CLK_ENABLE();

  /* Configure SPI SCK */
  GPIO_InitStruct.Pin       = NUCLEO_SPI_SCK_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = NUCLEO_SPI_SCK_AF;
  HAL_GPIO_Init(NUCLEO_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);

  /* Configure SPI MOSI */
  GPIO_InitStruct.Pin       = NUCLEO_SPI_MOSI_PIN;
  GPIO_InitStruct.Alternate = NUCLEO_SPI_MOSI_AF;
  GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
  HAL_GPIO_Init(NUCLEO_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);

  /* Configure SPI MISO */
  GPIO_InitStruct.Pin = NUCLEO_SPI_MISO_PIN;
  GPIO_InitStruct.Alternate = NUCLEO_SPI_MISO_AF;
  HAL_GPIO_Init(NUCLEO_SPI_MISO_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  DeInitializes SPI MSP.
  * @param  hspi SPI handler
  * @retval None
  */
static void SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);

  /* Configure SPI SCK, MOSI and MISO pins */
  gpio_init_structure.Pin = NUCLEO_SPI_SCK_PIN;
  HAL_GPIO_DeInit(NUCLEO_SPI_SCK_GPIO_PORT, gpio_init_structure.Pin);

  gpio_init_structure.Pin = NUCLEO_SPI_MOSI_PIN;
  HAL_GPIO_DeInit(NUCLEO_SPI_MOSI_GPIO_PORT, gpio_init_structure.Pin);

  gpio_init_structure.Pin = NUCLEO_SPI_MISO_PIN;
  HAL_GPIO_DeInit(NUCLEO_SPI_MISO_GPIO_PORT, gpio_init_structure.Pin);

  /* Disable SPI clock */
  NUCLEO_SPI_SCK_GPIO_CLK_DISABLE();
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


