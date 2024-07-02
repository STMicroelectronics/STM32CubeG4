/**
  ******************************************************************************
  * @file    SMBUS/SMBUS_TSENSOR/Src/stm32g4xx_tsensor.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the SMBUS  
  *          temperature sensor (STTS751) mounted on STM32G474E-EVAL1 board . 
  *          It implements a high level communication layer for initialized,
  *          read temperature or get the status of the temperature sensor.
  *          This driver implements too the SMBUS ALERT protocol.
  *     +--------------------------------------------------------------------+
  *     |                        Pin assignment                              |                 
  *     +----------------------------------------+--------------+------------+
  *     |  STM32G4xx SMBUS Pins                  |   STTS751    |   Pin      |
  *     +----------------------------------------+--------------+------------+
  *     | .                                      |   Addr/Therm |    1       |
  *     | .                                      |   GND        |    2 (0V)  |
  *     | .                                      |   VDD        |    3 (3.3V)|
  *     | TSENSOR_SMBUS_SCL_PIN/ SCL             |   SCL        |    4       |
  *     | TSENSOR_SMBUS_ALERT_PIN/ SMBUS ALERT   |   EVENT      |    5       |
  *     | TSENSOR_SMBUS_SDA_PIN/ SDA             |   SDA        |    6       |
  *     +----------------------------------------+--------------+------------+
  *      =================================================================== 
  *     Notes:
  *         - The Temperature Sensor (STTS751) is compatible
  *           with the SMBUS protocol.
  *         - Jumper JP5 and JP6 needs to be set in I2C2 position.
  *         - Jumper JP1 (SMB) needs to be set.
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
#include "stm32g4xx_tsensor.h"

/** @addtogroup STM32G4xx_HAL_Examples
  * @{
  */

/** @addtogroup SMBUS_TSENSOR
  * @{
  */ 

/** @defgroup TSENSOR
  * @brief     This file provides a set of functions needed to drive the 
  *            Temperature Sensor STTS751.
  * @{
  */

/** @defgroup TSENSOR_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @defgroup TSENSOR_Private_Defines
  * @{
  */
#define TSENSOR_SMBUS               I2C3
#define TSENSOR_TIMEOUT             1000
/**
  * @}
  */

/** @defgroup TSENSOR_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup TSENSOR_Private_Variables
  * @{
  */ 
/* SMBUS handler declaration */
extern SMBUS_HandleTypeDef hsmbus3;
/**
  * @}
  */

/** @defgroup TSENSOR_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */

/** @defgroup TSENSOR_Private_Functions
  * @{
  */

/**
  * @brief  Set TSENSOR Initialization.
  * @param  DeviceAddr : Device ID address.
  * @param  pInitStruct: pointer to a TSENSOR_InitTypeDef structure 
  *         that contains the configuration setting for the TSENSOR.
  * @retval TSENSOR status
  */
TSENSOR_StatusTypeDef TSENSOR_Init(uint16_t DeviceAddr, TSENSOR_InitTypeDef *pInitStruct)
{  
  uint8_t buffertx[2] = {0,0};

  /*##-1- Verify that Temperature Sensor is ready ############################*/
  if (TSENSOR_IsReady(DeviceAddr, 20) != TSENSOR_OK )
  {
    return TSENSOR_ERROR;
  }

  /*##-2- Set the Configuration Register #####################################*/
  buffertx[0] = TSENSOR_CONFIGURATION;
  buffertx[1] = (uint8_t)(pInitStruct->AlertMode | pInitStruct->ConversionMode | pInitStruct->ConversionResolution);
  if (HAL_SMBUS_Master_Transmit_IT(&hsmbus3, DeviceAddr, &buffertx[0], sizeof(buffertx), SMBUS_FIRST_AND_LAST_FRAME_NO_PEC) != HAL_OK)
  {
    return TSENSOR_ERROR;
  }
  while(HAL_SMBUS_GetState(&hsmbus3) != HAL_SMBUS_STATE_READY);
  
  /*##-3- Set the Conversion Rate ############################################*/
  buffertx[0] = TSENSOR_CONVERSION_RATE;
  buffertx[1] = (uint8_t)(pInitStruct->ConversionRate);
  if (HAL_SMBUS_Master_Transmit_IT(&hsmbus3, DeviceAddr ,&buffertx[0], sizeof(buffertx), SMBUS_FIRST_AND_LAST_FRAME_NO_PEC) != HAL_OK)
  {
    return TSENSOR_ERROR;
  }
  while(HAL_SMBUS_GetState(&hsmbus3) != HAL_SMBUS_STATE_READY);

  /*##-4- Set the HIGH Limit Temperature #####################################*/
  buffertx[0] = TSENSOR_TEMP_HIGH_LIMIT_HIGH_BYTE;
  buffertx[1] = (uint8_t)(pInitStruct->TemperatureLimitHigh);
  if (HAL_SMBUS_Master_Transmit_IT(&hsmbus3, DeviceAddr, &buffertx[0], sizeof(buffertx), SMBUS_FIRST_AND_LAST_FRAME_NO_PEC) != HAL_OK)
  {
    return TSENSOR_ERROR;
  }
  while(HAL_SMBUS_GetState(&hsmbus3) != HAL_SMBUS_STATE_READY);

  /*##-5- Set the low Limit Temperature ######################################*/
  buffertx[0] = TSENSOR_TEMP_LOW_LIMIT_HIGH_BYTE;
  buffertx[1] = (uint8_t)(pInitStruct->TemperatureLimitLow);
  if (HAL_SMBUS_Master_Transmit_IT(&hsmbus3, DeviceAddr, &buffertx[0], sizeof(buffertx), SMBUS_FIRST_AND_LAST_FRAME_NO_PEC) != HAL_OK)
  {
    return TSENSOR_ERROR;
  }
  while(HAL_SMBUS_GetState(&hsmbus3) != HAL_SMBUS_STATE_READY);
  
  
  /*##-6- Manage Alert Mode setting ##########################################*/
  /* Clear an old alert if limit status is set */
  /* To clear an alert, need to perform an Alert Response command */
  if(((TSENSOR_ReadStatus(DeviceAddr) & TSENSOR_TEMP_EXCEED_HIGH_LIMIT) == TSENSOR_TEMP_EXCEED_HIGH_LIMIT)
     || ((TSENSOR_ReadStatus(DeviceAddr) & TSENSOR_TEMP_IS_OR_BELOW_LOW_LIMIT) == TSENSOR_TEMP_IS_OR_BELOW_LOW_LIMIT))
  {
    TSENSOR_AlerteResponseAddressRead();
  }

  /* Check Alert Mode requested */
  if(pInitStruct->AlertMode == TSENSOR_ALERT_ENABLE)
  {
    /* Enable SMBUS Alert */
    HAL_SMBUS_EnableAlert_IT(&hsmbus3);
  }
  
  /* Return status OK*/
  return TSENSOR_OK;
}

/**
  * @brief  Checks if temperature sensor is ready for communication
  * @param  DeviceAddr : Device ID address.
  * @param  Trials: Number of trials
  * @retval TSENSOR status
  */
TSENSOR_StatusTypeDef TSENSOR_IsReady(uint16_t DeviceAddr, uint32_t Trials)
{
  /*##-1- Check is Temperature Sensor is Ready to use ########################*/
  if (HAL_SMBUS_IsDeviceReady(&hsmbus3, DeviceAddr, Trials, TSENSOR_TIMEOUT) != HAL_OK)
  {
    return TSENSOR_ERROR;
  }
  
  /* Return status OK*/
  return TSENSOR_OK;
}

/**
  * @brief  Read The Temperature Sensor Status
  * @param  DeviceAddr : Device ID address.
  * @retval Temperature Sensor Status
  */
uint8_t TSENSOR_ReadStatus(uint16_t DeviceAddr)
{
  uint8_t bufferrx[1] = {0};
  uint8_t buffertx[1] = {0};

  /*##-1- Send Status Command ################################################*/
  buffertx[0] = TSENSOR_STATUS;
  if (HAL_SMBUS_Master_Transmit_IT(&hsmbus3, DeviceAddr, &buffertx[0], sizeof(buffertx), SMBUS_FIRST_AND_LAST_FRAME_NO_PEC) != HAL_OK)
  {
    return TSENSOR_ERROR;
  }
  while(HAL_SMBUS_GetState(&hsmbus3) != HAL_SMBUS_STATE_READY);
  
  /*##-2- Retrieve Status Data Byte ##########################################*/
  if (HAL_SMBUS_Master_Receive_IT(&hsmbus3, DeviceAddr, &bufferrx[0], sizeof(buffertx), SMBUS_FIRST_AND_LAST_FRAME_NO_PEC) != HAL_OK)
  {
    return TSENSOR_ERROR;
  }
  while(HAL_SMBUS_GetState(&hsmbus3) != HAL_SMBUS_STATE_READY);

  /* Return Temperature Sensor Status */
  return (uint8_t)(bufferrx[0]);
}

/**
  * @brief  Read temperature value of the Temperature Sensor.
  * @param  DeviceAddr : Device ID address.
  * @retval Temperature measured
  */
uint16_t TSENSOR_ReadTemp(uint16_t DeviceAddr)
{
  uint8_t bufferrx[2] = {0, 0};
  uint8_t buffertx[2] = {TSENSOR_TEMP_VALUE_HIGH_BYTE, TSENSOR_TEMP_VALUE_LOW_BYTE};
  uint16_t tmp = 0;
  uint8_t count = 0;

  for(count = 0; count < 2; count++)
  {
    /*##-1- Send Temperature Read Command ####################################*/
    if (HAL_SMBUS_Master_Transmit_IT(&hsmbus3, DeviceAddr, &buffertx[count], 1, SMBUS_FIRST_FRAME) != HAL_OK)
    {
      return TSENSOR_ERROR;
    }
    while(HAL_SMBUS_GetState(&hsmbus3) != HAL_SMBUS_STATE_READY);
    
    /*##-2- Retrieve Temperature Data Byte ###################################*/
    if (HAL_SMBUS_Master_Receive_IT(&hsmbus3, DeviceAddr, &bufferrx[count], 1, SMBUS_LAST_FRAME_NO_PEC) != HAL_OK)
    {
      return TSENSOR_ERROR;
    }
    while(HAL_SMBUS_GetState(&hsmbus3) != HAL_SMBUS_STATE_READY);
  }  

  tmp = (uint16_t)(bufferrx[0] << 8);
  tmp |= bufferrx[1];
  
  /* Return Temperature value */
  return (uint16_t)(tmp >> 4);
}

/**
  * @brief  Get the address of the Temperature Sensor in alert mode.
  * @param  None
  * @retval Temperature Sensor Address.
  */
uint8_t TSENSOR_AlerteResponseAddressRead(void)
{
  uint8_t bufferrx[1] = {0};

  /*##-1- Retrieve Alert Temperature Sensor Address ##########################*/
  HAL_SMBUS_Master_Receive_IT(&hsmbus3, 0x18, &bufferrx[0], 1, SMBUS_FIRST_AND_LAST_FRAME_NO_PEC);  
  while(HAL_SMBUS_GetState(&hsmbus3) != HAL_SMBUS_STATE_READY);
  
  /* Return Temperature Sensor Address */
  return bufferrx[0];
}

/**
  * @brief  SMBUS error callbacks.
  * @param  hsmbus : Pointer to a SMBUS_HandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
void HAL_SMBUS_ErrorCallback(SMBUS_HandleTypeDef *hsmbus)
{
  if(HAL_SMBUS_GetError(hsmbus) == HAL_SMBUS_ERROR_ALERT)
  {
    /* Inform upper layer of an alert occurs */
    TSENSOR_ErrorCallback(TSENSOR_ALERT);
  }
  else
  {
    /* Inform upper layer of an error occurs */
    TSENSOR_ErrorCallback(TSENSOR_ERROR);
  }
  
}

/**
  * @brief  TSENSOR error callbacks.
  * @param  Error : Temperature Sensor Error status.
  * @retval None
  */
__weak void TSENSOR_ErrorCallback(uint16_t Error)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the TSENSOR_ErrorCallback could be implemented in the user file
   */ 
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


