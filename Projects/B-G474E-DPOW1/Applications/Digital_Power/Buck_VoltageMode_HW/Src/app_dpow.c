/**
  ******************************************************************************
  * @file         app_dpow.c
  * @author       STMicroelectronics
  * @brief        Implementation of the STM32G474RE-Discovery board.
  * @details      This file provides functions to manage wave forms and defines 
                  to convert temporal data into HRTIM ticks
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
  * @history 2021-11-19 1.0     NSa     Updating file according "Common" application source code
  * @history 2022-01-04 1.0     RGo     Header modifications
  * @history 2022-01-10 2.0     NSa     Moved Systick_Handler to ISR file and Load_Handler to 
  *                                     app code file
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* Exported function ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
static const float scaleTable[] = { 32, 16, 8, 4, 2, 1, 1.0f / 2, 1.0f / 4 };

// If using CCM-SRAM for ISR, also see pragma for ADC ISR in main.c file
#ifdef USE_CCMSRAM
#pragma location = ".ccmram"
#endif

G4_Demo_t Demo = { 0 };
WAVEFORM_DISPLAY_INFO_t Waveform = {0};

extern void LoadHandler(void);
/* Exported macro ------------------------------------------------------------*/


/* @brief Records user values
* @details
* This stored the user value in to a local buffer. Once all of the values are stored
* we change state to playing the values back to the IDE
*/
void WaveFormRecord(int32_t ChA, int32_t ChB)
{
  switch (Waveform.m_State)
  {
  case WAVEFORM_WAIT_FOR_TRIGGER:
    break;

  case WAVEFORM_RECORD:
    Waveform.m_Buf[0][Waveform.m_IndexRec] = ChA;
    Waveform.m_Buf[1][Waveform.m_IndexRec] = ChB;
    if (++Waveform.m_IndexRec >= WAVEFORM_BUF_SIZE)
    {
      Waveform.m_State = WAVEFORM_PLAY;
      Waveform.m_IndexPlay = 0;
    }
    break;
  case WAVEFORM_PLAY:
    break;
  }
}

/* @brief Waits for a trigger to start recording
* @details
* This waits until we detect a trigger then starts recording the user values.
*/
void WaveFormTrigger(void)
{
  switch (Waveform.m_State)
  {
  case WAVEFORM_WAIT_FOR_TRIGGER:
    Waveform.m_State = WAVEFORM_RECORD;
    Waveform.m_IndexRec = 0;
    break;

  case WAVEFORM_RECORD:
    if (Waveform.m_IndexRec > 80)
    {
      Waveform.m_State = WAVEFORM_PLAY;
      Waveform.m_IndexPlay = 0;
    }
    break;

  case WAVEFORM_PLAY:
    break;
  }
}


/* @brief Display captured point using 'Set Data Breakpoint' 
* @details
* This re-plays the data recorded on the IAR using the 'Set Data Breakpoint' feature.
* When *pChA/B is written to the IAR detects the change and displays the value on the
* ST-LINK TimeLine view. 
*/
void WaveFormDisplay(int32_t *pChA, int32_t *pChB)
{
 
  switch (Waveform.m_State)
  {
  case WAVEFORM_WAIT_FOR_TRIGGER:
    /*
    if (++Waveform.m_IndexTrig >= 60)
    {
      Waveform.m_State = WAVEFORM_RECORD;
      Waveform.m_IndexRec = 0;
    }
    */
    break;

  case WAVEFORM_RECORD:
    break;

  case WAVEFORM_PLAY:
    *pChA = Waveform.m_Buf[0][Waveform.m_IndexPlay];
    *pChB = Waveform.m_Buf[1][Waveform.m_IndexPlay];
    if (++Waveform.m_IndexPlay >= Waveform.m_IndexRec)
    {
      Waveform.m_State = WAVEFORM_WAIT_FOR_TRIGGER;
      Waveform.m_IndexTrig = 0;
    }
    break;
  }
}


/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  
  /* JOYSTICK UP enables load transients load */
  if (GPIO_Pin == JOYSTICK_UP_Pin)
  {
    Demo.bTransient = 1;
  }    
  
  /* JOYSTICK DOWN disables load transients load */
  if (GPIO_Pin == JOYSTICK_DOWN_Pin)
  {
    Demo.bTransient = 0;
  }      
    
  /* JOYSTICK RIGHT increases the activated Resistors load */
  if (GPIO_Pin == JOYSTICK_RIGHT_Pin)
  {
    /* Clamp the value to Max if over-pressed by user */
    if (Demo.NbrActiveLoad < 2)
    {
      Demo.NbrActiveLoad += 1;
      WaveFormTrigger();
    }
  }

  /* JOYSTICK LEFT decreases the activated Resistors load */
  if (GPIO_Pin == JOYSTICK_LEFT_Pin)
  {
    /* Clamp the value to Min if over-pressed by user */
    if (Demo.NbrActiveLoad > 0)
    {
      Demo.NbrActiveLoad -= 1;
      WaveFormTrigger();
    }
  }
  
  LoadHandler();
  
}

void Demo_Error_Handler(void)
{
  while (1)
  {
    /* LED5 (Red LED) is blinking */
    HAL_GPIO_TogglePin(GPIOB, LED_UP_RED_Pin);
    HAL_Delay(1000);
  }
}

/* Private defines -----------------------------------------------------------*/

/**
 * @brief Convert ns to HRTIM ticks
 * @details
 * example code to convert 100ns to ticks
 *   HrtimNsToTicks(&hhrtim1, HRTIM_TIMERID_TIMER_A, 100)
 * @retval number of ticker per ns
 */
uint16_t HAL_HRTIM_NsToTicks(HRTIM_HandleTypeDef *hhrtim, uint32_t Timers, int ns)
{
  int scaleIndex = __HAL_HRTIM_GETCLOCKPRESCALER(hhrtim, Timers);
  uint32_t ticks = (uint32_t)(fHRTIM_HZ * scaleTable[scaleIndex] * ns * 1.0e-9f);

  return MIN(ticks, UINT16_MAX);
}


/**
 * @brief Convert Frequence to HRTIM ticks
 * @details
 * example code to convert 100ns to ticks
 *   HrtimNsToTicks(&hhrtim1, HRTIM_TIMERID_TIMER_A, 100)
 * @retval number of ticker per ns
 */
uint16_t HAL_HRTIM_FreqToTicks(HRTIM_HandleTypeDef *hhrtim, uint32_t Timers, int Freq)
{
  int scaleIndex = __HAL_HRTIM_GETCLOCKPRESCALER(hhrtim, Timers);
  uint32_t ticks = (uint32_t)(fHRTIM_HZ * scaleTable[scaleIndex] / Freq);

  return MIN(ticks, UINT16_MAX);
}


/**
 * @brief Convert ns to HRTIM deadband ticks
 * @retval number of ticker per ns
 */
uint16_t HAL_HRTIM_DeadBandNsToTicks(int ns)
{
  uint32_t ticks = (uint32_t)(fHRTIM_HZ * 8 * ns * 1.0e-9f);

  return MIN(ticks, (1 << 9) - 1);
}

/**
  * @brief This function handles the loads.
  */
void LoadHandler(void)
{
  switch (Demo.NbrActiveLoad)
  {
  case 2:
    /* Enable Load_1 */
    HAL_GPIO_WritePin(GPIOC, BUCKBOOST_LOAD_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, LED_RIGHT_GREEN_Pin, GPIO_PIN_SET);    
    /* Enable Load_2 */
    HAL_GPIO_WritePin(GPIOC, BUCKBOOST_LOAD_2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, LED_LEFT_ORANGE_Pin, GPIO_PIN_SET);
    break;
  case 1:
    /* Enable Load_1 */
    HAL_GPIO_WritePin(GPIOC, BUCKBOOST_LOAD_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, LED_RIGHT_GREEN_Pin, GPIO_PIN_SET);
    /* Disable Load_2 */
    HAL_GPIO_WritePin(GPIOC, BUCKBOOST_LOAD_2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, LED_LEFT_ORANGE_Pin, GPIO_PIN_RESET);
    break;
  case 0:
  default:
    /* Disable Load_1 */
    HAL_GPIO_WritePin(GPIOC, BUCKBOOST_LOAD_1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, LED_RIGHT_GREEN_Pin, GPIO_PIN_RESET);
    /* Disable Load_2 */    
    HAL_GPIO_WritePin(GPIOC, BUCKBOOST_LOAD_2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, LED_LEFT_ORANGE_Pin, GPIO_PIN_RESET);
    break;
  }
}

