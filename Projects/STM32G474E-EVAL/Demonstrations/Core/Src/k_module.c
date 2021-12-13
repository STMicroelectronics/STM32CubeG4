/**
  ******************************************************************************
  * @file    k_module.c
  * @author  MCD Application Team
  * @brief   This file provides the kernel module functions
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


