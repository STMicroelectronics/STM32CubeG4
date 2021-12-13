/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           USB_Device/CDC_Standalone/USB_Device/App/usbd_cdc_if.h
  * @author         MCD Application Team
  * @brief          Header for usbd_cdc_if.c file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CDC_IF_H__
#define __USBD_CDC_IF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief For Usb device.
  * @{
  */

/** @defgroup USBD_CDC_IF USBD_CDC_IF
  * @brief Usb VCP device module
  * @{
  */

/** @defgroup USBD_CDC_IF_Exported_Defines USBD_CDC_IF_Exported_Defines
  * @brief Defines.
  * @{
  */
/* Define size for the receive and transmit buffer over CDC */
#define APP_RX_DATA_SIZE  2048
#define APP_TX_DATA_SIZE  2048
/* USER CODE BEGIN EXPORTED_DEFINES */
#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define DMAx_CLK_ENABLE()                do {                                \
                                              __HAL_RCC_DMA1_CLK_ENABLE();    \
                                              __HAL_RCC_DMAMUX1_CLK_ENABLE(); \
                                         }while(0)
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                     GPIO_PIN_9
#define USARTx_TX_GPIO_PORT               GPIOA
#define USARTx_TX_AF                      GPIO_AF7_USART1
#define USARTx_RX_PIN                     GPIO_PIN_10
#define USARTx_RX_GPIO_PORT               GPIOA
#define USARTx_RX_AF                      GPIO_AF7_USART1

/* Definition for USARTx's DMA */
#define USARTx_TX_DMA_CHANNEL             DMA1_Channel1
#define USARTx_RX_DMA_CHANNEL             DMA1_Channel2

/* Definition for USARTx's DMA Request */
#define USARTx_TX_DMA_REQUEST             DMA_REQUEST_USART1_TX

/* Definition for USARTx's NVIC */
#define USARTx_DMA_TX_IRQn                DMA1_Channel1_IRQn
#define USARTx_RX_IRQn                    DMA1_Channel2_IRQn
#define USARTx_DMA_TX_IRQHandler          DMA1_Channel1_IRQHandler
#define USARTx_RX_IRQHandler              DMA1_Channel2_IRQHandler

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                       USART1_IRQn
#define USARTx_IRQHandler                 USART1_IRQHandler

/* Size of Reception buffer */
#define RXBUFFERSIZE                      10

/* Definition for TIMx clock resources */
#define TIMx                             TIM2
#define TIMx_CLK_ENABLE                  __HAL_RCC_TIM2_CLK_ENABLE
#define TIMx_FORCE_RESET()               __HAL_RCC_TIM2_FORCE_RESET()
#define TIMx_RELEASE_RESET()             __HAL_RCC_TIM2_RELEASE_RESET()

/* Definition for TIMx's NVIC */
#define TIMx_IRQn                        TIM2_IRQn
#define TIMx_IRQHandler                  TIM2_IRQHandler
/* USER CODE END EXPORTED_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Types USBD_CDC_IF_Exported_Types
  * @brief Types.
  * @{
  */

/* USER CODE BEGIN EXPORTED_TYPES */

/* USER CODE END EXPORTED_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Macros USBD_CDC_IF_Exported_Macros
  * @brief Aliases.
  * @{
  */

/* USER CODE BEGIN EXPORTED_MACRO */

/* USER CODE END EXPORTED_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

/** CDC Interface callback. */
extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_FunctionsPrototype USBD_CDC_IF_Exported_FunctionsPrototype
  * @brief Public functions declaration.
  * @{
  */

uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);

/* USER CODE BEGIN EXPORTED_FUNCTIONS */

/* USER CODE END EXPORTED_FUNCTIONS */

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

#endif /* __USBD_CDC_IF_H__ */

