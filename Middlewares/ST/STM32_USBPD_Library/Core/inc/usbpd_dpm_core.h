/**
  ******************************************************************************
  * @file    usbpd_dpm_core.h
  * @author  MCD Application Team
  * @brief   Header file for usbpd_dpm_core.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#ifdef _RTOS
#if defined(USBPD_TCPM_MODULE_ENABLED)
#define TCPM_ALARMBOX_MESSAGES_MAX      (3U * USBPD_PORT_COUNT)
#endif /* USBPD_TCPM_MODULE_ENABLED */
#endif /* _RTOS */
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
#if !defined(USBPDCORE_LIB_NO_PD)
void                USBPD_DPM_TimerCounter(void);
#endif /* USBPDCORE_LIB_NO_PD */
/* USER CODE BEGIN functions */

/* USER CODE END functions */
#ifdef __cplusplus
}
#endif

#endif /* __USBPD_DPM_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
