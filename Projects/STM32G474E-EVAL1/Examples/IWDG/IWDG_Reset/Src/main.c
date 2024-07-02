/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    IWDG/IWDG_Reset/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use the IWDG HAL API
  *          to update at regular period the IWDG counter and how to simulate
  *          a software fault generating an MCU IWDG reset on expiry of a
  *          programmed time period.
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
#define IWDG_WINDOW IWDG_WINDOW_DISABLE
#define IWDG_RELOAD (uwLsiFreq / 32)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
IWDG_HandleTypeDef hiwdg;

/* USER CODE BEGIN PV */
uint32_t IwdgStatus = 0;
TIM_HandleTypeDef    htim16;

uint16_t tmpCC4[2];
uint32_t uwLsiFreq;
__IO uint32_t uwCaptureNumber = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_IWDG_Init(void);
/* USER CODE BEGIN PFP */
static void GetLSIFrequency(void);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

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
  /* Configure LED1, LED2, LED3, LED4  */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);

  /*##-1- Check if the system has resumed from IWDG reset ####################*/
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != 0x00u)
  {
    /* IWDGRST flag set: Turn LED1 on and set IwdgStatus */
    IwdgStatus = 1;
    BSP_LED_On(LED1);

    /* Insert 4s delay */
    HAL_Delay(4000);

    /* Notification done: Turn LED1 off */
    BSP_LED_Off(LED1);
  }

  /* Clear reset flags in any cases */
  __HAL_RCC_CLEAR_RESET_FLAGS();
  IwdgStatus = 0;

  /*##-2- Get the LSI frequency: TIM16 is used to measure the LSI frequency ###*/
  GetLSIFrequency();

  /*##-3- Configure & Start the IWDG peripheral #########################################*/

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
   /* Configure User push-button */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* Toggle LED3 */
    BSP_LED_Toggle(LED3);

    /* Insert 990 ms delay */
    HAL_Delay(990);

    /* Refresh IWDG: reload counter */
    if(HAL_IWDG_Refresh(&hiwdg) != HAL_OK)
    {
      /* Refresh Error */
      Error_Handler();
    }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
  hiwdg.Init.Window = IWDG_WINDOW;
  hiwdg.Init.Reload = IWDG_RELOAD;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief  Configures IWDG to measure the LSI oscillator frequency.
  * @param  None
  * @retval LSI Frequency
  */
static void GetLSIFrequency(void)
{
  TIM_IC_InitTypeDef    TIMInput_Config;


  /* Configure the TIM peripheral *********************************************/
  /* Set TIMx instance */
  htim16.Instance = TIM16;

  /* TIM16 configuration: Input Capture mode ---------------------
     The LSI oscillator is connected to TIM16 CH1.
     The Rising edge is used as active edge.
     The TIM16 CCR1 is used to compute the frequency value.
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

  /* Connect internally the IWDG_CH1 Input Capture to the LSI clock output */
  HAL_TIMEx_TISelection(&htim16, TIM_TIM16_TI1_LSI, TIM_CHANNEL_1 );

  /* Connect internally the MCO to LSI */
  HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_LSI, RCC_MCODIV_1);

  /* Configure the Input Capture of channel 1 */
  TIMInput_Config.ICPolarity  = TIM_ICPOLARITY_RISING;
  TIMInput_Config.ICSelection = TIM_ICSELECTION_DIRECTTI;
  TIMInput_Config.ICPrescaler = TIM_ICPSC_DIV8;
  TIMInput_Config.ICFilter    = 0;
  if(HAL_TIM_IC_ConfigChannel(&htim16, &TIMInput_Config,TIM_CHANNEL_1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Start the TIM Input Capture measurement in interrupt mode */
  if(HAL_TIM_IC_Start_IT(&htim16, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Wait until the IWDG get 2 LSI edges */
  while(uwCaptureNumber != 2)
  {
  }

  /* Disable IWDG CC1 Interrupt Request */
  HAL_TIM_IC_Stop_IT(&htim16, TIM_CHANNEL_1);

  /* Deinitialize the IWDG peripheral registers to their default reset values */
  HAL_TIM_IC_DeInit(&htim16);
}

/**
  * @brief  Input Capture callback in non blocking mode
  * @param  htim : TIM IC handle
  * @retval None
*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  uint32_t lsiperiod = 0;

  /* Get the Input Capture value */
  tmpCC4[uwCaptureNumber++] = HAL_TIM_ReadCapturedValue(&htim16, TIM_CHANNEL_1);

  if (uwCaptureNumber >= 2)
  {
    /* Compute the period length */
    lsiperiod = (uint16_t)(0xFFFF - tmpCC4[0] + tmpCC4[1] + 1);

    /* Frequency computation */
    uwLsiFreq = (uint32_t) SystemCoreClock / lsiperiod;
    uwLsiFreq *= 8;
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
  /* Turn LED3 on */
  BSP_LED_On(LED3);

  IwdgStatus = 1;
  /* Infinite loop */
  while(1)
  {
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
