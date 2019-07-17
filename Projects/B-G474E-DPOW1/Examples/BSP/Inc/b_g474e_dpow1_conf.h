/**
  ******************************************************************************
  * @file    b_g474e_dpow1_conf.h
  * @author  MCD Application Team
  * @brief   B-G474E-DPOW1 board configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef B_G474E_DPOW1_CONF_H
#define B_G474E_DPOW1_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
/** @addtogroup BSP
  * @{
  */

/** @addtogroup B-G474E-DPOW1
  * @{
  */

/** @defgroup B-G474E-DPOW1_CONFIG Config
  * @{
  */

/** @defgroup B-G474E-DPOW1_CONFIG_Exported_Constants Exported Constants
  * @{
  */

/* COM define */
#define USE_BSP_COM_FEATURE                 1U

/* COM LOG define */
#define USE_COM_LOG                         0U

/* IRQ priorities */
#define BSP_JOY_SEL_IT_PRIORITY            15U

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

#endif /* B_G474E_DPOW1_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
