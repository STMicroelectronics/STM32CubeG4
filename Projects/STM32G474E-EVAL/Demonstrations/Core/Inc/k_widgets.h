/**
  ******************************************************************************
  * @file    k_widgets.h
  * @author  MCD Application Team
  * @brief   Widgets interface
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
#ifndef _K_WIDGETS_H
#define _K_WIDGETS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/* Progress Bar descriptor */
typedef struct {
  uint32_t textColor;
  uint32_t backgroungColor;
  uint16_t xpos;
  uint16_t ypos;
  uint16_t width;
  uint16_t height;
} kWidgetsProgressBar_t;

/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void * kWidgets_ProgressBarCreate(kWidgetsProgressBar_t * pProgressBar, uint32_t MaxValue);
void kWidgets_ProgressBarDestroy(void * hwidgetProgressBar);
void kWidgets_ProgressBarUpdate(void * hwidgetProgressBar, uint32_t Progress);
void kWidgets_ProgressBarReset(void * hwidgetProgressBar);

#ifdef __cplusplus
}
#endif

#endif /* _K_MEM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
