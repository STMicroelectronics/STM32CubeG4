/**
  ******************************************************************************
  * @file    app_calendar.h
  * @author  MCD Application Team
  * @brief   Calendar application interface
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
#ifdef __APP_CALENDAR_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Includes ------------------------------------------------------------------*/
#include "k_module.h"

/* External constants --------------------------------------------------------*/
/* RTC divider to get 1s time base */
#define RTC_ASYNCH_PREDIV   0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV    0x00FF /* LSE as RTC clock */

/* External variables --------------------------------------------------------*/
GLOBAL const K_ModuleItem_Typedef ModuleCalendar;

/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


