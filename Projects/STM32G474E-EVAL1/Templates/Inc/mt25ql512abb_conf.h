/**
  ******************************************************************************
  * @file    mt25ql512abb_conf.h
  * @author  MCD Application Team
  * @brief   This file contains all the description of the MT25QL512ABB QSPI memory.
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
#ifndef MT25QL512ABB_CONF_H
#define MT25QL512ABB_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx.h"
#include "stm32g4xx_hal.h"

/** @addtogroup BSP
  * @{
  */
#define CONF_QSPI_ODS             MT25QL512ABB_EVCR_ODS_30   /* MT25QL512ABB Output Driver Strength         */


#ifdef __cplusplus
}
#endif

#endif /* MT25QL512ABB_CONF_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


