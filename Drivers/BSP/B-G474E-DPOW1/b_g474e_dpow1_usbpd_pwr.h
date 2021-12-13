/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    b_g474e_dpow1_usbpd_pwr.h
  * @author  MCD Application Team
  * @brief   Header file for b_g474e_dpow1_usbpd_pwr.c file
  ******************************************************************************
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32G474E_DISCO_USBPD_PWR_H
#define STM32G474E_DISCO_USBPD_PWR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_gpio.h"
#include "b_g474e_dpow1_conf.h"
#include "b_g474e_dpow1_errno.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup B-G474E-DPOW1
  * @{
  */

/** @addtogroup B-G474E-DPOW1_USBPD_PWR
  * @{
  */

/** @defgroup B-G474E-DPOW1_USBPD_PWR_Exported_Types Exported Types
  * @{
  */

/**
  * @brief  Power role
  */
typedef enum
{
  POWER_ROLE_SOURCE = 0,
  POWER_ROLE_SINK,
  POWER_ROLE_DUAL
}
USBPD_PWR_PowerRoleTypeDef;

/**
  * @brief  Voltage control mode
  */
typedef enum
{
  DCDC_CTRL_MODE_UNKNOWN = 0,
  DCDC_CTRL_MODE_GPIO,
  DCDC_CTRL_MODE_PWM,
} USBPD_PWR_DCDCCtrlModeTypeDef;

/**
  * @brief  VBUS connection status
  */
typedef enum
{
  VBUS_CONNECTED = 0,
  VBUS_NOT_CONNECTED
} USBPD_PWR_VBUSConnectionStatusTypeDef;

/**
  * @brief VBUS Detection Callback
  */
typedef void USBPD_PWR_VBUSDetectCallbackFunc(uint32_t Instance,
                                              USBPD_PWR_VBUSConnectionStatusTypeDef VBUSConnectionStatus);

/**
  * @}
  */

/** @defgroup B-G474E-DPOW1_USBPD_PWR_Exported_Constants Exported Constants
  * @{
  */
/**
  * @brief  Number of TypeC ports
  */
#define USBPD_PWR_INSTANCES_NBR           1U

/**
  * @brief  Type-C port identifier
  */
#define USBPD_PWR_TYPE_C_PORT_1           0U

/**
  * @brief  CC pin identifier
  */
#define USBPD_PWR_TYPE_C_CC1              1U
#define USBPD_PWR_TYPE_C_CC2              2U

/**
  * @brief  VBUS disconnection threshold values (in mV)
  */
#define USBPD_PWR_HIGH_VBUS_THRESHOLD     (2800U)
#define USBPD_PWR_LOW_VBUS_THRESHOLD      (750U)
#define USBPD_PWR_VBUS_THRESHOLD_5V       (2150U)
#define USBPD_PWR_VBUS_THRESHOLD_9V       (7000U)
#define USBPD_PWR_VBUS_THRESHOLD_15V      (12500U)
#define USBPD_PWR_VBUS_THRESHOLD_20V      (17000U)
#define USBPD_PWR_VBUS_THRESHOLD_APDO     (2150U)

/**
  * @brief  VBUS discharge parameters
  */
#define USBPD_PWR_DISCHARGE_MARGIN        (500U)
#define USBPD_PWR_DISCHARGE_TIME            (6U)

/**
  * @brief  Calibration settings
  */
#define USBPD_PWR_DCDC_PRECISION          (40U)     /* DCDC output precision set to 40mV (Noise)*/
#define USBPD_PWR_CALIBRATION_ENABLED     (1U)
#define USBPD_PWR_CALIBRATION_DISABLED    (0U)

/**
  * @brief  Standard VBUS voltage levels
  */
#define USBPD_PWR_VBUS_5V                 5000U
#define USBPD_PWR_VBUS_9V                 9000U
#define USBPD_PWR_VBUS_15V                15000U

/**
  * @brief  power timeout
  */
#define USBPD_PWR_TIMEOUT_PDO             250U         /* Timeout for PDO to PDO or PDO to APDO at 250ms*/
#define USBPD_PWR_TIMEOUT_APDO            25U          /* Timeout for APDO to APDO at 25ms*/

/**
  * @brief  Invalid value set during issue with voltage setting
  */
#define USBPD_PWR_INVALID_VALUE           0xFFFFFFFFU

/* On B-G474E-DPOW1 board, following setup has been used :
      VSENSE (USBPD_VIN)      => PC2 */
#define VSENSE_GPIO_PORT                  GPIOC
#define VSENSE_GPIO_PIN                   LL_GPIO_PIN_2
#define VSENSE_GPIO_ENABLE_CLOCK()        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

/* PC2 is used as ADC12_IN8 input for ADC measurement of VBUS voltage :
   ADC1 (Common 12) Channel 8.
*/
#define VSENSE_ADC_INSTANCE               ADC1
#define VSENSE_ADC_COMMON                 ADC12_COMMON
#define VSENSE_ADC_RANK                   LL_ADC_REG_RANK_1
#define VSENSE_ADC_CHANNEL                LL_ADC_CHANNEL_8

/* Enable ADC clock (core clock) */
#define VSENSE_ADC_ENABLE_CLOCK()         LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);

/**
  * @}
  */

/** @defgroup B-G474E-DPOW1_USBPD_PWR_Exported_Functions Exported Functions
  * @{
  */
/* Common functions */
int32_t BSP_USBPD_PWR_Init(uint32_t Instance);

int32_t BSP_USBPD_PWR_Deinit(uint32_t Instance);

int32_t BSP_USBPD_PWR_VBUSInit(uint32_t Instance);

int32_t BSP_USBPD_PWR_VBUSDeInit(uint32_t Instance);

int32_t BSP_USBPD_PWR_VBUSOn(uint32_t Instance);

int32_t BSP_USBPD_PWR_VBUSOff(uint32_t Instance);

int32_t BSP_USBPD_PWR_VBUSSetVoltage_Fixed(uint32_t Instance,
                                           uint32_t VbusTargetInmv,
                                           uint32_t OperatingCurrent,
                                           uint32_t MaxOperatingCurrent);

int32_t BSP_USBPD_PWR_VBUSSetVoltage_Variable(uint32_t Instance,
                                              uint32_t VbusTargetMaxInmv,
                                              uint32_t VbusTargetMinInmv,
                                              uint32_t OperatingCurrent,
                                              uint32_t MaxOperatingCurrent);

int32_t BSP_USBPD_PWR_VBUSSetVoltage_Battery(uint32_t Instance,
                                             uint32_t VbusTargetMin,
                                             uint32_t VbusTargetMax,
                                             uint32_t OperatingPower,
                                             uint32_t MaxOperatingPower);

int32_t BSP_USBPD_PWR_VBUSSetVoltage_APDO(uint32_t Instance,
                                          uint32_t VbusTargetInmv,
                                          uint32_t OperatingCurrent,
                                          int32_t Delta);

int32_t BSP_USBPD_PWR_VBUSGetVoltage(uint32_t Instance, uint32_t *pVoltage);

int32_t BSP_USBPD_PWR_VBUSGetCurrent(uint32_t Instance, int32_t *pCurrent);

int32_t BSP_USBPD_PWR_VCONNInit(uint32_t Instance,
                                uint32_t CCPinId);

int32_t BSP_USBPD_PWR_VCONNDeInit(uint32_t Instance,
                                  uint32_t CCPinId);

int32_t BSP_USBPD_PWR_VCONNOn(uint32_t Instance,
                              uint32_t CCPinId);

int32_t BSP_USBPD_PWR_VCONNOff(uint32_t Instance,
                               uint32_t CCPinId);

int32_t BSP_USBPD_PWR_SetVBUSDisconnectionThreshold(uint32_t Instance,
                                                    uint32_t VoltageThreshold);

int32_t BSP_USBPD_PWR_RegisterVBUSDetectCallback(uint32_t Instance,
                                                 USBPD_PWR_VBUSDetectCallbackFunc *pfnVBUSDetectCallback);

int32_t BSP_USBPD_PWR_VBUSIsOn(uint32_t Instance, uint8_t *pState);

int32_t BSP_USBPD_PWR_VCONNIsOn(uint32_t Instance,
                                uint32_t CCPinId, uint8_t *pState);
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

#endif /* STM32G474E_DISCO_USBPD_PWR_H */

