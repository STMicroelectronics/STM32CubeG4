/**
  ******************************************************************************
  * @file         app_dpow.h
  * @author       STMicroelectronics
  * @brief        Header file of app_dpow source file
  * @details      This file provides function declarations and defines used by 
                  app_dpow source file
  ******************************************************************************
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  *
 * @history Date       Version Person  Change
 * @history ---------- ------- ------- ----------------------------------------
  * @history 2020-02-14 1.0     PBo     Created with the support of Biricha Digital Power LTd
  * @history 2021-12-23 1.0     NSa     Adding 3p3z source data structure and removing 
                                        include "HW_3p3z_controller.h"
  * @history 2022-01-04 1.0     RGo     Header modifications
  * @history 2022-01-10 2.0     NSa     Moved Systick_Handler to ISR file and Load_Handler to 
  *                                     app code file
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _APP_DPOW_H
#define _APP_DPOW_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <assert.h>

/* Private includes ----------------------------------------------------------*/
#include "stm32g4xx_ll_hrtim.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_adc.h"

/* Exported types ------------------------------------------------------------*/

/**
 * Holding structure for 3p3z values.
 */
typedef struct CNTRL_DataFloat
{
    int32_t             m_Out;
    int32_t             m_Ref;
    float               m_A1;
    float               m_A2;
    float               m_A3;
    float               m_B0;
    float               m_B1;
    float               m_B2;
    float               m_B3;
    float               m_E[3];
    float               m_U[3];
    float               m_K;
    int16_t             m_Ks;
    float               m_Min;
    float               m_Max;
} CNTRL_DataFloat;

typedef struct G4_Demo_t
{
        CNTRL_DataFloat CtrlFloat;
        uint32_t periodBuck;    // Number of ticks per period, read from the hrtim register
        uint32_t periodBoost;   // Number of ticks per period, read from the hrtim register
        uint8_t NbrActiveLoad;  // Memorizes the number of active Resistors load
        uint8_t bTransient;
} G4_Demo_t;

typedef enum WAVEFORM_STATE_t
{
  WAVEFORM_RECORD,
  WAVEFORM_PLAY,
  WAVEFORM_WAIT_FOR_TRIGGER
}WAVEFORM_STATE_t;

#define WAVEFORM_BUF_SIZE 128

typedef struct WAVEFORM_DISPLAY_INFO_t
{
  uint16_t      m_Buf[2][WAVEFORM_BUF_SIZE];
  uint16_t      m_IndexRec;
  uint16_t      m_IndexPlay;
  uint16_t      m_IndexTrig;
  WAVEFORM_STATE_t          m_State;
}WAVEFORM_DISPLAY_INFO_t;

/* Exported constants --------------------------------------------------------*/
/* These are board values */
#define ADC_VREF                (3.3f) // DAC and ADC

#define ADC_VALUE_TO_VIN_SCALE  (ADC_VREF / 4095U)
#define FEED_BACK_SCALE         ( (49.0f + 470.0f + 1800.0f) / 1800.0f )
#define ADC_SCALE               (ADC_VALUE_TO_VIN_SCALE * FEED_BACK_SCALE)
#define DAC_MAX                 (4095U)

/* These are ST values */
//#define DAC_MAX                 ((1<<12)-1)  DAC_DIGITAL_SCALE_12BITS 4095U
#define HRTIM_MIN_DUTY          (0x60)  // Min duty when using hi-res pwm
#define fHRTIM_HZ               170e6

#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define MAX(a,b) ((a)>(b) ? (a) : (b))

/* Exported macro ------------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Exported function ---------------------------------------------------------*/
uint16_t HAL_HRTIM_NsToTicks(HRTIM_HandleTypeDef *hhrtim, uint32_t Timers, int ns);
uint16_t HAL_HRTIM_FreqToTicks(HRTIM_HandleTypeDef *hhrtim, uint32_t Timers, int FreqHz);
uint16_t HAL_HRTIM_DeadBandNsToTicks(int ns);
void Demo_Error_Handler(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void WaveFormDisplay(int32_t *pChA, int32_t *pChB);
void LoadHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* _APP_DPOW_H */

