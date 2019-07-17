/**
  ******************************************************************************
  * @file    app_whiteled.h
  * @author  MCD Application Team
  * @brief   White led application interface
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
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
