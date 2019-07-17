/**
  ******************************************************************************
  * @file    k_window.h
  * @author  MCD Application Team
  * @brief   Header for k_window.c file
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
