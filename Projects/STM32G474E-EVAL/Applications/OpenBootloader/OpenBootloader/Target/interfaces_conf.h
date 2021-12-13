/**
  ******************************************************************************
  * @file    interfaces_conf.h
  * @author  MCD Application Team
  * @brief   Contains Interfaces configuration
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
#ifndef INTERFACES_CONF_H
#define INTERFACES_CONF_H

#define MEMORIES_SUPPORTED                7U

/* ------------------------- Definitions for USART -------------------------- */
#define USARTx                            USART1

#define USARTx_TX_PIN                     GPIO_PIN_9
#define USARTx_TX_GPIO_PORT               GPIOA
#define USARTx_RX_PIN                     GPIO_PIN_10
#define USARTx_RX_GPIO_PORT               GPIOA
#define USARTx_ALTERNATE                  GPIO_AF7_USART1

/* ------------------------- Definitions for FDCAN -------------------------- */
#define FDCANx                            FDCAN1

#define FDCANx_TX_PIN                     GPIO_PIN_9
#define FDCANx_TX_GPIO_PORT               GPIOB
#define FDCANx_TX_AF                      GPIO_AF9_FDCAN1
#define FDCANx_RX_PIN                     GPIO_PIN_8
#define FDCANx_RX_GPIO_PORT               GPIOB
#define FDCANx_RX_AF                      GPIO_AF9_FDCAN1

#define FDCANx_IT0_IRQn                   FDCAN1_IT0_IRQn
#define FDCANx_IT1_IRQn                   FDCAN1_IT1_IRQn
#define FDCANx_IRQHandler                 FDCAN1_IRQHandler

#define FDCANx_FORCE_RESET()              __HAL_RCC_FDCAN_FORCE_RESET()
#define FDCANx_RELEASE_RESET()            __HAL_RCC_FDCAN_RELEASE_RESET()

#endif /* INTERFACES_CONF_H */


