/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SMARTCARD/SMARTCARD_T0_MFX/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use SMARTCARD HAL API (SMARTCARD instance)
  *          to access to a Smartcard's information.
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

#if defined(HAL_UART_MODULE_ENABLED)
#include <stdio.h>
#endif /* HAL_UART_MODULE_ENABLED */

#include "smartcard.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#if defined(HAL_UART_MODULE_ENABLED)
#define UART_TIMEOUT_VALUE   1000
#endif /* HAL_UART_MODULE_ENABLED */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
SMARTCARD_HandleTypeDef hsmartcard3;

/* USER CODE BEGIN PV */
/* Directories & Files ID */
const uint8_t MasterRoot[2] = {0x3F, 0x00};
const uint8_t GSMDir[2] = {0x7F, 0x20};
const uint8_t ICCID[2] = {0x2F, 0xE2};
const uint8_t IMSI[2] = {0x6F, 0x07};
const uint8_t CHV1[8] = {'0', '0', '0', '0', '0', '0', '0', '0'};

/* APDU Transport Structures */
SC_ADPU_Commands SC_ADPU;
SC_ADPU_Response SC_Response;
uint32_t CHV1Status = 0, i = 0;
__IO uint8_t ICCID_Content[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
__IO uint8_t IMSI_Content[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_SMARTCARD_Init(void);
/* USER CODE BEGIN PFP */
#if defined(HAL_UART_MODULE_ENABLED)
#if defined(__ICCARM__)
/* New definition from EWARM V9, compatible with EWARM8 */
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)
#elif defined ( __CC_ARM ) || defined(__ARMCC_VERSION)
/* ARM Compiler 5/6*/
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif /* __ICCARM__ */
#endif /* HAL_UART_MODULE_ENABLED */
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
  /* Initialize the IO expander */
  BSP_IOEXPANDER_Init(0, IOEXPANDER_IO_MODE);

  /* Initialize LEDs */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART3_SMARTCARD_Init();
  /* USER CODE BEGIN 2 */

  SC_State SCState = SC_POWER_OFF;

  /* Configure Smartcard Interface GPIO pins */
  SC_IOConfig();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /*-------------------------------- Idle task ---------------------------------*/
  while (1)
  {
#if defined(HAL_UART_MODULE_ENABLED)
    printf("-- Waiting for card insertion -- (LED1 blinking) \n\r");
#endif /* HAL_UART_MODULE_ENABLED */

    /* Loop while no Smartcard is detected */
    while(SC_IsCardInserted() == 0)
    {
      BSP_LED_Toggle(LED1);
      HAL_Delay(200);
    }
    BSP_LED_On(LED1);

    /* Start SC Demo ---------------------------------------------------------*/
#if defined(HAL_UART_MODULE_ENABLED)
    printf("-- Card inserted -- (All LEDs On) \n\r");
#endif /* HAL_UART_MODULE_ENABLED */

    /* Wait A2R --------------------------------------------------------------*/
    SCState = SC_POWER_ON;

    SC_ADPU.Header.CLA = 0x00;
    SC_ADPU.Header.INS = SC_GET_A2R;
    SC_ADPU.Header.P1 = 0x00;
    SC_ADPU.Header.P2 = 0x00;
    SC_ADPU.Body.LC = 0x00;

    while(SCState != SC_ACTIVE_ON_T0)
    {
      SC_Handler(&SCState, &SC_ADPU, &SC_Response);
    }

#if defined(HAL_UART_MODULE_ENABLED)
    printf("-- Card powered ON : ATR received --  \n\r");
#endif /* HAL_UART_MODULE_ENABLED */

    /* Wait a little after ATR message reception */
    HAL_Delay(100);

    /* Apply the Procedure Type Selection (PTS) */
    SC_PTSConfig();

#if defined(HAL_UART_MODULE_ENABLED)
    printf("-- PTS procedure performed --  \n\r");
#endif /* HAL_UART_MODULE_ENABLED */

    /* Inserts delay(400ms) for Smartcard clock resynchronisation */
    HAL_Delay(400);

    /* Select MF -------------------------------------------------------------*/
    SC_ADPU.Header.CLA = SC_CLA_GSM11;
    SC_ADPU.Header.INS = SC_SELECT_FILE;
    SC_ADPU.Header.P1 = 0x00;
    SC_ADPU.Header.P2 = 0x00;
    SC_ADPU.Body.LC = 0x02;

    for(i = 0; i < SC_ADPU.Body.LC; i++)
    {
      SC_ADPU.Body.Data[i] = MasterRoot[i];
    }
    while(i < LC_MAX)
    {
      SC_ADPU.Body.Data[i++] = 0;
    }
    SC_ADPU.Body.LE = 0;

    SC_Handler(&SCState, &SC_ADPU, &SC_Response);

#if defined(HAL_UART_MODULE_ENABLED)
    printf("-- ### SELECT MF ###  \n\r");
#endif /* HAL_UART_MODULE_ENABLED */

    /* Get Response on MF ----------------------------------------------------*/
    if(SC_Response.SW1 == SC_DF_SELECTED)
    {
      SC_ADPU.Header.CLA = SC_CLA_GSM11;
      SC_ADPU.Header.INS = SC_GET_RESPONSE;
      SC_ADPU.Header.P1 = 0x00;
      SC_ADPU.Header.P2 = 0x00;
      SC_ADPU.Body.LC = 0x00;
      SC_ADPU.Body.LE = SC_Response.SW2;

      SC_Handler(&SCState, &SC_ADPU, &SC_Response);
    }

#if defined(HAL_UART_MODULE_ENABLED)
    printf("-- ### GET RESPONSE ###  \n\r");
#endif /* HAL_UART_MODULE_ENABLED */

    /* Select ICCID ----------------------------------------------------------*/
    if(((SC_Response.SW1 << 8) | (SC_Response.SW2)) == SC_OP_TERMINATED)
    {
      /* Check if the CHV1 is enabled */
      if((SC_Response.Data[13] & 0x80) == 0x00)
      {
        CHV1Status = 0x01;
#if defined(HAL_UART_MODULE_ENABLED)
        printf("-- CHV1 is enabled --  \n\r");
#endif /* HAL_UART_MODULE_ENABLED */
      }
      else
      {
#if defined(HAL_UART_MODULE_ENABLED)
        printf("-- CHV1 is disabled --  \n\r");
#endif /* HAL_UART_MODULE_ENABLED */
      }

      /* Send APDU Command for ICCID selection */
      SC_ADPU.Header.CLA = SC_CLA_GSM11;
      SC_ADPU.Header.INS = SC_SELECT_FILE;
      SC_ADPU.Header.P1 = 0x00;
      SC_ADPU.Header.P2 = 0x00;
      SC_ADPU.Body.LC = 0x02;

      for(i = 0; i < SC_ADPU.Body.LC; i++)
      {
        SC_ADPU.Body.Data[i] = ICCID[i];
      }
      while(i < LC_MAX)
      {
        SC_ADPU.Body.Data[i++] = 0;
      }
      SC_ADPU.Body.LE = 0;

      SC_Handler(&SCState, &SC_ADPU, &SC_Response);

#if defined(HAL_UART_MODULE_ENABLED)
      printf("-- ### SELECT EF ICCID ###  \n\r");
#endif /* HAL_UART_MODULE_ENABLED */

    }

    /* Read Binary in ICCID --------------------------------------------------*/
    if(SC_Response.SW1 == SC_EF_SELECTED)
    {
      SC_ADPU.Header.CLA = SC_CLA_GSM11;
      SC_ADPU.Header.INS = SC_READ_BINARY;
      SC_ADPU.Header.P1 = 0x00;
      SC_ADPU.Header.P2 = 0x00;
      SC_ADPU.Body.LC = 0x00;

      SC_ADPU.Body.LE = 10;

      SC_Handler(&SCState, &SC_ADPU, &SC_Response);
    }

#if defined(HAL_UART_MODULE_ENABLED)
    printf("-- ### READ BINARY (ICCID) ###  \n\r");
#endif /* HAL_UART_MODULE_ENABLED */

    /* Select GSMDir ---------------------------------------------------------*/
    if(((SC_Response.SW1 << 8) | (SC_Response.SW2)) == SC_OP_TERMINATED)
    {
      /* Copy the ICCID File content into ICCID_Content buffer */
      for(i = 0; i < SC_ADPU.Body.LE; i++)
      {
        ICCID_Content[i] =  SC_Response.Data[i];
      }
#if defined(HAL_UART_MODULE_ENABLED)
      printf("--     ICCID Value =  ");
      for(i = 0; i < SC_ADPU.Body.LE; i++)
      {
        printf("%x  ",ICCID_Content[i]);
      }
      printf("\n\r");
#endif /* HAL_UART_MODULE_ENABLED */

      /* Send APDU Command for GSMDir selection */
      SC_ADPU.Header.CLA = SC_CLA_GSM11;
      SC_ADPU.Header.INS = SC_SELECT_FILE;
      SC_ADPU.Header.P1 = 0x00;
      SC_ADPU.Header.P2 = 0x00;
      SC_ADPU.Body.LC = 0x02;

      for(i = 0; i < SC_ADPU.Body.LC; i++)
      {
        SC_ADPU.Body.Data[i] = GSMDir[i];
      }
      while(i < LC_MAX)
      {
        SC_ADPU.Body.Data[i++] = 0;
      }
      SC_ADPU.Body.LE = 0;

      SC_Handler(&SCState, &SC_ADPU, &SC_Response);

#if defined(HAL_UART_MODULE_ENABLED)
      printf("-- ### SELECT DF GSM ###  \n\r");
#endif /* HAL_UART_MODULE_ENABLED */
    }

    /* Select IMSI -----------------------------------------------------------*/
    if(SC_Response.SW1 == SC_DF_SELECTED)
    {
      SC_ADPU.Header.CLA = SC_CLA_GSM11;
      SC_ADPU.Header.INS = SC_SELECT_FILE;
      SC_ADPU.Header.P1 = 0x00;
      SC_ADPU.Header.P2 = 0x00;
      SC_ADPU.Body.LC = 0x02;

      for(i = 0; i < SC_ADPU.Body.LC; i++)
      {
        SC_ADPU.Body.Data[i] = IMSI[i];
      }
      while(i < LC_MAX)
      {
        SC_ADPU.Body.Data[i++] = 0;
      }
      SC_ADPU.Body.LE = 0;

      SC_Handler(&SCState, &SC_ADPU, &SC_Response);

#if defined(HAL_UART_MODULE_ENABLED)
      printf("-- ### SELECT EF IMSI ###  \n\r");
#endif /* HAL_UART_MODULE_ENABLED */
    }

    /* Get Response on IMSI File ---------------------------------------------*/
    if(SC_Response.SW1 == SC_EF_SELECTED)
    {
      SC_ADPU.Header.CLA = SC_CLA_GSM11;
      SC_ADPU.Header.INS = SC_GET_RESPONSE;
      SC_ADPU.Header.P1 = 0x00;
      SC_ADPU.Header.P2 = 0x00;
      SC_ADPU.Body.LC = 0x00;
      SC_ADPU.Body.LE = SC_Response.SW2;

      SC_Handler(&SCState, &SC_ADPU, &SC_Response);
    }

    /* Read Binary in IMSI ---------------------------------------------------*/
    if(CHV1Status == 0x00)
    {
      if(((SC_Response.SW1 << 8) | (SC_Response.SW2)) == SC_OP_TERMINATED)
      {
        /* Enable CHV1 (PIN1) ------------------------------------------------*/
        /* CHV1 enabling should be done using proper value of used card CHV1 */
        /* Please fill corresponding value in CHV1[array] . Current value is set to "00000000". */
        /*
        SC_ADPU.Header.CLA = SC_CLA_GSM11;
        SC_ADPU.Header.INS = SC_ENABLE;
        SC_ADPU.Header.P1 = 0x00;
        SC_ADPU.Header.P2 = 0x01;
        SC_ADPU.Body.LC = 0x04;

        for(i = 0; i < SC_ADPU.Body.LC; i++)
        {
          SC_ADPU.Body.Data[i] = CHV1[i];
        }
        while(i < LC_MAX)
        {
          SC_ADPU.Body.Data[i++] = 0;
        }
        SC_ADPU.Body.LE = 0;

        SC_Handler(&SCState, &SC_ADPU, &SC_Response);
        */
#if defined(HAL_UART_MODULE_ENABLED)
        /* printf("-- ### CHV1 ENABLE ###  \n\r"); */
#endif /* HAL_UART_MODULE_ENABLED */
      }
    }
    else
    {
      if(((SC_Response.SW1 << 8) | (SC_Response.SW2)) == SC_OP_TERMINATED)
      {
        /* Verify CHV1 (PIN1) ------------------------------------------------*/
        /* CHV1 enabling should be done using proper value of used card CHV1 */
        /* Please fill corresponding value in CHV1[array] . Current value is set to "00000000". */
        SC_ADPU.Header.CLA = SC_CLA_GSM11;
        SC_ADPU.Header.INS = SC_VERIFY;
        SC_ADPU.Header.P1 = 0x00;
        SC_ADPU.Header.P2 = 0x01;
        SC_ADPU.Body.LC = 0x08;

        for(i = 0; i < SC_ADPU.Body.LC; i++)
        {
          SC_ADPU.Body.Data[i] = CHV1[i];
        }
        while(i < LC_MAX)
        {
          SC_ADPU.Body.Data[i++] = 0;
        }
        SC_ADPU.Body.LE = 0;

        SC_Handler(&SCState, &SC_ADPU, &SC_Response);
#if defined(HAL_UART_MODULE_ENABLED)
        printf("-- ### CHV1 VERIFY ###  \n\r");
#endif /* HAL_UART_MODULE_ENABLED */
      }
    }
    /* Read Binary in IMSI ---------------------------------------------------*/
    if(((SC_Response.SW1 << 8) | (SC_Response.SW2)) == SC_OP_TERMINATED)
    {
#if defined(HAL_UART_MODULE_ENABLED)
      printf("-- ### CHV1 Operation successful ###  \n\r");
#endif /* HAL_UART_MODULE_ENABLED */
      SC_ADPU.Header.CLA = SC_CLA_GSM11;
      SC_ADPU.Header.INS = SC_READ_BINARY;
      SC_ADPU.Header.P1 = 0x00;
      SC_ADPU.Header.P2 = 0x00;
      SC_ADPU.Body.LC = 0x00;

      SC_ADPU.Body.LE = 9;

      SC_Handler(&SCState, &SC_ADPU, &SC_Response);
    }
    else
    {
#if defined(HAL_UART_MODULE_ENABLED)
      printf("-- ### CHV1 Operation unsuccessful : SW1/SW2=0x%4x ###  \n\r", ((SC_Response.SW1 << 8) | (SC_Response.SW2)));
#endif /* HAL_UART_MODULE_ENABLED */
    }

#if defined(HAL_UART_MODULE_ENABLED)
    printf("-- ### READ BINARY (IMSI) ###  \n\r");
#endif /* HAL_UART_MODULE_ENABLED */

    if(((SC_Response.SW1 << 8) | (SC_Response.SW2)) == SC_OP_TERMINATED)
    {
      /* Copy the IMSI File content into IMSI_Content buffer */
      for(i = 0; i < SC_ADPU.Body.LE; i++)
      {
        IMSI_Content[i] =  SC_Response.Data[i];
      }
    }
#if defined(HAL_UART_MODULE_ENABLED)
    printf("--     IMSI Value =  ");
    for(i = 0; i < SC_ADPU.Body.LE; i++)
    {
      printf("%x  ",IMSI_Content[i]);
    }
    printf("\n\r");
#endif /* HAL_UART_MODULE_ENABLED */

    /* Disable the Smartcard interface */
    SCState = SC_POWER_OFF;
    SC_Handler(&SCState, &SC_ADPU, &SC_Response);

#if defined(HAL_UART_MODULE_ENABLED)
    printf("-- Card powered OFF --  \n\r");
#endif /* HAL_UART_MODULE_ENABLED */

#if defined(HAL_UART_MODULE_ENABLED)
    printf("-- Waiting for card removal -- (LED1 blinking) \n\r");
#endif /* HAL_UART_MODULE_ENABLED */

    /* Loop while a Smartcard is detected */
    while(SC_IsCardInserted() == 1)
    {
      BSP_LED_Toggle(LED1);
      HAL_Delay(200);
    }
    BSP_LED_Off(LED1);
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
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_SMARTCARD_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  hsmartcard3.Instance = USART3;
  hsmartcard3.Init.BaudRate = 10386;
  hsmartcard3.Init.WordLength = SMARTCARD_WORDLENGTH_9B;
  hsmartcard3.Init.StopBits = SMARTCARD_STOPBITS_1_5;
  hsmartcard3.Init.Parity = SMARTCARD_PARITY_EVEN;
  hsmartcard3.Init.Mode = SMARTCARD_MODE_TX_RX;
  hsmartcard3.Init.CLKPolarity = SMARTCARD_POLARITY_LOW;
  hsmartcard3.Init.CLKPhase = SMARTCARD_PHASE_1EDGE;
  hsmartcard3.Init.CLKLastBit = SMARTCARD_LASTBIT_ENABLE;
  hsmartcard3.Init.OneBitSampling = SMARTCARD_ONE_BIT_SAMPLE_DISABLE;
  hsmartcard3.Init.Prescaler = 22;
  hsmartcard3.Init.GuardTime = 0;
  hsmartcard3.Init.NACKEnable = SMARTCARD_NACK_ENABLE;
  hsmartcard3.Init.TimeOutEnable = SMARTCARD_TIMEOUT_DISABLE;
  hsmartcard3.Init.BlockLength = 0;
  hsmartcard3.Init.AutoRetryCount = 3;
  hsmartcard3.Init.ClockPrescaler = SMARTCARD_PRESCALER_DIV1;
  hsmartcard3.AdvancedInit.AdvFeatureInit = SMARTCARD_ADVFEATURE_RXOVERRUNDISABLE_INIT|SMARTCARD_ADVFEATURE_DMADISABLEONERROR_INIT;
  hsmartcard3.AdvancedInit.OverrunDisable = SMARTCARD_ADVFEATURE_OVERRUN_DISABLE;
  hsmartcard3.AdvancedInit.DMADisableonRxError = SMARTCARD_ADVFEATURE_DMA_DISABLEONRXERROR;
  if (HAL_SMARTCARD_Init(&hsmartcard3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_SMARTCARDEx_SetTxFifoThreshold(&hsmartcard3, SMARTCARD_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_SMARTCARDEx_SetRxFifoThreshold(&hsmartcard3, SMARTCARD_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_SMARTCARDEx_DisableFifoMode(&hsmartcard3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  SC_MFX_EXTI_Callback();
}

/**
  * @brief SMARTCARD error callbacks
  * @param hsc: usart handle
  * @retval None
  */
void HAL_SMARTCARD_ErrorCallback(SMARTCARD_HandleTypeDef *hsc)
{
  if(HAL_SMARTCARD_GetError(hsc) & HAL_SMARTCARD_ERROR_FE)
  {
    __HAL_SMARTCARD_FLUSH_DRREGISTER(hsc);
    /* Resend the byte that failed to be received (by the Smartcard) correctly */
    SC_ParityErrorHandler();
  }

  if(HAL_SMARTCARD_GetError(hsc) & HAL_SMARTCARD_ERROR_PE)
  {
    /* Enable SC_USART RXNE Interrupt (until receiving the corrupted byte) */
    __HAL_SMARTCARD_ENABLE_IT(hsc, SMARTCARD_IT_RXNE);
    /* Flush the SC_USART DR register */
    __HAL_SMARTCARD_FLUSH_DRREGISTER(hsc);
  }
  if(HAL_SMARTCARD_GetError(hsc) & HAL_SMARTCARD_ERROR_NE)
  {
    __HAL_SMARTCARD_FLUSH_DRREGISTER(hsc);
  }

  if(HAL_SMARTCARD_GetError(hsc) & HAL_SMARTCARD_ERROR_ORE)
  {
    __HAL_SMARTCARD_FLUSH_DRREGISTER(hsc);
  }
}

/**
  * @brief Rx Transfer completed callbacks
  * @param hsc: usart handle
  * @retval None
  */
void HAL_SMARTCARD_RxCpltCallback(SMARTCARD_HandleTypeDef *hsc)
{
  /* Disable SC_USART RXNE Interrupt */
  __HAL_SMARTCARD_DISABLE_IT(hsc, SMARTCARD_IT_RXNE);
  hsc->Instance->RDR;
}

#if defined(HAL_UART_MODULE_ENABLED)
/**
  * @brief  Retargets the C library __write function to the IAR function iar_fputc.
  * @param  file: file descriptor.
  * @param  ptr: pointer to the buffer where the data is stored.
  * @param  len: length of the data to write in bytes.
  * @retval length of the written data in bytes.
  */
#if defined(__ICCARM__)
size_t __write(int file, unsigned char const *ptr, size_t len)
{
  size_t idx;
  unsigned char const *pdata = ptr;

  for (idx = 0; idx < len; idx++)
  {
    iar_fputc((int)*pdata);
    pdata++;
  }
  return len;
}
#endif /* __ICCARM__ */

/**
  * @brief  Retargets the C library printf function to the huart1.
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, UART_TIMEOUT_VALUE);

  return ch;
}

/**
  * @brief  Retargets the C library scanf function to the huart1.
  * @param  f: pointer to file (not used)
  * @retval The character received
  */
int fgetc(FILE * f)
{
  uint8_t ch = 0;
  /* We received the charracter on the handler of the huart1 */
  /* The handler must be initialise before */
  HAL_UART_Receive(&huart1, (uint8_t *)&ch, 1, UART_TIMEOUT_VALUE);

  return ch;
}
#endif /* HAL_UART_MODULE_ENABLED */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  /* Toggle LED3 */
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
