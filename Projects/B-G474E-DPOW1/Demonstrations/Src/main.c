/**
  ******************************************************************************
  * @file    Demonstrations/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program body
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
#define __MAIN_C

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ledRGB.h"
#include "BuckBoost.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t falling_slope = 0;
static uint8_t counter32 = 0;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Demo_4Led(void);
static void TriangleGenerationInit(void);
static void TriangleGenerationDeInit(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32G4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the System clock to have a frequency of 170 MHz */
  SystemClock_Config();

  /* Initialize leds and joystick */
  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_ORANGE);
  BSP_LED_Init(LED_RED);
  BSP_LED_Init(LED_GREEN);
  BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL);

  /* Start with 4 leds demo */
  StateMachine = STATE_4LED;

  /* Infinite loop */
  while (1)
  {
    UserAction = 0;
    UserEvent = 0;

    switch (StateMachine)
    {
    case STATE_4LED:
      /* In parallel of 4 Led demo, there is a triangle generation on TP4 */
      /* showing advantage of HRTIM (*32 clock multiplier) compare to general purpose timer capabilities  */
      TriangleGenerationInit();
      Demo_4Led();
      TriangleGenerationDeInit();
      break;

    case STATE_LED_COLOR:
      Demo_LedColor();
      break;

    case STATE_LED_WHITE:
      Demo_LedWhite();
      break;

    case STATE_BUCK:
      Demo_Buck();
      break;

    case STATE_BUCKBOOST:
      Demo_BuckBoost();
      break;

    case MAX_STATE:
      StateMachine = STATE_4LED;
      break;
      
    default :
      Error_Handler();
      break;
    }

    /* Switch to next demo */
    if ((UserAction == 1) && (UserEvent == JOY_SEL))
    {
      StateMachine ++;
    }
  }
}

/**
  * @brief  Demo4 leds: switch on/off each led one after the other, faster and faster
  * @param  None
  * @retval None
  */
static void Demo_4Led(void)
{
  uint32_t tempo = 400;

  /* switch on/off each led one after the other, faster and faster */
  while(UserEvent != JOY_SEL )
  {
    /* Red LED ON */
    BSP_LED_On(LED_RED);
    HAL_Delay(tempo);
    BSP_LED_Off(LED_RED);

    /* Green LED ON */
    BSP_LED_On(LED_GREEN);
    HAL_Delay(tempo);
    BSP_LED_Off(LED_GREEN);

    /* Blue LED ON */
    BSP_LED_On(LED_BLUE);
    HAL_Delay(tempo);
    BSP_LED_Off(LED_BLUE);

    /* Orange LED ON */
    BSP_LED_On(LED_ORANGE);
    HAL_Delay(tempo);
    BSP_LED_Off(LED_ORANGE);

    /* Decrease time between blinking sequence */
    tempo = tempo / 2;
    if(tempo == 1)
    {
      tempo = 400;
    }
  }

  /* Reset all signal LEDs */
  BSP_LED_Off(LED_RED);
  BSP_LED_Off(LED_GREEN);
  BSP_LED_Off(LED_BLUE);
  BSP_LED_Off(LED_ORANGE);
}


/**
* @brief  Configure HRTIM to generate a triangle on channel F2
*         Triangle can be observed on TP4 "RC Sinus"
* @param  None
* @retval None
*/
static void TriangleGenerationInit(void)
{
  GPIO_InitTypeDef         GPIO_InitStructure           = {0};
  HRTIM_OutputCfgTypeDef   HRTIM_OutputStructure        = {0};
  HRTIM_TimeBaseCfgTypeDef HRTIM_TimeBaseStructure      = {0};
  HRTIM_TimerCfgTypeDef    HRTIM_TimerWaveStructure     = {0};
  HRTIM_CompareCfgTypeDef  HRTIM_CompareStructure       = {0};

  /* GPIOC Peripheral clock enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /* HRTIM output channel configuration : HRTIM_CHF2 / PC7 */
  GPIO_InitStructure.Pin       = GPIO_PIN_7;
  GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Pull      = GPIO_NOPULL;
  GPIO_InitStructure.Alternate = GPIO_AF13_HRTIM1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Enable HRTIM clock */
  __HAL_RCC_HRTIM1_CLK_ENABLE();

  HrtimHandle.Instance = HRTIM1;
  HAL_HRTIM_Init(&HrtimHandle);

  /* HRTIM calibration */
  if(HAL_HRTIM_DLLCalibrationStart(&HrtimHandle, HRTIM_CALIBRATIONRATE_3) != HAL_OK)
  {
    Error_Handler();
  }
  if(HAL_HRTIM_PollForDLLCalibration(&HrtimHandle, HAL_MAX_DELAY) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure the output features */
  HRTIM_OutputStructure.Polarity              = HRTIM_OUTPUTPOLARITY_HIGH;
  HRTIM_OutputStructure.SetSource             = HRTIM_OUTPUTSET_TIMPER;
  HRTIM_OutputStructure.ResetSource           = HRTIM_OUTPUTRESET_TIMCMP1;
  HRTIM_OutputStructure.IdleMode              = HRTIM_OUTPUTIDLEMODE_NONE;
  HRTIM_OutputStructure.IdleLevel             = HRTIM_OUTPUTIDLELEVEL_INACTIVE;
  HRTIM_OutputStructure.FaultLevel            = HRTIM_OUTPUTFAULTLEVEL_NONE;
  HRTIM_OutputStructure.ChopperModeEnable     = HRTIM_OUTPUTCHOPPERMODE_DISABLED;
  HRTIM_OutputStructure.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;

  if(HAL_HRTIM_WaveformOutputConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F,
                                    HRTIM_OUTPUT_TF2, &HRTIM_OutputStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure HRTIM1_TIMF waveform */
  HRTIM_TimerWaveStructure.InterruptRequests     = (HRTIM_MASTER_IT_NONE | HRTIM_TIM_IT_NONE);
  HRTIM_TimerWaveStructure.DMARequests           = (HRTIM_MASTER_DMA_NONE | HRTIM_TIM_DMA_NONE);
  HRTIM_TimerWaveStructure.DMASrcAddress         = 0;
  HRTIM_TimerWaveStructure.DMADstAddress         = 0;
  HRTIM_TimerWaveStructure.DMASize               = 0;
  HRTIM_TimerWaveStructure.HalfModeEnable        = HRTIM_HALFMODE_DISABLED;
  HRTIM_TimerWaveStructure.InterleavedMode       = HRTIM_INTERLEAVED_MODE_DISABLED;
  HRTIM_TimerWaveStructure.StartOnSync           = HRTIM_SYNCSTART_DISABLED;
  HRTIM_TimerWaveStructure.ResetOnSync           = HRTIM_SYNCRESET_DISABLED;
  HRTIM_TimerWaveStructure.DACSynchro            = HRTIM_DACSYNC_NONE;
  HRTIM_TimerWaveStructure.PreloadEnable         = HRTIM_PRELOAD_DISABLED;
  HRTIM_TimerWaveStructure.UpdateGating          = HRTIM_UPDATEGATING_INDEPENDENT;
  HRTIM_TimerWaveStructure.BurstMode             = HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;
  HRTIM_TimerWaveStructure.RepetitionUpdate      = HRTIM_UPDATEONREPETITION_DISABLED;
  HRTIM_TimerWaveStructure.PushPull              = HRTIM_UPDATEONREPETITION_ENABLED;
  HRTIM_TimerWaveStructure.FaultEnable           = HRTIM_TIMFAULTENABLE_NONE;
  HRTIM_TimerWaveStructure.FaultLock             = HRTIM_TIMFAULTLOCK_READWRITE;
  HRTIM_TimerWaveStructure.DeadTimeInsertion     = HRTIM_TIMDEADTIMEINSERTION_DISABLED;
  HRTIM_TimerWaveStructure.BalancedIdleAutomaticResume = HRTIM_OUTPUTBIAR_DISABLED;
  HRTIM_TimerWaveStructure.DelayedProtectionMode = HRTIM_TIMER_F_DELAYEDPROTECTION_DISABLED;
  HRTIM_TimerWaveStructure.UpdateTrigger         = HRTIM_TIMUPDATETRIGGER_TIMER_F;
  HRTIM_TimerWaveStructure.ResetTrigger          = HRTIM_TIMRESETTRIGGER_NONE;
  HRTIM_TimerWaveStructure.ResetUpdate           = HRTIM_TIMUPDATEONRESET_DISABLED;
  HRTIM_TimerWaveStructure.ReSyncUpdate          = HRTIM_TIMERESYNC_UPDATE_UNCONDITIONAL;

  if(HAL_HRTIM_WaveformTimerConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F, &HRTIM_TimerWaveStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* We want 2MHz period */
  /* Thanks to HRTIM prescaler (*32) and system clock 170MHz */
  /* Compare value = (170MHz * 32) / 2MHz = 2720 = 0xAA0 */
  /* Also set repetition counter to get interrupt every 50us. Repetition counter =2MHz*50us = 100 */
  HRTIM_TimeBaseStructure.Period            = 0xAA0;
  HRTIM_TimeBaseStructure.RepetitionCounter = 100;
  HRTIM_TimeBaseStructure.PrescalerRatio    = HRTIM_PRESCALERRATIO_MUL32;
  HRTIM_TimeBaseStructure.Mode              = HRTIM_MODE_CONTINUOUS;

  if(HAL_HRTIM_TimeBaseConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F, &HRTIM_TimeBaseStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initialize compare value */
  /* Start from 400mV. (400mV * Period=0xAA0) / 3300mV = 330 */
  TriangCmp = 330;
  HRTIM_CompareStructure.AutoDelayedMode    = HRTIM_AUTODELAYEDMODE_REGULAR;
  HRTIM_CompareStructure.AutoDelayedTimeout = 0; /* meaningless in regular mode */
  HRTIM_CompareStructure.CompareValue       = TriangCmp;
  if(HAL_HRTIM_WaveformCompareConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F,
                                     HRTIM_COMPAREUNIT_1, &HRTIM_CompareStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable interrupt on repetition event */
  __HAL_HRTIM_TIMER_ENABLE_IT(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F, HRTIM_TIM_IT_REP);

  /* Enable the TF2 output */
  if(HAL_HRTIM_WaveformOutputStart(&HrtimHandle, HRTIM_OUTPUT_TF2) != HAL_OK)
  {
    Error_Handler();
  }

  /* Start slave timer F */
  if(HAL_HRTIM_WaveformCountStart(&HrtimHandle, HRTIM_TIMERID_TIMER_F) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable and set HRTIM F compare Interrupt */
  HAL_NVIC_SetPriority(HRTIM1_TIMF_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(HRTIM1_TIMF_IRQn);
}


/**
* @brief  Deinitialize triangle generation
* @param  None
* @retval None
*/
static void TriangleGenerationDeInit(void)
{
  /* Stop HRTIM F2 waveform generation */
  if(HAL_HRTIM_WaveformOutputStop(&HrtimHandle, HRTIM_OUTPUT_TF2) != HAL_OK)
  {
    Error_Handler();
  }

  /* Disable HRTIM interrupt */
  __HAL_HRTIM_TIMER_DISABLE_IT(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F, HRTIM_TIM_IT_REP);

  /* Stop slave timer F */
  HAL_HRTIM_WaveformCountStop(&HrtimHandle, HRTIM_TIMERID_TIMER_F);

  /* Deinitialize HRTIM */
  HAL_HRTIM_DeInit(&HrtimHandle);
  __HAL_RCC_HRTIM1_CLK_DISABLE();
  __HAL_RCC_HRTIM1_FORCE_RESET();
  HAL_Delay(1);
  __HAL_RCC_HRTIM1_RELEASE_RESET();

  /* Deinit GPIO PC7 */
  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_7);
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 170000000
  *            HCLK(Hz)                       = 170000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_M                          = 4
  *            PLL_N                          = 85
  *            PLL_P                          = 10
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            Flash Latency(WS)              = 8
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /* Enable voltage range 1 boost mode for frequency above 150 Mhz */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);
  __HAL_RCC_PWR_CLK_DISABLE();

  /* Activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM            = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN            = 85;
  RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV10;
  RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType           = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
                                           RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource        = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider       = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider      = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider      = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_8) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /**Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_PLL;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
* @brief  Error handler
* @param  None
* @retval None
*/
void Error_Handler(void)
{
  while(1)
  {
    /* blink all 4 leds in case of error */
    HAL_Delay(100);
    BSP_LED_Toggle(LED_ORANGE);
    BSP_LED_Toggle(LED_RED);
    BSP_LED_Toggle(LED_GREEN);
    BSP_LED_Toggle(LED_BLUE);
  }
}

/**
* @brief  BSP joystick call back
* @param  None
* @retval None
*/
void BSP_JOY_Callback(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin)
{
  switch(JoyPin)
  {
  case JOY_DOWN :
  case JOY_UP :
  case JOY_RIGHT :
  case JOY_LEFT :
  case JOY_SEL :
    UserEvent = JoyPin;
    break;
  default :
    break;
  }
  UserAction = 1;
}

/**
* @brief  HRTIM repetition event call back
* @param  None
* @retval None
*/
void HAL_HRTIM_RepetitionEventCallback(HRTIM_HandleTypeDef *hhrtim,
                                       uint32_t TimerIdx)
{
  /* This function is called every 50us in case of triangle generation */
  /* 50us = (Repetition counter / 2MHz) */

  if(falling_slope == 0)
  {
    /* Falling slope: PWM dutycle is updated only 1 time out of 32 to simulate regular timer  */
    counter32++;
    if(counter32 > 32)
    {
      counter32 = 0;
      TriangCmp = TriangCmp + 32;

      /* Stop at 1V. (1000mV * Period=0xAA0) / 3300mV =820 */
      if (TriangCmp >= 820)
      {
        falling_slope = 1;
      }
    }
  }
  else
  {
    /* Rising slope: PWM dutycle is updated 32 time out of 32 to show HRTIM clock *32 multiplier capability  */
    TriangCmp -= 1;

    /* Start from 400mV. (400mV * Period=0xAA0) / 3300mV = 330 */
    if (TriangCmp <= 330)
    {
      falling_slope = 0;
    }
  }

  /* Update PWM duty cycle */
  __HAL_HRTIM_SETCOMPARE(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F, HRTIM_COMPAREUNIT_1, TriangCmp);
}



