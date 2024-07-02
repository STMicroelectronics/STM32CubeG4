/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    DAC/DAC_SignalsGeneration/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a short description of how to use the DAC
  *          peripheral to generate several signals.
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  DAC_WAVE_SAWTOOTH,
  DAC_WAVE_SINE
} t_wavetype;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SAWTOOTH_NB_STEPS  60
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac1;
DMA_HandleTypeDef hdma_dac1_ch1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;

/* USER CODE BEGIN PV */
__IO uint8_t ubKeyPressed = RESET;
__IO t_wavetype ubSelectedWavesForm = DAC_WAVE_SAWTOOTH;

/* Sine wave values for a complete symbol */
uint16_t sinewave[60] = {
0x07ff,0x08cb,0x0994,0x0a5a,0x0b18,0x0bce,0x0c79,0x0d18,0x0da8,0x0e29,0x0e98,0x0ef4,0x0f3e,0x0f72,0x0f92,0x0f9d,
0x0f92,0x0f72,0x0f3e,0x0ef4,0x0e98,0x0e29,0x0da8,0x0d18,0x0c79,0x0bce,0x0b18,0x0a5a,0x0994,0x08cb,0x07ff,0x0733,
0x066a,0x05a4,0x04e6,0x0430,0x0385,0x02e6,0x0256,0x01d5,0x0166,0x010a,0x00c0,0x008c,0x006c,0x0061,0x006c,0x008c,
0x00c0,0x010a,0x0166,0x01d5,0x0256,0x02e6,0x0385,0x0430,0x04e6,0x05a4,0x066a,0x0733};

static DAC_ChannelConfTypeDef sDacConfig = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_DAC1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */
static void DAC_ChangeWave(t_wavetype wave);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DAC1_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  /* Configure LED3 */
  BSP_LED_Init(LED3);
  /* Configure User push-button in Interrupt mode */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
  
  /*## Start Time Base triggers ############################################*/
  /* Enable TIM peripheral counter */
  if (HAL_TIM_Base_Start(&htim2) != HAL_OK)
  {
    /* Counter enable error */
    Error_Handler();
  }
  if (HAL_TIM_Base_Start(&htim6) != HAL_OK)
  {
    /* Counter enable error */
    Error_Handler();
  }
  
  /*## Start DAC conversions ###############################################*/
  /* Start DAC wave generation */
  if (HAL_DAC_Start(&hdac1, DAC_CHANNEL_1) != HAL_OK)
  {
    /* DAC conversion start error */
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  BSP_LED_On(LED1);
  while (1)
  {
    /* If the Key is pressed */
    if (ubKeyPressed != RESET)
    {
      /* select waves forms according to the User push-button status */
      if (ubSelectedWavesForm == DAC_WAVE_SAWTOOTH) ubSelectedWavesForm = DAC_WAVE_SINE;
      else ubSelectedWavesForm = DAC_WAVE_SAWTOOTH;
      
      DAC_ChangeWave(ubSelectedWavesForm);

      ubKeyPressed = RESET;
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 75;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DAC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC1_Init(void)
{

  /* USER CODE BEGIN DAC1_Init 0 */

  /* USER CODE END DAC1_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC1_Init 1 */

  /* USER CODE END DAC1_Init 1 */

  /** DAC Initialization
  */
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_HighFrequency = DAC_HIGH_FREQUENCY_INTERFACE_MODE_AUTOMATIC;
  sConfig.DAC_DMADoubleDataMode = DISABLE;
  sConfig.DAC_SignedFormat = DISABLE;
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_T2_TRGO;
  sConfig.DAC_Trigger2 = DAC_TRIGGER_T6_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_EXTERNAL;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Sawtooth wave generation on DAC OUT1
  */
  if (HAL_DACEx_SawtoothWaveGenerate(&hdac1, DAC_CHANNEL_1, DAC_SAWTOOTH_POLARITY_INCREMENT, 0, 0x10000/SAWTOOTH_NB_STEPS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC1_Init 2 */
  if (HAL_DACEx_SelfCalibrate(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* Save DAC configuration to reconfigure it upon needs later */
  sDacConfig = sConfig;
  /* USER CODE END DAC1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 149;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 0;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 2499;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

static void DAC_ChangeWave(t_wavetype wave)
{
  uint32_t tmp;
  
  /* Suspend Time Base triggers */
  if (HAL_TIM_Base_Stop(&htim2) != HAL_OK)
  {
    /* Counter enable error */
    Error_Handler();
  }
  if (HAL_TIM_Base_Stop(&htim6) != HAL_OK)
  {
    /* Counter enable error */
    Error_Handler();
  }
  
  switch(wave)
  {
    case DAC_WAVE_SAWTOOTH:
      if (HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1) != HAL_OK)
      {
        /* DAC conversion start error */
        Error_Handler();
      }
      break;
    case DAC_WAVE_SINE:
      if (HAL_DAC_Stop(&hdac1, DAC_CHANNEL_1) != HAL_OK)
      {
        /* DAC conversion start error */
        Error_Handler();
      }
      break;
    default:
      Error_Handler();
      break;
  }

  /* Re-configure DAC */
  tmp = sDacConfig.DAC_Trigger;
  sDacConfig.DAC_Trigger = sDacConfig.DAC_Trigger2;
  sDacConfig.DAC_Trigger2 = tmp;
  if (HAL_DAC_ConfigChannel(&hdac1, &sDacConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Generate new wave */
  switch(wave)
  {
    case DAC_WAVE_SAWTOOTH:
      if (HAL_DACEx_SawtoothWaveGenerate(&hdac1, DAC_CHANNEL_1, DAC_SAWTOOTH_POLARITY_INCREMENT, 0, 0x10000/SAWTOOTH_NB_STEPS) != HAL_OK)
      {
        Error_Handler();
      }
      if (HAL_DAC_Start(&hdac1, DAC_CHANNEL_1) != HAL_OK)
      {
        /* DAC conversion start error */
        Error_Handler();
      }
      break;
    case DAC_WAVE_SINE:
      if (HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1,
                            (uint32_t *)sinewave,
                            60,
                            DAC_ALIGN_12B_R
                           ) != HAL_OK)
      {
        /* DAC conversion start error */
        Error_Handler();
      }
      break;
    default:
      Error_Handler();
      break;
  }
  
  /* Resume Time Base triggers */
  if (HAL_TIM_Base_Start(&htim2) != HAL_OK)
  {
    /* Counter enable error */
    Error_Handler();
  }
  if (HAL_TIM_Base_Start(&htim6) != HAL_OK)
  {
    /* Counter enable error */
    Error_Handler();
  }
}


/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{ 
  /* Change the wave */
  ubKeyPressed = SET;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* Error if LED3 is slowly blinking (1 sec. period) */
  while(1)
  {    
    BSP_LED_Toggle(LED3); 
    HAL_Delay(1000);
  }
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
