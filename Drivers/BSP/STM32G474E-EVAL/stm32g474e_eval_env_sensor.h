/**
  ******************************************************************************
  * @file    stm32g474e_eval_env_sensor.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the
  *          stm32g474e_eval_env_sensor.c firmware driver.
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
#ifndef STM32G474E_EVAL_ENV_SENSOR_H
#define STM32G474E_EVAL_ENV_SENSOR_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g474e_eval_conf.h"
#include "stm32g474e_eval_errno.h"

#include "../Components/stts751/stts751.h"
#include "../Components/Common/env_sensor.h"

#include <math.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @addtogroup STM32G474E-EVAL_ENV_SENSOR
  * @{
  */

/** @defgroup STM32G474E-EVAL_ENV_SENSOR_Exported_Types Exported Types
  * @{
  */

/* Environmental Sensor instance Info */
typedef struct
{
  uint8_t Temperature;
  uint8_t Pressure;
  uint8_t Humidity;
  uint8_t LowPower;
  float_t HumMaxOdr;
  float_t TempMaxOdr;
  float_t PressMaxOdr;
} ENV_SENSOR_Capabilities_t;

typedef struct
{
  uint32_t Functions;
} ENV_SENSOR_Ctx_t;
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_ENV_SENSOR_Exported_Constants Exported Constants
  * @{
  */
#ifndef USE_ENV_SENSOR_STTS751
#define USE_ENV_SENSOR_STTS751          1U
#endif

/* Temperature Sensor hardware I2C address : 1001000 */
#define STTS751_I2C_ADDRESS             0x90

#define ENV_TEMPERATURE      1U

#if defined(STTS751_MANUFACTURER)
#define STTS751_ID           STTS751_MANUFACTURER
#elif defined(STTS751_ID_MAN)
#define STTS751_ID           STTS751_ID_MAN
#else
#error "STTS751_ID not defined"
#endif

#define ENV_FUNCTIONS_NBR    1U
#define ENV_INSTANCES_NBR    (USE_ENV_SENSOR_STTS751)
#define ENV_INSTANCE         0U

#if (ENV_INSTANCES_NBR == 0)
#error "No environmental sensor instance has been selected"
#endif
/**
  * @}
  */

/** @addtogroup STM32G474E-EVAL_ENV_SENSOR_Exported_Variables
  * @{
  */
extern void *EnvCompObj;
extern ENV_SENSOR_Ctx_t EnvCtx;
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_ENV_SENSOR_Exported_Functions Exported Functions
  * @{
  */
int32_t BSP_ENV_SENSOR_Init(uint32_t Instance, uint32_t Functions);
int32_t BSP_ENV_SENSOR_DeInit(uint32_t Instance);
int32_t BSP_ENV_SENSOR_GetCapabilities(uint32_t Instance, ENV_SENSOR_Capabilities_t *Capabilities);
int32_t BSP_ENV_SENSOR_ReadID(uint32_t Instance, uint8_t *Id);
int32_t BSP_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float_t *Odr);
int32_t BSP_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float_t Odr);
int32_t BSP_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float_t *Value);
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
#ifdef __cplusplus
}
#endif

#endif /* STM32G474E_EVAL_ENV_SENSOR_H */
