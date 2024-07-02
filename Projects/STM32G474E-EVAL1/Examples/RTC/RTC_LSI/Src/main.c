/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    RTC/RTC_LSI/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32G4xx RTC HAL API to configure
  *          Time and Date.
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

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define WAKEUP_TIMER_ENABLE 0x32F2

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim16;

/* USER CODE BEGIN PV */
/* Buffer used for displaying Time */
__IO uint32_t uwLsiFreq = 0;
__IO uint32_t RTCStatus = 0;

uint16_t tmpCCTIM_CHANNEL[2] = {0, 0};
__IO uint32_t uwCaptureNumber = 0;
__IO uint32_t uwPeriodValue = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM16_Init(void);
/* USER CODE BEGIN PFP */
static uint32_t GetLSIFrequency(void);
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
  /* STM32G4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* Configure LED1 */
  BSP_LED_Init(LED1);

  /* Configure Wkup/Tamper push-button button */
  BSP_PB_Init(BUTTON_USER,BUTTON_MODE_GPIO);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */
  /*##-2- Check if data stored in BackUp register1: Wakeup timer enable #######*/
  /* Read the Back Up Register 1 Data */
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) == WAKEUP_TIMER_ENABLE)
  {
    /* if the wakeup timer is enabled then deactivate it to disable the wakeup timer interrupt */
    if (HAL_RTCEx_DeactivateWakeUpTimer(&hrtc) != HAL_OK)
    {
      /* Initialization Error */
      Error_Handler();
    }
  }

  /*##-3- Configure the RTC Wakeup peripheral #################################*/
  /* Setting the Wakeup time to 1 s
       If RTC_WAKEUPCLOCK_CK_SPRE_16BITS is selected, the frequency is 1Hz,
       this allows to get a wakeup time equal to 1 s if the counter is 0x0 */
  HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0x0, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);

  /*##-4- Write 'wakeup timer enabled' tag in RTC Backup data Register 1 #######*/
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, WAKEUP_TIMER_ENABLE);

  /* Wait Until push-button pressed */
  while(BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_SET)
  {
  }

  /* Wait Until push-button released */
  while(BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_RESET)
  {
  }
  
  /* Get the LSI frequency:  TIM16 is used to measure the LSI frequency */
  uwLsiFreq = GetLSIFrequency();
  
  /* Update the Calendar Configuration with the LSI exact value */
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 0x7F;
  hrtc.Init.SynchPrediv = (uwLsiFreq/128) - 1;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  
  if(HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  RTCStatus = 1;
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
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
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  hrtc.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the WakeUp
  */
  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 0;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 0xFFFF;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim16, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

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
  * @brief  Input Capture callback in non blocking mode 
  * @param  htim : TIM IC handle
  * @retval None
*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  /* Get the Input Capture value */
  tmpCCTIM_CHANNEL[uwCaptureNumber++] = HAL_TIM_ReadCapturedValue(&htim16, TIM_CHANNEL_1);
  
  if (uwCaptureNumber >= 2)
  {
    if ( tmpCCTIM_CHANNEL[0] > tmpCCTIM_CHANNEL[1] )
    {
      /* Compute the period length */
      uwPeriodValue = (uint16_t)(0xFFFF - tmpCCTIM_CHANNEL[0] + tmpCCTIM_CHANNEL[1] + 1);
    }
    else
    {
      /* Compute the period length */
      uwPeriodValue = (uint16_t)(tmpCCTIM_CHANNEL[1] - tmpCCTIM_CHANNEL[0]);
    }
    /* Frequency computation */ 
    uwLsiFreq = (uint32_t) SystemCoreClock / uwPeriodValue;
    uwLsiFreq *= 8;
  }
}

/**
  * @brief  WakeUp timer callback
  * @param  hrtc : RTC handle
  * @retval None
  */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{    
  /* Toggle LED1 */
  BSP_LED_Toggle(LED1);
}

/**
  * @brief  Configures TIM16 to measure the LSI oscillator frequency. 
  * @param  None
  * @retval LSI Frequency
  */
static uint32_t GetLSIFrequency(void)
{
  TIM_IC_InitTypeDef    TIMInput_Config;

  /* Configure the TIM peripheral *********************************************/ 
  /* Set TIMx instance */  
  htim16.Instance = TIM16;
  
  /* TIM16 configuration: Input Capture mode ---------------------
     The LSI oscillator is connected to TIM16 TIM_CHANNEL_1.
     The Rising edge is used as active edge.
     The TIM16 Capture/Compare register associated to TIM_CHANNEL_1 
     is used to compute the frequency value. 
  ------------------------------------------------------------ */
  htim16.Init.Prescaler         = 0; 
  htim16.Init.CounterMode       = TIM_COUNTERMODE_UP;  
  htim16.Init.Period            = 0xFFFF; 
  htim16.Init.ClockDivision     = 0;     
  if(HAL_TIM_IC_Init(&htim16) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  HAL_TIMEx_TISelection(&htim16, TIM_TIM16_TI1_LSI, TIM_CHANNEL_1);
  
  /* Configure the Input Capture of TIM_CHANNEL_1 */
  TIMInput_Config.ICPolarity  = TIM_ICPOLARITY_RISING;
  TIMInput_Config.ICSelection = TIM_ICSELECTION_DIRECTTI;
  TIMInput_Config.ICPrescaler = TIM_ICPSC_DIV8;
  TIMInput_Config.ICFilter    = 0;
  if(HAL_TIM_IC_ConfigChannel(&htim16, &TIMInput_Config, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Start the TIM Input Capture measurement in interrupt mode */
  if(HAL_TIM_IC_Start_IT(&htim16, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Wait until the TIM16 gets 2 LSI edges */
  while(uwCaptureNumber != 2)
  {
  }

  /* Disable TIM16 Capture/Compare channel Interrupt Request */
  HAL_TIM_IC_Stop_IT(&htim16, TIM_CHANNEL_1);
  
  /* Deinitialize the TIM16 peripheral registers to their default reset values */
  HAL_TIM_IC_DeInit(&htim16);

  return uwLsiFreq;
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
  RTCStatus = 0xE;
  while (1)
  {
    /* Toggle LED3 with a period of one second */
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
