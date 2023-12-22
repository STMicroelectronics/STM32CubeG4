/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FMAC/FMAC_Adaptive_FIR_AN5305/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use FMAC HAL API (FMAC instance)
  *          to perform an adaptive FIR filter in DMA mode.
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
#include <stdio.h>
#include "2ToneNoise.h"
#include "3ToneNoise.h"
#include "b_g474e_dpow1.h"
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
FMAC_HandleTypeDef hfmac;
DMA_HandleTypeDef hdma_fmac_preload;
DMA_HandleTypeDef hdma_fmac_write;
DMA_HandleTypeDef hdma_fmac_read;

/* USER CODE BEGIN PV */
/* FMAC configuration structure */
FMAC_FilterConfigTypeDef sFmacConfig;

/* Array of input values in Q1.15 format */
static int16_t aInputValues[2][ARRAY_SIZE];

static int16_t aFilterCoeffB[COEFF_VECTOR_B_SIZE];

/* Array of calculated filtered data in Q1.15 format (two parts) */
static int16_t aCalculatedFilteredData[ARRAY_SIZE];

/* Expected number of calculated samples for the used aCalculatedFilteredDataX */
uint16_t CurrentInputArraySize;

/* Expected number of calculated samples for the used aCalculatedFilteredDataX */
uint16_t ExpectedCalculatedFilteredDataSize;

/* Status of the calculation */
__IO uint32_t FilterPreloadCallbackCount   = 0;
__IO uint32_t HalfGetDataCallbackCount     = 0;
__IO uint32_t GetDataCallbackCount         = 0;
__IO uint32_t OutputDataReadyCallbackCount = 0;
__IO uint32_t ErrorCount                   = 0;
uint32_t OldValue;

/* Frame selelector */
uint8_t OldFrame = 0;
uint8_t Frame = 1;
/* Auxiliary counter */
uint32_t Index;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_DMA_Init(void);
static void MX_FMAC_Init(void);
/* USER CODE BEGIN PFP */

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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Configure LED4 and LED5 */
  BSP_LED_Init(LED4);
  BSP_LED_Init(LED5);

  /* Configure unused LED2 and LED3 */
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);
  BSP_LED_Off(LED2);
  BSP_LED_Off(LED3);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_FMAC_Init();
  /* USER CODE BEGIN 2 */
  /* Fill first frame with input signal and zero the other frame */
  /* Normally samples would come from ADC or other source, in this example the */
  /* same data is reloaded from Flash */

  for (Index=0; Index<ARRAY_SIZE; Index++)
  {
    aInputValues[0][Index] = TwoToneNoise[Index];
    aInputValues[1][Index] = 0;
  }
  /* generate filter coefficients (in this example they are simply read from flash */
  for (Index=0; Index<COEFF_VECTOR_B_SIZE; Index++)
  {
    aFilterCoeffB[Index] = TwoToneFilter[Index];
  }

  /*## Static FMAC configuration parameters #################################*/
  sFmacConfig.InputBaseAddress  = INPUT_BUFFER_BASE;
  sFmacConfig.InputBufferSize   = INPUT_BUFFER_SIZE;
  sFmacConfig.InputThreshold    = INPUT_THRESHOLD;
  sFmacConfig.CoeffBaseAddress  = COEFFICIENT_BUFFER_BASE;
  sFmacConfig.CoeffBufferSize   = COEFFICIENT_BUFFER_SIZE;
  sFmacConfig.OutputBaseAddress = OUTPUT_BUFFER_BASE;
  sFmacConfig.OutputBufferSize  = OUTPUT_BUFFER_SIZE;
  sFmacConfig.OutputThreshold   = OUTPUT_THRESHOLD;
  sFmacConfig.pCoeffA           = NULL;
  sFmacConfig.CoeffASize        = 0;
  sFmacConfig.CoeffBSize        = COEFF_VECTOR_B_SIZE;
  sFmacConfig.Filter            = FMAC_FUNC_CONVO_FIR;
  sFmacConfig.InputAccess       = FMAC_BUFFER_ACCESS_DMA;
  sFmacConfig.OutputAccess      = FMAC_BUFFER_ACCESS_DMA;
#if defined(CLIP_ENABLED)
  sFmacConfig.Clip              = FMAC_CLIP_ENABLED;
#else
  sFmacConfig.Clip              = FMAC_CLIP_DISABLED;
#endif
  sFmacConfig.P                 = COEFF_VECTOR_B_SIZE;
  sFmacConfig.Q                 = FILTER_PARAM_Q_NOT_USED;
  sFmacConfig.R                 = GAIN;

  /*### Main loop repeats each frame ########################################*/
  /* Repeat until required number of frames have been processed */
  do {
    /* Point to filter coefficients for current frame*/
    sFmacConfig.pCoeffB           = aFilterCoeffB;
    /* Configure filter and load coefficients by polling */
    if (HAL_FMAC_FilterConfig(&hfmac, &sFmacConfig) != HAL_OK)
    {
      /* Configuration Error */
      Error_Handler();
    }

    /* Preload the filter state at end of previous frame */
    if (HAL_FMAC_FilterPreload_DMA(&hfmac, &aInputValues[Frame][ARRAY_SIZE-COEFF_VECTOR_B_SIZE+1], COEFF_VECTOR_B_SIZE-1, NULL, 0) != HAL_OK)
    {
      /* Configuration Error */
      Error_Handler();
    }
    /* Switch input frames */
    OldFrame = Frame;
    Frame = (Frame ? 0 : 1);

    /* Wait for preload to finish */
    while (HAL_FMAC_GetState(&hfmac) != HAL_FMAC_STATE_READY);

    /* Start calculation of FIR filter in DMA mode */
    ExpectedCalculatedFilteredDataSize = ARRAY_SIZE;
    if (HAL_FMAC_FilterStart(&hfmac, aCalculatedFilteredData, &ExpectedCalculatedFilteredDataSize) != HAL_OK)
    {
      /* Processing Error */
      Error_Handler();
    }

    /*## Append data to start the DMA process after the preloaded data handling ##*/
    CurrentInputArraySize = ARRAY_SIZE;
    if (HAL_FMAC_AppendFilterData(&hfmac,
                                &aInputValues[Frame][0],
                                &CurrentInputArraySize) != HAL_OK)
    {
      ErrorCount++;
    }
    /* While processing is going on, fill next frame with input signal */
    /* Normally samples would come from ADC or other source, in this example the */
    /* data are loaded from Flash */
    for (Index=0; Index<ARRAY_SIZE; Index++)
    {
      aInputValues[OldFrame][Index] = ThreeToneNoise[Index];
    }
    /* Update filter coefficients for next frame */
    /* Normally this would be done by the adaptive algorithm, in this example the */
    /* coefficients are loaded from flash */
    for (Index=0; Index<COEFF_VECTOR_B_SIZE; Index++)
    {
      aFilterCoeffB[Index] = ThreeToneFilter[Index];
    }
    /* Wait for FMAC to finish processing frame */
    while (OutputDataReadyCallbackCount == OldValue);
    OldValue = OutputDataReadyCallbackCount;

    /* Stop the calculation of FIR filter in polling/DMA mode */
    if (HAL_FMAC_FilterStop(&hfmac) != HAL_OK)
    {
      /* Processing Error */
      Error_Handler();
    }
  /* end of loop */
  } while(OutputDataReadyCallbackCount < DATA_RDY_CALLBACK_COUNT);
  {
    if(ErrorCount != 0)
    {
      /* Processing Error */
      Error_Handler();
    }
  }

  /*## Check the final error status ############################################*/
  if(ErrorCount != 0)
  {
    /* Processing Error */
    Error_Handler();
  }

  /* Reached the end of processing : Turn LED4 on */
  BSP_LED_On(LED4);

#ifdef PRINT_OUTPUT
  printf("Filtered output for frame %d\n",Frame);
  for (Index=0; Index<ARRAY_SIZE; Index++)
    printf("%d\n",aCalculatedFilteredData[Index]);
#endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
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
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
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
  * @brief FMAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_FMAC_Init(void)
{

  /* USER CODE BEGIN FMAC_Init 0 */

  /* USER CODE END FMAC_Init 0 */

  /* USER CODE BEGIN FMAC_Init 1 */

  /* USER CODE END FMAC_Init 1 */
  hfmac.Instance = FMAC;
  if (HAL_FMAC_Init(&hfmac) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FMAC_Init 2 */

  /* USER CODE END FMAC_Init 2 */

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
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}

/* USER CODE BEGIN 4 */
/**
  * @brief FMAC filter preload callback
  * @par hfmac: FMAC HAL handle
  * @retval None
  */
void HAL_FMAC_FilterPreloadCallback(FMAC_HandleTypeDef *hfmac)
{
  FilterPreloadCallbackCount++;;
}

/**
  * @brief FMAC half get data callback
  * @par hfmac: FMAC HAL handle
  * @retval None
  */
void HAL_FMAC_HalfGetDataCallback(FMAC_HandleTypeDef *hfmac)
{
  HalfGetDataCallbackCount++;
}

/**
  * @brief FMAC get data callback
  * @par hfmac: FMAC HAL handle
  * @retval None
  */
void HAL_FMAC_GetDataCallback(FMAC_HandleTypeDef *hfmac)
{
  GetDataCallbackCount++;
}

/**
  * @brief FMAC output data ready callback
  * @par hfmac: FMAC HAL handle
  * @retval None
  */
void HAL_FMAC_OutputDataReadyCallback(FMAC_HandleTypeDef *hfmac)
{
  OutputDataReadyCallbackCount++;
}

/**
  * @brief FMAC error callback
  * @par hfmac: FMAC HAL handle
  * @retval None
  */
void HAL_FMAC_ErrorCallback(FMAC_HandleTypeDef *hfmac)
{
  ErrorCount++;
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
  while(1)
  {
    /* LED5 is blinking */
    BSP_LED_Toggle(LED5);
    HAL_Delay(500);
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
