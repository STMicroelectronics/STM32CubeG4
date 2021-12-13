/**
  ******************************************************************************
  * @file    SMBUS/SMBUS_TSENSOR/Src/stm32g4xx_tsensor.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for
  *          the stm32g4xx_tsensor.c temperature sensor driver.
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
#ifndef __TSENSOR_H
#define __TSENSOR_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
   
/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup TSENSOR
  * @{
  */

/** @defgroup TSENSOR_Exported_Types
  * @{
  */ 
/** 
  * @brief  Temperature Sensor driver structure definition  
  */
typedef struct
{
  uint8_t AlertMode;            /* Alert Mode Temperature out of range*/
  uint8_t ConversionMode;       /* Continuous/One Shot Mode */
  uint8_t ConversionResolution; /* Temperature Resolution */
  uint8_t ConversionRate;       /* Number of measure per second */
  uint8_t TemperatureLimitHigh; /* High Temperature Limit Range */
  uint8_t TemperatureLimitLow;  /* Low Temperature Limit Range */
}TSENSOR_InitTypeDef;

/** 
  * @brief  TSENSOR Status structures definition  
  */  
typedef enum 
{
  TSENSOR_OK       = 0x00,
  TSENSOR_ERROR    = 0x01,
  TSENSOR_ALERT    = 0x02
} TSENSOR_StatusTypeDef;

/** @defgroup TSENSOR_Exported_Constants
  * @{
  */
/******************************************************************************/
/*************************** START REGISTER MAPPING  **************************/
/******************************************************************************/
/***************************** Read Access Only *******************************/
#define TSENSOR_STATUS                          0x01  /*!< Status Register */
#define TSENSOR_TEMP_VALUE_HIGH_BYTE            0x00  /*!< Temperature value high byte Register */
#define TSENSOR_TEMP_VALUE_LOW_BYTE             0x02  /*!< Temperature value low byte Register */

#define TSENSOR_PRODUCT_ID                      0xFD  /*!< Product Identifier Register */
#define TSENSOR_MANUFACTURER_ID                 0xFE  /*!< Manufacturer Identifier Register */
#define TSENSOR_REVISION_ID                     0xFF  /*!< Revision Number Identifier Register */

   /***************************** Write Access Only ******************************/
#define TSENSOR_ONE_SHOT                        0x0F  /*!< Single Conversion Request Register */

   
/***************************** Read/Write Access ******************************/
#define TSENSOR_CONFIGURATION                   0x03  /*!< Configuration Register */
#define TSENSOR_CONVERSION_RATE                 0x04  /*!< Conversion Rate Register */
#define TSENSOR_TEMP_HIGH_LIMIT_HIGH_BYTE       0x05  /*!< High Temperature Limit high byte Register */
#define TSENSOR_TEMP_HIGH_LIMIT_LOW_BYTE        0x06  /*!< High Temperature Limit low byte Register */
#define TSENSOR_TEMP_LOW_LIMIT_HIGH_BYTE        0x07  /*!< Low Temperature Limit high byte Register */
#define TSENSOR_TEMP_LOW_LIMIT_LOW_BYTE         0x08  /*!< Low Temperature Limit low byte Register */


#define TSENSOR_THERM_LIMIT                     0x20  /*!< THERM Limit Register */
#define TSENSOR_THERM_HYSTERISIS                0x21  /*!< THERM Hysteris Register */
#define TSENSOR_SMBUS_TIMEOUT                   0x22  /*!< SMBUS Timeout Enabling Register */


/******************************************************************************/
/**************************** END REGISTER MAPPING  ***************************/
/******************************************************************************/
/** @defgroup Status_Value
  * @{
  */
#define TSENSOR_TEMP_CROSS_THERM_LIMIT          ((uint8_t)0x01) /* Temperature measured has crossed the Therm limit */
#define TSENSOR_TEMP_IS_OR_BELOW_LOW_LIMIT      ((uint8_t)0x20) /* Temperature is at or below the Low Limit         */
#define TSENSOR_TEMP_EXCEED_HIGH_LIMIT          ((uint8_t)0x40) /* Temperature Limit High has been exceeded         */
#define TSENSOR_BUSY                            ((uint8_t)0x80) /* Temperature Conversion is in progress            */
/**
  * @}
  */

/** @defgroup EVENT_Pin_selection 
  * @{
  */
#define TSENSOR_ALERT_ENABLE                    ((uint8_t)0x00)
#define TSENSOR_ALERT_DISABLE                   ((uint8_t)0x80)
/**
  * @}
  */

/** @defgroup Conversion_Mode_Selection 
  * @{
  */
#define TSENSOR_CONTINUOUS_MODE                 ((uint8_t)0x00)
#define TSENSOR_ONE_SHOT_MODE                   ((uint8_t)0x40)
/**
  * @}
  */

/** @defgroup Conversion_Bits_Resolution_Selection
  * @{
  */
#define TSENSOR_CONV_9BITS                      ((uint8_t)0x08)
#define TSENSOR_CONV_10BITS                     ((uint8_t)0x00)
#define TSENSOR_CONV_11BITS                     ((uint8_t)0x04)
#define TSENSOR_CONV_12BITS                     ((uint8_t)0x0C)
/**
  * @}
  */

/** @defgroup Conversion_Rate_Per_Second_Selection
  * @{
  */
#define TSENSOR_ONE_SIXTEENTH_PER_SECOND        ((uint8_t)0x00) /* 0.0625 conversion/Second     */
#define TSENSOR_ONE_EIGHTH_PER_SECOND           ((uint8_t)0x01) /* 0.125 conversion/Second      */
#define TSENSOR_ONE_QUATER_PER_SECOND           ((uint8_t)0x02) /* 0.25 conversion/Second       */
#define TSENSOR_HALF_PER_SECOND                 ((uint8_t)0x03) /* 0.5 conversion/Second        */
#define TSENSOR_ONE_PER_SECOND                  ((uint8_t)0x04) /* 1 conversion/Second          */
#define TSENSOR_TWO_PER_SECOND                  ((uint8_t)0x05) /* 2 conversions/Second         */
#define TSENSOR_FOUR_PER_SECOND                 ((uint8_t)0x05) /* 4 conversions/Second         */
#define TSENSOR_HEIGH_PER_SECOND                ((uint8_t)0x07) /* 8 conversions/Second         */
#define TSENSOR_SIXTEEN_PER_SECOND              ((uint8_t)0x08) /* 16 conversions/Second        */
#define TSENSOR_THIRTY_TWO_PER_SECOND           ((uint8_t)0x09) /* 32 conversions/Second        */
/**
  * @}
  */

/**
  * @}
  */
 
/** @defgroup TSENSOR_Exported_Functions
  * @{
  */
/* Sensor Configuration Functions */ 
TSENSOR_StatusTypeDef   TSENSOR_Init(uint16_t DeviceAddr, TSENSOR_InitTypeDef *pInitStruct);
TSENSOR_StatusTypeDef   TSENSOR_IsReady(uint16_t DeviceAddr, uint32_t Trials);

/* Sensor Request Functions */
uint8_t                 TSENSOR_ReadStatus(uint16_t DeviceAddr);
uint16_t                TSENSOR_ReadTemp(uint16_t DeviceAddr);
uint8_t                 TSENSOR_AlerteResponseAddressRead(void);

/* Sensor Callbacks */
void                    TSENSOR_ErrorCallback(uint16_t Error);
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
  
#endif /* __TSENSOR_H */



