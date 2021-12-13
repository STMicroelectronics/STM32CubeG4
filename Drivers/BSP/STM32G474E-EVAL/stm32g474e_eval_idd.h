 /**
  ******************************************************************************
  * @file    stm32g474e_eval_idd.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32g474e_eval_idd.c driver.
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
#ifndef STM32G474E_EVAL_IDD_H
#define STM32G474E_EVAL_IDD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g474e_eval_conf.h"
#include "stm32g474e_eval_errno.h"
#include "../Components/Common/idd.h"
#include "../Components/mfxstm32l152/mfxstm32l152.h"

#if (USE_BSP_IO_CLASS == 1)

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @addtogroup STM32G474E-EVAL_IDD
  * @{
  */

/** @defgroup STM32G474E-EVAL_IDD_Exported_Constants Exported Constants
  * @{
  */
#define IDD_INSTANCES_NBR                      1U

#define BSP_IDD_PRE_DELAY_MIN                  0U      /*!< Minimum value in millisecond before starting acquisition. */
#define BSP_IDD_PRE_DELAY_MAX                  2540U   /*!< Maximum value in millisecond before starting acquisition. This is the default configuration after BSP_IDD_Init. */
#define BSP_IDD_ACQUISITION_DURATION_MIN       0U      /*!< Minmum duration in millisecond of the acquisation. */
#define BSP_IDD_ACQUISITION_DURATION_MAX       25000U  /*!< Maximum duration in millisecond of the acquisation. This is the default configuration after BSP_IDD_Init */
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_IDD_Exported_Types Exported Types
  * @{
  */
typedef struct
{
  int32_t PreDelay;            /*!< Specifies Pre delay value in millisecond.
                                    Limitation, BSP_IDD_PRE_DELAY_MIN <= PreDelay <= BSP_IDD_PRE_DELAY_MAX */
  int32_t AcquisitionDuration; /*!< Specifies the acquisition duration value in millisecond of one measurement
                                    Limitation, BSP_IDD_ACQUISITION_DURATION_MIN <= AcquisitionDuration <= BSP_IDD_ACQUISITION_DURATION_MAX */
}BSP_IDD_Config_t;
/**
  * @}
  */

/** @addtogroup STM32G474E-EVAL_IDD_Exported_Variables
  * @{
  */
extern EXTI_HandleTypeDef hidd_exti[IDD_INSTANCES_NBR];
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup STM32G474E-EVAL_IDD_Exported_Functions
  * @{
  */
int32_t   BSP_IDD_Init(uint32_t Instance);
int32_t   BSP_IDD_DeInit(uint32_t Instance);
int32_t   BSP_IDD_Reset(uint32_t Instance);
int32_t   BSP_IDD_EnterLowPower(uint32_t Instance);
int32_t   BSP_IDD_ExitLowPower(uint32_t Instance);
int32_t   BSP_IDD_StartMeasurement(uint32_t Instance);
int32_t   BSP_IDD_Config(uint32_t Instance, BSP_IDD_Config_t * IddConfig);
int32_t   BSP_IDD_GetValue(uint32_t Instance, uint32_t *IddValue);
int32_t   BSP_IDD_EnableIT(uint32_t Instance);
int32_t   BSP_IDD_ClearIT(uint32_t Instance);
int32_t   BSP_IDD_GetITStatus(uint32_t Instance);
int32_t   BSP_IDD_DisableIT(uint32_t Instance);

void      BSP_IDD_Callback(void);
void      BSP_IDD_IRQHandler(void);

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

#endif /* #if (USE_BSP_IO_CLASS == 1) */

#ifdef __cplusplus
}
#endif

#endif /* STM32G474E-EVAL_IDD_H */

