/**
  ******************************************************************************
  * @file    utils.h
  * @author  MCD Application Team
  * @brief   System information functions
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




