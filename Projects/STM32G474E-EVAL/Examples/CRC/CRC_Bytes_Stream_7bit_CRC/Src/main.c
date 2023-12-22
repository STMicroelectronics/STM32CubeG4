/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    CRC/CRC_Bytes_Stream_7bit_CRC/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use the STM32G4xx CRC HAL API
  *          to compute 7-bit CRC codes from buffers of data bytes (8-bit data),
  *          based on a user-defined generating polynomial.
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
#define BUFFER_SIZE_5  5  /* CRC7_DATA8_TEST5[] is 5-byte long   */
#define BUFFER_SIZE_17 17 /* CRC7_DATA8_TEST17[] is 17-byte long */
#define BUFFER_SIZE_1  1  /* CRC7_DATA8_TEST1[] is 1-byte long   */
#define BUFFER_SIZE_2  2  /* CRC7_DATA8_TEST2[] is 2-byte long   */

/* User-defined polynomial */
#define CRC_POLYNOMIAL_7B  0x65  /* X^7 + X^6 + X^5 + X^2 + 1,
                                   used in Train Communication Network, IEC 60870-5[17] */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

/* USER CODE BEGIN PV */
/* Used for storing CRC Value */
__IO uint32_t uwCRCValue = 0;

/* Bytes buffers that will consecutively yield CRCs */
static const uint8_t CRC7_DATA8_TEST5[5]   = {0x12, 0x34, 0xBA, 0x71, 0xAD};
static const uint8_t CRC7_DATA8_TEST17[17] = {0x12, 0x34, 0xBA, 0x71, 0xAD,
                                              0x11, 0x56, 0xDC, 0x88, 0x1B,
                                              0xEE, 0x4D, 0x82, 0x93, 0xA6,
                                              0x7F, 0xC3
                                             };
static const uint8_t CRC7_DATA8_TEST1[1]   = {0x19};
static const uint8_t CRC7_DATA8_TEST2[2]   = {0xAB, 0xCD};

uint32_t * CRC7_DATA8_PTR_TEST1 = (uint32_t *)CRC7_DATA8_TEST1;
uint32_t * CRC7_DATA8_PTR_TEST2 = (uint32_t *)CRC7_DATA8_TEST2;


/* Expected CRC Values */
/* The 7 LSB bits are the 7-bit long CRC */
uint32_t uwExpectedCRCValue_1 = 0x00000057;    /* First byte stream CRC  */
uint32_t uwExpectedCRCValue_2 = 0x0000006E;    /* Second byte stream CRC */
uint32_t uwExpectedCRCValue_3 = 0x0000004B;    /* Third byte stream CRC  */
uint32_t uwExpectedCRCValue_4 = 0x00000026;    /* Fourth byte stream CRC */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_CRC_Init(void);
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */
  /* Configure LED1 and LED3 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED3);

  /****************************************************************************/
  /*                                                                          */
  /*         CRC computation of a first bytes stream                          */
  /*                                                                          */
  /****************************************************************************/

  /* The 7-bit long CRC of a 5-byte buffer is computed. After peripheral initialization,
     the CRC calculator is initialized with the default value that is 0x7F for
     a 7-bit CRC.

    The computed CRC is stored in uint32_t uwCRCValue. The 7-bit long CRC is made of
    uwCRCValue 7 LSB bits. */

  uwCRCValue = HAL_CRC_Accumulate(&hcrc, (uint32_t *)&CRC7_DATA8_TEST5, BUFFER_SIZE_5);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != uwExpectedCRCValue_1)
  {
    /* Wrong CRC value: Turn LED3 on */
    Error_Handler();
  }


  /****************************************************************************/
  /*                                                                          */
  /*         CRC computation of a second bytes stream                         */
  /*                                                                          */
  /****************************************************************************/

  /* The 7-bit long CRC of a 17-byte buffer is computed. The CRC calculator
    is not re-initialized, instead the previously computed CRC is used
    as initial value. */

  uwCRCValue = HAL_CRC_Accumulate(&hcrc, (uint32_t *)&CRC7_DATA8_TEST17, BUFFER_SIZE_17);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != uwExpectedCRCValue_2)
  {
    /* Wrong CRC value: Turn LED3 on */
    Error_Handler();
  }


  /****************************************************************************/
  /*                                                                          */
  /*         CRC computation of a single byte                                 */
  /*                                                                          */
  /****************************************************************************/

  /* The 7-bit long CRC of a 1-byte buffer is computed. The CRC calculator
    is not re-initialized, instead the previously computed CRC is used
    as initial value. */

  uwCRCValue = HAL_CRC_Accumulate(&hcrc, (uint32_t *)CRC7_DATA8_PTR_TEST1, BUFFER_SIZE_1);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != uwExpectedCRCValue_3)
  {
    /* Wrong CRC value: Turn LED3 on */
    Error_Handler();
  }


  /****************************************************************************/
  /*                                                                          */
  /*         CRC computation of the last bytes stream                         */
  /*                                                                          */
  /****************************************************************************/

  /* The 7-bit long CRC of a 2-byte buffer is computed. The CRC calculator
    is re-initialized with the default value that is 0x7F for a 7-bit CRC.
    This is done with a call to HAL_CRC_Calculate() instead of
    HAL_CRC_Accumulate(). */

  uwCRCValue = HAL_CRC_Calculate(&hcrc, (uint32_t *)CRC7_DATA8_PTR_TEST2, BUFFER_SIZE_2);

  /* Compare the CRC value to the expected one */
  if (uwCRCValue != uwExpectedCRCValue_4)
  {
    /* Wrong CRC value: Turn LED3 on */
    Error_Handler();
  }
  else
  {
    /* Right CRC value: Turn LED1 on */
    BSP_LED_On(LED1);
  }
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
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.GeneratingPolynomial = 101;
  hcrc.Init.CRCLength = CRC_POLYLENGTH_7B;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/* USER CODE BEGIN 4 */
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
  while (1)
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
