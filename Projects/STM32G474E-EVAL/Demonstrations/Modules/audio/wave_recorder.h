/**
  ******************************************************************************
  * @file    wave_recorder.h
  * @author  MCD Application Team
  * @brief   WAVE recorder interface
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
#ifndef __WAVE_RECORDER_H
#define __WAVE_RECORDER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
  
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void WaveRecorder_Demo(void);
void WaveRecorder_UserAction(uint8_t sel);

#ifdef __cplusplus
}
#endif

#endif /* __WAVE_RECORDER_H */


