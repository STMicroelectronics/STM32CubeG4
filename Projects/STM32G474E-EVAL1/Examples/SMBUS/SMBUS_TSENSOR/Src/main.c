/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SMBUS/SMBUS_TSENSOR/Src/main.c 
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32G4xx SMBUS HAL API.
  *          The communication is done using an Temperature Sensor STTS751
  *          on STM32G474E-EVAL1 Eval board.
  *          =================================================================== 
  *          Notes:
  *           - The Temperature Sensor (STTS751) is compatible
  *             with the SMBUS protocol.
  *          ===================================================================
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
#include "stlogo.h"
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
SMBUS_HandleTypeDef hsmbus3;

/* USER CODE BEGIN PV */
/* Private define ------------------------------------------------------------*/
#define TSENSOR_ADDR            0x90    /* STTS751 Address  */
#define TEMPERATURE_LOW         23      /* 23°C             */
#define TEMPERATURE_HIGH        27      /* 27°C             */

/* Private variables ---------------------------------------------------------*/
TSENSOR_InitTypeDef TSENSOR_InitStructure;

/* Useful variables */
uint16_t temperaturevalue = 0;
uint8_t  alertoccurs = 0;
uint8_t  requestsample = 0;
uint32_t tick = 0;
uint8_t  addressalert = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C3_SMBUS_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void Display_ExampleDescription(void);
static void TSENSOR_SetHint(void);
static void TSENSOR_Display_Temperature(uint16_t Temperature);

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

  /* Configure LED3 */
  BSP_LED_Init(LED3);

  /*##-1- Initialize the LCD #################################################*/
  BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);
  UTIL_LCD_SetFuncDriver(&LCD_Driver); /* SetFunc before setting device */
  UTIL_LCD_SetDevice(0);            /* SetDevice after funcDriver is set */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C3_SMBUS_Init();
  /* USER CODE BEGIN 2 */

  /* Configure User push-button */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);
   
  /*##-2- Display welcome messages on LCD ####################################*/  
  Display_ExampleDescription();

  /* Wait for User push-button press before starting the Example */
  while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_RESET)
  {
  }
  
  /* Wait for User push-button release before starting the Example */
  while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_SET)
  {
  }

  /*##-3- Display Example Template ###########################################*/  
  TSENSOR_SetHint();

  /*##-4- Configure the Temperature Sensor ###################################*/
  /* Conversion 12 bits in continuous mode at one conversion per second */
  /* Alert outside range Limit Temperature */
  TSENSOR_InitStructure.AlertMode             = TSENSOR_ALERT_ENABLE;
  TSENSOR_InitStructure.ConversionMode        = TSENSOR_CONTINUOUS_MODE;
  TSENSOR_InitStructure.ConversionResolution  = TSENSOR_CONV_12BITS;
  TSENSOR_InitStructure.ConversionRate        = TSENSOR_ONE_PER_SECOND;           
  TSENSOR_InitStructure.TemperatureLimitHigh  = TEMPERATURE_HIGH;
  TSENSOR_InitStructure.TemperatureLimitLow   = TEMPERATURE_LOW;

  if (TSENSOR_Init(TSENSOR_ADDR, &TSENSOR_InitStructure) != TSENSOR_OK)
  {
    /* Initialization Error */
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    UTIL_LCD_DisplayStringAt(0, 115, (uint8_t*)"Initialization problem", CENTER_MODE); 
    Error_Handler();
  }

  /* Initialize tick counter */
  tick = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /*##-5- Main loop to manage Alert and display Temperature Measured #########*/
  while (1)
  {
    /* Check if a new temperature read is requested to display                */
    if (requestsample == 1)
    {
      /* Read and Display the current temperature                             */
      temperaturevalue = TSENSOR_ReadTemp(TSENSOR_ADDR);

      /* Clear previous warning message */
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);    
      UTIL_LCD_DisplayStringAt(0, 160, (uint8_t *)"                       ", CENTER_MODE);
      UTIL_LCD_DisplayStringAt(0, 175, (uint8_t *)"                       ", CENTER_MODE);

      TSENSOR_Display_Temperature(temperaturevalue);
      
      /* Reset the sampling request                                           */
      tick = HAL_GetTick();
      requestsample = 0;
    }
    else
    {
      /* Request a Temperature sampling each 1s <-> 1000 ms                   */
      if (HAL_GetTick() >= tick + 1000)
      {
        /* Set the sampling request                                           */
        requestsample = 1;
      }
    }
    
    /* Check if an alert occurs                                               */
    if (alertoccurs == 1)
    {
      /* Get the Address of temperature sensor in Alert Mode                  */
      addressalert = TSENSOR_AlerteResponseAddressRead();
      
      /* Display warning message depends on Limit value                       */
      if ((TSENSOR_ReadStatus(addressalert) & TSENSOR_TEMP_EXCEED_HIGH_LIMIT) == TSENSOR_TEMP_EXCEED_HIGH_LIMIT)
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
        /* Display warning message Temperature high limit exceeded            */
        UTIL_LCD_DisplayStringAt(0, 160, (uint8_t *)"Temperature Limit High", CENTER_MODE);
        UTIL_LCD_DisplayStringAt(0, 175, (uint8_t *)"has been exceeded", CENTER_MODE);
      }
      else
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
        /* Display warning message Temperature is at or blow low limit        */
        UTIL_LCD_DisplayStringAt(0, 160, (uint8_t *)" Temperature is at or ", CENTER_MODE);
        UTIL_LCD_DisplayStringAt(0, 175, (uint8_t *)"below the Low Limit", CENTER_MODE);
      }
      alertoccurs = 0;
    }
        
    HAL_Delay(5);
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
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_SMBUS_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hsmbus3.Instance = I2C3;
  hsmbus3.Init.Timing = 0x00303D5B;
  hsmbus3.Init.AnalogFilter = SMBUS_ANALOGFILTER_ENABLE;
  hsmbus3.Init.OwnAddress1 = 2;
  hsmbus3.Init.AddressingMode = SMBUS_ADDRESSINGMODE_7BIT;
  hsmbus3.Init.DualAddressMode = SMBUS_DUALADDRESS_DISABLE;
  hsmbus3.Init.OwnAddress2 = 0;
  hsmbus3.Init.OwnAddress2Masks = SMBUS_OA2_NOMASK;
  hsmbus3.Init.GeneralCallMode = SMBUS_GENERALCALL_DISABLE;
  hsmbus3.Init.NoStretchMode = SMBUS_NOSTRETCH_DISABLE;
  hsmbus3.Init.PacketErrorCheckMode = SMBUS_PEC_DISABLE;
  hsmbus3.Init.PeripheralMode = SMBUS_PERIPHERAL_MODE_SMBUS_HOST;
  hsmbus3.Init.SMBusTimeout = 0x000080C3;
  if (HAL_SMBUS_Init(&hsmbus3) != HAL_OK)
  {
    Error_Handler();
  }

  /** configuration Alert Mode
  */
  if (HAL_SMBUS_EnableAlert_IT(&hsmbus3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

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
  __HAL_RCC_GPIOG_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Display main example message
  * @param  None
  * @retval None
  */
static void Display_ExampleDescription(void)
{
  uint32_t x_size;
  uint32_t y_size;
  
  /* Clear the LCD */ 
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE); 
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  BSP_LCD_DisplayOn(0);
  
  /* Set GUI font */
  UTIL_LCD_SetFont(&UTIL_LCD_DEFAULT_FONT);

  /* Set the LCD Text Color */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);

  /* Display LCD messages */
  UTIL_LCD_DisplayStringAt(0, 10, (uint8_t *)"STM32G474QETx", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 35, (uint8_t *)"Example", CENTER_MODE);
  
  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);
  
  /* Draw Bitmap */
  UTIL_LCD_DrawBitmap((x_size - 80)/2, 65, (uint8_t *)stlogo);
  
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, y_size- 20, (uint8_t *)"Copyright (c) STMicroelectronics 2019", CENTER_MODE);
  
  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_FillRect(0, y_size/2 + 15, x_size, 60, UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_BLUE_DARK); 
  UTIL_LCD_DisplayStringAt(0, y_size/2 + 15, (uint8_t *)"Press User push-button", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, y_size/2 + 30, (uint8_t *)"to start :", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, y_size/2 + 45, (uint8_t *)" TEMPERATURE SENSOR Example", CENTER_MODE);
}

/**
  * @brief  Display TSENSOR Demo Hint
  * @param  None
  * @retval None
  */
static void TSENSOR_SetHint(void)
{
  uint32_t x_size;
  uint32_t y_size;
  
  /* Clear the LCD */ 
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  
  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);
  
  /* Set LCD Demo description */
  UTIL_LCD_FillRect(0, 0, x_size, 80, UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t*)"TEMPERATURE SENSOR", CENTER_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, 30, (uint8_t*)"This example shows how to", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 45, (uint8_t*)"read a Temperature", CENTER_MODE); 
  UTIL_LCD_DisplayStringAt(0, 60, (uint8_t*)"and manage temperature limit alert", CENTER_MODE);

  UTIL_LCD_DrawRect(10, 90, x_size - 20, y_size - 100, UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_DrawRect(11, 91, x_size - 22, y_size - 102, UTIL_LCD_COLOR_ST_BLUE_DARK);

  /* Prepare LCD to display */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_FillRect(12, 92, x_size - 24, y_size - 104, UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
}

  /**
  * @brief  Display temperature
  * @param  temperature : temperature value
  * @retval None
  */
static void TSENSOR_Display_Temperature(uint16_t Temperature)
{
  uint8_t tempcelsiusdisplay[] = "+abc.dddd C";
  uint16_t temperaturevalue = Temperature;
  uint16_t temperaturevaluecelsius = 0;
  uint32_t tempcelsius = 0;

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, 115, (uint8_t*)"Measured Temperature : ", CENTER_MODE);

  /* Change Font size to display Temperature Value */
  UTIL_LCD_SetFont(&Font16);

  /* Verify the sign of the temperature */
  if (temperaturevalue <= 2048)
  {
    /* Positive temperature measured */
    tempcelsiusdisplay[0] = '+';
    /* Initialize the temperature sensor value*/
    temperaturevaluecelsius = temperaturevalue;
    /* Set Text color to Green */
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_GREEN_DARK);
  }
  else
  {
    /* Negative temperature measured */
    tempcelsiusdisplay[0] = '-';
    /* Remove temperature value sign */
    temperaturevaluecelsius = 0x1000 - temperaturevalue;
    /* Set Text color to Blue */
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  }

  tempcelsius = 0;
  /* Calculate temperature digits in °C */
  if (temperaturevaluecelsius & 0x01)
  {
    tempcelsius += 625;     
  }
  if (temperaturevaluecelsius & 0x02)
  {
    tempcelsius += 1250;
  }
  if (temperaturevaluecelsius & 0x04)
  {
    tempcelsius += 2500;
  }
  if (temperaturevaluecelsius & 0x08)
  {
    tempcelsius += 5000;
  }
  
  tempcelsiusdisplay[5] = (tempcelsius / 1000) + 0x30;
  tempcelsiusdisplay[6] = ((tempcelsius % 1000) / 100) + 0x30;
  tempcelsiusdisplay[7] = (((tempcelsius % 1000) % 100) / 10)+ 0x30;
  tempcelsiusdisplay[8] = (((tempcelsius % 1000) % 100) % 10) + 0x30;
    
  temperaturevaluecelsius >>= 4;

  tempcelsiusdisplay[1] = (temperaturevaluecelsius / 100) + 0x30;
  tempcelsiusdisplay[2] = ((temperaturevaluecelsius % 100) / 10) + 0x30;
  tempcelsiusdisplay[3] = ((temperaturevaluecelsius % 100) % 10) + 0x30;

  /* Display Temperature value on LCD */
  UTIL_LCD_DisplayStringAt(0, 145, tempcelsiusdisplay, CENTER_MODE); 
}

/**
  * @brief  TSENSOR error callbacks.
  * @param  Error : Temperature Sensor Error status.
  * @retval None
  */
void TSENSOR_ErrorCallback(uint16_t Error)
{
  if (Error == TSENSOR_ALERT)
  {
    alertoccurs = 1;
  }
  else
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
  /* User can add his own implementation to report the HAL error return state */
  /* Clear Previous message on the LCD */
  uint32_t x_size;
  uint32_t y_size;
  
  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);
  
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_FillRect(12, 92, x_size - 24, y_size- 104, UTIL_LCD_COLOR_WHITE);

  /* Display Communication error message */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
  UTIL_LCD_DisplayStringAt(0, 115, (uint8_t*)"An error occurs during", CENTER_MODE); 
  UTIL_LCD_DisplayStringAt(0, 130, (uint8_t*)"communication with", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 145, (uint8_t*)"the temperature sensor", CENTER_MODE);

  /* Turn LED3 on */
  BSP_LED_On(LED3);
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
