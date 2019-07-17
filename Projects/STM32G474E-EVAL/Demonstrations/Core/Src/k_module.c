/**
  ******************************************************************************
  * @file    k_module.c
  * @author  MCD Application Team
  * @brief   This file provides the kernel module functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include <string.h>
#include "main.h"
#include "k_config.h"
#include "k_module.h"

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static K_ModuleItem_Typedef    kmodule_info[MODULE_MAX];
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Add module inside the module list
  * @param  kModuleId    module id
            kModuleInfo  module information
  * @retval none
  */
void kModule_Add(uint8_t kModuleId, K_ModuleItem_Typedef kModuleInfo)
{
  kmodule_info[kModuleId] = kModuleInfo;
}

/**
  * @brief  Check if the ressouce associated to a module are present
  * @param  None
  * @retval KMODULE_RETURN status
  */
KMODULE_RETURN kModule_CheckRessource(void)
{
uint8_t index;

  for(index = 0; index <  MODULE_MAX; index++)
  {
    if(kmodule_info[index].kModuleRessouceCheck != NULL)
    {
      if(kmodule_info[index].kModuleRessouceCheck() != KMODULE_OK)
      {
        return KMODULE_RESMISSIG;
      }
    }
  }
  return KMODULE_OK;
}

/**
  * @brief  Execute a module
  * @param  moduleid   id of the module to execute
  * @retval KMODULE_RETURN status
  */
KMODULE_RETURN kModule_Execute(uint8_t moduleid)
{

  /* Module Preprocessing  */
  if(kmodule_info[moduleid].kModulePreExec != NULL)
  {
    if(kmodule_info[moduleid].kModulePreExec() != KMODULE_OK)
    {
      return KMODULE_ERROR_PRE;
    }
  }

  /* Module Execution      */
  if(kmodule_info[moduleid].kModuleExec != NULL)
  {
    if(kmodule_info[moduleid].kModuleExec() != KMODULE_OK)
    {
      return KMODULE_ERROR_EXEC;
    }
  }

  /* Module Postprocessing */
  if(kmodule_info[moduleid].kModulePostExec != NULL)
  {
    if(kmodule_info[moduleid].kModulePostExec() != KMODULE_OK)
    {
      return KMODULE_ERROR_POST;
    }
  }

  return KMODULE_OK;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
