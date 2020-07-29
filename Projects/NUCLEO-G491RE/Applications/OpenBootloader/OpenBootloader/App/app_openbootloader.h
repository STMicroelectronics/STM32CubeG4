/**
  ******************************************************************************
  * @file    app_openbootloader.h
  * @author  MCD Application Team
  * @brief   Header for app_openbootloader.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Image license SLA0044,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                       www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_OPENBOOTLOADER_H
#define APP_OPENBOOTLOADER_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void OpenBootloader_Init(void);
void OpenBootloader_DeInit(void);
void OpenBootloader_ProtocolDetection(void);

#endif /* APP_OPENBOOTLOADER_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
