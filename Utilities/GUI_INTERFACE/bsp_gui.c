/**
  ******************************************************************************
  * @file    bsp_gui.c
  * @author  MCD Application Team
  * @brief   This file contains phy interface control functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#if defined(_GUI_INTERFACE)
#include "bsp_gui.h"
#include "usbpd_pdo_defs.h"
#include "usbpd_gui_memmap.h"
#if defined(STM32F072xB)  || defined(STM32F051x8)
#include "stm32f0xx.h"
#include "usbpd_pwr_if.h"
#else
#include "stm32g0xx.h"
#endif /* STM32F072xB */
#include "usbpd_dpm_user.h"
#if defined(_VDM)
#include "usbpd_vdm_user.h"
#endif /* _VDM */

/* Private typedef -----------------------------------------------------------*/
/* Private Defines */
extern USBPD_PWR_Port_PDO_Storage_TypeDef PWR_Port_PDO_Storage[USBPD_PORT_COUNT];
extern USBPD_SettingsTypeDef       DPM_Settings[USBPD_PORT_COUNT];
extern USBPD_SettingsTypeDef       DPM_Flash_Settings[USBPD_PORT_COUNT];
extern USBPD_USER_SettingsTypeDef  DPM_USER_Settings[USBPD_PORT_COUNT];
extern USBPD_USER_SettingsTypeDef  DPM_USER_FLASH_Settings[];
#if defined(_VDM)
extern USBPD_VDM_SettingsTypeDef   DPM_VDM_Settings[USBPD_PORT_COUNT];
#endif /* _VDM */

/* Private function prototypes -----------------------------------------------*/
static GUI_StatusTypeDef SavePDOInFlash(uint32_t Address, uint32_t* pListOfPDO);
static GUI_StatusTypeDef SaveSettingsInFlash(uint32_t Address, uint32_t *pSettings, uint32_t Size);

GUI_StatusTypeDef BSP_GUI_SaveDataInFlash(void)
{
  GUI_StatusTypeDef status = GUI_ERROR;
  FLASH_EraseInitTypeDef erase_init;
  uint32_t page_error;

  /* Disable interrupts */
  __disable_irq();

  /* Init Flash registers for writing */
  HAL_FLASH_Unlock();

  /* Erase the page associated to the GUI parameters */
  erase_init.TypeErase  = FLASH_TYPEERASE_PAGES;
#if defined(STM32F072xB)|| defined(STM32F051x8)
  erase_init.PageAddress        = ADDR_FLASH_LAST_PAGE;
#else
  erase_init.Page       = INDEX_PAGE;
#endif /* STM32F072xB || STM32F051x8 */
  erase_init.NbPages    = 1;

#if defined(STM32F072xB)  || defined(STM32F051x8)
#else
  /* TEMPORARY */
  if(FLASH->SR != 0x00)
  {
    FLASH->SR = FLASH_SR_OPTVERR;
  }
#endif
  status = HAL_OK == HAL_FLASHEx_Erase(&erase_init, &page_error) ? GUI_OK : GUI_ERASE_ERROR;

  /* If Erase is OK, program the new data */
  if ((0xFFFFFFFF == page_error) && (GUI_OK == status))
  {
#ifdef GUI_FLASH_ADDR_NB_PDO_SNK_P0
    /* Save the nb of sink and src PDO */
    uint64_t value = 0;
#if defined(_SNK) || defined(_DRP)
    value |= PWR_Port_PDO_Storage[USBPD_PORT_0].SinkPDO.NumberOfPDO;
#endif /* _SNK || _DRP */
#if defined(_SRC) || defined(_DRP)
    value |= (PWR_Port_PDO_Storage[USBPD_PORT_0].SourcePDO.NumberOfPDO << 8);
#endif /* _SRC || _DRP */
#if USBPD_PORT_COUNT==2
#if defined(_SNK) || defined(_DRP)
    value |= (PWR_Port_PDO_Storage[USBPD_PORT_1].SinkPDO.NumberOfPDO   << 16);
#endif /* _SNK || _DRP */
#if defined(_SRC) || defined(_DRP)
    value |= (PWR_Port_PDO_Storage[USBPD_PORT_1].SourcePDO.NumberOfPDO << 24);
#endif /* _SRC || _DRP */
#endif /* USBPD_PORT_COUNT==2 */
   status = HAL_OK == HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, GUI_FLASH_ADDR_NB_PDO_SNK_P0, value)? GUI_OK : GUI_WRITE_ERROR;
#endif  /* GUI_FLASH_ADDR_NB_PDO_SNK_P0 */

#if defined(_SRC) || defined(_DRP)
    /* Save PORT0_PDO_ListSRC */
    if (GUI_OK == status)
    {
      status = SavePDOInFlash(GUI_FLASH_ADDR_PDO_SRC_P0, PWR_Port_PDO_Storage[USBPD_PORT_0].SourcePDO.ListOfPDO);
    }
#endif /* _SRC || _DRP */

#if defined(_SNK) || defined(_DRP)
    /* Save PORT0_PDO_ListSNK */
    if (GUI_OK == status)
    {
      status = SavePDOInFlash(GUI_FLASH_ADDR_PDO_SNK_P0, PWR_Port_PDO_Storage[USBPD_PORT_0].SinkPDO.ListOfPDO);
    }
#endif /* _SNK || _DRP */

#if USBPD_PORT_COUNT==2
#if defined(_SRC) || defined(_DRP)
        /* Save PORT1_PDO_ListSRC */
        if (GUI_OK == status)
        {
          status = SavePDOInFlash(GUI_FLASH_ADDR_PDO_SRC_P1, PWR_Port_PDO_Storage[USBPD_PORT_1].SourcePDO.ListOfPDO);
        }
#endif /* _SRC || _DRP */

#if defined(_SNK) || defined(_DRP)
        /* Save PORT1_PDO_ListSNK */
        if (GUI_OK == status)
        {
          status = SavePDOInFlash(GUI_FLASH_ADDR_PDO_SNK_P1, PWR_Port_PDO_Storage[USBPD_PORT_1].SinkPDO.ListOfPDO);
        }
#endif /* _SNK || _DRP */
#endif /* USBPD_PORT_COUNT==2 */

    /* Save DPM_Settings of port 0 */
    if (GUI_OK == status)
    {
      status = SaveSettingsInFlash(GUI_FLASH_ADDR_DPM_SETTINGS, (uint32_t*)DPM_Settings, sizeof(USBPD_SettingsTypeDef));
    }

    /* Save DPM_Settings of port 0 */
    if (GUI_OK == status)
    {
      status = SaveSettingsInFlash(GUI_FLASH_ADDR_DPM_USER_SETTINGS, (uint32_t*)DPM_USER_Settings, sizeof(USBPD_USER_SettingsTypeDef));
    }

#if defined(_VDM)
    /* Save DPM_Settings of port 0 */
    if (GUI_OK == status)
    {
      status = SaveSettingsInFlash(GUI_FLASH_ADDR_DPM_VDM_SETTINGS, (uint32_t*)DPM_VDM_Settings, sizeof(USBPD_VDM_SettingsTypeDef));
    }
#endif /* _VDM */
  }

  /* Lock the flash afer end of operations */
  HAL_FLASH_Lock();

  /* Disable interrupts */
  __enable_irq();

  return status;
}

static GUI_StatusTypeDef SavePDOInFlash(uint32_t Address, uint32_t *pListOfPDO)
{
  uint64_t data_in_64;
  uint32_t index, index_flash;
  uint32_t value[2];
  GUI_StatusTypeDef status = GUI_OK;

  /* Save PORT0_PDO_ListSRC */
  for (index = 0, index_flash = 0; ((index < USBPD_MAX_NB_PDO) && (GUI_OK == status)); index++, index_flash++)
  {
    value[0] = pListOfPDO[index];
    index++;
    if (index < USBPD_MAX_NB_PDO)
    {
      value[1] = pListOfPDO[index];
    }
    else
    {
      value[1] = (0xFFFFFFFF);
    }

    data_in_64 = value[0] | (uint64_t)value[1] << 32;

    /* Save in the FLASH */
    status = HAL_OK == HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (Address + (8 * index_flash)) , data_in_64) ? GUI_OK : GUI_WRITE_ERROR;
  }
  return status;
}

static GUI_StatusTypeDef SaveSettingsInFlash(uint32_t Address, uint32_t *pSettings, uint32_t Size)
{
  uint64_t data_in_64;
  uint32_t index, index_flash;
  uint32_t value[2];
  GUI_StatusTypeDef status = GUI_OK;

  uint32_t nb_double = ((Size * USBPD_PORT_COUNT) / 4);
  uint8_t remaining = ((Size * USBPD_PORT_COUNT) % 4);

  /* Save Settings in the FLASH */
  for (index = 0, index_flash = 0; ((index < nb_double) && (GUI_OK == status)); index++, index_flash++)
  {
    value[0] = pSettings[index];
    index++;
    if (index < nb_double)
    {
      value[1] = pSettings[index];
    }
    else
    {
      if (0 == remaining)
        value[1] = (0xFFFFFFFF);
      else
        while(1);
    }

    data_in_64 = value[0] | (uint64_t)value[1] << 32;

    /* Save in the FLASH */
    status = HAL_OK == HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (Address + (8 * index_flash)) , data_in_64)? GUI_OK : GUI_WRITE_ERROR;
  }
  return status;
}
#endif /* _GUI_INTERFACE */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
