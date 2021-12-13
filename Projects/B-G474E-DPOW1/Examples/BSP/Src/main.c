/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  * @brief   This application is an example for using the Joystick
  *          and the LED available on B-G474E-DPOW1 board (MB1428)
  *          from STMicroelectronics interfaces.
  *
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32g4xx_hal.h"

/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint32_t ButtonState = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
void  Error_Handler(void);
void  HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void  HAL_MspInit(void);
void  HAL_MspDeInit(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{
  /* USER CODE BEGIN 1 */
  int32_t status = 0;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */

  /* USER CODE BEGIN 2 */

 /* -1- Initialize LEDs mounted on B-G474E-DPOW1 board */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_ORANGE);
  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_RED);

  HAL_Delay(500);

  BSP_LED_On(LED_GREEN);
  HAL_Delay(100);
  if (BSP_LED_GetState(LED_GREEN) != 1) Error_Handler();
  BSP_LED_On(LED_BLUE);
  HAL_Delay(300);
  if (BSP_LED_GetState(LED_BLUE) != 1) Error_Handler();
  BSP_LED_On(LED_ORANGE);
  HAL_Delay(200);
  if (BSP_LED_GetState(LED_ORANGE) != 1) Error_Handler();
  BSP_LED_On(LED_RED);
  HAL_Delay(400);
  if (BSP_LED_GetState(LED_RED) != 1) Error_Handler();

  HAL_Delay(500);

  BSP_LED_Off(LED_GREEN);
  BSP_LED_Off(LED_ORANGE);
  BSP_LED_Off(LED_BLUE);
  BSP_LED_Off(LED_RED);
  HAL_Delay(400);
  if (BSP_LED_GetState(LED_GREEN) != 0) Error_Handler();
  if (BSP_LED_GetState(LED_ORANGE) != 0) Error_Handler();
  if (BSP_LED_GetState(LED_BLUE) != 0) Error_Handler();
  if (BSP_LED_GetState(LED_RED) != 0) Error_Handler();

  HAL_Delay(500);

  /* Configure JOYStick  */
  if (BSP_JOY_Init(JOY1, JOY_MODE_GPIO, JOY_ALL) != BSP_ERROR_NONE) BSP_LED_On(LED_RED);

  while (BSP_JOY_GetState(JOY1) != JOY_SEL )
  {
      BSP_LED_Toggle(LED_GREEN);
      HAL_Delay((40));
      BSP_LED_Toggle(LED_BLUE);
      HAL_Delay((40));
      BSP_LED_Toggle(LED_ORANGE);
      HAL_Delay((40));
      BSP_LED_Toggle(LED_RED);
      HAL_Delay((40));
   }

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (status == 0)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
     HAL_Delay(100);
     if (BSP_JOY_GetState(JOY1) == JOY_SEL )
     {
       BSP_LED_On(LED_GREEN);
       BSP_LED_On(LED_ORANGE);
       BSP_LED_On(LED_BLUE);
       BSP_LED_On(LED_RED);

       status = 1;
       break;
     }
     if (BSP_JOY_GetState(JOY1) == JOY_RIGHT )
     {
       BSP_LED_On(LED_GREEN);
       BSP_LED_Off(LED_ORANGE);
       BSP_LED_Off(LED_BLUE);
       BSP_LED_Off(LED_RED);
     }
     if (BSP_JOY_GetState(JOY1) == JOY_LEFT )
     {
       BSP_LED_Off(LED_GREEN);
       BSP_LED_On(LED_ORANGE);
       BSP_LED_Off(LED_BLUE);
       BSP_LED_Off(LED_RED);
     }
     if (BSP_JOY_GetState(JOY1) == JOY_DOWN )
     {
       BSP_LED_Off(LED_GREEN);
       BSP_LED_Off(LED_ORANGE);
       BSP_LED_On(LED_BLUE);
       BSP_LED_Off(LED_RED);
     }
     if (BSP_JOY_GetState(JOY1) == JOY_UP )
     {
       BSP_LED_Off(LED_GREEN);
       BSP_LED_Off(LED_ORANGE);
       BSP_LED_Off(LED_BLUE);
       BSP_LED_On(LED_RED);
     }
  }
  HAL_Delay(500);
  BSP_LED_Off(LED_GREEN);
  BSP_LED_Off(LED_ORANGE);
  BSP_LED_Off(LED_BLUE);
  BSP_LED_Off(LED_RED);
  HAL_Delay(500);
  /* deConfigure JOYStick  */
  if (BSP_JOY_DeInit(JOY1, JOY_ALL) != BSP_ERROR_NONE) BSP_LED_On(LED_RED);

  /* Configure JOYStick  */
  if (BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL) != BSP_ERROR_NONE) BSP_LED_On(LED_RED);

  status = 0;

  while (status == 0)
  {
   HAL_Delay(1000);
  }

  /* USER CODE END 3 */

}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 170000000
  *            HCLK(Hz)                       = 170000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_M                          = 4
  *            PLL_N                          = 85
  *            PLL_P                          = 2
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            Flash Latency(WS)              = 8
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Enable voltage range 1 boost mode for frequency above 150 Mhz */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);
  __HAL_RCC_PWR_CLK_DISABLE();

  /* Activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM            = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN            = 85;
  RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType           = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
                                           RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource        = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider       = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider      = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider      = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_8) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /**Configure the main internal regulator output voltage
    */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Configure the Systick
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

void  HAL_MspInit(void)
{
}

void  HAL_MspDeInit(void)
{
}

/**
  * @brief  BSP Joystick Callback.
  * @param  JOY Joystick instance
  * @param  JoyPin Specifies the pin connected EXTI line
  * @retval None.
  */
void BSP_JOY_Callback(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin)
{
     if (JOY != JOY1) return;

     if (JoyPin == JOY_UP )
     {
       BSP_LED_Off(LED_GREEN);
       BSP_LED_Off(LED_ORANGE);
       BSP_LED_Off(LED_BLUE);
       BSP_LED_On(LED_RED);
     }
     if (JoyPin == JOY_DOWN )
     {
       BSP_LED_Off(LED_GREEN);
       BSP_LED_Off(LED_ORANGE);
       BSP_LED_On(LED_BLUE);
       BSP_LED_Off(LED_RED);
     }
     if (JoyPin == JOY_LEFT)
     {
       BSP_LED_Off(LED_GREEN);
       BSP_LED_On(LED_ORANGE);
       BSP_LED_Off(LED_BLUE);
       BSP_LED_Off(LED_RED);
     }
     if (JoyPin == JOY_RIGHT )
     {
       BSP_LED_On(LED_GREEN);
       BSP_LED_Off(LED_ORANGE);
       BSP_LED_Off(LED_BLUE);
       BSP_LED_Off(LED_RED);
     }
     if (JoyPin == JOY_SEL )
     {
       BSP_LED_On(LED_GREEN);
       BSP_LED_On(LED_ORANGE);
       BSP_LED_On(LED_BLUE);
       BSP_LED_On(LED_RED);
     }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler */
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(char* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */

/**
  * @}
*/


