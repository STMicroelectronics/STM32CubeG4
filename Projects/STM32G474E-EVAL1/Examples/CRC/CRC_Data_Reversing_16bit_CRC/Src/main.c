/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    CRC/CRC_Data_Reversing_16bit_CRC/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use the STM32G4xx CRC HAL API
  *          to compute a 16-bit CRC code from a data bytes (8-bit data) buffer,
  *          based on a user-defined generating polynomial and initialization
  *          value, with input and output data reversing features enabled.
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
#define BUFFER_SIZE  9  /* Input buffer CRC16_DATA8[] is 9-byte long */

/* User-defined polynomial */
#define CRC_POLYNOMIAL_16B  0x1021  /* X^16 + X^12 + X^5 + 1, CRC-CCITT generating polynomial */

/* User-defined CRC init value */
/* As the CRC is 16-bit long, the init value is 16-bit long as well */
#define CRC_INIT_VALUE      0x5ABE
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

/* USER CODE BEGIN PV */

/* Used for storing CRC Value */
__IO uint32_t uwCRCValue = 0;

/* Input data buffer */
static const uint8_t CRC16_DATA8[BUFFER_SIZE] = {0x4D, 0x3C, 0x2B, 0x1A,
                                                 0xBE, 0x71, 0xC9, 0x8A,
                                                 0x5E};

/* Check input data buffer, used to verify data reversal options */
static const uint8_t CRC16_DATA8_CHECK[BUFFER_SIZE] = {0x58, 0xD4, 0x3C, 0xB2,
                                                       0x51, 0x93, 0x8E, 0x7D,
                                                       0x7A};

/* Expected CRC value yielded by CRC16_DATA8[] input buffer
   with output data reversal feature disabled (default)
   The 16 LSB bits are the 16-bit long CRC. */
uint32_t uwExpectedCRCValue = 0x0000C20A;
/* Expected CRC value yielded by CRC16_DATA8[] input buffer
   with output data reversal feature enabled.
   The 16 LSB bits are the 16-bit long CRC. */
uint32_t uwExpectedCRCValue_reversed;

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

  /* The bit reversal is done on a full word basis.

     The input stream yielded by CRC16_DATA8[] is the sequence of
     bytes 0x4D, 0x3C, 0x2B, 0x1A, 0xBE, 0x71, ...
     meaning that the first word written in the CRC DR register is
      0x4D3C2B1A.

     Bit reversal done by the hardware on the full word leads to the actual
     CRC processing of
      0x58D43CB2.

     Similarly, the second word written in the peripheral is 0xBE71C98A, leading to
     the processing of 0x51938E7D after hardware input data reversal.

     Note that when the software writes less than a word in the peripheral (to minimize the
     number of write accesses for a given number of bytes), the bit-reversal operation
     is carried out only on the inputted data.
     Therefore, the last written data is the last byte 0x5E, processed as 0x7A
     by the hardware after bit-reversal to wrap-up the CRC computation.

     This means that the field InputDataInversionMode set to CRC_INPUTDATA_INVERSION_WORD
     applied to {0x4D, 0x3C, 0x2B, 0x1A, 0xBE, 0x71, 0xC9, 0x8A, 0x5E}
     yields the same result as InputDataInversionMode set to CRC_INPUTDATA_INVERSION_NONE
     applied to {0x58, 0xD4, 0x3C, 0xB2, 0x51, 0x93, 0x8E, 0x7D, 0x7A}.

  */

  /****************************************************************************/
  /*                                                                          */
  /*                      CRC computation                                     */
  /*                                                                          */
  /****************************************************************************/

  /* The 16-bit long CRC of the 9-byte buffer is computed. After peripheral initialization,
     the CRC calculator is initialized with the user-defined value that is 0x5ABE.

    The computed CRC is stored in uint32_t uwCRCValue. The 16-bit long CRC is made of
    uwCRCValue 16 LSB bits. */

  uwCRCValue = HAL_CRC_Accumulate(&hcrc, (uint32_t *)&CRC16_DATA8, BUFFER_SIZE);

  /* Compare the CRC value to the expected one (reversed) */
  uwExpectedCRCValue_reversed = (__RBIT(uwExpectedCRCValue) >> 16);
  if (uwCRCValue != uwExpectedCRCValue_reversed)
  {
    /* Wrong CRC value: Turn LED3 on */
    Error_Handler();
  }



  /****************************************************************************/
  /*                                                                          */
  /*         CRC computation with reversal options disabled                   */
  /*                                                                          */
  /****************************************************************************/

  /* Next, the input and output data inversion features are disabled and
     it is verified that the input data CRC16_DATA8_CHECK[] which is defined with
     a different endianness scheme yields the same CRC.

     As explained above, CRC16_DATA8_CHECK is obtained from CRC16_DATA8
     by a bit-reversal operation carried out on full word and vice versa. */


  /* Reversal options are disabled */
  hcrc.Init.InputDataInversionMode  = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;

   /* Peripheral re-initialization. The CRC calculator is initialized again with
     the user-defined value that is 0x5ABE. */
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }


  /*  The newly computed CRC is stored in uint32_t uwCRCValue. */
  uwCRCValue = HAL_CRC_Accumulate(&hcrc, (uint32_t *)&CRC16_DATA8_CHECK, BUFFER_SIZE);


  /* Compare the new CRC value to the expected one, taking into account the
     output data reversal feature is disabled. */
  if (uwCRCValue != uwExpectedCRCValue)
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
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_DISABLE;
  hcrc.Init.GeneratingPolynomial = 4129;
  hcrc.Init.CRCLength = CRC_POLYLENGTH_16B;
  hcrc.Init.InitValue = 0x5ABE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_WORD;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE;
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
