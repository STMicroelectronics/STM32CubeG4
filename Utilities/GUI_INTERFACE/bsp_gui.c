/**
  ******************************************************************************
  * @file    bsp_gui.c
  * @author  MCD Application Team
  * @brief   This file contains phy interface control functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#if defined(_GUI_INTERFACE)
#include "bsp_gui.h"
#include "usbpd_dpm_conf.h"
#if defined(GUI_FLASH_MAGIC_NUMBER)
#include "usbpd_trace.h"
#include "tracer_emb.h"
#endif /* GUI_FLASH_MAGIC_NUMBER */

/* Private typedef -----------------------------------------------------------*/
/* Private Defines */
extern USBPD_SettingsTypeDef       DPM_Settings[USBPD_PORT_COUNT];
extern uint8_t USBPD_NbPDO[4];
#if defined(_VDM)
extern USBPD_VDM_SettingsTypeDef   DPM_VDM_Settings[USBPD_PORT_COUNT];
#endif /* _VDM */

/* Private Variables */
#if defined(GUI_FLASH_MAGIC_NUMBER)
#if defined(FLASH_TYPEPROGRAM_DOUBLEWORD)
static const uint64_t MagicNumber = 0xDEADBABEDEADF00DU;
#elif defined(FLASH_TYPEPROGRAM_QUADWORD)
static const uint64_t MagicNumber[2] = { 0xDEADBABEDEADF00DU, 0xDEADBABEDEADF00DU };
#else
#warning "Flash program option undefined"
#endif  /* FLASH_TYPEPROGRAM_DOUBLEWORD */
#endif /* GUI_FLASH_MAGIC_NUMBER */

/* Private Macro */
#if defined(GUI_FLASH_MAGIC_NUMBER)
#define GUI_CHECK_IF_MEMORY_IS_CORRUPTED(__ADDR__)
#else
#define GUI_CHECK_IF_MEMORY_IS_CORRUPTED(__ADDR__) if (0xFFFFFFFFu != *((uint32_t*)(__ADDR__)))
#endif /* GUI_FLASH_MAGIC_NUMBER */

/* Private function prototypes -----------------------------------------------*/
static GUI_StatusTypeDef        LoadPDOFromFlash(uint32_t Address, uint32_t *pListOfPDO);
static GUI_StatusTypeDef        LoadSettingsFromFlash(uint32_t Address, uint32_t *pSettings, uint32_t Size);
static GUI_StatusTypeDef        SavePDOInFlash(uint32_t Address, uint32_t *pListOfPDO);
static GUI_StatusTypeDef        SaveSettingsInFlash(uint32_t Address, uint32_t *pSettings, uint32_t Size);

GUI_StatusTypeDef BSP_GUI_LoadDataFromFlash(void)
{
  GUI_StatusTypeDef _status = GUI_ERROR;
#if defined(GUI_FLASH_MAGIC_NUMBER)
  uint32_t _addr = GUI_FLASH_MAGIC_NUMBER;
#else
  uint32_t _addr = GUI_FLASH_ADDR_NB_PDO_SNK_P0;
#endif /* GUI_FLASH_MAGIC_NUMBER */

  /* Check that we did not reach the end of page */
  if (GUI_FLASH_SIZE_RESERVED < 0)
  {
    goto _exit;
  }

#if defined(GUI_FLASH_MAGIC_NUMBER)
  /* check that GUI area has not been corrupted */
#if defined(FLASH_TYPEPROGRAM_DOUBLEWORD)
  if ((0xFFFFFFFFu != *((uint32_t *)_addr)) && (MagicNumber != *((uint64_t *)_addr)))
#elif defined(FLASH_TYPEPROGRAM_QUADWORD)
  if ((0xFFFFFFFFu != *((uint32_t *)_addr)) && (MagicNumber[0] != *((uint64_t *)_addr)))
#else
#warning "Flash program option undefined"
#endif  /* FLASH_TYPEPROGRAM_DOUBLEWORD */
  {
    /* Memory has been corrupted */
    USBPD_TRACE_Add(USBPD_TRACE_DEBUG, 0U, 0U, (uint8_t *)"GUI Memory is corrupted", sizeof("GUI Memory is corrupted"));
    goto _exit;
  }
  if (0xFFFFFFFFu == *((uint32_t *)_addr))
  {
    /* Memory is empty no need to retrieve data from GUI area */
    goto _exit;
  }
  _addr = GUI_FLASH_ADDR_NB_PDO_SNK_P0;
#endif /* GUI_FLASH_MAGIC_NUMBER */

  GUI_CHECK_IF_MEMORY_IS_CORRUPTED(_addr)
  {
    uint32_t *_ptr = (uint32_t *)USBPD_NbPDO;
    USPBPD_WRITE32(_ptr, *((uint32_t *)_addr));
    _status = GUI_OK;
  }

#if defined(_SRC) || defined(_DRP)
  /* Load PORT0_PDO_ListSRC */
  _status |= LoadPDOFromFlash(GUI_FLASH_ADDR_PDO_SRC_P0, PORT0_PDO_ListSRC);
#endif /* _SRC || _DRP */

#if defined(_SNK) || defined(_DRP)
  /* Load PORT0_PDO_ListSNK */
  _status |= LoadPDOFromFlash(GUI_FLASH_ADDR_PDO_SNK_P0, PORT0_PDO_ListSNK);
#endif /* _SNK || _DRP */

#if USBPD_PORT_COUNT==2
#if defined(_SRC) || defined(_DRP)
  /* Load PORT1_PDO_ListSRC */
  _status |= LoadPDOFromFlash(GUI_FLASH_ADDR_PDO_SRC_P1, PORT1_PDO_ListSRC);
#endif /* _SRC || _DRP */

#if defined(_SNK) || defined(_DRP)
  /* Load PORT1_PDO_ListSNK */
  _status |= LoadPDOFromFlash(GUI_FLASH_ADDR_PDO_SNK_P1, PORT1_PDO_ListSNK);
#endif /* _SNK || _DRP */
#endif /* USBPD_PORT_COUNT==2 */

  /* Load DPM_Settings of port 0 */
  _status |= LoadSettingsFromFlash(GUI_FLASH_ADDR_DPM_SETTINGS, (uint32_t *)DPM_Settings,
                                   sizeof(USBPD_SettingsTypeDef) * (uint32_t)USBPD_PORT_COUNT);

#if defined(GUI_FLASH_ADDR_DPM_ID_SETTINGS)
  /* Load DPM_ID_Settings */
  _status |= LoadSettingsFromFlash(GUI_FLASH_ADDR_DPM_ID_SETTINGS, (uint32_t *)DPM_ID_Settings,
                                   sizeof(USBPD_IdSettingsTypeDef) * (uint32_t)USBPD_PORT_COUNT);
#endif /* GUI_FLASH_ADDR_DPM_ID_SETTINGS */

  /* Load DPM_Settings of port 0 */
  _status |= LoadSettingsFromFlash(GUI_FLASH_ADDR_DPM_USER_SETTINGS, (uint32_t *)DPM_USER_Settings,
                                   sizeof(USBPD_USER_SettingsTypeDef) * (uint32_t)USBPD_PORT_COUNT);
#if defined(GUI_FLASH_ADDR_DPM_ID_SETTINGS)
  /* Overwrite ID Settings in DPM_USER_Settings */
#if defined(USBPD_REV30_SUPPORT)
#if _SRC_CAPA_EXT && (defined(_SRC)||defined(_DRP))
  DPM_USER_Settings[USBPD_PORT_0].DPM_SRCExtendedCapa.XID = DPM_ID_Settings[USBPD_PORT_0].XID;
  DPM_USER_Settings[USBPD_PORT_0].DPM_SRCExtendedCapa.VID = DPM_ID_Settings[USBPD_PORT_0].VID;
  DPM_USER_Settings[USBPD_PORT_0].DPM_SRCExtendedCapa.PID = DPM_ID_Settings[USBPD_PORT_0].PID;
#if USBPD_PORT_COUNT==2
  DPM_USER_Settings[USBPD_PORT_1].DPM_SRCExtendedCapa.XID = DPM_ID_Settings[USBPD_PORT_1].XID;
  DPM_USER_Settings[USBPD_PORT_1].DPM_SRCExtendedCapa.VID = DPM_ID_Settings[USBPD_PORT_1].VID;
  DPM_USER_Settings[USBPD_PORT_1].DPM_SRCExtendedCapa.PID = DPM_ID_Settings[USBPD_PORT_1].PID;
#endif /* USBPD_PORT_COUNT==2 */
#endif /* _SRC_CAPA_EXT && (_SRC || _DRP) */
#if defined(_SNK)||defined(_DRP)
  DPM_USER_Settings[USBPD_PORT_0].DPM_SNKExtendedCapa.XID = DPM_ID_Settings[USBPD_PORT_0].XID;
  DPM_USER_Settings[USBPD_PORT_0].DPM_SNKExtendedCapa.VID = DPM_ID_Settings[USBPD_PORT_0].VID;
  DPM_USER_Settings[USBPD_PORT_0].DPM_SNKExtendedCapa.PID = DPM_ID_Settings[USBPD_PORT_0].PID;
#if USBPD_PORT_COUNT==2
  DPM_USER_Settings[USBPD_PORT_1].DPM_SNKExtendedCapa.XID = DPM_ID_Settings[USBPD_PORT_1].XID;
  DPM_USER_Settings[USBPD_PORT_1].DPM_SNKExtendedCapa.VID = DPM_ID_Settings[USBPD_PORT_1].VID;
  DPM_USER_Settings[USBPD_PORT_1].DPM_SNKExtendedCapa.PID = DPM_ID_Settings[USBPD_PORT_1].PID;
#endif /* USBPD_PORT_COUNT==2 */
#endif /* _SNK || _DRP */
#if _MANU_INFO
  DPM_USER_Settings[USBPD_PORT_0].DPM_ManuInfoPort.VID = DPM_ID_Settings[USBPD_PORT_0].VID;
  DPM_USER_Settings[USBPD_PORT_0].DPM_ManuInfoPort.PID = DPM_ID_Settings[USBPD_PORT_0].PID;
#if USBPD_PORT_COUNT==2
  DPM_USER_Settings[USBPD_PORT_1].DPM_ManuInfoPort.VID = DPM_ID_Settings[USBPD_PORT_1].VID;
  DPM_USER_Settings[USBPD_PORT_1].DPM_ManuInfoPort.PID = DPM_ID_Settings[USBPD_PORT_1].PID;
#endif /* USBPD_PORT_COUNT==2 */
#endif /* _MANU_INFO */
#endif /* USBPD_REV30_SUPPORT */
#endif /* GUI_FLASH_ADDR_DPM_ID_SETTINGS */

#if defined(_VDM)
  /* Save DPM_Settings of port 0 */
  _status |= LoadSettingsFromFlash(GUI_FLASH_ADDR_DPM_VDM_SETTINGS, (uint32_t *)DPM_VDM_Settings,
                                   sizeof(USBPD_VDM_SettingsTypeDef) * (uint32_t)USBPD_PORT_COUNT);
#if defined(GUI_FLASH_ADDR_DPM_ID_SETTINGS)
  /* Overwrite ID Settings in VDM settings */
  DPM_VDM_Settings[USBPD_PORT_0].VDM_XID_SOP      = DPM_ID_Settings[USBPD_PORT_0].XID;
  DPM_VDM_Settings[USBPD_PORT_0].VDM_USB_VID_SOP  = DPM_ID_Settings[USBPD_PORT_0].VID;
  DPM_VDM_Settings[USBPD_PORT_0].VDM_PID_SOP      = DPM_ID_Settings[USBPD_PORT_0].PID;
#if USBPD_PORT_COUNT==2
  DPM_VDM_Settings[USBPD_PORT_1].VDM_XID_SOP      = DPM_ID_Settings[USBPD_PORT_1].XID;
  DPM_VDM_Settings[USBPD_PORT_1].VDM_USB_VID_SOP  = DPM_ID_Settings[USBPD_PORT_1].VID;
  DPM_VDM_Settings[USBPD_PORT_1].VDM_PID_SOP      = DPM_ID_Settings[USBPD_PORT_1].PID;
#endif /* USBPD_PORT_COUNT==2 */
#endif /* _VDM */
#endif /* GUI_FLASH_ADDR_DPM_ID_SETTINGS */

_exit:
  return _status;
}

GUI_StatusTypeDef BSP_GUI_SaveDataInFlash(void)
{
  GUI_StatusTypeDef status = GUI_OK;
  FLASH_EraseInitTypeDef erase_init;
  uint32_t page_error;

  /* Disable interrupts */
  __disable_irq();

  /* Init Flash registers for writing */
  (void)HAL_FLASH_Unlock();

  /* Erase the page associated to the GUI parameters */
  erase_init.TypeErase  = FLASH_TYPEERASE_PAGES;

#if defined(STM32F072xB)|| defined(STM32F051x8)
  erase_init.PageAddress  = ADDR_FLASH_LAST_PAGE;
#else
  erase_init.Page       = INDEX_PAGE;
#endif /* STM32F072xB || STM32F051x8 */
#if defined (FLASH_OPTR_DBANK)
  erase_init.Banks      = FLASH_BANK_2;
#elif defined(FLASH_BANK_2)
  erase_init.Banks      = FLASH_BANK_2;
#elif defined(FLASH_BANK_1)
  erase_init.Banks      = FLASH_BANK_1;
#endif /* FLASH_OPTR_DBANK */
  erase_init.NbPages    = 1;

#if defined(FLASH_SR_OPTVERR)
  /* Specific handling of STM32G0 and STM32G4 flash devices for allowing erase operations */
  if (FLASH->SR != 0x00)
  {
    FLASH->SR = FLASH_SR_OPTVERR;
  }
#endif /* FLASH_SR_OPTVERR */

  if (HAL_OK != HAL_FLASHEx_Erase(&erase_init, &page_error))
  {
    status = GUI_ERASE_ERROR;
  }

  /* If Erase is OK, program the new data */
  if ((0xFFFFFFFFU == page_error) && (GUI_OK == status))
  {
#if defined(GUI_FLASH_MAGIC_NUMBER)
    /* Save magic Number */
#if defined(FLASH_TYPEPROGRAM_DOUBLEWORD)
    if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, GUI_FLASH_MAGIC_NUMBER, MagicNumber))
    {
      status = GUI_WRITE_ERROR;
    }
#elif defined(FLASH_TYPEPROGRAM_QUADWORD)
    if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, GUI_FLASH_MAGIC_NUMBER, (uint32_t)MagicNumber))
    {
      status = GUI_WRITE_ERROR;
    }
#else
#warning "Flash program option undefined"
#endif  /* FLASH_TYPEPROGRAM_DOUBLEWORD */
#endif /* GUI_FLASH_MAGIC_NUMBER */

#ifdef GUI_FLASH_ADDR_NB_PDO_SNK_P0
    if (GUI_OK == status)
    {
#if defined(FLASH_TYPEPROGRAM_DOUBLEWORD)
      /* Save the nb of sink and src PDO */
      uint64_t value = 0;
      value |= USBPD_NbPDO[0];
      value |= (USBPD_NbPDO[1] << 8);
      value |= (USBPD_NbPDO[2] << 16);
      value |= (USBPD_NbPDO[3] << 24);
      if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, GUI_FLASH_ADDR_NB_PDO_SNK_P0, value))
      {
        status = GUI_WRITE_ERROR;
      }
#elif defined(FLASH_TYPEPROGRAM_QUADWORD)
      /* Save the nb of sink and src PDO */
      uint64_t value[2] = { 0, 0 };
      value[0] |= USBPD_NbPDO[0];
      value[0] |= (uint64_t)(USBPD_NbPDO[1]) << 8;
      value[0] |= (uint64_t)(USBPD_NbPDO[2]) << 16;
      value[0] |= (uint64_t)(USBPD_NbPDO[3]) << 24;
      if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, GUI_FLASH_ADDR_NB_PDO_SNK_P0, (uint32_t)value))
      {
        status = GUI_WRITE_ERROR;
      }
#else
#warning "Flash program option undefined"
#endif  /* FLASH_TYPEPROGRAM_DOUBLEWORD */
    }
#endif  /* GUI_FLASH_ADDR_NB_PDO_SNK_P0 */

#if defined(_SRC) || defined(_DRP)
    /* Save PORT0_PDO_ListSRC */
    if (GUI_OK == status)
    {
      status = SavePDOInFlash(GUI_FLASH_ADDR_PDO_SRC_P0, PORT0_PDO_ListSRC);
    }
#endif /* _SRC || _DRP */

#if defined(_SNK) || defined(_DRP)
    /* Save PORT0_PDO_ListSNK */
    if (GUI_OK == status)
    {
      status = SavePDOInFlash(GUI_FLASH_ADDR_PDO_SNK_P0, PORT0_PDO_ListSNK);
    }
#endif /* _SNK || _DRP */

#if USBPD_PORT_COUNT==2
#if defined(_SRC) || defined(_DRP)
    /* Save PORT1_PDO_ListSRC */
    if (GUI_OK == status)
    {
      status = SavePDOInFlash(GUI_FLASH_ADDR_PDO_SRC_P1, PORT1_PDO_ListSRC);
    }
#endif /* _SRC || _DRP */

#if defined(_SNK) || defined(_DRP)
    /* Save PORT1_PDO_ListSNK */
    if (GUI_OK == status)
    {
      status = SavePDOInFlash(GUI_FLASH_ADDR_PDO_SNK_P1, PORT1_PDO_ListSNK);
    }
#endif /* _SNK || _DRP */
#endif /* USBPD_PORT_COUNT==2 */

    /* Save DPM_Settings of port 0 */
    if (GUI_OK == status)
    {
      status = SaveSettingsInFlash(GUI_FLASH_ADDR_DPM_SETTINGS,
                                   (uint32_t *)DPM_Settings,
                                   sizeof(USBPD_SettingsTypeDef) * (uint32_t)USBPD_PORT_COUNT);
    }

#if defined(GUI_FLASH_ADDR_DPM_ID_SETTINGS)
    /* Save DPM_ID_Settings */
    if (GUI_OK == status)
    {
      status = SaveSettingsInFlash(GUI_FLASH_ADDR_DPM_ID_SETTINGS,
                                   (uint32_t *)DPM_ID_Settings,
                                   sizeof(USBPD_IdSettingsTypeDef));
    }
#endif /* GUI_FLASH_ADDR_DPM_ID_SETTINGS */

    /* Save DPM_Settings of port 0 */
    if (GUI_OK == status)
    {
      status = SaveSettingsInFlash(GUI_FLASH_ADDR_DPM_USER_SETTINGS,
                                   (uint32_t *)DPM_USER_Settings,
                                   sizeof(USBPD_USER_SettingsTypeDef) * (uint32_t)USBPD_PORT_COUNT);
    }

#if defined(_VDM)
    /* Save DPM_Settings of port 0 */
    if (GUI_OK == status)
    {
      status = SaveSettingsInFlash(GUI_FLASH_ADDR_DPM_VDM_SETTINGS,
                                   (uint32_t *)DPM_VDM_Settings,
                                   sizeof(USBPD_VDM_SettingsTypeDef) * (uint32_t)USBPD_PORT_COUNT);
    }
#endif /* _VDM */
  }

  /* Lock the flash after end of operations */
  (void) HAL_FLASH_Lock();

  /* Enable interrupts */
  __enable_irq();

  return status;
}

static GUI_StatusTypeDef SavePDOInFlash(uint32_t Address, uint32_t *pListOfPDO)
{
#if defined(FLASH_TYPEPROGRAM_DOUBLEWORD)
  uint64_t data_in_64;
  uint32_t index;
  uint32_t index_flash;
  uint32_t value[2];
  GUI_StatusTypeDef status = GUI_OK;

  index_flash = 0U;
  for (index = 0U; ((index < USBPD_MAX_NB_PDO) && (GUI_OK == status)); index += 1U)
  {
    value[0] = pListOfPDO[index];
    index++;
    if (index < USBPD_MAX_NB_PDO)
    {
      value[1] = pListOfPDO[index];
    }
    else
    {
      value[1] = (0xFFFFFFFFU);
    }

    data_in_64 = value[0] | ((uint64_t)value[1] << 32U);

    /* Save in the FLASH */
    if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (Address + (8U * index_flash)), data_in_64))
    {
      status = GUI_WRITE_ERROR;
    }
    index_flash += 1;
  }
  return status;
#elif defined(FLASH_TYPEPROGRAM_QUADWORD)
  uint64_t data_in_64[2];
  uint32_t index;
  uint32_t index_flash;
  uint32_t value[4];
  GUI_StatusTypeDef status = GUI_OK;

  index_flash = 0U;
  for (index = 0U; ((index < USBPD_MAX_NB_PDO) && (GUI_OK == status)); index += 1U)
  {
    value[0] = pListOfPDO[index];
    index++;
    if (index < USBPD_MAX_NB_PDO)
    {
      value[1] = pListOfPDO[index];
    }
    else
    {
      value[1] = (0xFFFFFFFFU);
    }
    index++;
    if (index < USBPD_MAX_NB_PDO)
    {
      value[2] = pListOfPDO[index];
    }
    else
    {
      value[2] = (0xFFFFFFFFU);
    }
    index++;
    if (index < USBPD_MAX_NB_PDO)
    {
      value[3] = pListOfPDO[index];
    }
    else
    {
      value[3] = (0xFFFFFFFFU);
    }

    data_in_64[0] = value[0] | ((uint64_t)value[1] << 32);
    data_in_64[1] = value[2] | ((uint64_t)value[3] << 32);

    /* Save in the FLASH */
    if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, (Address + (16U * index_flash)), (uint32_t) data_in_64))
    {
      status = GUI_WRITE_ERROR;
    }
    index_flash += 1U;
  }
  return status;
#else
#warning "Flash program option undefined"
#endif  /* FLASH_TYPEPROGRAM_DOUBLEWORD */
}

static GUI_StatusTypeDef SaveSettingsInFlash(uint32_t Address, uint32_t *pSettings, uint32_t Size)
{
#if defined(FLASH_TYPEPROGRAM_DOUBLEWORD)
  uint64_t data_in_64;
  uint32_t index;
  uint32_t index_flash;
  uint32_t value[2];
  uint32_t nb_u32   = ((Size) / 4U);
  GUI_StatusTypeDef status = GUI_OK;

  /* Ensure nb of bytes to be writeent is 4-multiple */
  if (((Size) % 4U) != 0U)
  {
    return (GUI_ERROR);
  }

  /* Save Settings in the FLASH */
  index_flash = 0U;
  for (index = 0U; ((index < nb_u32) && (GUI_OK == status)); index += 1U)
  {
    value[0] = pSettings[index];
    index++;
    if (index < nb_u32)
    {
      value[1] = pSettings[index];
    }
    else
    {
      value[1] = (0xFFFFFFFFU);
    }

    data_in_64 = value[0] | ((uint64_t)value[1] << 32U);

    /* Save in the FLASH */
    if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (Address + (8U * index_flash)), data_in_64))
    {
      status = GUI_WRITE_ERROR;
    }
    index_flash += 1U;
  }
  return status;
#elif defined(FLASH_TYPEPROGRAM_QUADWORD)
  uint64_t data_in_64[2];
  uint32_t index;
  uint32_t index_flash;
  uint32_t value[4];
  uint32_t nb_u32   = ((Size) / 4U);
  GUI_StatusTypeDef status = GUI_OK;

  /* Ensure nb of bytes to be writeent is 4-multiple */
  if (((Size) % 4U) != 0U)
  {
    return (GUI_ERROR);
  }

  /* Save Settings in the FLASH */
  index_flash = 0U;
  for (index = 0U; ((index < nb_u32) && (GUI_OK == status)); index += 1U)
  {
    value[0] = pSettings[index];
    index++;
    if (index < nb_u32)
    {
      value[1] = pSettings[index];
    }
    else
    {
      value[1] = (0xFFFFFFFFU);
    }
    index++;
    if (index < nb_u32)
    {
      value[2] = pSettings[index];
    }
    else
    {
      value[2] = (0xFFFFFFFFU);
    }
    index++;
    if (index < nb_u32)
    {
      value[3] = pSettings[index];
    }
    else
    {
      value[3] = (0xFFFFFFFFU);
    }

    data_in_64[0] = value[0] | ((uint64_t)value[1] << 32);
    data_in_64[1] = value[2] | ((uint64_t)value[3] << 32);

    /* Save in the FLASH */
    if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, (Address + (16U * index_flash)), (uint32_t) data_in_64))
    {
      status = GUI_WRITE_ERROR;
    }
    index_flash += 1U;
  }
  return status;
#else
#warning "Flash program option undefined"
#endif  /* FLASH_TYPEPROGRAM_DOUBLEWORD */
}

static GUI_StatusTypeDef LoadPDOFromFlash(uint32_t Address, uint32_t *pListOfPDO)
{
  uint32_t _addr = Address;
  GUI_StatusTypeDef _status = GUI_ERROR;

  /* Check if FLASH is not empty to retrieve the data. Nethertheless keep data in the RAM */
  GUI_CHECK_IF_MEMORY_IS_CORRUPTED(_addr)
  {
    uint32_t _index;
    for (_index = 0U; _index < USBPD_MAX_NB_PDO; _index++)
    {
      pListOfPDO[_index] = *((uint32_t *)_addr);
      _addr = _addr + 4U;
    }
    _status = GUI_OK;
  }
  return _status;
}

static GUI_StatusTypeDef LoadSettingsFromFlash(uint32_t Address, uint32_t *pSettings, uint32_t Size)
{
  uint32_t _addr = Address;
  GUI_StatusTypeDef _status = GUI_ERROR;

  /* Check if FLASH is not empty to retrieve the data. Nethertheless keep data in the RAM */
  GUI_CHECK_IF_MEMORY_IS_CORRUPTED(_addr)
  {
    (void) memcpy(pSettings, ((uint32_t *)_addr), Size);
    _status = GUI_OK;
  }
  return _status;
}
#endif /* _GUI_INTERFACE */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
