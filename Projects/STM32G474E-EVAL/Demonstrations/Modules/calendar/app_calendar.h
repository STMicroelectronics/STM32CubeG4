/**
  ******************************************************************************
  * @file    app_calendar.h
  * @author  MCD Application Team
  * @brief   Calendar application interface
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
