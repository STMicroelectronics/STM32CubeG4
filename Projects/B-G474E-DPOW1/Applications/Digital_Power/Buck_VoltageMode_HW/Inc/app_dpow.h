/**
******************************************************************************
* @file   : app_dpow.h
* @brief  : Implementation of the STM32G474RE-Discovery board.
******************************************************************************
*
* COPYRIGHT(c) 2019 STMicroelectronics
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
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
#include "stm32g4xx_ll_dac.h"

#include "HW_3p3z_controller.h"

/* Exported types ------------------------------------------------------------*/
typedef struct G4_Demo_t
{
        CNTRL_DataFloat CtrlFloat;
        uint32_t periodBuck;    // Number of ticks per period, read from the hrtim register
        uint32_t periodBoost;   // Number of ticks per period, read from the hrtim register
        uint8_t NbrActiveLoad;  // Memorizes the number of active Resistors load
        uint8_t bTransient;
} G4_Demo_t;

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

/* Exported macro ------------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Exported function ---------------------------------------------------------*/
uint16_t HAL_HRTIM_NsToTicks(HRTIM_HandleTypeDef *hhrtim, uint32_t Timers, int ns);
uint16_t HAL_HRTIM_FreqToTicks(HRTIM_HandleTypeDef *hhrtim, uint32_t Timers, int FreqHz);
uint16_t HAL_HRTIM_DeadBandNsToTicks(int ns);
void Demo_Error_Handler(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif /* _APP_DPOW_H */
