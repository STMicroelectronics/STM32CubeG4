/**
  ******************************************************************************
  * @file    app_whiteled.h
  * @author  MCD Application Team
  * @brief   White led application interface
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
#undef GLOBAL
#ifdef __APP_WHITELED_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Includes ------------------------------------------------------------------*/
#include "k_module.h"

/* External constants --------------------------------------------------------*/

#define TRUE 0x01
#define FALSE 0x00

/* External variables --------------------------------------------------------*/
/* HRTIM handle declaration */
GLOBAL HRTIM_HandleTypeDef HrtimHandle;

GLOBAL const K_ModuleItem_Typedef ModuleWhiteLed;

/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
GLOBAL void HAL_HRTIM_BurstModePeriodCallback(HRTIM_HandleTypeDef * hhrtim);

