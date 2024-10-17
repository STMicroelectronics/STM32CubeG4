/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    I2C/I2C_TwoBoards_AdvComIT/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32G4xx I2C HAL API to transmit
  *          and receive a data buffer with a communication process based on
  *          IT transfer.
  *          The communication is done using 2 Boards.
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
/* Uncomment this line to use the board as master, if not it is used as slave */
//#define MASTER_BOARD
#define MASTER_REQ_READ    0x12
#define MASTER_REQ_WRITE   0x34
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
/* Buffer used for transmission */
uint8_t aTxBuffer[] = " ****I2C_TwoBoards communication based on IT****  ****I2C_TwoBoards communication based on IT****  ****I2C_TwoBoards communication based on IT**** ";

/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];
__IO uint16_t hTxNumData = 0;
__IO uint16_t hRxNumData = 0;
uint8_t bTransferRequest = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
static void Flush_Buffer(uint8_t *pBuffer, uint16_t BufferLength);
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
  MX_GPIO_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  /* Configure LED2 */
  BSP_LED_Init(LED2);


  /* ****************************************************************************
                      MASTER SECTION
  */

#ifdef MASTER_BOARD
  /* Configure User push-button */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

  /* Wait for User push-button press before starting the Communication */
  while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_SET)
  {
  }

  /* Wait for User push-button release before starting the Communication */
  while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_RESET)
  {
  }
#endif /* MASTER_BOARD */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
#ifdef MASTER_BOARD
    /* Initialize number of data variables */
    hTxNumData = TXBUFFERSIZE;
    hRxNumData = RXBUFFERSIZE;

    /* Update bTransferRequest to send buffer write request for Slave */
    bTransferRequest = MASTER_REQ_WRITE;

    /*##-2- Master sends write request for slave #############################*/
    do
    {
      if (HAL_I2C_Master_Transmit_IT(&hi2c1, (uint16_t)I2C_ADDRESS, (uint8_t *)&bTransferRequest, 1) != HAL_OK)
      {
        /* Error_Handler() function is called when error occurs. */
        Error_Handler();
      }

      /*  Before starting a new communication transfer, you need to check the current
          state of the peripheral; if it�s busy you need to wait for the end of current
          transfer before starting a new one.
          For simplicity reasons, this example is just waiting till the end of the
          transfer, but application may perform other tasks while transfer operation
          is ongoing. */
      while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      {
      }

      /* When Acknowledge failure occurs (Slave don't acknowledge it's address)
         Master restarts communication */
    }
    while (HAL_I2C_GetError(&hi2c1) == HAL_I2C_ERROR_AF);

    /*##-3- Master sends number of data to be written ########################*/
    do
    {
      if (HAL_I2C_Master_Transmit_IT(&hi2c1, (uint16_t)I2C_ADDRESS, (uint8_t *)&hTxNumData, 2) != HAL_OK)
      {
        /* Error_Handler() function is called when error occurs. */
        Error_Handler();
      }

      /*  Before starting a new communication transfer, you need to check the current
          state of the peripheral; if it�s busy you need to wait for the end of current
          transfer before starting a new one.
          For simplicity reasons, this example is just waiting till the end of the
          transfer, but application may perform other tasks while transfer operation
          is ongoing. */
      while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      {
      }

      /* When Acknowledge failure occurs (Slave don't acknowledge it's address)
         Master restarts communication */
    }
    while (HAL_I2C_GetError(&hi2c1) == HAL_I2C_ERROR_AF);

    /*##-4- Master sends aTxBuffer to slave ##################################*/
    do
    {
      if (HAL_I2C_Master_Transmit_IT(&hi2c1, (uint16_t)I2C_ADDRESS, (uint8_t *)aTxBuffer, TXBUFFERSIZE) != HAL_OK)
      {
        /* Error_Handler() function is called when error occurs. */
        Error_Handler();
      }

      /*  Before starting a new communication transfer, you need to check the current
          state of the peripheral; if it�s busy you need to wait for the end of current
          transfer before starting a new one.
          For simplicity reasons, this example is just waiting till the end of the
          transfer, but application may perform other tasks while transfer operation
          is ongoing. */
      while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      {
      }

      /* When Acknowledge failure occurs (Slave don't acknowledge it's address)
         Master restarts communication */
    }
    while (HAL_I2C_GetError(&hi2c1) == HAL_I2C_ERROR_AF);

    /* Update bTransferRequest to send buffer read request for Slave */
    bTransferRequest = MASTER_REQ_READ;

    /*##-5- Master sends read request for slave ##############################*/
    do
    {
      if (HAL_I2C_Master_Transmit_IT(&hi2c1, (uint16_t)I2C_ADDRESS, (uint8_t *)&bTransferRequest, 1) != HAL_OK)
      {
        /* Error_Handler() function is called when error occurs. */
        Error_Handler();
      }

      /*  Before starting a new communication transfer, you need to check the current
          state of the peripheral; if it�s busy you need to wait for the end of current
          transfer before starting a new one.
          For simplicity reasons, this example is just waiting till the end of the
          transfer, but application may perform other tasks while transfer operation
          is ongoing. */
      while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      {
      }

      /* When Acknowledge failure occurs (Slave don't acknowledge it's address)
         Master restarts communication */
    }
    while (HAL_I2C_GetError(&hi2c1) == HAL_I2C_ERROR_AF);

    /*##-6- Master sends number of data to be read ###########################*/
    do
    {
      if (HAL_I2C_Master_Transmit_IT(&hi2c1, (uint16_t)I2C_ADDRESS, (uint8_t *)&hRxNumData, 2) != HAL_OK)
      {
        /* Error_Handler() function is called when error occurs. */
        Error_Handler();
      }

      /*  Before starting a new communication transfer, you need to check the current
          state of the peripheral; if it�s busy you need to wait for the end of current
          transfer before starting a new one.
          For simplicity reasons, this example is just waiting till the end of the
          transfer, but application may perform other tasks while transfer operation
          is ongoing. */
      while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      {
      }

      /* When Acknowledge failure occurs (Slave don't acknowledge it's address)
         Master restarts communication */
    }
    while (HAL_I2C_GetError(&hi2c1) == HAL_I2C_ERROR_AF);

    /*##-7- Master receives aRxBuffer from slave #############################*/
    do
    {
      if (HAL_I2C_Master_Receive_IT(&hi2c1, (uint16_t)I2C_ADDRESS, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
      {
        /* Error_Handler() function is called when error occurs. */
        Error_Handler();
      }

      /*  Before starting a new communication transfer, you need to check the current
          state of the peripheral; if it�s busy you need to wait for the end of current
          transfer before starting a new one.
          For simplicity reasons, this example is just waiting till the end of the
          transfer, but application may perform other tasks while transfer operation
          is ongoing. */
      while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      {
      }

      /* When Acknowledge failure occurs (Slave don't acknowledge it's address)
         Master restarts communication */
    }
    while (HAL_I2C_GetError(&hi2c1) == HAL_I2C_ERROR_AF);

    /* Check correctness of received buffer ##################################*/
    if (Buffercmp((uint8_t *)aTxBuffer, (uint8_t *)aRxBuffer, hRxNumData))
    {
      /* Processing Error */
      Error_Handler();
    }

    /* Flush Rx buffers */
    Flush_Buffer((uint8_t *)aRxBuffer, RXBUFFERSIZE);

    /* Toggle LED2 */
    BSP_LED_Toggle(LED2);

    /* This delay permits to see LED2 toggling */
    HAL_Delay(25);
#else /* MASTER_BOARD */
    /* ****************************************************************************
                        SLAVE SECTION
    */
    /* Initialize number of data variables */
    hTxNumData = 0;
    hRxNumData = 0;

    /*##-2- Slave receive request from master ################################*/
    while (HAL_I2C_Slave_Receive_IT(&hi2c1, (uint8_t *)&bTransferRequest, 1) != HAL_OK)
    {
    }

    /*  Before starting a new communication transfer, you need to check the current
    state of the peripheral; if it�s busy you need to wait for the end of current
    transfer before starting a new one.
    For simplicity reasons, this example is just waiting till the end of the
    transfer, but application may perform other tasks while transfer operation
    is ongoing. */
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
    {
    }

    /* If master request write operation #####################################*/
    if (bTransferRequest == MASTER_REQ_WRITE)
    {
      /*##-3- Slave receive number of data to be read ########################*/
      while (HAL_I2C_Slave_Receive_IT(&hi2c1, (uint8_t *)&hRxNumData, 2) != HAL_OK);

      /*  Before starting a new communication transfer, you need to check the current
      state of the peripheral; if it�s busy you need to wait for the end of current
      transfer before starting a new one.
      For simplicity reasons, this example is just waiting till the end of the
      transfer, but application may perform other tasks while transfer operation
      is ongoing. */
      while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      {
      }

      /*##-4- Slave receives aRxBuffer from master ###########################*/
      while (HAL_I2C_Slave_Receive_IT(&hi2c1, (uint8_t *)aRxBuffer, hRxNumData) != HAL_OK);

      /*  Before starting a new communication transfer, you need to check the current
      state of the peripheral; if it�s busy you need to wait for the end of current
      transfer before starting a new one.
      For simplicity reasons, this example is just waiting till the end of the
      transfer, but application may perform other tasks while transfer operation
      is ongoing. */
      while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      {
      }

      /* Check correctness of received buffer ################################*/
      if (Buffercmp((uint8_t *)aTxBuffer, (uint8_t *)aRxBuffer, hRxNumData))
      {
        /* Processing Error */
        Error_Handler();
      }

      /* Flush Rx buffers */
      Flush_Buffer((uint8_t *)aRxBuffer, RXBUFFERSIZE);

      /* Toggle LED2 */
      BSP_LED_Toggle(LED2);
    }
    /* If master request write operation #####################################*/
    else
    {
      /*##-3- Slave receive number of data to be written #####################*/
      while (HAL_I2C_Slave_Receive_IT(&hi2c1, (uint8_t *)&hTxNumData, 2) != HAL_OK);

      /*  Before starting a new communication transfer, you need to check the current
      state of the peripheral; if it�s busy you need to wait for the end of current
      transfer before starting a new one.
      For simplicity reasons, this example is just waiting till the end of the
      transfer, but application may perform other tasks while transfer operation
      is ongoing. */
      while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      {
      }

      /*##-4- Slave transmit aTxBuffer to master #############################*/
      while (HAL_I2C_Slave_Transmit_IT(&hi2c1, (uint8_t *)aTxBuffer, RXBUFFERSIZE) != HAL_OK);

      /*  Before starting a new communication transfer, you need to check the current
      state of the peripheral; if it�s busy you need to wait for the end of current
      transfer before starting a new one.
      For simplicity reasons, this example is just waiting till the end of the
      transfer, but application may perform other tasks while transfer operation
      is ongoing. */
      while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
      {
      }
    }

#endif /* MASTER_BOARD */
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00303D5B;
  hi2c1.Init.OwnAddress1 = I2C_ADDRESS;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }

  /** I2C Fast mode Plus enable
  */
  HAL_I2CEx_EnableFastModePlus(I2C_FASTMODEPLUS_I2C1);
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/**
  * @brief  Flushes the buffer
  * @param  pBuffer: buffers to be flushed.
  * @param  BufferLength: buffer's length
  * @retval None
  */
static void Flush_Buffer(uint8_t *pBuffer, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    *pBuffer = 0;

    pBuffer++;
  }
}

/**
  * @brief  I2C error callbacks.
  * @param  I2cHandle: I2C handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
{
  /** Error_Handler() function is called when error occurs.
    * 1- When Slave doesn't acknowledge its address, Master restarts communication.
    * 2- When Master doesn't acknowledge the last data transferred, Slave doesn't care in this example.
    */
  if (HAL_I2C_GetError(I2cHandle) != HAL_I2C_ERROR_AF)
  {
    Error_Handler();
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
  /* Error if LED2 is slowly blinking (1 sec. period) */
  while (1)
  {
    BSP_LED_Toggle(LED2);
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
  Error_Handler();
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
