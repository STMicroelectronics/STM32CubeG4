/**
  ******************************************************************************
  * @file    optionbytes_interface.c
  * @author  MCD Application Team
  * @brief   Contains Option Bytes access functions
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

/* Includes ------------------------------------------------------------------*/
#include "platform.h"

#include "openbl_mem.h"

#include "app_openbootloader.h"
#include "common_interface.h"
#include "optionbytes_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef OB1_Descriptor =
{
  OB1_START_ADDRESS,
  OB1_END_ADDRESS,
  (48),
  OB_AREA,
  OPENBL_OB_Read,
  OPENBL_OB_Write,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

OPENBL_MemoryTypeDef OB2_Descriptor =
{
  OB2_START_ADDRESS,
  OB2_END_ADDRESS,
  (48),
  OB_AREA,
  OPENBL_OB_Read,
  OPENBL_OB_Write,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  This function is used to read data from a given address.
  * @param  Address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_OB_Read(uint32_t Address)
{
  return (*(uint8_t *)(Address));
}

/**
  * @brief  This function is used to write data in Option bytes.
  * @param  Address The address where that data will be written.
  * @param  Data The data to be written.
  * @param  DataLength The length of the data to be written.
  * @retval None.
  */
void OPENBL_OB_Write(uint32_t Address, uint8_t *Data, uint32_t DataLength)
{
  /* Unlock the FLASH & Option Bytes Registers access */
  HAL_FLASH_Unlock();
  HAL_FLASH_OB_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  /* Write RDP Level */
  if (DataLength >= 1)
  {
    WRITE_REG(FLASH->OPTR, *(Data));
  }

  /* Write USER OPT */
  if (DataLength >= 4)
  {
    WRITE_REG(FLASH->OPTR, (*(Data) | (*(Data + 1) << 8) | (*(Data + 2) << 16) | (*(Data + 3) << 24)));
  }

  /* Write PCROPA */
  if (DataLength >= 10)
  {
    WRITE_REG(FLASH->PCROP1SR, (*(Data + 8) | (*(Data + 9) << 8)));
    WRITE_REG(FLASH->PCROP1ER, (*(Data + 16) | (*(Data + 17) << 8)));
  }

  /*Write PCROP_RDP */
  if (DataLength >= 20)
  {
    WRITE_REG(FLASH->PCROP1SR, (*(Data + 8) | (*(Data + 9) << 8)));
    WRITE_REG(FLASH->PCROP1ER, (*(Data + 16) | (*(Data + 17) << 8) | (*(Data + 18) << 16) | (*(Data + 19) << 24)));
  }

  /* Write protection of bank 1 area WRPA 1 area */
  if (DataLength >= 25)
  {
    FLASH->WRP1AR = ((*(Data + 26) << FLASH_WRP1AR_WRP1A_END_Pos) | *(Data + 24));
  }

  /* Write protection of bank 1 area WRPA 2 area */
  if (DataLength >= 33)
  {
    FLASH->WRP1BR = ((*(Data + 34) << FLASH_WRP1BR_WRP1B_END_Pos) | *(Data + 32));
  }

  /* Write STICKY area */
  if (DataLength >= 41)
  {
    WRITE_REG(FLASH->SEC1R, *(Data + 40));
  }

  /* Write BOOT_EP */
  if (DataLength >= 43)
  {
    WRITE_REG(FLASH->SEC1R, *(Data + 42));
  }

  /* Write PCROP of bank 2 */
  if (DataLength >= 58)
  {
    WRITE_REG(FLASH->PCROP2SR, (*(Data + 56) | (*(Data + 57) << 8)));
    WRITE_REG(FLASH->PCROP2ER, (*(Data + 64) | (*(Data + 65) << 8)));
  }

  /* Write protection of bank 2 area WRPB 1 area */
  if (DataLength >= 73)
  {
    FLASH->WRP2AR = ((*(Data + 74) << FLASH_WRP2AR_WRP2A_END_Pos) | *(Data + 72));
  }

  /* Write protection of bank 2 area WRPB 2 area */
  if (DataLength >= 81)
  {
    FLASH->WRP2BR = ((*(Data + 82) << FLASH_WRP2BR_WRP2B_END_Pos) | *(Data + 80));
  }

  /* Write STICKY area */
  if (DataLength >= 89)
  {
    WRITE_REG(FLASH->SEC2R, *(Data + 88));
  }

  SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
}


