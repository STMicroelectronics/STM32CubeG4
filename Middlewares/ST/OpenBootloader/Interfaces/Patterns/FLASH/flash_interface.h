/**
  ******************************************************************************
  * @file    flash_interface.h
  * @author  MCD Application Team
  * @brief   Header for flash_interface.c module
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
#ifndef FLASH_INTERFACE_H
#define FLASH_INTERFACE_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void OPENBL_FLASH_JumpToAddress(uint32_t Address);
void OPENBL_FLASH_Lock(void);
void OPENBL_FLASH_OB_Unlock(void);
void OPENBL_FLASH_OB_Launch(void);
uint8_t OPENBL_FLASH_Read(uint32_t Address);
void OPENBL_FLASH_SetReadOutProtectionLevel(uint32_t Level);
void OPENBL_FLASH_Write(uint32_t Address, uint8_t *Data, uint32_t DataLength);
void OPENBL_FLASH_Unlock(void);
ErrorStatus OPENBL_FLASH_MassErase(uint8_t *p_Data, uint32_t DataLength);
ErrorStatus OPENBL_FLASH_Erase(uint8_t *p_Data, uint32_t DataLength);
ErrorStatus OPENBL_FLASH_SetWriteProtection(FunctionalState State, uint8_t *ListOfPages, uint32_t Length);

uint32_t OPENBL_FLASH_GetReadOutProtectionLevel(void);

#endif /* FLASH_INTERFACE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
