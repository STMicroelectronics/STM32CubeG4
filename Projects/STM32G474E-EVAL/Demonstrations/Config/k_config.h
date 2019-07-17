/**
  ******************************************************************************
  * @file    k_config.h
  * @author  MCD Application Team
  * @brief   Header for k_config.c file
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
#ifndef K_CONFIG_H
#define K_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
#define EVAL_BOARD   "STM32G474E-EVAL"
#define DEMO_VERSION "Version 1.0"
#define DEMO_DATE    "21 - Dec - 2018"

#define DEMO_INFO1   "MCD Application Team"
#define DEMO_INFO2   "COPYRIGHT 2018"
#define DEMO_INFO3   "STMicroelectronics"

#define CHOOSEFILE_MAXLEN   255

typedef enum{
 MODULE_MAIN_APP,
 MODULE_LOWPOWER,
 MODULE_TSENSOR,
 MODULE_VIEWBMP,
 MODULE_FILESBRO,
 MODULE_APPLICATIONS,
 MODULE_AUDIO,
 MODULE_CALENDAR,
 MODULE_WHITELED,
 MODULE_MATH,
 MODULE_UCPD,
 MODULE_MAX,
 MODULE_NONE = 0xFF
} MODULES_INFO;

#define countof(a) (sizeof(a) / sizeof(*(a)))
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* K_LOG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
