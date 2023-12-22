/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    CORDIC/CORDIC_Sin_DMA/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use the STM32G4xx CORDIC HAL API
  *          to compute Sine on arrays of data (Q1.31 format) in DMA mode.
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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CORDIC_HandleTypeDef hcordic;
DMA_HandleTypeDef hdma_cordic_write;
DMA_HandleTypeDef hdma_cordic_read;

/* USER CODE BEGIN PV */
/* CORDIC configuration structure */
CORDIC_ConfigTypeDef sCordicConfig;

/* Array of angles in Q1.31 format, regularly incremented from 0 to 2*pi */
static int32_t aAngles[ARRAY_SIZE] =
{
  0x00000000, 0x04000000, 0x08000000, 0x0C000000,
  0x10000000, 0x14000000, 0x18000000, 0x1C000000,
  0x20000000, 0x24000000, 0x28000000, 0x2C000000,
  0x30000000, 0x34000000, 0x38000000, 0x3C000000,
  0x40000000, 0x44000000, 0x48000000, 0x4C000000,
  0x50000000, 0x54000000, 0x58000000, 0x5C000000,
  0x60000000, 0x64000000, 0x68000000, 0x6C000000,
  0x70000000, 0x74000000, 0x78000000, 0x7C000000,
  0x80000000, 0x84000000, 0x88000000, 0x8C000000,
  0x90000000, 0x94000000, 0x98000000, 0x9C000000,
  0xA0000000, 0xA4000000, 0xA8000000, 0xAC000000,
  0xB0000000, 0xB4000000, 0xB8000000, 0xBC000000,
  0xC0000000, 0xC4000000, 0xC8000000, 0xCC000000,
  0xD0000000, 0xD4000000, 0xD8000000, 0xDC000000,
  0xE0000000, 0xE4000000, 0xE8000000, 0xEC000000,
  0xF0000000, 0xF4000000, 0xF8000000, 0xFC000000
};

/* Array of reference sines in Q1.31 format */
static int32_t aRefSin[ARRAY_SIZE] =
{
  0x00000000, 0x0C8BD35E, 0x18F8B83C, 0x25280C5D,
  0x30FBC54D, 0x3C56BA70, 0x471CECE6, 0x5133CC94,
  0x5A827999, 0x62F201AC, 0x6A6D98A4, 0x70E2CBC6,
  0x7641AF3C, 0x7A7D055B, 0x7D8A5F3F, 0x7F62368F,
  0x80000000, 0x7F62368F, 0x7D8A5F3F, 0x7A7D055B,
  0x7641AF3C, 0x70E2CBC6, 0x6A6D98A4, 0x62F201AC,
  0x5A827999, 0x5133CC94, 0x471CECE6, 0x3C56BA70,
  0x30FBC54D, 0x25280C5D, 0x18F8B83C, 0x0C8BD35E,
  0x00000000, 0xF3742CA2, 0xE70747C4, 0xDAD7F3A3,
  0xCF043AB3, 0xC3A94590, 0xB8E3131A, 0xAECC336C,
  0xA57D8667, 0x9D0DFE54, 0x9592675C, 0x8F1D343A,
  0x89BE50C4, 0x8582FAA5, 0x8275A0C1, 0x809DC971,
  0x80000000, 0x809DC971, 0x8275A0C1, 0x8582FAA5,
  0x89BE50C4, 0x8F1D343A, 0x9592675C, 0x9D0DFE54,
  0xA57D8667, 0xAECC336C, 0xB8E3131A, 0xC3A94590,
  0xCF043AB3, 0xDAD7F3A3, 0xE70747C4, 0xF3742CA2
};

/* Array of calculated sines in Q1.31 format */
static int32_t aCalculatedSin[ARRAY_SIZE];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_DMA_Init(void);
static void MX_CORDIC_Init(void);
/* USER CODE BEGIN PFP */
uint32_t Check_Residual_Error(int32_t VarA, int32_t VarB, uint32_t MaxError);

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
  /* Configure LED1 and LED3 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED3);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_CORDIC_Init();
  /* USER CODE BEGIN 2 */
  /*## Configure the CORDIC peripheral ####################################*/
  sCordicConfig.Function         = CORDIC_FUNCTION_SINE;     /* sine function */
  sCordicConfig.Precision        = CORDIC_PRECISION_6CYCLES; /* max precision for q1.31 sine */
  sCordicConfig.Scale            = CORDIC_SCALE_0;           /* no scale */
  sCordicConfig.NbWrite          = CORDIC_NBWRITE_1;         /* One input data: angle. Second input data (modulus) is 1 after cordic reset */
  sCordicConfig.NbRead           = CORDIC_NBREAD_1;          /* One output data: sine */
  sCordicConfig.InSize           = CORDIC_INSIZE_32BITS;     /* q1.31 format for input data */
  sCordicConfig.OutSize          = CORDIC_OUTSIZE_32BITS;    /* q1.31 format for output data */

  if (HAL_CORDIC_Configure(&hcordic, &sCordicConfig) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }

  /*## Start calculation of sines in DMA mode #############################*/
  if (HAL_CORDIC_Calculate_DMA(&hcordic, aAngles, aCalculatedSin,
                               ARRAY_SIZE, CORDIC_DMA_DIR_IN_OUT) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*  Before starting a new process, you need to check the current state of the peripheral;
      if it’s busy you need to wait for the end of current transfer before starting a new one.
      For simplicity reasons, this example is just waiting till the end of the
      process, but application may perform other tasks while transfer operation
      is ongoing. */
  while (HAL_CORDIC_GetState(&hcordic) != HAL_CORDIC_STATE_READY)
  {
  }

  /*## Compare CORDIC results to the reference values #####################*/
  for (uint32_t i = 0; i < ARRAY_SIZE; i++)
  {
    if (Check_Residual_Error(aCalculatedSin[i], aRefSin[i], DELTA) == FAIL)
    {
      Error_Handler();
    }
  }

  /* Correct CORDIC output values: Turn LED1 on */
  BSP_LED_On(LED1);

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
  * @brief CORDIC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CORDIC_Init(void)
{

  /* USER CODE BEGIN CORDIC_Init 0 */

  /* USER CODE END CORDIC_Init 0 */

  /* USER CODE BEGIN CORDIC_Init 1 */

  /* USER CODE END CORDIC_Init 1 */
  hcordic.Instance = CORDIC;
  if (HAL_CORDIC_Init(&hcordic) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CORDIC_Init 2 */

  /* USER CODE END CORDIC_Init 2 */

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

}

/* USER CODE BEGIN 4 */
/**
  * @brief  Check delta between two values is below threshold
  * @param  VarA First input variable
  * @param  VarB Second input variable
  * @param  MaxError Maximum delta allowed between VarA and VarB
  * @retval Status
  *           PASS: Delta is below threshold
  *           FAIL: Delta is above threshold
  */
uint32_t Check_Residual_Error(int32_t VarA, int32_t VarB, uint32_t MaxError)
{
  uint32_t status = PASS;

  if ((VarA - VarB) >= 0)
  {
    if ((VarA - VarB) > MaxError)
    {
      status = FAIL;
    }
  }
  else
  {
    if ((VarB - VarA) > MaxError)
    {
      status = FAIL;
    }
  }

  return status;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  while (1)
  {
    /* LED3 is blinking */
    BSP_LED_Toggle(LED3);
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

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
