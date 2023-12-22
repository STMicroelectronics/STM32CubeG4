/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "stm32g474e_eval.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac1;
DMA_HandleTypeDef hdma_dac1_ch2;

OPAMP_HandleTypeDef hopamp1;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
__IO uint32_t UserButtonStatus = 0;  /* set to 1 after User push-button interrupt  */
__IO uint32_t UserButtonPressState = 0; /* Incremented at each press, representing application execution steps */

uint32_t factorytrimmingvaluep = 0, factorytrimmingvaluen = 0;
uint32_t oldtrimmingvaluep = 0, oldtrimmingvaluen = 0; 
uint32_t newtrimmingvaluep = 0, newtrimmingvaluen = 0; 

uint32_t ledblinkingduration = 0; 

const uint16_t Sine12bit[32] = { 2000, 2390, 2765, 3111, 3414, 3662, 3847, 3961,
                                 4000, 3961, 3847, 3662, 3414, 3111, 2765, 2390,
                                 2000, 1609, 1234,  888,  585,  337,  152,   38,
                                 0,    38,   152,   337,  585,  888, 1234, 1609};
                                 
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_DAC1_Init(void);
static void MX_OPAMP1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void OPAMP_Calibrate_Before_Run (void);
static void OPAMP_Calibrate_After_Run(void);
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
  MX_OPAMP1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  /* Initialize LED on board */
  BSP_LED_Init(LED1);
  /* Configure User push-button in Interrupt mode */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /*##- Start TIM2 peripheral counter ######################################*/
  if (HAL_TIM_Base_Start(&htim2) != HAL_OK)
  {
    /* TIM start error */
    Error_Handler();
  }
  
  /*##- Start DAC1 DAC_CHANNEL_2 and associated DMA1_Channel1 ##############################*/
  if (HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_2, (uint32_t*)Sine12bit, 
                       32, DAC_ALIGN_12B_R) != HAL_OK)
  {
    /* Start DMA Error */
    Error_Handler();
  }
  
  /*##-1- Calibrate OPAMP1    #################################################*/
  
  /* Press user button to launch OPAMP1 calibration */   
  while(UserButtonStatus != 1);
  HAL_Delay(200); 
  UserButtonStatus = 0;
  UserButtonPressState++;
  
  OPAMP_Calibrate_Before_Run();
 
  /* Note: LED1 will blink:                                                 */
  /*       - quickly: Calibrated trimming are different from Factory Trimming */
  /*       - slowly : Calibrated trimming are     same as    Factory Trimming */
  
  /*##-2- Start OPAMP1    #####################################################*/

  /* Press user button to launch OPAMP1 */ 
  while(UserButtonStatus != 1)
  {
    BSP_LED_Toggle(LED1);
    HAL_Delay(ledblinkingduration); 
  }
  BSP_LED_Off(LED1);
  HAL_Delay(200);
  UserButtonStatus = 0;
  UserButtonPressState++;

  /* Enable OPAMP1 */
  HAL_OPAMP_Start(&hopamp1);
  
  /*##-3- Modify OPAMP1 setting while ON ################################*/
  
  /* Press user button to change OPAMP1 setting on the fly (dummy change,      */
  /* without application purpose in this example: change non-inverting input  */
  /* source).                                                                 */
  while(UserButtonStatus != 1);
  HAL_Delay(200);
  UserButtonStatus = 0;
  UserButtonPressState++;
  
  /* Change the OPAMP1 input selection */
  hopamp1.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO0;
  /* Update OPAMP1 config */
  HAL_OPAMP_Init(&hopamp1); 
  
  /*##-4- Stop OPAMP1 #########################################################*/
  
  /* Press user button to stop OPAMP1 */   
  while(UserButtonStatus != 1);
  HAL_Delay(200);
  UserButtonStatus = 0;
  UserButtonPressState++;

  /* Disable OPAMP1 */
  HAL_OPAMP_Stop(&hopamp1);
  
  /*##-5- Calibrate OPAMP1    #################################################*/
  
  /* Press user button to launch calibration */ 
  while(UserButtonStatus != 1);
  HAL_Delay(200);
  UserButtonStatus = 0;
  UserButtonPressState++;

  OPAMP_Calibrate_After_Run();

  /* Note: LED1 will blink:                                                               */
  /*       - quickly: New calibrated trimming are different from ones calibrated before run */
  /*       - slowly : New calibrated trimming are    same as     ones calibrated before run */
  
  while(UserButtonStatus != 1)
  {
    BSP_LED_Toggle (LED1);
    HAL_Delay(ledblinkingduration); 
  }
  BSP_LED_Off(LED1);
  UserButtonPressState++;
  
  /*##-6- End of tests       #################################################*/
  
  while (1)
  {
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

  /** DAC channel OUT2 config
  */
  sConfig.DAC_HighFrequency = DAC_HIGH_FREQUENCY_INTERFACE_MODE_AUTOMATIC;
  sConfig.DAC_DMADoubleDataMode = DISABLE;
  sConfig.DAC_SignedFormat = DISABLE;
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_T2_TRGO;
  sConfig.DAC_Trigger2 = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_BOTH;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC1_Init 2 */

  /* USER CODE END DAC1_Init 2 */

}

/**
  * @brief OPAMP1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_OPAMP1_Init(void)
{

  /* USER CODE BEGIN OPAMP1_Init 0 */

  /* USER CODE END OPAMP1_Init 0 */

  /* USER CODE BEGIN OPAMP1_Init 1 */

  /* USER CODE END OPAMP1_Init 1 */
  hopamp1.Instance = OPAMP1;
  hopamp1.Init.PowerMode = OPAMP_POWERMODE_NORMAL;
  hopamp1.Init.Mode = OPAMP_FOLLOWER_MODE;
  hopamp1.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO1;
  hopamp1.Init.InternalOutput = DISABLE;
  hopamp1.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
  hopamp1.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
  if (HAL_OPAMP_Init(&hopamp1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OPAMP1_Init 2 */

  /* USER CODE END OPAMP1_Init 2 */

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
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xFFFF;
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

/**             
  * @brief  OPAMP Calibration before OPAMP runs
  * @note   
  * @note   
  * @param  None
  * @retval None
  */
void OPAMP_Calibrate_Before_Run(void)
{

  /* Retrieve Factory Trimming */
   factorytrimmingvaluep = HAL_OPAMP_GetTrimOffset(&hopamp1, OPAMP_FACTORYTRIMMING_P);
   factorytrimmingvaluen = HAL_OPAMP_GetTrimOffset(&hopamp1, OPAMP_FACTORYTRIMMING_N);
    
   /* Run OPAMP calibration */
   HAL_OPAMP_SelfCalibrate(&hopamp1);
   
   /* Store trimming value */
   oldtrimmingvaluep = hopamp1.Init.TrimmingValueP;
   oldtrimmingvaluen = hopamp1.Init.TrimmingValueN;

   /* Are just calibrated trimming same or different from Factory Trimming */
   if  ((oldtrimmingvaluep != factorytrimmingvaluep) || (oldtrimmingvaluen != factorytrimmingvaluen))
   {
     /* Calibrated trimming are different from Factory Trimming */
     /* LED blinks quickly */
     ledblinkingduration = 250;       
   }
   
   else
   {
     /* Calibrated trimming same as Factory Trimming */  
    /* LED blinks slowly */
     ledblinkingduration = 1500;
   }
  
   /* With use temperature sensor   */
   /* Calibration */
}

/**             
  * @brief  OPAMP Calibration after OPAMP runs
  * @note   
  * @note   
  * @param  None
  * @retval None
  */
void OPAMP_Calibrate_After_Run(void)
{
  /* Run OPAMP calibration */
  HAL_OPAMP_SelfCalibrate(&hopamp1);
   
  /* Store trimming value */
  newtrimmingvaluep = hopamp1.Init.TrimmingValueP;
  newtrimmingvaluen = hopamp1.Init.TrimmingValueN;

  /* Are just calibrated trimming same or different from Factory Trimming */
  if  ((oldtrimmingvaluep != newtrimmingvaluep) || (oldtrimmingvaluen != newtrimmingvaluen))
  {
    /* New calibrated trimming are different from ones calibrated before run */
    /* LED blinks quickly */
    ledblinkingduration = 250;
  }
   
  else
  {
    /* New calibrated trimming are same as ones calibrated before run */
    /* LED blinks slowly */
    ledblinkingduration = 1500;
  } 
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
 if (GPIO_Pin == USER_BUTTON_PIN)
 {
   /* Set variable to report push button event to main program */
   UserButtonStatus = SET;
 }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
  Error_Handler();
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
