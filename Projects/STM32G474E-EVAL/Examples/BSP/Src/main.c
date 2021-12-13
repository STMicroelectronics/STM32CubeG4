/**
  ******************************************************************************
  * @file    BSP/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to test the BSP of the STM32G474E-EVAL board.
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
#include <stdio.h>
#include "stlogo.h"

/** @addtogroup STM32G4xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP_Mode
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

/***********************************************/
/* Please uncomment lines to execute more demo */
/***********************************************/
BSP_DemoTypedef  BSP_examples[]=
{
  {Led_demo,    "LED"},
  {Button_demo, "BUTTON"},
#if (USE_BSP_IO_CLASS == 1)
  {Io_demo,     "IO"},
  {Joy_demo,    "JOYSTICK"},
  {Lcd_demo,    "LCD"},
 // {Idd_demo,    "IDD"},
#endif
//  {Pot_demo,    "POTENTIOMETER"}, /* Please test POT without other tests to avoid perturbation from MFX on PA0 */

#if defined(__GNUC__)  || defined(__ICCARM__)   
  {AudioPlay_demo, "AUDIO PLAY"},
#endif 

   //{AudioRecAnalog_demo, "ANALOG REC"},
  {SD_demo,     "SD polling"},
//  {Com_demo,    "COM"}, /* Please test COM without other tests and with USE_COM_LOG = 1 */
//  {SRAM_demo,   "SRAM"},
  {Bus_demo,    "BUS"},
//  {QSPI_demo,   "QSPI"},
};

__IO uint32_t UserButtonPressed = 0;

/* Private define ------------------------------------------------------------*/
#define KEYS_NUMBER              6 /* Button + joystick */
#define DEBOUNCE                 100 /* 100ms */
#if defined(__GNUC__)
extern void initialise_monitor_handles(void);
#endif
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t DemoIndex = 0;
__IO uint8_t NbLoop = 1;
uint32_t ButtonState = 0;
/*
BSP_SD_CardInfo SD_CardInfo = {0};
uint8_t SDcard_present = SD_NOT_PRESENT;

uint8_t DataBloc[SD_BLOCK_LEN];
uint8_t DataBloc4[4*SD_BLOCK_LEN];
*/

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Display_DemoDescription(void);
static void Flush_scanf(void);
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
#if defined(__GNUC__)
  initialise_monitor_handles();
#endif
  HAL_Init();

  /* Configure the system clock to 170 MHz */
  SystemClock_Config();

  BSP_LED_Init(LED_RED);
  BSP_LED_Init(LED1);

  /* Configure JOY Button */
  if (BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL) != BSP_ERROR_NONE)     BSP_LED_On(LED_RED);

  /* LCD initialization */
  if (BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE) == BSP_ERROR_NONE)
  {
    Display_DemoDescription();
  }
  else
  {
    BSP_LED_On(LED_RED);
  }
  /* Configure USER Button */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Run Application */
  while(1)
  {
    if(UserButtonPressed == 1)
    {
      UserButtonPressed = 0;
      BSP_LED_On(LED1);
      BSP_examples[DemoIndex++].DemoFunc();

      if(DemoIndex >= COUNT_OF_EXAMPLE(BSP_examples))
      {
        NbLoop++;
        DemoIndex = 0;
      }
      Display_DemoDescription();
    }
    HAL_Delay(400);
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
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

/**
  * @brief  Display main demo messages
  * @param  None
  * @retval None
  */
static void Display_DemoDescription(void)
{
  char desc[58];
  uint32_t x_size;
  uint32_t y_size;

  UTIL_LCD_SetFuncDriver(&LCD_Driver); /* SetFunc before setting device */
  UTIL_LCD_SetDevice(0);            /* SetDevice after funcDriver is set */

  BSP_LCD_Clear(0, LCD_COLOR_RGB565_CYAN);
  BSP_LCD_DisplayOn(0);

  UTIL_LCD_SetFont(&Font20);
  /* Set the LCD Text Color */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_LIGHTBLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_DARKBLUE);

  /* Display LCD messages */
  UTIL_LCD_DisplayStringAt( 0, 10, (uint8_t *)"STM32G474E BSP", CENTER_MODE);
  UTIL_LCD_DisplayStringAt( 0, 35, (uint8_t *)"Drivers examples", CENTER_MODE);

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  /* Draw Bitmap */
  BSP_LCD_DrawBitmap(0, (x_size - 80)/2, 65, (uint8_t *)stlogo);

  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt( 0, y_size - 20, (uint8_t *)"Copyright (c) STMicroelectronics 2019", CENTER_MODE);

  UTIL_LCD_SetFont(&Font16);
  BSP_LCD_FillRect(0, 0, y_size/2 + 15, x_size, 60, LCD_COLOR_RGB565_BLUE);
  UTIL_LCD_SetTextColor( UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor( UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DisplayStringAt( 0, y_size / 2 + 30, (uint8_t *)"Press User button to start :", CENTER_MODE);
  sprintf(desc,"%s example", BSP_examples[DemoIndex].DemoName);
  UTIL_LCD_DisplayStringAt( 0, y_size/2 + 45, (uint8_t *)desc, CENTER_MODE);
}

/**
  * @brief  Check for user input
  * @param  None
* @retval Input state (1 : active / 0 : Inactive)
  */
uint8_t CheckForUserInput(void)
{
  if(BSP_PB_GetState(BUTTON_USER) == GPIO_PIN_RESET)
  {
    while (BSP_PB_GetState(BUTTON_USER) == GPIO_PIN_RESET);
    return 1 ;
  }
  return 0;

}

/**
  * @brief  Ask user for result.
  * @param  None
  * @retval None
  */
uint32_t CheckResult(void)
{
  uint32_t result = 0;
  BSP_JOY_Init(JOY1, JOY_MODE_GPIO, JOY_RIGHT);
  BSP_JOY_Init(JOY1, JOY_MODE_GPIO, JOY_LEFT);

  printf("If result is OK press JOY_RIGHT (pass), otherwise press JOY_LEFT (fail) \n");

   while ((BSP_JOY_GetState(JOY1) == JOY_RIGHT) || (BSP_JOY_GetState(JOY1) != JOY_RIGHT));
  if (BSP_JOY_GetState(JOY1) == JOY_LEFT)
  {
    printf("Test is FAIL!!!\n");
    HAL_Delay(1000);
    result = 1;
  }
  else
  {
    printf("Test is PASS\n");
    HAL_Delay(1000);
    result = 0;
  }
  BSP_JOY_DeInit(JOY1, JOY_RIGHT);
  BSP_JOY_DeInit(JOY1, JOY_LEFT);
  return result;

}

/**
  * @brief  Ask user to start the test.
  * @param  None
  * @retval None
  */
void StartTest(void)
{
  uint8_t tmp = 0;
  do {
    printf("Press s to start the test\n");
    scanf("%c", &tmp);
    Flush_scanf();
  } while (tmp != 's');
}

/**
  * @brief  Flush scanf buffer until "\n".
  * @param  None
  * @retval None
  */
static void Flush_scanf(void)
{
  while ( getchar() != '\n' );
}

/**
  * @brief  Button Callback
  * @param  Button Specifies the pin connected EXTI line
  * @retval None
  */
void BSP_PB_Callback(Button_TypeDef Button)
{
  if(Button == BUTTON_USER)
  {
    //if(check_switches(0))
    {
      UserButtonPressed = 1;
    }
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    BSP_PB_Callback(BUTTON_USER);
}

void ButtonPendingCallback(void)
{
  BSP_PB_Callback(BUTTON_USER);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
     /* Toggle LED4 with a period of one second */
    BSP_LED_Toggle(LED4);
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */


