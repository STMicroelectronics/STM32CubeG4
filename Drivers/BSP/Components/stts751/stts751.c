/**
 ******************************************************************************
 * @file    stts751.c
 * @author  MEMS Software Solutions Team
 * @brief   STTS751 driver file
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
#include "stts751.h"

/** @addtogroup BSP BSP
 * @{
 */

/** @addtogroup Component Component
 * @{
 */

/** @defgroup STTS751 STTS751
 * @{
 */

/** @defgroup STTS751_Exported_Variables STTS751 Exported Variables
 * @{
 */

STTS751_CommonDrv_t STTS751_COMMON_Driver =
{
  STTS751_Init,
  STTS751_DeInit,
  STTS751_ReadID,
  STTS751_GetCapabilities,
};

STTS751_TEMP_Drv_t STTS751_TEMP_Driver =
{
  STTS751_TEMP_Enable,
  STTS751_TEMP_Disable,
  STTS751_TEMP_GetOutputDataRate,
  STTS751_TEMP_SetOutputDataRate,
  STTS751_TEMP_GetTemperature,
};

/**
 * @}
 */

/** @defgroup STTS751_Private_Function_Prototypes STTS751 Private Function Prototypes
 * @{
 */

static int32_t ReadRegWrap(void *Handle, uint8_t Reg, uint8_t *pData, uint16_t Length);
static int32_t WriteRegWrap(void *Handle, uint8_t Reg, uint8_t *pData, uint16_t Length);

/**
 * @}
 */

/** @defgroup STTS751_Exported_Functions STTS751 Exported Functions
 * @{
 */

/**
 * @brief  Register Component Bus IO operations
 * @param  pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_RegisterBusIO(STTS751_Object_t *pObj, STTS751_IO_t *pIO)
{
  int32_t ret;

  if (pObj == NULL)
  {
    ret = STTS751_ERROR;
  }
  else
  {
    pObj->IO.Init      = pIO->Init;
    pObj->IO.DeInit    = pIO->DeInit;
    pObj->IO.BusType   = pIO->BusType;
    pObj->IO.Address   = pIO->Address;
    pObj->IO.WriteReg  = pIO->WriteReg;
    pObj->IO.ReadReg   = pIO->ReadReg;
    pObj->IO.GetTick   = pIO->GetTick;

    pObj->Ctx.read_reg  = ReadRegWrap;
    pObj->Ctx.write_reg = WriteRegWrap;
    pObj->Ctx.handle   = pObj;

    if (pObj->IO.Init != NULL)
    {
      ret = pObj->IO.Init();
    }
    else
    {
      ret = STTS751_ERROR;
    }
  }

  return ret;
}

/**
 * @brief  Initialize the STTS751 sensor
 * @param  pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_Init(STTS751_Object_t *pObj)
{
  if (pObj->is_initialized == 0U)
  {
    /* Disable EVENT pin of SMBus. */
    if (stts751_pin_event_route_set(&(pObj->Ctx),  PROPERTY_ENABLE) != STTS751_OK)
    {
      return STTS751_ERROR;
    }

    /* Set default ODR */
    pObj->temp_odr = 1.0f;

    /* Set the resolution to the maximum allowed value */
    if (stts751_resolution_set(&(pObj->Ctx), STTS751_12bit) != STTS751_OK)
    {
      return STTS751_ERROR;
    }

    /* Put the component in standby mode. */
    if (stts751_temp_data_rate_set(&(pObj->Ctx), STTS751_TEMP_ODR_OFF) != STTS751_OK)
    {
      return STTS751_ERROR;
    }
  }

  pObj->is_initialized = 1;

  return STTS751_OK;
}

/**
 * @brief  Deinitialize the STTS751 sensor
 * @param  pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_DeInit(STTS751_Object_t *pObj)
{
  if (pObj->is_initialized == 1U)
  {
    /* Put the component in standby mode */
    if (STTS751_TEMP_Disable(pObj) != STTS751_OK)
    {
      return STTS751_ERROR;
    }
  }

  pObj->is_initialized = 0;

  return STTS751_OK;
}

/**
 * @brief  Get WHO_AM_I value
 * @param  pObj the device pObj
 * @param  Id the WHO_AM_I value
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_ReadID(STTS751_Object_t *pObj, uint8_t *Id)
{
  stts751_id_t buf;

  if (stts751_device_id_get(&(pObj->Ctx), &buf) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  *Id = buf.manufacturer_id;

  return STTS751_OK;
}

/**
 * @brief  Get STTS751 sensor capabilities
 * @param  pObj Component object pointer
 * @param  Capabilities pointer to STTS751 sensor capabilities
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_GetCapabilities(STTS751_Object_t *pObj, STTS751_Capabilities_t *Capabilities)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);

  Capabilities->Humidity    = 0;
  Capabilities->Pressure    = 0;
  Capabilities->Temperature = 1;
  Capabilities->LowPower    = 0;
  Capabilities->HumMaxOdr   = 0.0f;
  Capabilities->TempMaxOdr  = 32.0f;
  Capabilities->PressMaxOdr = 0.0f;
  return STTS751_OK;
}

/**
 * @brief  Get the STTS751 initialization status
 * @param  pObj the device pObj
 * @param  Status 1 if initialized, 0 otherwise
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_Get_Init_Status(STTS751_Object_t *pObj, uint8_t *Status)
{
  if (pObj == NULL)
  {
    return STTS751_ERROR;
  }

  *Status = pObj->is_initialized;

  return STTS751_OK;
}

/**
 * @brief  Enable the STTS751 temperature sensor
 * @param  pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_TEMP_Enable(STTS751_Object_t *pObj)
{
  /* Check if the component is already enabled */
  if (pObj->temp_is_enabled == 1U)
  {
    return STTS751_OK;
  }

  /* Power on the component and set the odr. */
  if (STTS751_TEMP_SetOutputDataRate(pObj, pObj->temp_odr) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  pObj->temp_is_enabled = 1;

  return STTS751_OK;
}

/**
 * @brief  Disable the STTS751 temperature sensor
 * @param  pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_TEMP_Disable(STTS751_Object_t *pObj)
{
  /* Check if the component is already disabled */
  if (pObj->temp_is_enabled == 0U)
  {
    return STTS751_OK;
  }

  /* Save the current odr. */
  if (STTS751_TEMP_GetOutputDataRate(pObj, &pObj->temp_odr) != STTS751_OK)
  {
    return STTS751_ERROR;
  }
  
  /* Put the component in standby mode. */
  if (stts751_temp_data_rate_set(&(pObj->Ctx), STTS751_TEMP_ODR_OFF) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  pObj->temp_is_enabled = 0;

  return STTS751_OK;
}

/**
 * @brief  Get the STTS751 temperature sensor output data rate
 * @param  pObj the device pObj
 * @param  Odr pointer where the output data rate is written
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_TEMP_GetOutputDataRate(STTS751_Object_t *pObj, float *Odr)
{
  int32_t ret = STTS751_OK;
  stts751_odr_t odr_low_level;

  if (stts751_temp_data_rate_get(&(pObj->Ctx), &odr_low_level) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  switch (odr_low_level)
  {
    case STTS751_TEMP_ODR_OFF:
	case STTS751_TEMP_ODR_ONE_SHOT:
      *Odr = 0.0f;
      break;

    case STTS751_TEMP_ODR_62mHz5:
      *Odr = 0.0625f;
      break;

    case STTS751_TEMP_ODR_125mHz:
      *Odr = 0.125f;
      break;

    case STTS751_TEMP_ODR_250mHz:
      *Odr = 0.250f;
      break;

    case STTS751_TEMP_ODR_500mHz:
      *Odr = 0.500f;
      break;

    case STTS751_TEMP_ODR_1Hz:
      *Odr = 1.0f;
      break;

    case STTS751_TEMP_ODR_2Hz:
      *Odr = 2.0f;
      break;

    case STTS751_TEMP_ODR_4Hz:
      *Odr = 4.0f;
      break;

    case STTS751_TEMP_ODR_8Hz:
      *Odr = 8.0f;
      break;

    case STTS751_TEMP_ODR_16Hz:
      *Odr = 16.0f;
      break;

    case STTS751_TEMP_ODR_32Hz:
      *Odr = 32.0f;
      break;

    default:
      ret = STTS751_ERROR;
      break;
  }

  return ret;
}

/**
 * @brief  Set the STTS751 temperature sensor output data rate
 * @param  pObj the device pObj
 * @param  Odr the output data rate value to be set
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_TEMP_SetOutputDataRate(STTS751_Object_t *pObj, float Odr)
{
  stts751_odr_t new_odr;
  stts751_tres_t res;

  /* Get the current resolution */
  if (stts751_resolution_get(&(pObj->Ctx), &res) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  /* If the requested odr is 16Hz we cannot use the 12 bits resolution */
  if(Odr == 16.0f && res == STTS751_12bit)
  {
    /* We force resolution to the maximum allowed value */
    if (stts751_resolution_set(&(pObj->Ctx), STTS751_11bit) != STTS751_OK)
    {
      return STTS751_ERROR;
    }
  }

  /* If the requested odr is 32Hz we cannot use the 12 bits and 11 bits resolutions */
  if(Odr == 32.0f && (res == STTS751_12bit || res == STTS751_11bit))
  {
    /* We force resolution to the maximum allowed value */
    if (stts751_resolution_set(&(pObj->Ctx), STTS751_10bit) != STTS751_OK)
    {
      return STTS751_ERROR;
    }
  }

  new_odr = (Odr <= 0.0625f) ? STTS751_TEMP_ODR_62mHz5
          : (Odr <= 0.125f ) ? STTS751_TEMP_ODR_125mHz
          : (Odr <= 0.25f  ) ? STTS751_TEMP_ODR_250mHz
          : (Odr <= 0.5f   ) ? STTS751_TEMP_ODR_500mHz
          : (Odr <= 1.0f   ) ? STTS751_TEMP_ODR_1Hz
          : (Odr <= 2.0f   ) ? STTS751_TEMP_ODR_2Hz
          : (Odr <= 4.0f   ) ? STTS751_TEMP_ODR_4Hz
          : (Odr <= 8.0f   ) ? STTS751_TEMP_ODR_8Hz
          : (Odr <= 16.0f  ) ? STTS751_TEMP_ODR_16Hz
          :                    STTS751_TEMP_ODR_32Hz;

  if (stts751_temp_data_rate_set(&(pObj->Ctx), new_odr) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  return STTS751_OK;
}

/**
 * @brief  Get the STTS751 temperature value
 * @param  pObj the device pObj
 * @param  Value pointer where the temperature value is written
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_TEMP_GetTemperature(STTS751_Object_t *pObj, float *Value)
{
  int16_t raw_value;

  /* Get the temperature */
  if (stts751_temperature_raw_get(&(pObj->Ctx), &raw_value) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  *Value = stts751_from_lsb_to_celsius(raw_value);

  return STTS751_OK;
}

/**
 * @brief  Get the STTS751 temperature data ready bit value
 * @param  pObj the device pObj
 * @param  Status the status of data ready bit
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_TEMP_Get_DRDY_Status(STTS751_Object_t *pObj, uint8_t *Status)
{
  uint8_t val;

  if (stts751_flag_busy_get(&(pObj->Ctx), &val) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  if(val)
  {
    *Status = 0;
  } else
  {
    *Status = 1;
  }

  return STTS751_OK;
}

/**
 * @brief  Set the STTS751 high temperature threshold value
 * @param  pObj the device pObj
 * @param  Value the high temperature threshold to be set
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_TEMP_SetHighTemperatureThreshold(STTS751_Object_t *pObj, float Value)
{
  int16_t raw_value;

  raw_value = stts751_from_celsius_to_lsb(Value);

  /* Set the temperature threshold */
  if (stts751_high_temperature_threshold_set(&(pObj->Ctx), raw_value) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  return STTS751_OK;
}

/**
 * @brief  Set the STTS751 low temperature threshold value
 * @param  pObj the device pObj
 * @param  Value the high temperature threshold to be set
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_TEMP_SetLowTemperatureThreshold(STTS751_Object_t *pObj, float Value)
{
  int16_t raw_value;

  raw_value = stts751_from_celsius_to_lsb(Value);

  /* Set the temperature threshold */
  if (stts751_low_temperature_threshold_set(&(pObj->Ctx), raw_value) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  return STTS751_OK;
}

/**
 * @brief  Get the STTS751 temperature limits status
 * @param  pObj the device pObj
 * @param  HighLimit indicates that high temperature limit has been exceeded
 * @param  LowhLimit indicates that low temperature limit has been exceeded
 * @param  ThermLimit indicates that therm temperature limit has been exceeded
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_TEMP_GetTemperatureLimitStatus(STTS751_Object_t *pObj, uint8_t *HighLimit, uint8_t *LowLimit, uint8_t *ThermLimit)
{
  stts751_status_t status;

  /* Read status register */
  if (stts751_status_reg_get(&(pObj->Ctx), &status) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  if(HighLimit != NULL)
  {
    *HighLimit = status.t_high;
  }

  if(LowLimit != NULL)
  {
    *LowLimit = status.t_low;
  }

  if(ThermLimit != NULL)
  {
    *ThermLimit = status.thrm;
  }

  return STTS751_OK;
}

/**
 * @brief  Enable or disable interrupt on EVENT pin
 * @param  pObj the device pObj
 * @param  Enable 0 disable the EVENT pin, 1 enable EVENT pin
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_TEMP_SetEventPin(STTS751_Object_t *pObj, uint8_t Enable)
{
  uint8_t state;

  /* The MASK1 bit in configuration register has inverted logic */
  if (Enable == 0) state = PROPERTY_ENABLE; else state = PROPERTY_DISABLE;

  if (stts751_pin_event_route_set(&(pObj->Ctx),  state) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  return STTS751_OK;
}

/**
 * @brief  Get the STTS751 register value
 * @param  pObj the device pObj
 * @param  Reg address to be read
 * @param  Data pointer where the value is written
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_Read_Reg(STTS751_Object_t *pObj, uint8_t Reg, uint8_t *Data)
{
  if (stts751_read_reg(&(pObj->Ctx), Reg, Data, 1) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  return STTS751_OK;
}

/**
 * @brief  Set the STTS751 register value
 * @param  pObj the device pObj
 * @param  Reg address to be written
 * @param  Data value to be written
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_Write_Reg(STTS751_Object_t *pObj, uint8_t Reg, uint8_t Data)
{
  if (stts751_write_reg(&(pObj->Ctx), Reg, &Data, 1) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  return STTS751_OK;
}

/**
 * @brief  Set the STTS751 One Shot Mode
 * @param  pObj the device pObj
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_Set_One_Shot(STTS751_Object_t *pObj)
{
  /* Start One Shot Measurement */
  if(stts751_temp_data_rate_set(&(pObj->Ctx), STTS751_TEMP_ODR_ONE_SHOT) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  return STTS751_OK;
}

/**
 * @brief  Get the STTS751 One Shot Status
 * @param  pObj the device pObj
 * @param  Status pointer to the one shot status (1 means measurements available, 0 means measurements not available yet)
 * @retval 0 in case of success, an error code otherwise
 */
int32_t STTS751_Get_One_Shot_Status(STTS751_Object_t *pObj, uint8_t *Status)
{
  uint8_t busy;

  /* Get Busy flag */
  if(stts751_flag_busy_get(&(pObj->Ctx), &busy) != STTS751_OK)
  {
    return STTS751_ERROR;
  }

  if(busy)
  {
    *Status = 0;
  }
  else
  {
    *Status = 1;
  }

  return STTS751_OK;
}

/**
 * @}
 */

/** @defgroup STTS751_Private_Functions STTS751 Private Functions
 * @{
 */

/**
 * @brief  Wrap Read register component function to Bus IO function
 * @param  Handle the device handler
 * @param  Reg the register address
 * @param  pData the stored data pointer
 * @param  Length the length
 * @retval 0 in case of success, an error code otherwise
 */
static int32_t ReadRegWrap(void *Handle, uint8_t Reg, uint8_t *pData, uint16_t Length)
{
  uint16_t i;
  int32_t ret = STTS751_OK;
  STTS751_Object_t *pObj = (STTS751_Object_t *)Handle;

  if (pObj->IO.BusType == (uint32_t)STTS751_I2C_BUS) /* I2C */
  {
    for (i = 0; i < Length; i++)
    {
      ret = pObj->IO.ReadReg(pObj->IO.Address, (Reg + i), &pData[i], 1);
      if (ret != STTS751_OK)
      {
        return STTS751_ERROR;
      }
    }
  }

  return ret;
}

/**
 * @brief  Wrap Write register component function to Bus IO function
 * @param  Handle the device handler
 * @param  Reg the register address
 * @param  pData the stored data pointer
 * @param  Length the length
 * @retval 0 in case of success, an error code otherwise
 */
static int32_t WriteRegWrap(void *Handle, uint8_t Reg, uint8_t *pData, uint16_t Length)
{
  uint16_t i;
  int32_t ret = STTS751_OK;
  STTS751_Object_t *pObj = (STTS751_Object_t *)Handle;

  if (pObj->IO.BusType == (uint32_t)STTS751_I2C_BUS) /* I2C */
  {
	for (i = 0; i < Length; i++)
    {
      ret = pObj->IO.WriteReg(pObj->IO.Address, (Reg + i), &pData[i], 1);
      if (ret != STTS751_OK)
      {
        return STTS751_ERROR;
      }
    }
  }

  return ret;
}

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
