/**
  ******************************************************************************
  * @file    utils.h
  * @author  MCD Application Team
  * @brief   System information functions
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
#ifndef __UTILS_H
#define __UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

  /* Includes ----------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void      UTILS_SetImageId(uint32_t ImageId);
uint32_t  UTILS_GetImageId(void);
#ifdef __cplusplus
}
#endif

#endif /* __UTILS_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

