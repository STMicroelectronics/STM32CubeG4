/**
  ******************************************************************************
  * @file    stm32g474e_eval_conf.h
  * @author  MCD Application Team
  * @brief   STM32G474E-EVAL board configuration file.
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
#ifndef STM32G474E_EVAL_CONF_H
#define STM32G474E_EVAL_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @defgroup STM32G474E-EVAL_CONFIG Config
  * @{
  */

/** @defgroup STM32G474E-EVAL_CONFIG_Exported_Constants Exported Constants
  * @{
  */

/* COM define */
#define USE_BSP_COM_FEATURE                 0U

/* COM LOG define */
#define USE_COM_LOG                         0U

/* POT define */
#define USE_BSP_POT_FEATURE                 0U

/* COMP define :
   depends on SB8 and SB10 configuration : refer to UM */
#define USE_BSP_POT_COMP_FEATURE            0U

/* IO Expander define */
#define USE_BSP_IO_CLASS                    1U

/* JOY define */
#define USE_BSP_JOY_FEATURE                 1U

/* IRQ priorities */
#define BSP_SRAM_IT_PRIORITY                15U
#define BSP_IOEXPANDER_IT_PRIORITY          14U
#define BSP_BUTTON_USER_IT_PRIORITY         15U
#define BSP_AUDIO_OUT_IT_PRIORITY           13U
#define BSP_AUDIO_IN_IT_PRIORITY            12U

/* Audio codecs defines */
#define USE_AUDIO_CODEC_WM8994              1U

/* Default Audio IN internal buffer size */
#define DEFAULT_AUDIO_IN_BUFFER_SIZE        2048U

/* I2C3 Frequency in Hz  */
#define BUS_I2C3_FREQUENCY                  100000U   /* Frequency of I2C3 = 100 kHz*/

/* SPI2 Baud rate in bps  */
#define BUS_SPI2_BAUDRATE                   12500000U /* baud rate of SPIn = 12.5 Mbps */
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

#endif /* STM32G474E_EVAL_CONF_H */


