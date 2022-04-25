/**
  ******************************************************************************
  * @file   : HW_3p3z_controller.h
  * @brief  : Implementation of a 3p3z controller implemented in software.
  *           This file contains the common defines of the application.
  ******************************************************************************
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _3p3z_H
#define _3p3z_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* These are variables that need to be shared between main.c and stm32xx_it.c */
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

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define MAX(a,b) ((a)>(b) ? (a) : (b))

/* Exported function ---------------------------------------------------------*/
/**
 * @brief     Inits the 3p3z structure.
 * @details
 * This inits the 3p3z structure.
 *
 * # Examples #
 * This uses the raw coefs from WDS and passes the values to the 3p3z init function
 * ~~~~
 * CNTRL_3p3zFloatInit( &CtrlFloat,
 *                      B0,B1,B2,B3,
 *                      A1,A2,A3, K,
 *                      REF, DUTY_TICKS_MIN, DUTY_TICKS_MAX );
 * ~~~~
 */
__STATIC_INLINE void CNTRL_3p3zFloatInit( CNTRL_DataFloat* Ptr,
                                 float cB0, float cB1, float cB2, float cB3,
                                 float cA1, float cA2, float cA3, float cK,
                                 int32_t Ref, float PwmMin, float PwmMax )
{
    memset( Ptr, 0, sizeof(*Ptr));

    Ptr->m_Ref    = Ref;
    Ptr->m_Out   = 0;
    Ptr->m_K   = cK;

    Ptr->m_A1 = cA1;
    Ptr->m_A2 = cA2;
    Ptr->m_A3 = cA3;
    Ptr->m_B0 = cB0*cK;
    Ptr->m_B1 = cB1*cK;
    Ptr->m_B2 = cB2*cK;
    Ptr->m_B3 = cB3*cK;

    Ptr->m_Min  = PwmMin;
    Ptr->m_Max  = PwmMax;
}

/**
 * @brief     3p3z controller.
 * @details
 * This is a 3p3z controller.
 *
 * # Examples #
 * This runs the 3p3z controller and updates the hi-res PWM.
 * ~~~~
 * Adc = __HAL_ADC1_GET_VALUE();
 * CNTRL_3p3zFloat(&CtrlFloat, Adc);
 * __HAL_HRTIM_SETCOMPARE( &hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_1, CtrlFloat.m_Out );
 * ~~~~
 */
__STATIC_INLINE void CNTRL_3p3zFloat( CNTRL_DataFloat* Ptr, int32_t Feedback )
{
    float acc;

    acc  = Ptr->m_B3*Ptr->m_E[2]; Ptr->m_E[2] = Ptr->m_E[1];
    acc += Ptr->m_B2*Ptr->m_E[1]; Ptr->m_E[1] = Ptr->m_E[0];
    acc += Ptr->m_B1*Ptr->m_E[0]; Ptr->m_E[0] = Ptr->m_Ref - Feedback;
    acc += Ptr->m_B0*Ptr->m_E[0];

    acc += Ptr->m_A3*Ptr->m_U[2]; Ptr->m_U[2] = Ptr->m_U[1];
    acc += Ptr->m_A2*Ptr->m_U[1]; Ptr->m_U[1] = Ptr->m_U[0];
    acc += Ptr->m_A1*Ptr->m_U[0];

    acc = MIN( acc , Ptr->m_Max );
    acc = MAX( acc , -Ptr->m_Max );
    Ptr->m_U[0] = acc;

    if (acc < Ptr->m_Min) { acc = Ptr->m_Min; }
    Ptr->m_Out = (int32_t)acc;
}


/**
 * @brief     2p2z controller.
 * @details
 * This is a 2p2z controller.
 *
 * # Examples #
 * This runs the 3p3z controller and updates the hi-res PWM.
 * ~~~~
 * Adc = __HAL_ADC1_GET_VALUE();
 * CNTRL_2p2zFloat(&CtrlFloat, Adc);
 * __HAL_DAC1_SET_VALUE_12B_R ( CtrlFloat.m_Out );
 * ~~~~
 */
__STATIC_INLINE void CNTRL_2p2zFloat(CNTRL_DataFloat* Ptr, int32_t Feedback)
{
  float acc;

  acc = Ptr->m_B2*Ptr->m_E[1]; Ptr->m_E[1] = Ptr->m_E[0];
  acc += Ptr->m_B1*Ptr->m_E[0]; Ptr->m_E[0] = Ptr->m_Ref - Feedback;
  acc += Ptr->m_B0*Ptr->m_E[0];

  acc += Ptr->m_A2*Ptr->m_U[1]; Ptr->m_U[1] = Ptr->m_U[0];
  acc += Ptr->m_A1*Ptr->m_U[0];

  acc = MIN(acc, Ptr->m_Max);
  acc = MAX(acc, -Ptr->m_Max);
  Ptr->m_U[0] = acc;

  if (acc < Ptr->m_Min) { acc = Ptr->m_Min; }
  Ptr->m_Out = (int32_t)acc;
}


/* Private defines -----------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* _3p3z_H */

