/**
  ******************************************************************************
  * @file    Demonstrations/Src/BuckBoost.c
  * @author  MCD Application Team
  * @brief   Buck and Boost demonstration
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
#define __BUCKBOOST_C

/* Includes ------------------------------------------------------------------*/
#include "BuckBoost.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Handler of the Peripherals needed */
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
FMAC_HandleTypeDef hfmac;
DMA_HandleTypeDef hdma_fmac_read;
HRTIM_HandleTypeDef hhrtim1;
RTC_HandleTypeDef hrtc;

/* Filter coefficients for FMAC usage */
/* Coefficients A[0:2] (feedback coefficients) in Q1.15 format */
static int16_t aFilterCoeffA[COEFF_VECTOR_A_SIZE];
/* Coefficients B[0:3] (feed-forward taps) in Q1.15 format */
static int16_t aFilterCoeffB[COEFF_VECTOR_B_SIZE];
/* Output data to preload in Q1.15 format */
static int16_t aOutputDataToPreload[COEFF_VECTOR_A_SIZE];
/* Controller left bit shift (absolute value) */
static uint8_t KC_shift;
/* Output voltage reference */
static int32_t REF;

/* Expected number of calculated samples */
uint16_t ExpectedCalculatedOutputSize = (uint16_t) 1;
uint32_t *Fmac_Wdata;
int16_t Fmac_output;

/* Private function prototypes -----------------------------------------------*/
static void BuckBoost_Init(void);
static void BuckBoost_DeInit(void);

static void GPIO_Init(void);
static void GPIO_DeInit(void);
static void DMA_Init(void);
static void DMA_DeInit(void);
static void RTC_Init(void);
static void RTC_DeInit(void);
static void ADC1_Init(void);
static void ADC1_DeInit(void);
static void HRTIM1_Init(void);
static void HRTIM1_DeInit(void);
static void FMAC_Init(void);
static void FMAC_DeInit(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Buck demonstration
  * @param  None
  * @retval None
  */
void Demo_Buck(void)
{
  /* 200kHz power supply at 170MHz */
  /* Coefficients A[0:2] (feedback coefficients) in Q1.15 format */
  aFilterCoeffA[0] = (int16_t) (0x616);
  aFilterCoeffA[1] = (int16_t) (0xFE93);
  aFilterCoeffA[2] = (int16_t) (0xFF57);
  /* Coefficients B[0:3] (feed-forward taps) in Q1.15 format - Negate values */
  aFilterCoeffB[0] = (int16_t) (-0x599C);
  aFilterCoeffB[1] = (int16_t) (-0xB177);
  aFilterCoeffB[2] = (int16_t) (-0xA6BB);
  aFilterCoeffB[3] = (int16_t) (-0x4EE0);
  /* Output data to preload in Q1.15 format */
  aOutputDataToPreload[0] = (int16_t) (0x0000);
  aOutputDataToPreload[0] = (int16_t) (0x0000);
  aOutputDataToPreload[0] = (int16_t) (0x0000);
  /* Controller left bit shift (absolute value) */
  KC_shift = (5);
  /* Output voltage reference */
  REF = (750);

  Demo.NbrActiveLoad = 0u;
  Demo.Mode = MANUAL;

  /* Initialize peripherals */
  BuckBoost_Init();

  /* Start calculation of IIR filter */
  if (HAL_FMAC_FilterStart(&hfmac,&Fmac_output,&ExpectedCalculatedOutputSize) != HAL_OK)
  {
    Error_Handler();
  }

  /* Perform an ADC automatic self-calibration and enable ADC */
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);

  /* Start ADC and DMA */
  Fmac_Wdata = (uint32_t *) FMAC_WDATA;
  HAL_ADC_Start_DMA(&hadc1,Fmac_Wdata,1);

  /* Set PWM period and initialize its duty cycle - Buck part */
  Demo.periodBuck = HAL_HRTIM_FreqToTicks(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, SwitchFreqBoost);
  __HAL_HRTIM_SETPERIOD(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, Demo.periodBuck);
  __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_1, HRTIM_MIN_DUTY); // Start-Up value for PWM
  __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_3, HRTIM_MAX_DUTY); // Max allowed for PWM

  /* Start the PWMs */
  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2);
  HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_C);

  /* Toggle led and manage applied Load(s) until SEL key is pressed */
  BuckBoost_LoadSelection();

  /* Deinitialize buck and boost demo */
  BuckBoost_DeInit();
}


/**
  * @brief  Buck and Boost demonstration
  * @param  None
  * @retval None
  */
void Demo_BuckBoost(void)
{
  /* coefficients for 200kHz power supply at 170MHz */
  /* Coefficients A[0:2] (feedback coefficients) in Q1.15 format */
  aFilterCoeffA[0] = (int16_t) (0x616);
  aFilterCoeffA[1] = (int16_t) (0xFE93);
  aFilterCoeffA[2] = (int16_t) (0xFF57);
  /* Coefficients B[0:3] (feed-forward taps) in Q1.15 format - Negate values */
  aFilterCoeffB[0] = (int16_t) (-0x704C);
  aFilterCoeffB[1] = (int16_t) (-0x9A96);
  aFilterCoeffB[2] = (int16_t) (-0x8FF0);
  aFilterCoeffB[3] = (int16_t) (-0x65A6);
  /* Output data to preload in Q1.15 format */
  aOutputDataToPreload[0] = (int16_t) (0x0000);
  aOutputDataToPreload[0] = (int16_t) (0x0000);
  aOutputDataToPreload[0] = (int16_t) (0x0000);
  /* Controller left bit shift (absolute value) */
  KC_shift = (5);
  /* Output voltage reference */
  REF = (1130);

  Demo.NbrActiveLoad = 0u;
  Demo.Mode = MANUAL;

  /* Initialize peripherals */
  BuckBoost_Init();

  /* Start calculation of IIR filter */
  if (HAL_FMAC_FilterStart(&hfmac,&Fmac_output,&ExpectedCalculatedOutputSize) != HAL_OK)
  {
    Error_Handler();
  }

  /* Perform an ADC automatic self-calibration and enable ADC */
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);

  /* Start ADC and DMA */
  Fmac_Wdata = (uint32_t *) FMAC_WDATA;
  HAL_ADC_Start_DMA(&hadc1,Fmac_Wdata,1);

  /* Set PWM period and fix its duty cycle - Buck part */
  Demo.periodBuck = HAL_HRTIM_FreqToTicks(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, SwitchFreqBuck);
  __HAL_HRTIM_SETPERIOD(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, Demo.periodBuck);
  __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_1, Demo.periodBuck * 50 / 100); // Fix value for PWM
  __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_3, HRTIM_MAX_DUTY); // Max allowed PWM

  /* Set PWM period and initialize its duty cycle - Boost part */
  Demo.periodBoost = HAL_HRTIM_FreqToTicks(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, SwitchFreqBoost);
  __HAL_HRTIM_SETPERIOD(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, Demo.periodBoost);
  __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_COMPAREUNIT_1, HRTIM_MIN_DUTY); // Start-Up value for PWM
  __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_COMPAREUNIT_3, HRTIM_MAX_DUTY); // Max allowed PWM

  /* Start the PWMs */
  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TC1 | HRTIM_OUTPUT_TC2); // Buck
  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); // Boost
  HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_C | HRTIM_TIMERID_TIMER_D);

  /* Toggle led and manage applied Load(s) until SEL key is pressed */
  BuckBoost_LoadSelection();

  /* Deinitialize buck and boost demo */
  BuckBoost_DeInit();
}


/**
  * @brief  Buck and boost initialization: GPIO, DMA, RTC, ADC, HRTIM, FMAC
  * @param  None
  * @retval None
  */
void BuckBoost_Init(void)
{

  /* Enable peripheral clocks */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_RTC_ENABLE();
  __HAL_RCC_ADC12_CLK_ENABLE();
  __HAL_RCC_HRTIM1_CLK_ENABLE();
  __HAL_RCC_FMAC_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMAMUX1_CLK_ENABLE();

  /* Initialize peripherals */
  GPIO_Init();
  DMA_Init();
  RTC_Init();
  ADC1_Init();
  HRTIM1_Init();
  FMAC_Init();
  
  /* Check existing voltage on JP1 input connector */
  CheckInputVoltage();
  
  /* Set signal LED used */
  BSP_LED_On(LED_BLUE);
  BSP_LED_Off(LED_GREEN);
  BSP_LED_Off(LED_ORANGE);
  BSP_LED_Off(LED_RED);
}


/**
  * @brief  Buck and boost deinitialization: GPIO, DMA, RTC, ADC, HRTIM, FMAC
  * @param  None
  * @retval None
  */
void BuckBoost_DeInit(void)
{
  /* Reset all signal LEDs */
  BSP_LED_Off(LED_BLUE);
  BSP_LED_Off(LED_GREEN);
  BSP_LED_Off(LED_ORANGE);
  BSP_LED_Off(LED_RED);

  /* De-initialize the peripherals used */
  FMAC_DeInit();
  HRTIM1_DeInit();
  ADC1_DeInit();
  RTC_DeInit();
  DMA_DeInit();
  GPIO_DeInit();
  
  /* Disable peripheral clocks */
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_RTC_DISABLE();
  __HAL_RCC_ADC12_CLK_DISABLE();
  __HAL_RCC_HRTIM1_CLK_DISABLE();
  __HAL_RCC_FMAC_CLK_DISABLE();
  __HAL_RCC_DMA1_CLK_DISABLE();
  __HAL_RCC_DMAMUX1_CLK_DISABLE();
}

/**
  * @brief  Buck and boost initialization: GPIO, ADC, RTC, HRTIM
  * @param  None
  * @retval None
  */
void BuckBoost_LoadSelection(void)
{
  /* Toggle led until SEL key is pressed */
  while(UserEvent != JOY_SEL )
  {
    BSP_LED_On(LED_BLUE);
    if (UserAction != 0x00u)
    {
      /* Checks the joystick usage */
      switch(UserEvent)
      {
      case JOY_RIGHT :
        {
          Demo.NbrActiveLoad += 1u;
          /* Clamp the value to Max if over-pressed by user */
          if (Demo.NbrActiveLoad >= 2u)
          {
            Demo.NbrActiveLoad = 2u;
          }

          UserAction = 0u;
          break;
        }

      case JOY_LEFT :
        {
          Demo.NbrActiveLoad -= 1u;
          /* Clamp the value to Min if over-pressed by user */
          if (Demo.NbrActiveLoad <= 0u)
          {
            Demo.NbrActiveLoad = 0u;
          }
          UserAction = 0u;
          break;
        }

      case JOY_UP :
        {
          Demo.Mode = AUTOMATIC;
          UserAction = 0u;
          break;
        }

      case JOY_DOWN :
        {
          Demo.Mode = MANUAL;
          UserAction = 0u;
          break;
        }

       default:
         break;
      }

      /* Keypress debounce */
      HAL_Delay(200);
    }

    /* Toggle the load in automatic mode between No and Full load */
    if (Demo.Mode == AUTOMATIC)
    {
      if (Demo.NbrActiveLoad >= 1u)
      {
        Demo.NbrActiveLoad = 0u;
      }
      else
      {
        Demo.NbrActiveLoad = 2u;
      }
      
      /* Wait for 1s (Buck) or 1.5s (Buck&Boost) to catch up signal on scope */
      HAL_Delay(100 * (StateMachine^4));
    }

    /* Manage the LEDs and the Resistors network figuring output load */
    switch(Demo.NbrActiveLoad)
    {
    case 0u :
      {
        /* Disable Load_1 & Load_2 (No load) */
        HAL_GPIO_WritePin(GPIOC, BUCKBOOST_LOAD_1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, BUCKBOOST_LOAD_2_Pin, GPIO_PIN_RESET);
        BSP_LED_Off(LED_GREEN);
        BSP_LED_Off(LED_ORANGE);
        break;
      }

    case 1u :
      {
        /* Enable Load_1 and disable Load_2  (Half load) */
        HAL_GPIO_WritePin(GPIOC, BUCKBOOST_LOAD_1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, BUCKBOOST_LOAD_2_Pin, GPIO_PIN_RESET);
        BSP_LED_On(LED_GREEN);
        BSP_LED_Off(LED_ORANGE);
        break;
      }

    case 2u :
      {
        /* Enable Load_1 & Load_2 (Full load) */
        HAL_GPIO_WritePin(GPIOC, BUCKBOOST_LOAD_1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, BUCKBOOST_LOAD_2_Pin, GPIO_PIN_SET);
        BSP_LED_On(LED_GREEN);
        BSP_LED_On(LED_ORANGE);
        break;
      }

      default:
        break;
    }
  }
}


/**
  * @brief  Check absence of input voltage on JP1 connector
  * @note   Safe the USB Type-C usage
  * @param  None
  * @retval None
  */
void CheckInputVoltage(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};
  uint16_t ADCInput = 0;
  int32_t InputVoltage;

  /* Change the default ADC initialization */
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /* Configure the BUCKBOOST_VIN signal */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  
  /* Measure BUCKBOOST_VIN */
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, 100);
  ADCInput = HAL_ADC_GetValue(&hadc1);
  InputVoltage = __HAL_ADC_CALC_DATA_TO_VOLTAGE(3300, ADCInput, ADC_RESOLUTION_12B);
  if (InputVoltage <= (int32_t)(+0.10) )
  {
    /* Allow the USB Type-C connector as Input Voltage */
    HAL_GPIO_WritePin(BUCKBOOST_USBPD_EN_GPIO_Port, BUCKBOOST_USBPD_EN_Pin, GPIO_PIN_SET);
  }
  else
  {
    /* Make sure the USB Type-C connector is Not used as Input Voltage */
    HAL_GPIO_WritePin(BUCKBOOST_USBPD_EN_GPIO_Port, BUCKBOOST_USBPD_EN_Pin, GPIO_PIN_RESET);
  }
  HAL_ADC_Stop(&hadc1);


  /* Restore the default ADC initialization */
  hadc1.Init.DataAlign = ADC_DATAALIGN_LEFT;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_HRTIM_TRG1;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /* ReConfigure the BUCKBOOST_VOUT signal */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.OffsetNumber = ADC_OFFSET_1;
  sConfig.Offset = REF;
  sConfig.OffsetSign = ADC_OFFSET_SIGN_NEGATIVE;
  sConfig.OffsetSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /*Configure GPIO pins : BUCKBOOST_LOAD_1_Pin BUCKBOOST_LOAD_2_Pin */
  GPIO_InitStruct.Pin = BUCKBOOST_LOAD_1_Pin|BUCKBOOST_LOAD_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : BUCKBOOST_USBPD_EN_Pin */
  GPIO_InitStruct.Pin = BUCKBOOST_USBPD_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

#ifdef DSMPS_DEBUG
  /*Configure GPIO pin : GPO1_Pin */
  GPIO_InitStruct.Pin = GPO1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPO1_GPIO_Port, &GPIO_InitStruct);
#endif /* DSMPS_DEBUG */

  /** GPIO Configuration
  PC3     ------> BUCKBOOST_USBPD_EN 
  PC14    ------> BUCKBOOST_LOAD_1
  PC15    ------> BUCKBOOST_LOAD_2
  PB9     ------> GPO1 (debug pin toggled during ISR)
  */
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, BUCKBOOST_LOAD_1_Pin|BUCKBOOST_LOAD_2_Pin, GPIO_PIN_RESET);
}


/**
  * @brief GPIO Deinitialization Function
  * @param None
  * @retval None
  */
static void GPIO_DeInit(void)
{
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, BUCKBOOST_LOAD_1_Pin|BUCKBOOST_LOAD_2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BUCKBOOST_USBPD_EN_GPIO_Port, BUCKBOOST_USBPD_EN_Pin, GPIO_PIN_RESET);

  /** GPIO Configuration
  PC3     ------> BUCKBOOST_USBPD_EN 
  PC14    ------> BUCKBOOST_LOAD_1
  PC15    ------> BUCKBOOST_LOAD_2
  */
  HAL_GPIO_DeInit(GPIOC, BUCKBOOST_LOAD_1_Pin|BUCKBOOST_LOAD_2_Pin|BUCKBOOST_USBPD_EN_Pin);
}


/** 
  * Enable DMA controller clock
  */
static void DMA_Init(void) 
{
  /* ADC1 <-> DMA Init */
  /* ADC1 Init */
  hdma_adc1.Instance = DMA1_Channel1;
  hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
  hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_adc1.Init.MemInc = DMA_MINC_DISABLE;
  hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_adc1.Init.Mode = DMA_CIRCULAR;
  hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
  if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
  {
    Error_Handler();
  }
  /* Connect the DMA channel to the ADC structure */
  __HAL_LINKDMA(&hadc1,DMA_Handle,hdma_adc1);

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
  /* Because CubeFW enables DMA interrupt even if not enabled */
  HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
}


/** 
  * Disable DMA controller clock
  */
static void DMA_DeInit(void) 
{
  /* ADC1 <-> DMA DeInit */
  if (HAL_DMA_DeInit(&hdma_adc1) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* DMA interrupt Deinit */
  HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
}


/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void RTC_Init(void)
{
  /**Initialize RTC
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
  * @brief RTC Deinitialization Function
  * @param None
  * @retval None
  */
static void RTC_DeInit(void)
{
  /** Deinitialize RTC
  */
  if (HAL_RTC_DeInit(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void ADC1_Init(void)
{
  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /**Common config
  */
  /* ADC clock configured to use asynchronous clock */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  /* left aligned data equivalent to 8dB gain */
  hadc1.Init.DataAlign = ADC_DATAALIGN_LEFT;
  hadc1.Init.GainCompensation = 0;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  /* Conversion triggered by HRTimer Trig 1 rising edge */
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_HRTIM_TRG1;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  /* Allow ADC to generate DMA requests after every conversion */
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  /* Use offset feature */
  sConfig.OffsetNumber = ADC_OFFSET_1;
  sConfig.Offset = REF;
  sConfig.OffsetSign = ADC_OFFSET_SIGN_NEGATIVE;
  sConfig.OffsetSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /**ADC1 GPIO Configuration    
  PA1     ------> ADC1_IN2
  PA2     ------> ADC1_IN3
  PA3     ------> ADC1_IN4
  PB11    ------> ADC1_IN14 
  */
  GPIO_InitStruct.Pin = BUCKBOOST_VIN_Pin|BUCKBOOST_I_IN_AVG_Pin|BUCKBOOST_VOUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BUCKBOOST_I_IN_SENSE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUCKBOOST_I_IN_SENSE_GPIO_Port, &GPIO_InitStruct);
}


/**
  * @brief ADC1 Deinitialization Function
  * @param None
  * @retval None
  */
static void ADC1_DeInit(void)
{
  if (HAL_ADC_DeInit(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /**ADC1 GPIO Configuration    
  PA1     ------> ADC1_IN2
  PA2     ------> ADC1_IN3
  PA3     ------> ADC1_IN4
  PB11    ------> ADC1_IN14 
  */
  HAL_GPIO_DeInit(GPIOA, BUCKBOOST_VIN_Pin|BUCKBOOST_I_IN_AVG_Pin|BUCKBOOST_VOUT_Pin);
  HAL_GPIO_DeInit(BUCKBOOST_I_IN_SENSE_GPIO_Port, BUCKBOOST_I_IN_SENSE_Pin);
}


/**
  * @brief HRTIM1 Initialization Function
  * @param None
  * @retval None
  */
static void HRTIM1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* HRTIMER configuration */
  HRTIM_ADCTriggerCfgTypeDef pADCTriggerCfg = {0};
  HRTIM_TimeBaseCfgTypeDef pTimeBaseCfg = {0};
  HRTIM_TimerCtlTypeDef pTimerCtl = {0};
  HRTIM_TimerCfgTypeDef pTimerCfg = {0};
  HRTIM_CompareCfgTypeDef pCompareCfg = {0};
  HRTIM_DeadTimeCfgTypeDef pDeadTimeCfg = {0};
  HRTIM_OutputCfgTypeDef pOutputCfg = {0};

  hhrtim1.Instance = HRTIM1;
  hhrtim1.Init.HRTIMInterruptResquests = HRTIM_IT_NONE;
  hhrtim1.Init.SyncOptions = HRTIM_SYNCOPTION_NONE;
  if (HAL_HRTIM_Init(&hhrtim1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_HRTIM_DLLCalibrationStart(&hhrtim1, HRTIM_CALIBRATIONRATE_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_HRTIM_PollForDLLCalibration(&hhrtim1, 10) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the ADC triggering
  */
  switch (StateMachine)
  {
  case STATE_BUCK:
    {
      pADCTriggerCfg.UpdateSource = HRTIM_ADCTRIGGERUPDATE_TIMER_C;
      pADCTriggerCfg.Trigger = HRTIM_ADCTRIGGEREVENT13_TIMERC_PERIOD;
      if (HAL_HRTIM_ADCTriggerConfig(&hhrtim1, HRTIM_ADCTRIGGER_1, &pADCTriggerCfg) != HAL_OK)
      {
    	Error_Handler();
      }
    break;
    }

  case STATE_BUCKBOOST:
    {
      pADCTriggerCfg.UpdateSource = HRTIM_ADCTRIGGERUPDATE_TIMER_D;
      pADCTriggerCfg.Trigger = HRTIM_ADCTRIGGEREVENT13_TIMERD_PERIOD;
      if (HAL_HRTIM_ADCTriggerConfig(&hhrtim1, HRTIM_ADCTRIGGER_1, &pADCTriggerCfg) != HAL_OK)
      {
      	Error_Handler();
      }
      break;
  }
      
    default :
      Error_Handler();
      break;
    }

  if (HAL_HRTIM_ADCPostScalerConfig(&hhrtim1, HRTIM_ADCTRIGGER_1, 0x0) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Configure TIMER C (buck part) & D (Boost part) */
  pTimeBaseCfg.Period = 27200;
  pTimeBaseCfg.RepetitionCounter = 0x00;
  pTimeBaseCfg.PrescalerRatio = HRTIM_PRESCALERRATIO_MUL32;
  pTimeBaseCfg.Mode = HRTIM_MODE_CONTINUOUS;
  if (HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, &pTimeBaseCfg) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pTimeBaseCfg) != HAL_OK)
  {
    Error_Handler();
  }
  pTimerCtl.UpDownMode = HRTIM_TIMERUPDOWNMODE_UP;
  pTimerCtl.GreaterCMP1 = HRTIM_TIMERGTCMP1_EQUAL;
  pTimerCtl.GreaterCMP3 = HRTIM_TIMERGTCMP3_EQUAL;
  pTimerCtl.DualChannelDacEnable = HRTIM_TIMER_DCDE_DISABLED;
  if (HAL_HRTIM_WaveformTimerControl(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, &pTimerCtl) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_HRTIM_WaveformTimerControl(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pTimerCtl) != HAL_OK)
  {
    Error_Handler();
  }
  pTimerCfg.InterruptRequests = HRTIM_TIM_IT_NONE;
  pTimerCfg.DMARequests = HRTIM_TIM_DMA_NONE;
  pTimerCfg.DMASrcAddress = 0x0000;
  pTimerCfg.DMADstAddress = 0x0000;
  pTimerCfg.DMASize = 0x1;
  pTimerCfg.HalfModeEnable = HRTIM_HALFMODE_DISABLED;
  pTimerCfg.InterleavedMode = HRTIM_INTERLEAVED_MODE_DISABLED;
  pTimerCfg.StartOnSync = HRTIM_SYNCSTART_DISABLED;
  pTimerCfg.ResetOnSync = HRTIM_SYNCRESET_DISABLED;
  pTimerCfg.DACSynchro = HRTIM_DACSYNC_NONE;
  pTimerCfg.PreloadEnable = HRTIM_PRELOAD_DISABLED;
  pTimerCfg.UpdateGating = HRTIM_UPDATEGATING_INDEPENDENT;
  pTimerCfg.BurstMode = HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;
  pTimerCfg.RepetitionUpdate = HRTIM_UPDATEONREPETITION_DISABLED;
  pTimerCfg.PushPull = HRTIM_TIMPUSHPULLMODE_DISABLED;
  pTimerCfg.FaultEnable = HRTIM_TIMFAULTENABLE_NONE;
  pTimerCfg.FaultLock = HRTIM_TIMFAULTLOCK_READWRITE;
  pTimerCfg.DeadTimeInsertion = HRTIM_TIMDEADTIMEINSERTION_ENABLED;
  pTimerCfg.DelayedProtectionMode = HRTIM_TIMER_A_B_C_DELAYEDPROTECTION_DISABLED;
  pTimerCfg.UpdateTrigger = HRTIM_TIMUPDATETRIGGER_NONE;
  pTimerCfg.ResetTrigger = HRTIM_TIMRESETTRIGGER_NONE;
  pTimerCfg.ResetUpdate = HRTIM_TIMUPDATEONRESET_ENABLED;
  pTimerCfg.ReSyncUpdate = HRTIM_TIMERESYNC_UPDATE_UNCONDITIONAL;
  if (HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, &pTimerCfg) != HAL_OK)
  {
    Error_Handler();
  }
  pTimerCfg.DelayedProtectionMode = HRTIM_TIMER_D_E_DELAYEDPROTECTION_DISABLED;
  if (HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pTimerCfg) != HAL_OK)
  {
    Error_Handler();
  }
  
  /** Configure the HRTIM comparison units
  */
  pCompareCfg.AutoDelayedMode = HRTIM_AUTODELAYEDMODE_REGULAR;
  pCompareCfg.AutoDelayedTimeout = 0x0000;
  /* Secure with maximum duty cycle */
  pCompareCfg.CompareValue = 5000;
  if (HAL_HRTIM_WaveformCompareConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_3, &pCompareCfg) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_HRTIM_WaveformCompareConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_COMPAREUNIT_3, &pCompareCfg) != HAL_OK)
  {
    Error_Handler();
  }
  
  /** Configure High and Low side switches
  */
  /* Tune Deadtime */
  pDeadTimeCfg.Prescaler = HRTIM_TIMDEADTIME_PRESCALERRATIO_MUL8;
  pDeadTimeCfg.RisingValue = 250;
  pDeadTimeCfg.RisingSign = HRTIM_TIMDEADTIME_RISINGSIGN_POSITIVE;
  pDeadTimeCfg.RisingLock = HRTIM_TIMDEADTIME_RISINGLOCK_WRITE;
  pDeadTimeCfg.RisingSignLock = HRTIM_TIMDEADTIME_RISINGSIGNLOCK_WRITE;
  pDeadTimeCfg.FallingValue = 250;
  pDeadTimeCfg.FallingSign = HRTIM_TIMDEADTIME_FALLINGSIGN_POSITIVE;
  pDeadTimeCfg.FallingLock = HRTIM_TIMDEADTIME_FALLINGLOCK_WRITE;
  pDeadTimeCfg.FallingSignLock = HRTIM_TIMDEADTIME_FALLINGSIGNLOCK_WRITE;
  if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, &pDeadTimeCfg) != HAL_OK)
  {
    Error_Handler();
  }
  pDeadTimeCfg.Prescaler = HRTIM_TIMDEADTIME_PRESCALERRATIO_MUL8;
  pDeadTimeCfg.RisingValue = 250;
  pDeadTimeCfg.RisingSign = HRTIM_TIMDEADTIME_RISINGSIGN_POSITIVE;
  pDeadTimeCfg.RisingLock = HRTIM_TIMDEADTIME_RISINGLOCK_WRITE;
  pDeadTimeCfg.RisingSignLock = HRTIM_TIMDEADTIME_RISINGSIGNLOCK_WRITE;
  pDeadTimeCfg.FallingValue = 300;
  pDeadTimeCfg.FallingSign = HRTIM_TIMDEADTIME_FALLINGSIGN_POSITIVE;
  pDeadTimeCfg.FallingLock = HRTIM_TIMDEADTIME_FALLINGLOCK_WRITE;
  pDeadTimeCfg.FallingSignLock = HRTIM_TIMDEADTIME_FALLINGSIGNLOCK_WRITE;
  if (HAL_HRTIM_DeadTimeConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pDeadTimeCfg) != HAL_OK)
  {
    Error_Handler();
  }
  pOutputCfg.Polarity = HRTIM_OUTPUTPOLARITY_HIGH;
  pOutputCfg.SetSource = HRTIM_OUTPUTSET_TIMPER;
  pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_TIMCMP1|HRTIM_OUTPUTRESET_TIMCMP3;
  pOutputCfg.IdleMode = HRTIM_OUTPUTIDLEMODE_NONE;
  pOutputCfg.IdleLevel = HRTIM_OUTPUTIDLELEVEL_ACTIVE;
  pOutputCfg.FaultLevel = HRTIM_OUTPUTFAULTLEVEL_INACTIVE;
  pOutputCfg.ChopperModeEnable = HRTIM_OUTPUTCHOPPERMODE_DISABLED;
  pOutputCfg.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;
  if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_OUTPUT_TC1, &pOutputCfg) != HAL_OK)
  {
    Error_Handler();
  }
  pOutputCfg.IdleLevel = HRTIM_OUTPUTIDLELEVEL_INACTIVE;
  pOutputCfg.FaultLevel = HRTIM_OUTPUTFAULTLEVEL_NONE;
  if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD1, &pOutputCfg) != HAL_OK)
  {
    Error_Handler();
  }
  pOutputCfg.SetSource = HRTIM_OUTPUTSET_NONE;
  pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_NONE;
  pOutputCfg.FaultLevel = HRTIM_OUTPUTFAULTLEVEL_INACTIVE;
  if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_OUTPUT_TC2, &pOutputCfg) != HAL_OK)
  {
    Error_Handler();
  }
  pOutputCfg.IdleLevel = HRTIM_OUTPUTIDLELEVEL_ACTIVE;
  pOutputCfg.FaultLevel = HRTIM_OUTPUTFAULTLEVEL_NONE;
  if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD2, &pOutputCfg) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pTimeBaseCfg) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_HRTIM_WaveformTimerControl(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pTimerCtl) != HAL_OK)
  {
    Error_Handler();
  }

  /**HRTIM1 GPIO Configuration
  PB12     ------> HRTIM1_CHC1
  PB13     ------> HRTIM1_CHC2
  PB14     ------> HRTIM1_CHD1
  PB15     ------> HRTIM1_CHD2 
  */
  GPIO_InitStruct.Pin = BUCKBOOST_P1_DRIVE_Pin|BUCKBOOST_N1_DRIVE_Pin|BUCKBOOST_N2_DRIVE_Pin|BUCKBOOST_P2_DRIVE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF13_HRTIM1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


/**
  * @brief HRTIM1 Deinitialization Function
  * @param None
  * @retval None
  */
static void HRTIM1_DeInit(void)
{
  if (HAL_HRTIM_DeInit(&hhrtim1) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_RCC_HRTIM1_CLK_DISABLE();
  __HAL_RCC_HRTIM1_FORCE_RESET();
  HAL_Delay(1);
  __HAL_RCC_HRTIM1_RELEASE_RESET();
}


/**
  * @brief FMAC Initialization Function
  * @param None
  * @retval None
  */
static void FMAC_Init(void)
{
  hfmac.Instance = FMAC;
  if (HAL_FMAC_Init(&hfmac) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure the FMAC peripheral */
  Demo.sFmacConfig.InputBaseAddress  = INPUT_BUFFER_BASE;
  Demo.sFmacConfig.InputBufferSize   = INPUT_BUFFER_SIZE;
  Demo.sFmacConfig.InputThreshold    = INPUT_THRESHOLD;
  Demo.sFmacConfig.CoeffBaseAddress  = COEFFICIENT_BUFFER_BASE;
  Demo.sFmacConfig.CoeffBufferSize   = COEFFICIENT_BUFFER_SIZE;
  Demo.sFmacConfig.OutputBaseAddress = OUTPUT_BUFFER_BASE;
  Demo.sFmacConfig.OutputBufferSize  = OUTPUT_BUFFER_SIZE;
  Demo.sFmacConfig.OutputThreshold   = OUTPUT_THRESHOLD;
  Demo.sFmacConfig.pCoeffA           = aFilterCoeffA;
  Demo.sFmacConfig.CoeffASize        = COEFF_VECTOR_A_SIZE;
  Demo.sFmacConfig.pCoeffB           = aFilterCoeffB;
  Demo.sFmacConfig.CoeffBSize        = COEFF_VECTOR_B_SIZE;
  Demo.sFmacConfig.Filter            = FMAC_FUNC_IIR_DIRECT_FORM_1;
  Demo.sFmacConfig.InputAccess       = FMAC_BUFFER_ACCESS_NONE;
  Demo.sFmacConfig.OutputAccess      = FMAC_BUFFER_ACCESS_IT;
  Demo.sFmacConfig.Clip              = FMAC_CLIP_ENABLED;
  Demo.sFmacConfig.P                 = COEFF_VECTOR_B_SIZE;
  Demo.sFmacConfig.Q                 = COEFF_VECTOR_A_SIZE;
  Demo.sFmacConfig.R                 = KC_shift;

  
  if (HAL_FMAC_FilterConfig(&hfmac, &Demo.sFmacConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* Preload the input and output buffers */
  if (HAL_FMAC_FilterPreload(&hfmac, NULL, INPUT_BUFFER_SIZE,
                           aOutputDataToPreload, COEFF_VECTOR_A_SIZE) != HAL_OK)
  {
    Error_Handler();
  }

  /* FMAC interrupt Init */
  HAL_NVIC_SetPriority(FMAC_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(FMAC_IRQn);
}


/**
  * @brief FMAC Deinitialization Function
  * @param None
  * @retval None
  */
static void FMAC_DeInit(void)
{
  if (HAL_FMAC_DeInit(&hfmac) != HAL_OK)
  {
    Error_Handler();
  }
}


/* Private functions ---------------------------------------------------------*/
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
  uint32_t ticks = (uint32_t)((float)fHRTIM_HZ * scaleTable[scaleIndex] * (float)ns * 1.0e-9f);

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
  uint32_t ticks = (uint32_t)((float)fHRTIM_HZ * scaleTable[scaleIndex] / (float)Freq);

  return MIN(ticks, UINT16_MAX);
}



