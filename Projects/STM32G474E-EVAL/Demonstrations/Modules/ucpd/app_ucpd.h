/**
  ******************************************************************************
  * @file    app_ucpd.h
  * @author  MCD Application Team
  * @brief   UCPD application interface
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
#ifdef __APP_UCPD_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Includes ------------------------------------------------------------------*/
#include "k_module.h"

/* External variables --------------------------------------------------------*/
GLOBAL const K_ModuleItem_Typedef ModuleUcpd;

GLOBAL __IO  uint8_t  UcpdDemoRunning
#ifdef __APP_UCPD_C
  = 0;
#else
;
#endif

/* Exported functions --------------------------------------------------------*/
extern void UcpdJoyCallback(JOY_TypeDef JOY, uint32_t JoyPin);

/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


