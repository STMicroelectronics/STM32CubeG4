/**
  ******************************************************************************
  * @file    usbpd_gui_memmap_template.h
  * @author  MCD Application Team
  * @brief   This file contains memory mapping configuration to be able to run
  *          Cube-Monitor-UCPD on embedded side.
  *          This file should be copied to the application folder and renamed
  *          to usbpd_gui_memmap.h. Definitions should be updated according to
  *          application configuration.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef __USBPD_GUI_MEMMAP_H_
#define __USBPD_GUI_MEMMAP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32xxxx.h"
#include "usbpd_def.h"

/** @addtogroup STM32_USBPD_APPLICATION
  * @{
  */

/** @addtogroup STM32_USBPD_APPLICATION_GUI
  * @{
  */

/* Exported typedef ----------------------------------------------------------*/

/* Exported define -----------------------------------------------------------*/

/*
  TO BE USED FOR SERIES USING SECTORS (LIKE F4...)
  Following example is based on STM32F401xE
*/

/* Following definitions should be adapted to used Flash configuration :
   FLASH_SECTOR_ID : represents the ID of the sector used for storing USBPD settings (usually the last page)
   ADDR_FLASH_LAST_SECTOR : Flash address value of beginning of USBPD settings secteur
   ADDR_FLASH_PAGE_END : Flash address value of end of USBPD settings page
*/
/*#define FLASH_SECTOR_ID         FLASH_SECTOR_7*/ /* ID of the SECTOR to erase  */
/*#define ADDR_FLASH_SECTOR_7     0x08060000*/ /* Base @ of Sector 7, 128 Kbytes */
/*#define ADDR_FLASH_LAST_SECTOR  ADDR_FLASH_SECTOR_7*/  /* (FLASH_BANK1_END - FLASH_PAGE_SIZE + 1) : \
                                                        Base @ of Page 127, 2 Kbytes */
/*#define ADDR_FLASH_PAGE_END     (ADDR_FLASH_SECTOR_7 + (128 * 1024) - 1)*/
/*#define GUI_FLASH_MAGIC_NUMBER  ADDR_FLASH_LAST_SECTOR*/

/*
TO BE USED FOR SERIES USING PAGES (LIKE F0/G0/G4/L5...)
*/


/* Following definitions should be adapted to used Flash configuration :
   INDEX_PAGE : represents the number of the page used for storing USBPD settings (usually the last page)
   ADDR_FLASH_LAST_PAGE : Flash address value of beginning of USBPD settings page
   ADDR_FLASH_PAGE_END : Flash address value of end of USBPD settings page
*/
#define INDEX_PAGE              (FLASH_PAGE_NB - 1u)          /* Index of latest page                      */
#define ADDR_FLASH_LAST_PAGE    (FLASH_BASE + (INDEX_PAGE * FLASH_PAGE_SIZE))  /* Base @ of latest page    */
#define ADDR_FLASH_PAGE_END     (ADDR_FLASH_LAST_PAGE + FLASH_PAGE_SIZE - 1u)

#define GUI_FLASH_MAGIC_NUMBER         ADDR_FLASH_LAST_PAGE
/* should be aligned on double word */
#define GUI_FLASH_ADDR_NB_PDO_SNK_P0   (GUI_FLASH_MAGIC_NUMBER + 8u)
#define GUI_FLASH_ADDR_NB_PDO_SRC_P0   (GUI_FLASH_ADDR_NB_PDO_SNK_P0 + 1u)
#define GUI_FLASH_ADDR_NB_PDO_SNK_P1   (GUI_FLASH_ADDR_NB_PDO_SRC_P0 + 1u)
#define GUI_FLASH_ADDR_NB_PDO_SRC_P1   (GUI_FLASH_ADDR_NB_PDO_SNK_P1 + 1u)

/* should be aligned on double word */
#define GUI_FLASH_ADDR_PDO_SRC_P0      (GUI_FLASH_MAGIC_NUMBER + 16u)
/* New Address should be modulo 8 */
/* New Address = (Previous Address + (size / 8 * 8) + ((size % 8) + 7) / 8 * 8) */
#define GUI_FLASH_ADDR_PDO_SNK_P0      (GUI_FLASH_ADDR_PDO_SRC_P0 + ((USBPD_MAX_NB_PDO * 4) / 8 * 8)\
                                        + ((((USBPD_MAX_NB_PDO * 4) & 0x07) + 7) / 8 * 8 ))
#define GUI_FLASH_ADDR_PDO_SRC_P1      (GUI_FLASH_ADDR_PDO_SNK_P0 + ((USBPD_MAX_NB_PDO * 4) / 8 * 8)\
                                        + ((((USBPD_MAX_NB_PDO * 4) & 0x07) + 7) / 8 * 8 ))
#define GUI_FLASH_ADDR_PDO_SNK_P1      (GUI_FLASH_ADDR_PDO_SRC_P1 + ((USBPD_MAX_NB_PDO * 4) / 8 * 8)\
                                        + ((((USBPD_MAX_NB_PDO * 4) & 0x07) + 7) / 8 * 8 ))

#define GUI_FLASH_ADDR_DPM_SETTINGS    (GUI_FLASH_ADDR_PDO_SNK_P1 + ((USBPD_MAX_NB_PDO * 4) / 8 * 8)\
                                        + ((((USBPD_MAX_NB_PDO * 4) & 0x07) + 7) / 8 * 8 ))

/* Define by default USBPD_PORT_COUNT to 2 */
#define GUI_FLASH_ADDR_DPM_USER_SETTINGS  (GUI_FLASH_ADDR_DPM_SETTINGS\
                                           + (sizeof(USBPD_SettingsTypeDef) * USBPD_PORT_COUNT))

#define GUI_FLASH_ADDR_DPM_VDM_SETTINGS   (GUI_FLASH_ADDR_DPM_USER_SETTINGS\
                                           + (sizeof(USBPD_USER_SettingsTypeDef) * USBPD_PORT_COUNT))

#define GUI_FLASH_ADDR_DPM_ID_SETTINGS    (GUI_FLASH_ADDR_DPM_VDM_SETTINGS\
                                           + (sizeof(USBPD_VDM_SettingsTypeDef) * USBPD_PORT_COUNT))

#define GUI_FLASH_ADDR_RESERVED           (GUI_FLASH_ADDR_DPM_ID_SETTINGS\
                                           + (sizeof(USBPD_IdSettingsTypeDef) * USBPD_PORT_COUNT))

#define GUI_FLASH_SIZE_RESERVED           (ADDR_FLASH_PAGE_END - GUI_FLASH_ADDR_RESERVED)

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBPD_GUI_MEMMAP_H_ */

