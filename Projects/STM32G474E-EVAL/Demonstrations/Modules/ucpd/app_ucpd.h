/**
  ******************************************************************************
  * @file    app_ucpd.h
  * @author  MCD Application Team
  * @brief   UCPD application interface
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
