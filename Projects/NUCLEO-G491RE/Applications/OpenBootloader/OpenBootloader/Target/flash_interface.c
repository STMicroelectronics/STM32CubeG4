/**
  ******************************************************************************
  * @file    flash_interface.c
  * @author  MCD Application Team
  * @brief   Contains FLASH access functions
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
#include "flash_interface.h"
#include "stm32g4xx_hal_flash_ex.h"
/* Private typedef -----------------------------------------------------------*/
Send_BusyByte_Func *BusyByteCallback = 0;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static FLASH_ProcessTypeDef FlashProcess = {.Lock = HAL_UNLOCKED,
                                            .ErrorCode = HAL_FLASH_ERROR_NONE,
                                            .ProcedureOnGoing = FLASH_PROC_NONE,
                                            .Address = 0U,
                                            .Bank = FLASH_BANK_1,
                                            .Page = 0U,
                                            .NbPagesToErase = 0U,
                                            .CacheToReactivate = FLASH_CACHE_DISABLED};

uint32_t Flash_BusyState = FLASH_BUSY_STATE_DISABLED;

/* Private function prototypes -----------------------------------------------*/
static void OPENBL_FLASH_ProgramDoubleWord(uint32_t Address, uint64_t Data);
static ErrorStatus OPENBL_FLASH_EnableWriteProtection(uint8_t *pListOfPages, uint32_t Length);
static ErrorStatus OPENBL_FLASH_DisableWriteProtection(void);

static HAL_StatusTypeDef OPENBL_FLASH_SendBusyState(uint32_t Timeout);
static HAL_StatusTypeDef OPENBL_FLASH_OB_RDPConfig(uint32_t RDPLevel);
static HAL_StatusTypeDef OPENBL_FLASH_OB_WRPConfig(uint32_t WRPArea, uint32_t WRPStartOffset, uint32_t WRDPEndOffset);
static HAL_StatusTypeDef OPENBL_FLASH_OptionBytesProgram(FLASH_OBProgramInitTypeDef *pOBInit);
static HAL_StatusTypeDef OPENBL_FLASH_ExtendedErase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *pPageError);
static HAL_StatusTypeDef OPENBL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data);

/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef FLASH_Descriptor =
{
  FLASH_START_ADDRESS,
  FLASH_END_ADDRESS,
  (512U * 1024U),
  FLASH_AREA,
  OPENBL_FLASH_Read,
  OPENBL_FLASH_Write,
  OPENBL_FLASH_SetReadOutProtectionLevel,
  OPENBL_FLASH_SetWriteProtection,
  OPENBL_FLASH_JumpToAddress,
  NULL,
  OPENBL_FLASH_Erase
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Unlock the FLASH control register access.
  * @retval None.
  */
void OPENBL_FLASH_Unlock(void)
{
  HAL_FLASH_Unlock();
}

/**
  * @brief  Lock the FLASH control register access.
  * @retval None.
  */
void OPENBL_FLASH_Lock(void)
{
  HAL_FLASH_Lock();
}

/**
  * @brief  Unlock the FLASH Option Bytes Registers access.
  * @retval None.
  */
void OPENBL_FLASH_OB_Unlock(void)
{
  HAL_FLASH_Unlock();

  HAL_FLASH_OB_Unlock();
}

/**
  * @brief  Launch the option byte loading.
  * @retval None.
  */
void OPENBL_FLASH_OB_Launch(void)
{
  HAL_FLASH_OB_Launch();
}

/**
  * @brief  This function is used to read data from a given address.
  * @param  Address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_FLASH_Read(uint32_t Address)
{
  return (*(uint8_t *)(Address));
}

/**
  * @brief  This function is used to write data in FLASH memory.
  * @param  Address The address where that data will be written.
  * @param  Data The data to be written.
  * @param  DataLength The length of the data to be written.
  * @retval None.
  */
void OPENBL_FLASH_Write(uint32_t Address, uint8_t *pData, uint32_t DataLength)
{
  uint32_t index  = 0U;
  uint32_t length = DataLength;

  if (length & 7U)
  {
    length = (length & 0xFFFFFFF8U) + 8U;
  }

  /* Unlock the flash memory for write operation */
  OPENBL_FLASH_Unlock();

  for (index = 0U; index < length; (index += 8U))
  {
    OPENBL_FLASH_ProgramDoubleWord((Address + index), (uint64_t)(*((uint64_t *)((uint32_t)pData + index))));
  }

  /* Lock the Flash to disable the flash control register access */
  OPENBL_FLASH_Lock();
}

/**
  * @brief  This function is used to jump to a given address.
  * @param  Address The address where the function will jump.
  * @retval None.
  */
void OPENBL_FLASH_JumpToAddress(uint32_t Address)
{
  Function_Pointer jump_to_address;

  /* Deinitialize all HW resources used by the Bootloader to their reset values */
  OpenBootloader_DeInit();

  /* Enable IRQ */
  Common_EnableIrq();

  jump_to_address = (Function_Pointer)(*(__IO uint32_t *)(Address + 4U));

  /* Initialize user application's stack pointer */
  Common_SetMsp(*(__IO uint32_t *) Address);

  jump_to_address();
}

/**
  * @brief  Return the FLASH Read Protection level.
  * @retval The return value can be one of the following values:
  *         @arg OB_RDP_LEVEL_0: No protection
  *         @arg OB_RDP_LEVEL_1: Read protection of the memory
  *         @arg OB_RDP_LEVEL_2: Full chip protection
  */
uint32_t OPENBL_FLASH_GetReadOutProtectionLevel(void)
{
  FLASH_OBProgramInitTypeDef flash_ob;

  /* Get the Option bytes configuration */
  HAL_FLASHEx_OBGetConfig(&flash_ob);

  return flash_ob.RDPLevel;
}

/**
  * @brief  Return the FLASH Read Protection level.
  * @param  Level Can be one of these values:
  *         @arg OB_RDP_LEVEL_0: No protection
  *         @arg OB_RDP_LEVEL_1: Read protection of the memory
  *         @arg OB_RDP_LEVEL_2: Full chip protection
  * @retval None.
  */
void OPENBL_FLASH_SetReadOutProtectionLevel(uint32_t Level)
{
  FLASH_OBProgramInitTypeDef flash_ob;

  if (Level != OB_RDP_LEVEL2)
  {
    flash_ob.OptionType = OPTIONBYTE_RDP;
    flash_ob.RDPLevel   = Level;

    /* Unlock the FLASH registers & Option Bytes registers access */
    OPENBL_FLASH_OB_Unlock();

    /* Clear error programming flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    /* Change the RDP level */
    OPENBL_FLASH_OptionBytesProgram(&flash_ob);

    /* Reset PCROP registers if RDP level 0 is set */
    if (Level == OB_RDP_LEVEL0)
    {
      flash_ob.OptionType      = OPTIONBYTE_PCROP;
      flash_ob.PCROPConfig     = FLASH_BANK_1;
      flash_ob.PCROPStartAddr  = 0x08007FFFU;
      flash_ob.PCROPEndAddr    = 0x08000000U;

      /* Change PCROP1 registers */
      HAL_FLASHEx_OBProgram(&flash_ob);
    }
  }
}

/**
  * @brief  This function is used to enable or disable write protection of the specified FLASH areas.
  * @param  State Can be one of these values:
  *         @arg DISABLE: Disable FLASH write protection
  *         @arg ENABLE: Enable FLASH write protection
  * @param  ListOfPages Contains the list of pages to be protected.
  * @param  Length The length of the list of pages to be protected.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
ErrorStatus OPENBL_FLASH_SetWriteProtection(FunctionalState State, uint8_t *pListOfPages, uint32_t Length)
{
  ErrorStatus status = SUCCESS;

  if (State == ENABLE)
  {
    OPENBL_FLASH_EnableWriteProtection(pListOfPages, Length);
  }
  else if (State == DISABLE)
  {
    OPENBL_FLASH_DisableWriteProtection();
  }
  else
  {
    status = ERROR;
  }

  OPENBL_Disable_BusyState_Sending();
  return status;
}

/**
  * @brief  This function is used to start FLASH mass erase operation.
  * @param  *pData Pointer to the buffer that contains mass erase operation options.
  * @param  DataLength Size of the Data buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Mass erase operation done
  *          - ERROR:   Mass erase operation failed or the value of one parameter is not ok
  */
ErrorStatus OPENBL_FLASH_MassErase(uint8_t *pData, uint32_t DataLength)
{
  uint32_t page_error  = 0U;
  uint16_t bank_option = 0U;
  ErrorStatus status   = SUCCESS;
  FLASH_EraseInitTypeDef erase_init_struct;

  /* Unlock the flash memory for erase operation */
  OPENBL_FLASH_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  erase_init_struct.TypeErase = FLASH_TYPEERASE_MASSERASE;

  if (DataLength >= 2)
  {
    bank_option = *(uint16_t *)(pData);

    if (bank_option == FLASH_ERASE_ALL)
    {
      erase_init_struct.Banks = 0U;
    }
    else
    {
      status = ERROR;
    }

    if (status == SUCCESS)
    {
      if (OPENBL_FLASH_ExtendedErase(&erase_init_struct, &page_error) != HAL_OK)
      {
        status = ERROR;
      }
      else
      {
        status = SUCCESS;
      }
    }
  }
  else
  {
    status = ERROR;
  }

  /* Lock the Flash to disable the flash control register access */
  OPENBL_FLASH_Lock();

  return status;
}

/**
  * @brief  This function is used to erase the specified FLASH pages.
  * @param  *pData Pointer to the buffer that contains erase operation options.
  * @param  DataLength Size of the Data buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Erase operation done
  *          - ERROR:   Erase operation failed or the value of one parameter is not ok
  */
ErrorStatus OPENBL_FLASH_Erase(uint8_t *pData, uint32_t DataLength)
{
  uint32_t counter      = 0U;
  uint32_t pages_number = 0U;
  uint32_t page_error   = 0U;
  uint32_t errors       = 0U;
  ErrorStatus status    = SUCCESS;
  FLASH_EraseInitTypeDef erase_init_struct;

  /* Unlock the flash memory for erase operation */
  OPENBL_FLASH_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  pages_number  = (uint32_t)(*(uint16_t *)(pData));
  pData        += 2U;

  erase_init_struct.TypeErase = FLASH_TYPEERASE_PAGES;
  erase_init_struct.NbPages   = 1U;

  for (counter = 0U; ((counter < pages_number) && (counter < (DataLength / 2U))) ; counter++)
  {
    erase_init_struct.Page = ((uint32_t)(*(uint16_t *)(pData)));

    if (erase_init_struct.Page <= 255U)
    {
      erase_init_struct.Banks = FLASH_BANK_1;
    }
    else
    {
      status = ERROR;
    }

    if (status != ERROR)
    {
      if (OPENBL_FLASH_ExtendedErase(&erase_init_struct, &page_error) != HAL_OK)
      {
        errors++;
      }
    }
    else
    {
      /* Reset the status for next erase operation */
      status = SUCCESS;
    }

    pData += 2U;
  }

  /* Lock the Flash to disable the flash control register access */
  OPENBL_FLASH_Lock();

  if (errors > 0U)
  {
    status = ERROR;
  }
  else
  {
    status = SUCCESS;
  }

  return status;
}

/**
 * @brief  This function is used to Set Flash busy state variable to activate busy state sending
 *         during flash operations
 * @retval None.
*/
void OPENBL_Enable_BusyState_Sending(Send_BusyByte_Func *pCallback)
{
  /* Enable Flash busy state sending */
  Flash_BusyState = FLASH_BUSY_STATE_ENABLED;

  /* Update Wait Callback pointer */
  BusyByteCallback = pCallback;
}

/**
 * @brief  This function is used to disable the send of busy state in I2C non stretch mode.
 * @retval None.
*/
void OPENBL_Disable_BusyState_Sending(void)
{
  /* Disable Flash busy state sending */
  Flash_BusyState = FLASH_BUSY_STATE_DISABLED;
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Program double word at a specified FLASH address.
  * @param  Address specifies the address to be programmed.
  * @param  Data specifies the data to be programmed.
  * @retval None.
  */
static void OPENBL_FLASH_ProgramDoubleWord(uint32_t Address, uint64_t Data)
{
  /* Clear all FLASH errors flags before starting write operation */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  OPENBL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, Data);

  /* Disable Flash busy state sending */
  OPENBL_Disable_BusyState_Sending();
}

/**
  * @brief  This function is used to enable write protection of the specified FLASH areas.
  * @param  ListOfPages Contains the list of pages to be protected.
  * @param  Length The length of the list of pages to be protected.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
static ErrorStatus OPENBL_FLASH_EnableWriteProtection(uint8_t *pListOfPages, uint32_t Length)
{
  uint8_t wrp_start_offset = 0x7FU;
  uint8_t wrp_end_offset   = 0x00U;
  ErrorStatus status       = SUCCESS;
  FLASH_OBProgramInitTypeDef flash_ob;

  /* Unlock the FLASH registers & Option Bytes registers access */
  OPENBL_FLASH_OB_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  flash_ob.OptionType = OPTIONBYTE_WRP;

  /* Write protection of bank 1 area WRPA 1 area */
  if (Length >= 2U)
  {
    wrp_start_offset = *(pListOfPages);
    wrp_end_offset   = *(pListOfPages + 1U);

    flash_ob.WRPArea        = OB_WRPAREA_BANK1_AREAA;
    flash_ob.WRPStartOffset = wrp_start_offset;
    flash_ob.WRPEndOffset   = wrp_end_offset;

    OPENBL_FLASH_OptionBytesProgram(&flash_ob);
  }

  /* Write protection of bank 1 area WRPA 2 area */
  if (Length >= 4U)
  {
    wrp_start_offset = *(pListOfPages + 2U);
    wrp_end_offset   = *(pListOfPages + 3U);

    flash_ob.WRPArea        = OB_WRPAREA_BANK1_AREAB;
    flash_ob.WRPStartOffset = wrp_start_offset;
    flash_ob.WRPEndOffset   = wrp_end_offset;

    OPENBL_FLASH_OptionBytesProgram(&flash_ob);
  }

  return status;
}

/**
  * @brief  This function is used to disable write protection.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
static ErrorStatus OPENBL_FLASH_DisableWriteProtection(void)
{
  uint8_t wrp_start_offset = 0x7FU;
  uint8_t wrp_end_offset   = 0x00U;
  ErrorStatus status       = SUCCESS;
  FLASH_OBProgramInitTypeDef flash_ob;

  /* Unlock the FLASH registers & Option Bytes registers access */
  OPENBL_FLASH_OB_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  flash_ob.OptionType = OPTIONBYTE_WRP;

  /* Disable write protection of bank 1 area WRPA 1 area */
  flash_ob.WRPArea        = OB_WRPAREA_BANK1_AREAA;
  flash_ob.WRPStartOffset = wrp_start_offset;
  flash_ob.WRPEndOffset   = wrp_end_offset;

  OPENBL_FLASH_OptionBytesProgram(&flash_ob);

  /* Disable write protection of bank 1 area WRPA 2 area */
  flash_ob.WRPArea        = OB_WRPAREA_BANK1_AREAB;
  flash_ob.WRPStartOffset = wrp_start_offset;
  flash_ob.WRPEndOffset   = wrp_end_offset;

  OPENBL_FLASH_OptionBytesProgram(&flash_ob);

  return status;
}

/**
  * @brief  Perform a mass erase or erase the specified FLASH memory pages.
  * @param[in]  pEraseInit pointer to an FLASH_EraseInitTypeDef structure that
  *         contains the configuration information for the erasing.
  * @param[out]  PageError pointer to variable that contains the configuration
  *         information on faulty page in case of error (0xFFFFFFFF means that all
  *         the pages have been correctly erased).
  * @retval HAL_Status
  */
static HAL_StatusTypeDef OPENBL_FLASH_ExtendedErase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *pPageError)
{
  HAL_StatusTypeDef status;
  uint32_t page_index;

  /* Check the parameters */
  assert_param(IS_FLASH_TYPEERASE(pEraseInit->TypeErase));

  /* Process Locked */
  __HAL_LOCK(&FlashProcess);

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

  if (status == HAL_OK)
  {
    FlashProcess.ErrorCode = HAL_FLASH_ERROR_NONE;

    /* Deactivate the cache if they are activated to avoid data misbehavior */
    if (READ_BIT(FLASH->ACR, FLASH_ACR_ICEN) != 0U)
    {
      /* Disable instruction cache  */
      __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

      if (READ_BIT(FLASH->ACR, FLASH_ACR_DCEN) != 0U)
      {
        /* Disable data cache  */
        __HAL_FLASH_DATA_CACHE_DISABLE();
        FlashProcess.CacheToReactivate = FLASH_CACHE_ICACHE_DCACHE_ENABLED;
      }
      else
      {
        FlashProcess.CacheToReactivate = FLASH_CACHE_ICACHE_ENABLED;
      }
    }
    else if (READ_BIT(FLASH->ACR, FLASH_ACR_DCEN) != 0U)
    {
      /* Disable data cache  */
      __HAL_FLASH_DATA_CACHE_DISABLE();
      FlashProcess.CacheToReactivate = FLASH_CACHE_DCACHE_ENABLED;
    }
    else
    {
      FlashProcess.CacheToReactivate = FLASH_CACHE_DISABLED;
    }

    if (pEraseInit->TypeErase == FLASH_TYPEERASE_MASSERASE)
    {
      /* Check the parameters */
      assert_param(IS_FLASH_BANK(Banks));
      
      /* Mass erase to be done */
      SET_BIT(FLASH->CR, FLASH_CR_MER1);
      
      /* Proceed to erase all sectors */
      SET_BIT(FLASH->CR, FLASH_CR_STRT);

      if (Flash_BusyState == FLASH_BUSY_STATE_ENABLED)
      {
        /* Wait for last operation to be completed to send busy byte*/
        status = OPENBL_FLASH_SendBusyState(FLASH_TIMEOUT_VALUE);
      }
      else
      {
        /* Wait for last operation to be completed */
        status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
      }

      /* If the erase operation is completed, disable the MER1 Bit */
      CLEAR_BIT(FLASH->CR, (FLASH_CR_MER1));
    }
    else
    {
      /*Initialization of PageError variable*/
      *pPageError = 0xFFFFFFFFU;

      for (page_index = pEraseInit->Page; page_index < (pEraseInit->Page + pEraseInit->NbPages); page_index++)
      {
        FLASH_PageErase(page_index, pEraseInit->Banks);

        if (Flash_BusyState == FLASH_BUSY_STATE_ENABLED)
        {
          /* Wait for last operation to be completed to send busy byte*/
          status = OPENBL_FLASH_SendBusyState(FLASH_TIMEOUT_VALUE);
        }
        else
        {
          /* Wait for last operation to be completed */
          status = FLASH_WaitForLastOperation(PROGRAM_TIMEOUT);
        }

        /* If the erase operation is completed, disable the PER Bit */
        CLEAR_BIT(FLASH->CR, (FLASH_CR_PER | FLASH_CR_PNB));

        if (status != HAL_OK)
        {
          /* In case of error, stop erase procedure and return the faulty page */
          *pPageError = page_index;
          break;
        }
      }
    }

    /* Flush the caches to be sure of the data consistency */
    FLASH_FlushCaches();
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&FlashProcess);

  /* Disable Flash busy state sending */
  OPENBL_Disable_BusyState_Sending();

  return status;
}

static HAL_StatusTypeDef OPENBL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data)
{
  HAL_StatusTypeDef status;
  uint32_t prog_bit = 0;

  /* Check the parameters */
  assert_param(IS_FLASH_TYPEPROGRAM(TypeProgram));

  /* Process Locked */
  __HAL_LOCK(&FlashProcess);

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

  if (status == HAL_OK)
  {
    FlashProcess.ErrorCode = HAL_FLASH_ERROR_NONE;

    if (TypeProgram == FLASH_TYPEPROGRAM_DOUBLEWORD)
    {
      /* Program double-word (64-bit) at a specified address */

      /* Check the parameters */
      assert_param(IS_FLASH_PROGRAM_ADDRESS(Address));

      /* Set PG bit */
      SET_BIT(FLASH->CR, FLASH_CR_PG);

      /* Program first word */
      *(uint32_t *)Address = (uint32_t)Data;

      /* Program second word */
      *(uint32_t *)(Address + 4U) = (uint32_t)(Data >> 32U);
      
      prog_bit = FLASH_CR_PG;
    }

    if (Flash_BusyState == FLASH_BUSY_STATE_ENABLED)
    {
      /* Wait for last operation to be completed to send busy byte*/
      status = OPENBL_FLASH_SendBusyState(FLASH_TIMEOUT_VALUE);
    }
    else
    {
      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
    }

    /* If the program operation is completed, disable the PG or FSTPG Bit */
    if (prog_bit != 0U)
    {
      CLEAR_BIT(FLASH->CR, prog_bit);
    }
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&FlashProcess);

  /* return status */
  return status;
}

/**
  * @brief  Program Option bytes.
  * @param  pOBInit pointer to an FLASH_OBInitStruct structure that
  *         contains the configuration information for the programming.
  * @note   To configure any option bytes, the option lock bit OPTLOCK must be
  *         cleared with the call of HAL_FLASH_OB_Unlock() function.
  * @note   New option bytes configuration will be taken into account in two cases:
  *         - after an option bytes launch through the call of HAL_FLASH_OB_Launch()
  *         - after a power reset (BOR reset or exit from Standby/Shutdown modes)
  * @retval HAL_Status
  */
static HAL_StatusTypeDef OPENBL_FLASH_OptionBytesProgram(FLASH_OBProgramInitTypeDef *pOBInit)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_OPTIONBYTE(pOBInit->OptionType));

  /* Process Locked */
  __HAL_LOCK(&FlashProcess);

  FlashProcess.ErrorCode = HAL_FLASH_ERROR_NONE;

  /* Write protection configuration */
  if ((pOBInit->OptionType & OPTIONBYTE_WRP) != 0U)
  {
    /* Configure of Write protection on the selected area */
    if (OPENBL_FLASH_OB_WRPConfig(pOBInit->WRPArea, pOBInit->WRPStartOffset, pOBInit->WRPEndOffset) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  /* Read protection configuration */
  if ((pOBInit->OptionType & OPTIONBYTE_RDP) != 0U)
  {
    /* Configure the Read protection level */
    if (OPENBL_FLASH_OB_RDPConfig(pOBInit->RDPLevel) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&FlashProcess);

  /* Disable Flash busy state sending */
  OPENBL_Disable_BusyState_Sending();

  return status;
}

/**
  * @brief  Configure the write protection area into Option Bytes.
  * @note   When the memory read protection level is selected (RDP level = 1),
  *         it is not possible to program or erase Flash memory if the CPU debug
  *         features are connected (JTAG or single wire) or boot code is being
  *         executed from RAM or System flash, even if WRP is not activated.
  * @note   To configure any option bytes, the option lock bit OPTLOCK must be
  *         cleared with the call of HAL_FLASH_OB_Unlock() function.
  * @note   New option bytes configuration will be taken into account in two cases:
  *         - after an option bytes launch through the call of HAL_FLASH_OB_Launch()
  *         - after a power reset (BOR reset or exit from Standby/Shutdown modes)
  * @param  WRPArea specifies the area to be configured.
  *         This parameter can be one of the following values:
  *            @arg OB_WRPAREA_BANK1_AREAA: Flash Bank 1 Area A
  *            @arg OB_WRPAREA_BANK1_AREAB: Flash Bank 1 Area B
  *            @arg OB_WRPAREA_BANK2_AREAA: Flash Bank 2 Area A (*)
  *            @arg OB_WRPAREA_BANK2_AREAB: Flash Bank 2 Area B (*)
  * @note   (*) availability depends on devices
  * @param  WRPStartOffset specifies the start page of the write protected area.
  *         This parameter can be page number between 0 and (max number of pages in the bank - 1).
  * @param  WRDPEndOffset specifies the end page of the write protected area.
  *         This parameter can be page number between WRPStartOffset and (max number of pages in the bank - 1).
  * @retval HAL_Status
  */
static HAL_StatusTypeDef OPENBL_FLASH_OB_WRPConfig(uint32_t WRPArea, uint32_t WRPStartOffset, uint32_t WRDPEndOffset)
{
  HAL_StatusTypeDef status;

  /* Check the parameters */
  assert_param(IS_OB_WRPAREA(WRPArea));
  assert_param(IS_FLASH_PAGE(WRPStartOffset));
  assert_param(IS_FLASH_PAGE(WRDPEndOffset));

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

  if (status == HAL_OK)
  {
    /* Configure the write protected area */
    if (WRPArea == OB_WRPAREA_BANK1_AREAA)
    {
      FLASH->WRP1AR = ((WRDPEndOffset << FLASH_WRP1AR_WRP1A_END_Pos) | WRPStartOffset);
    }
    else if (WRPArea == OB_WRPAREA_BANK1_AREAB)
    {
      FLASH->WRP1BR = ((WRDPEndOffset << FLASH_WRP1BR_WRP1B_END_Pos) | WRPStartOffset);
    }

    /* Set OPTSTRT Bit */
    SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);

    if (Flash_BusyState == FLASH_BUSY_STATE_ENABLED)
    {
      /* Wait for last operation to be completed to send busy byte*/
      status = OPENBL_FLASH_SendBusyState(FLASH_TIMEOUT_VALUE);
    }
    else
    {
      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
    }
  }

  return status;
}

/**
  * @brief  Set the read protection level into Option Bytes.
  * @note   To configure any option bytes, the option lock bit OPTLOCK must be
  *         cleared with the call of HAL_FLASH_OB_Unlock() function.
  * @note   New option bytes configuration will be taken into account in two cases:
  *         - after an option bytes launch through the call of HAL_FLASH_OB_Launch()
  *         - after a power reset (BOR reset or exit from Standby/Shutdown modes)
  * @note   !!! Warning : When enabling OB_RDP level 2 it's no more possible
  *         to go back to level 1 or 0 !!!
  * @param  RDPLevel specifies the read protection level.
  *         This parameter can be one of the following values:
  *            @arg OB_RDP_LEVEL_0: No protection
  *            @arg OB_RDP_LEVEL_1: Memory Read protection
  *            @arg OB_RDP_LEVEL_2: Full chip protection
  *
  * @retval HAL_Status
  */
static HAL_StatusTypeDef OPENBL_FLASH_OB_RDPConfig(uint32_t RDPLevel)
{
  HAL_StatusTypeDef status;

  /* Check the parameters */
  assert_param(IS_OB_RDP_LEVEL(RDPLevel));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

  if (status == HAL_OK)
  {
    /* Configure the RDP level in the option bytes register */
    MODIFY_REG(FLASH->OPTR, FLASH_OPTR_RDP, RDPLevel);

    /* Set OPTSTRT Bit */
    SET_BIT(FLASH->CR, FLASH_CR_OPTSTRT);

    if (Flash_BusyState == FLASH_BUSY_STATE_ENABLED)
    {
      /* Wait for last operation to be completed to send busy byte*/
      status = OPENBL_FLASH_SendBusyState(FLASH_TIMEOUT_VALUE);
    }
    else
    {
      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
    }
  }
  return status;
}

/**
  * @brief  Wait for a FLASH operation to complete.
  * @param  Timeout maximum flash operation timeout.
  * @retval HAL_Status
  */
static HAL_StatusTypeDef OPENBL_FLASH_SendBusyState(uint32_t Timeout)
{
  /* Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
     Even if the FLASH operation fails, the BUSY flag will be reset and an error
     flag will be set */

  uint32_t tickstart = HAL_GetTick();
  uint32_t error;

  while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY))
  {
    if ((HAL_GetTick() - tickstart) > Timeout)
    {
      return HAL_TIMEOUT;
    }

    BusyByteCallback();
  }

  /* Check FLASH operation error flags */
  error = (FLASH->SR & FLASH_FLAG_SR_ERRORS);

  if (error != 0u)
  {
    /* Save the error code */
    FlashProcess.ErrorCode |= error;

    /* Clear error programming flags */
    __HAL_FLASH_CLEAR_FLAG(error);

    return HAL_ERROR;
  }

  /* Check FLASH End of Operation flag  */
  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP))
  {
    /* Clear FLASH End of Operation pending bit */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
  }

  /* If there is an error flag set */
  return HAL_OK;
}


