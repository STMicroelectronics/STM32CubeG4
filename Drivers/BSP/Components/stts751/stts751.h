/**
 ******************************************************************************
 * @file    stts751.h
 * @author  MEMS Software Solutions Team
 * @brief   STTS751 header driver file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STTS751_H
#define STTS751_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stts751_reg.h"
#include <string.h>

/** @addtogroup BSP BSP
 * @{
 */

/** @addtogroup Component Component
 * @{
 */

/** @addtogroup STTS751 STTS751
 * @{
 */

/** @defgroup STTS751_Exported_Types STTS751 Exported Types
 * @{
 */

typedef int32_t (*STTS751_Init_Func)(void);
typedef int32_t (*STTS751_DeInit_Func)(void);
typedef int32_t (*STTS751_GetTick_Func)(void);
typedef int32_t (*STTS751_WriteReg_Func)(uint16_t, uint16_t, uint8_t *, uint16_t);
typedef int32_t (*STTS751_ReadReg_Func)(uint16_t, uint16_t, uint8_t *, uint16_t);

typedef struct
{
  STTS751_Init_Func          Init;
  STTS751_DeInit_Func        DeInit;
  uint32_t                   BusType; /*0 means I2C */
  uint8_t                    Address;
  STTS751_WriteReg_Func      WriteReg;
  STTS751_ReadReg_Func       ReadReg;
  STTS751_GetTick_Func       GetTick;
} STTS751_IO_t;

typedef struct
{
  STTS751_IO_t       IO;
  stmdev_ctx_t       Ctx;
  uint8_t            is_initialized;
  uint8_t            temp_is_enabled;
  float              temp_odr;
} STTS751_Object_t;

typedef struct
{
  uint8_t Temperature;
  uint8_t Pressure;
  uint8_t Humidity;
  uint8_t LowPower;
  float   HumMaxOdr;
  float   TempMaxOdr;
  float   PressMaxOdr;
} STTS751_Capabilities_t;

typedef struct
{
  int32_t (*Init)(STTS751_Object_t *);
  int32_t (*DeInit)(STTS751_Object_t *);
  int32_t (*ReadID)(STTS751_Object_t *, uint8_t *);
  int32_t (*GetCapabilities)(STTS751_Object_t *, STTS751_Capabilities_t *);
} STTS751_CommonDrv_t;

typedef struct
{
  int32_t (*Enable)(STTS751_Object_t *);
  int32_t (*Disable)(STTS751_Object_t *);
  int32_t (*GetOutputDataRate)(STTS751_Object_t *, float *);
  int32_t (*SetOutputDataRate)(STTS751_Object_t *, float);
  int32_t (*GetTemperature)(STTS751_Object_t *, float *);
} STTS751_TEMP_Drv_t;

/**
 * @}
 */

/** @defgroup STTS751_Exported_Constants STTS751 Exported Constants
 * @{
 */
#define STTS751_I2C_BUS           0U

/** STTS751 error codes  **/
#define STTS751_OK                 0
#define STTS751_ERROR             -1

/**
 * @}
 */

/** @addtogroup STTS751_Exported_Functions STTS751 Exported Functions
 * @{
 */

int32_t STTS751_RegisterBusIO(STTS751_Object_t *pObj, STTS751_IO_t *pIO);
int32_t STTS751_Init(STTS751_Object_t *pObj);
int32_t STTS751_DeInit(STTS751_Object_t *pObj);
int32_t STTS751_ReadID(STTS751_Object_t *pObj, uint8_t *Id);
int32_t STTS751_GetCapabilities(STTS751_Object_t *pObj, STTS751_Capabilities_t *Capabilities);
int32_t STTS751_Get_Init_Status(STTS751_Object_t *pObj, uint8_t *Status);

int32_t STTS751_TEMP_Enable(STTS751_Object_t *pObj);
int32_t STTS751_TEMP_Disable(STTS751_Object_t *pObj);
int32_t STTS751_TEMP_GetOutputDataRate(STTS751_Object_t *pObj, float *Odr);
int32_t STTS751_TEMP_SetOutputDataRate(STTS751_Object_t *pObj, float Odr);
int32_t STTS751_TEMP_GetTemperature(STTS751_Object_t *pObj, float *Value);
int32_t STTS751_TEMP_Get_DRDY_Status(STTS751_Object_t *pObj, uint8_t *Status);
int32_t STTS751_TEMP_SetLowTemperatureThreshold(STTS751_Object_t *pObj, float Value);
int32_t STTS751_TEMP_SetHighTemperatureThreshold(STTS751_Object_t *pObj, float Value);
int32_t STTS751_TEMP_GetTemperatureLimitStatus(STTS751_Object_t *pObj, uint8_t *HighLimit, uint8_t *LowLimit, uint8_t *ThermLimit);
int32_t STTS751_TEMP_SetEventPin(STTS751_Object_t *pObj, uint8_t Enable);

int32_t STTS751_Read_Reg(STTS751_Object_t *pObj, uint8_t Reg, uint8_t *Data);
int32_t STTS751_Write_Reg(STTS751_Object_t *pObj, uint8_t Reg, uint8_t Data);

int32_t STTS751_Set_One_Shot(STTS751_Object_t *pObj);
int32_t STTS751_Get_One_Shot_Status(STTS751_Object_t *pObj, uint8_t *Status);

/**
 * @}
 */

/** @addtogroup STTS751_Exported_Variables STTS751 Exported Variables
 * @{
 */

extern STTS751_CommonDrv_t STTS751_COMMON_Driver;
extern STTS751_TEMP_Drv_t STTS751_TEMP_Driver;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif

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
