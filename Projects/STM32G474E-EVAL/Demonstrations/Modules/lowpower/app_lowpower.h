/**
  ******************************************************************************
  * @file    app_lowpower.c
  * @author  MCD Application Team
  * @brief   Low power application interface
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
#ifdef __APP_LOWPOWER_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Includes ------------------------------------------------------------------*/
#include "k_module.h"

/* external defines ----------------------------------------------------------*/
#define STANDBY                             0x01u
#define STOP                                0x02u
#define SLEEP                               0x03u

/* External variables --------------------------------------------------------*/
GLOBAL const K_ModuleItem_Typedef ModuleLowPower;

/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void LowPowerExitDisplay(uint32_t Mode);


