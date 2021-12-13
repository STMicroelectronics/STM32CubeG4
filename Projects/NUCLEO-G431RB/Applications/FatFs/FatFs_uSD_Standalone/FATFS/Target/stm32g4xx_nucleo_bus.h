/**
  ******************************************************************************
  * @file    stm32g4xx_nucleo_bus.h
  * @author  MCD Application Team
  * @brief   This file is the header of stm32g4xx_nucleo_bus.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32G431RB_NUCLEO_BUS_H
#define STM32G431RB_NUCLEO_BUS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_nucleo_conf.h"
#include "stm32g4xx_nucleo_errno.h"

/** @addtogroup BSP
  * @{
  */

/** @defgroup STM32G431RB_NUCLEO STM32G431RB_NUCLEO
  * @{
  */

/** @addtogroup STM32G431RB_NUCLEO_BUS STM32G431RB_NUCLEO BUS
  * @{
  */

/**
  * @brief  SPI Interface pins
  *         used to communicate with LCD and SD available on Adafruit 1.8" TFT shield
  */
#define NUCLEO_SPI                                     SPI1
#define NUCLEO_SPI_CLK_ENABLE()                        __HAL_RCC_SPI1_CLK_ENABLE()

#define NUCLEO_SPI_SCK_AF                              GPIO_AF5_SPI1
#define NUCLEO_SPI_SCK_GPIO_PORT                       GPIOA
#define NUCLEO_SPI_SCK_PIN                             GPIO_PIN_5
#define NUCLEO_SPI_SCK_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
#define NUCLEO_SPI_SCK_GPIO_CLK_DISABLE()              __HAL_RCC_GPIOA_CLK_DISABLE()

#define NUCLEO_SPI_MISO_AF                             GPIO_AF5_SPI1
#define NUCLEO_SPI_MISO_GPIO_PORT                      GPIOA
#define NUCLEO_SPI_MISO_PIN                            GPIO_PIN_6
#define NUCLEO_SPI_MISO_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define NUCLEO_SPI_MISO_GPIO_CLK_DISABLE()             __HAL_RCC_GPIOA_CLK_DISABLE()

#define NUCLEO_SPI_MOSI_AF                             GPIO_AF5_SPI1
#define NUCLEO_SPI_MOSI_GPIO_PORT                      GPIOA
#define NUCLEO_SPI_MOSI_PIN                            GPIO_PIN_7
#define NUCLEO_SPI_MOSI_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define NUCLEO_SPI_MOSI_GPIO_CLK_DISABLE()             __HAL_RCC_GPIOA_CLK_DISABLE()

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define NUCLEO_SPI_TIMEOUT_MAX                         1000U
#define NUCLEO_SPI_BAUDRATEPRESCALER                   SPI_BAUDRATEPRESCALER_8


/** @defgroup STM32G431RB_NUCLEO_BUS_Exported_Types BUS Exported Types
  * @{
  */

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
typedef struct
{
  pSPI_CallbackTypeDef  pMspSpiInitCb;
  pSPI_CallbackTypeDef  pMspSpiDeInitCb;
}BSP_SPI1_Cb_t;
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 1) */
/**
  * @}
  */


/** @defgroup STM32G431RB_NUCLEO_BUS_Exported_Functions BUS Exported Functions
  * @{
  */
int32_t BSP_SPI1_Init(void);
int32_t BSP_SPI1_DeInit(void);
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
int32_t BSP_SPI1_RegisterMspCallbacks(BSP_SPI1_Cb_t *CallBacks);
int32_t BSP_SPI1_RegisterDefaultMspCallbacks(void);
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 1) */
int32_t BSP_SPI1_Send(uint8_t *pTxData, uint32_t Length);
int32_t BSP_SPI1_Recv(uint8_t *pRxData, uint32_t Length);
int32_t BSP_SPI1_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint32_t Length);
HAL_StatusTypeDef MX_BSP_SPI1_Init(SPI_HandleTypeDef *hspi);
int32_t BSP_GetTick(void);
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

#ifdef __cplusplus
}
#endif

#endif /* STM32G431RB_NUCLEO_BUS_H  */


