/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_dpm_core.h
  * @author  MCD Application Team
  * @brief   Header file for usbpd_dpm_core.c file
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
/* USER CODE END Header */

#ifndef __USBPD_DPM_CORE_H_
#define __USBPD_DPM_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported typedef ----------------------------------------------------------*/
/* USER CODE BEGIN typedef */

/* USER CODE END typedef */

/* Exported define -----------------------------------------------------------*/

/* USER CODE BEGIN define */

/* USER CODE END define */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN constants */

/* USER CODE END constants */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN macro */

/* USER CODE END macro */

/* Exported variables --------------------------------------------------------*/
extern USBPD_ParamsTypeDef DPM_Params[USBPD_PORT_COUNT];
/* USER CODE BEGIN variables */

/* USER CODE END variables */

/* Exported functions --------------------------------------------------------*/
USBPD_StatusTypeDef USBPD_DPM_InitCore(void);
USBPD_StatusTypeDef USBPD_DPM_InitOS(void);
void                USBPD_DPM_Run(void);
void                USBPD_DPM_TimerCounter(void);
__WEAK void         USBPD_DPM_ErrorHandler(void);
/* USER CODE BEGIN functions */

/* USER CODE END functions */

#ifdef __cplusplus
}
#endif

#endif /* __USBPD_DPM_CORE_H_ */
