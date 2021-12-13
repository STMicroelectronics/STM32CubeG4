/**
  ******************************************************************************
  * @file    k_widgets.h
  * @author  MCD Application Team
  * @brief   Widgets interface
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


