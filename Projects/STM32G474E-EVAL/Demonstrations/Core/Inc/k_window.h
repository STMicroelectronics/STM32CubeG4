/**
  ******************************************************************************
  * @file    k_window.h
  * @author  MCD Application Team
  * @brief   Header for k_window.c file
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
#ifndef _K_WINDOW_H
#define _K_WINDOW_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stm32g4xx_hal.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

   /* Exported functions ------------------------------------------------------- */
void kWindow_Error(char *msg);
void kWindow_Popup(char *title, uint32_t title_tc, uint32_t title_bc ,char *Msg, uint32_t msg_tc, uint32_t msg_bc);


#ifdef __cplusplus
}
#endif

#endif /* _K_WINDOW_H */


