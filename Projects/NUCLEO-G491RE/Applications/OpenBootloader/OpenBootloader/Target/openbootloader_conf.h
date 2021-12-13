/**
  ******************************************************************************
  * @file    openbootloader_conf.h
  * @author  MCD Application Team
  * @brief   Contains Open Bootloader configuration
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
#ifndef OPENBOOTLOADER_CONF_H
#define OPENBOOTLOADER_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "platform.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* -------------------------------- Device ID ------------------------------- */
#define DEVICE_ID_MSB                     0x04              /* MSB byte of device ID */
#define DEVICE_ID_LSB                     0x79              /* LSB byte of device ID */

/* -------------------------- Definitions for Memories ---------------------- */
#define FLASH_START_ADDRESS               FLASH_BASE
#define FLASH_END_ADDRESS                 (FLASH_BASE + (512 * 1024))

#define RAM_START_ADDRESS                 SRAM_BASE
#define RAM_END_ADDRESS                   (SRAM_BASE + (96 * 1024))   /* Size of SRAM1 + SRAM2 (80 + 16) */

#define OB1_START_ADDRESS                 0x1fff7800U                /* Option bytes 1 registers address */
#define OB1_END_ADDRESS                   (OB1_START_ADDRESS + 48)   /* Option bytes 1 end address (48 bytes) */

#define OB2_START_ADDRESS                 0x1FFFF800U                /* Option bytes 2 registers address */
#define OB2_END_ADDRESS                   (OB2_START_ADDRESS + 48)   /* Option bytes 2 end address (48 bytes) */

#define OTP_START_ADDRESS                 0x1FFF7000                   /* OTP registers address */
#define OTP_END_ADDRESS                   (OTP_START_ADDRESS + 1024)  /* OTP end address (1024 bytes) */

#define ICP1_START_ADDRESS                 0x1FFF0000                  /* System memory registers address */
#define ICP1_END_ADDRESS                   (ICP1_START_ADDRESS + (28 * 1024))          /* System memory registers end address */

#define ICP2_START_ADDRESS                 0x1FFF8000                  /* System memory registers address */
#define ICP2_END_ADDRESS                   (ICP2_START_ADDRESS + (28 * 1024))          /* System memory registers end address */

#define OPENBL_RAM_SIZE                   0x00003000U               /* RAM used by the Open Bootloader */

#define RDP_LEVEL_0                       OB_RDP_LEVEL_0
#define RDP_LEVEL_1                       OB_RDP_LEVEL_1
#define RDP_LEVEL_2                       OB_RDP_LEVEL_2

#define AREA_ERROR                        0x0U              /* Error Address Area */
#define FLASH_AREA                        0x1U              /* Flash Address Area */
#define RAM_AREA                          0x2U              /* RAM Address area */
#define OB_AREA                           0x3U              /* Option bytes Address area */
#define OTP_AREA                          0x4U                /* OTP Address area */
#define ICP_AREA                          0x5U              /* System memory area */

#define FLASH_ERASE_ALL                   0xFFFF
#define FLASH_BANK1_ERASE                 0xFFFE
#define FLASH_BANK2_ERASE                 0xFFFD

#define INTERFACES_SUPPORTED              5U

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* OPENBOOTLOADER_CONF_H */


