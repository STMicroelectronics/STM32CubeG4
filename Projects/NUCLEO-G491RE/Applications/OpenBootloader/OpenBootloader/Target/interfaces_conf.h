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

/* ------------------------- Definitions for I2C -------------------------- */
#define I2Cx                              I2C2

#define I2Cx_SCL_PIN                      GPIO_PIN_4
#define I2Cx_SCL_PIN_PORT                 GPIOC
#define I2Cx_SDA_PIN                      GPIO_PIN_8
#define I2Cx_SDA_PIN_PORT                 GPIOA

#define I2C_ADDRESS                       (0x0000005FU << 0x01U)
#define OPENBL_I2C_TIMEOUT                0x011A91F8U
#define I2C_TIMING                        0x00800000U

#endif /* INTERFACES_CONF_H */


