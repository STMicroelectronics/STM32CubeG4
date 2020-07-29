/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_pwr_user.c
  * @author  MCD Application Team
  * @brief   USBPD PWR user code
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbpd_pwr_user.h"
#include "stm32g4xx_hal.h"
#if defined(_TRACE)
#include "usbpd_core.h"
#include "usbpd_trace.h"
#endif /* _TRACE */

/* USER CODE BEGIN include */
#include "main.h"
/* USER CODE END include */

/** @addtogroup BSP
  * @{
  */

/** @addtogroup POWER
  * @{
  */
/** @defgroup POWER_Private_Typedef Private Typedef
  * @{
  */
/* USER CODE BEGIN POWER_Private_Typedef */

/* USER CODE END POWER_Private_Typedef */
/**
  * @}
  */

/** @defgroup POWER_Private_Constants Private Constants
* @{
*/
/* USER CODE BEGIN POWER_Private_Constants */
extern ADC_HandleTypeDef hadc1;

#define VSENSE_ADC_INSTANCE             hadc1.Instance

#define VDDA_APPLI            3300U
/* USER CODE END POWER_Private_Constants */
/**
  * @}
  */

/** @defgroup POWER_Private_Macros Private Macros
  * @{
  */
#if defined(_TRACE)
#define PWR_DEBUG_TRACE(_PORT_, __MESSAGE__)  USBPD_TRACE_Add(USBPD_TRACE_DEBUG,    (_PORT_), 0u, (uint8_t*)(__MESSAGE__), sizeof(__MESSAGE__) - 1u)
#else
#define PWR_DEBUG_TRACE(_PORT_, __MESSAGE__)
#endif /* _TRACE */
/* USER CODE BEGIN POWER_Private_Macros */

/* USER CODE END POWER_Private_Macros */
/**
  * @}
  */

/** @defgroup POWER_Private_Variables Private Variables
  * @{
  */
/* USER CODE BEGIN POWER_Private_Variables */

/* USER CODE END POWER_Private_Variables */
/**
  * @}
  */

/** @defgroup POWER_Private_Functions Private Functions
  * @{
  */
/* USER CODE BEGIN POWER_Private_Prototypes */

/* USER CODE END POWER_Private_Prototypes */
/**
  * @}
  */

/** @defgroup POWER_Exported_Variables Exported Variables
  * @{
  */
/* USER CODE BEGIN POWER_Exported_Variables */

/* USER CODE END POWER_Exported_Variables */
/**
  * @}
  */

/** @addtogroup POWER_Exported_Functions
  * @{
  */
/**
  * @brief  Global initialization of PWR resource used by USB-PD
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_Init(uint32_t Instance)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_Init */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_Init */
}

/**
  * @brief  Global de-initialization of PWR resource used by USB-PD
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_Deinit(uint32_t Instance)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_Deinit */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_Deinit */
}

/**
  * @brief  Initialize the hardware resources used by the Type-C power delivery (PD)
  *         controller.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSInit(uint32_t Instance)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSInit */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    LL_ADC_REG_StartConversion(VSENSE_ADC_INSTANCE);
  }

  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSInit */
}

/**
  * @brief  Release the hardware resources used by the Type-C power delivery (PD)
  *         controller.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSDeInit(uint32_t Instance)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSDeInit */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSDeInit */
}

/**
  * @brief  Enable power supply over VBUS.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSOn(uint32_t Instance)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSOn */
  /* Check if instance is valid       */
  int32_t ret;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
    PWR_DEBUG_TRACE(Instance, "HELP: Update BSP_USBPD_PWR_VBUSOn");
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSOn */
}

/**
  * @brief  Disable power supply over VBUS.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSOff(uint32_t Instance)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSOff */
  /* Check if instance is valid       */
  int32_t ret;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
    PWR_DEBUG_TRACE(Instance, "HELP: Update BSP_USBPD_PWR_VBUSOff");
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSOff */
}

/**
  * @brief  Set a fixed/variable PDO and manage the power control.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @param  VbusTargetInmv the vbus Target (in mV)
  * @param  OperatingCurrent the Operating Current (in mA)
  * @param  MaxOperatingCurrent the Max Operating Current (in mA)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSSetVoltage_Fixed(uint32_t Instance,
                                           uint32_t VbusTargetInmv,
                                           uint32_t OperatingCurrent,
                                           uint32_t MaxOperatingCurrent)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSSetVoltage_Fixed */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSSetVoltage_Fixed */
}

/**
  * @brief  Set a fixed/variable PDO and manage the power control.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @param  VbusTargetMinInmv the vbus Target min (in mV)
  * @param  VbusTargetMaxInmv the vbus Target max (in mV)
  * @param  OperatingCurrent the Operating Current (in mA)
  * @param  MaxOperatingCurrent the Max Operating Current (in mA)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSSetVoltage_Variable(uint32_t Instance,
                                              uint32_t VbusTargetMinInmv,
                                              uint32_t VbusTargetMaxInmv,
                                              uint32_t OperatingCurrent,
                                              uint32_t MaxOperatingCurrent)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSSetVoltage_Variable */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSSetVoltage_Variable */
}

/**
  * @brief  Set a Battery PDO and manage the power control.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @param  VbusTargetMin the vbus Target min (in mV)
  * @param  VbusTargetMax the vbus Target max (in mV)
  * @param  OperatingPower the Operating Power (in mW)
  * @param  MaxOperatingPower the Max Operating Power (in mW)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSSetVoltage_Battery(uint32_t Instance,
                                             uint32_t VbusTargetMin,
                                             uint32_t VbusTargetMax,
                                             uint32_t OperatingPower,
                                             uint32_t MaxOperatingPower)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSSetVoltage_Battery */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSSetVoltage_Battery */
}

/**
  * @brief  Set a APDO and manage the power control.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @param  VbusTargetInmv the vbus Target (in mV)
  * @param  OperatingCurrent the Operating current (in mA)
  * @param  Delta Delta between with previous APDO (in mV), 0 means APDO start
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSSetVoltage_APDO(uint32_t Instance,
                                          uint32_t VbusTargetInmv,
                                          uint32_t OperatingCurrent,
                                          int32_t Delta)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSSetVoltage_APDO */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSSetVoltage_APDO */
}

/**
  * @brief  Get actual voltage level measured on the VBUS line.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @param  pVoltage Pointer on measured voltage level (in mV)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSGetVoltage(uint32_t Instance, uint32_t *pVoltage)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSGetVoltage */

  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_NONE;

  if ((Instance >= USBPD_PWR_INSTANCES_NBR) || (NULL == pVoltage))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    uint32_t voltage;

    voltage = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI,  ((uint32_t)LL_ADC_REG_ReadConversionData12(VSENSE_ADC_INSTANCE)), LL_ADC_RESOLUTION_12B); /* mV */

    /* STM32G474E_EVAL board is used */
    /* Value is multiplied by 7.030 according to board measurements.
       Theorically, it should have been 7.613 (Divider R323/R244 (49.9K/330K) for VSENSE */
    voltage *= 7030U;
    voltage /= 1000U;

    *pVoltage = voltage;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSGetVoltage */
}

/**
  * @brief  Get actual current level measured on the VBUS line.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @param  pCurrent Pointer on measured current level (in mA)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSGetCurrent(uint32_t Instance, int32_t *pCurrent)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSGetCurrent */
  /* Check if instance is valid       */
  int32_t ret;

  if ((Instance >= USBPD_PWR_INSTANCES_NBR) || (NULL == pCurrent))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    *pCurrent = 0;
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSGetCurrent */
}

/**
  * @brief  Initialize VCONN sourcing.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_CC1
  *         @arg @ref USBPD_PWR_TYPE_C_CC2
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VCONNInit(uint32_t Instance,
                                uint32_t CCPinId)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VCONNInit */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VCONNInit */
}

/**
  * @brief  Un-Initialize VCONN sourcing.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_CC1
  *         @arg @ref USBPD_PWR_TYPE_C_CC2
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VCONNDeInit(uint32_t Instance,
                                  uint32_t CCPinId)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VCONNDeInit */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VCONNDeInit */
}

/**
  * @brief  Enable VCONN sourcing.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_CC1
  *         @arg @ref USBPD_PWR_TYPE_C_CC2
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VCONNOn(uint32_t Instance,
                              uint32_t CCPinId)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VCONNOn */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VCONNOn */
}

/**
  * @brief  Disable VCONN sourcing.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @param  CCPinId CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_CC1
  *         @arg @ref USBPD_PWR_TYPE_C_CC2
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VCONNOff(uint32_t Instance,
                               uint32_t CCPinId)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VCONNOff */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VCONNOff */
}

/**
  * @brief  Get actual VCONN status.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_CC1
  *         @arg @ref USBPD_PWR_TYPE_C_CC2
  * @param  pState VCONN status (1: On, 0: Off)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VCONNIsOn(uint32_t Instance,
                                uint32_t CCPinId, uint8_t *pState)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VCONNIsOn */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  *pState = 0u;
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VCONNIsOn */
}

/**
  * @brief  Set the VBUS disconnection voltage threshold.
  * @note   Callback funtion registered through BSP_USBPD_PWR_RegisterVBUSDetectCallback
  *         function call is invoked when VBUS falls below programmed threshold.
  * @note   By default VBUS disconnection threshold is set to 3.3V
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @param  VoltageThreshold VBUS disconnection voltage threshold (in mV)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_SetVBUSDisconnectionThreshold(uint32_t Instance,
                                                    uint32_t VoltageThreshold)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_SetVBUSDisconnectionThreshold */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_SetVBUSDisconnectionThreshold */
}

/**
  * @brief  Register USB Type-C Current callback function.
  * @note   Callback function invoked when VBUS rises above 4V (VBUS present) or
  *         when VBUS falls below programmed threshold (VBUS absent).
  * @note   Callback funtion is un-registered when callback function pointer
  *         argument is NULL.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @param  pfnVBUSDetectCallback callback function pointer
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_RegisterVBUSDetectCallback(uint32_t  Instance,
                                                 USBPD_PWR_VBUSDetectCallbackFunc *pfnVBUSDetectCallback)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_RegisterVBUSDetectCallback */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_RegisterVBUSDetectCallback */
}

/**
  * @brief  Get actual VBUS status.
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @param  pState VBUS status (1: On, 0: Off)
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VBUSIsOn(uint32_t Instance, uint8_t *pState)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VBUSIsOn */
  /* Check if instance is valid       */
  int32_t ret;

  if (Instance >= USBPD_PWR_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
    PWR_DEBUG_TRACE(Instance, "HELP: Update BSP_USBPD_PWR_VBUSIsOn");
  }
  *pState = 0u;
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VBUSIsOn */
}

/**
  * @brief  Set state of VCC (specific functions)
  * @param  Instance Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg @ref USBPD_PWR_TYPE_C_PORT_1
  * @param  State VCC state
  * @retval BSP status
  */
__weak int32_t BSP_USBPD_PWR_VCCSetState(uint32_t Instance, uint32_t State)
{
  /* USER CODE BEGIN BSP_USBPD_PWR_VCCSetState */
  /* Check if instance is valid       */
  int32_t ret = BSP_ERROR_NONE;

  if (Instance != USBPD_PWR_TYPE_C_PORT_1)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
    PWR_DEBUG_TRACE(Instance, "ADVICE: Update BSP_USBPD_PWR_VCCSetState");
  }
  return ret;
  /* USER CODE END BSP_USBPD_PWR_VCCSetState */
}

/**
  * @}
  */

/** @addtogroup POWER_Private_Functions
  * @{
  */

/* USER CODE BEGIN POWER_Private_Functions */

/* USER CODE END POWER_Private_Functions */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

