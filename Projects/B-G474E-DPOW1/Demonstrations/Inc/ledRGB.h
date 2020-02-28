/**
  ******************************************************************************
  * @file    ledRGB.h
  * @author  MCD Application Team
  * @brief   Header for ledRGB.c file
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
