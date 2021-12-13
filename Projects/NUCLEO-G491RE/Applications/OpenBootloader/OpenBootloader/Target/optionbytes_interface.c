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

  if (Address == 0x1FFF7800)
  {

    /* Write RDP Level */
    if (DataLength >= 1U)
    {
      WRITE_REG(FLASH->OPTR, *(Data));
    }

    /* Write USER OPT */
    if (DataLength >= 4U)
    {
      WRITE_REG(FLASH->OPTR, (*(Data) | (*(Data + 1U) << 8) | (*(Data + 2U) << 16) | (*(Data + 3U) << 24)));
    }

    /* Write PCROPA */
    if (DataLength >= 10U)
    {
      WRITE_REG(FLASH->PCROP1SR, (*(Data + 8U) | (*(Data + 9U) << 8)));
      WRITE_REG(FLASH->PCROP1ER, (*(Data + 16U) | (*(Data + 17U) << 8)));
    }

    /*Write PCROP_RDP */
    if (DataLength >= 20U)
    {
      WRITE_REG(FLASH->PCROP1SR, (*(Data + 8U) | (*(Data + 9U) << 8)));
      WRITE_REG(FLASH->PCROP1ER, (*(Data + 16U) | (*(Data + 17U) << 8) | (*(Data + 18U) << 16) | (*(Data + 19U) << 24)));
    }

    /* Write protection of bank 1 area WRPA 1 area */
    if (DataLength >= 25U)
    {
      FLASH->WRP1AR = ((*(Data + 26U) << FLASH_WRP1AR_WRP1A_END_Pos) | *(Data + 24U));
    }

    /* Write protection of bank 1 area WRPA 2 area */
    if (DataLength >= 33U)
    {
      FLASH->WRP1BR = ((*(Data + 34U) << FLASH_WRP1BR_WRP1B_END_Pos) | *(Data + 32U));
    }

    /* Write STICKY area */
    if (DataLength >= 41U)
    {
      WRITE_REG(FLASH->SEC1R, ((*(Data + 42U) << FLASH_SEC1R_BOOT_LOCK_Pos) | *(Data + 40U)));
    }
  }

  SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);
}


