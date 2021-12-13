/**
  ******************************************************************************
  * @file    ledRGB.h
  * @author  MCD Application Team
  * @brief   Header for ledRGB.c file
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
#ifdef __LEDCOLOR_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LEDCOLOR_H
#define __LEDCOLOR_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private Variable ----------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
GLOBAL void Demo_LedColor(void);
GLOBAL void Demo_LedWhite(void);
#ifdef __cplusplus
}
#endif

#endif /* __LEDCOLOR_H */


