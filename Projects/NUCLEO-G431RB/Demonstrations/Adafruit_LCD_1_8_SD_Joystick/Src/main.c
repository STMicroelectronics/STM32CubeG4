/**
  ******************************************************************************
  * @file    Adafruit_LCD_1_8_SD_Joystick/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program body
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
#include "stlogo.h"

/** @addtogroup STM32G4xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  SHIELD_NOT_DETECTED = 0,
  SHIELD_DETECTED
}ShieldStatus;

/* Private define -------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/
uint8_t DemoIndex = 0;
uint8_t BlinkSpeed = 0;
uint32_t JoyState = JOY_NONE;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static ShieldStatus TFT_ShieldDetect(void);
static void Display_DemoDescription(void);

BSP_DemoTypedef  BSP_examples[]=
{
  {Joystick_demo, "JOYSTICK", 0},
  {LCD_demo, "LCD", 0},
  {SD_demo, "SD", 0},
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32G4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize User_Button on STM32G4xx-Nucleo ------------------*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Check the availability of adafruit 1.8" TFT shield on top of STM32NUCLEO
     board. This is done by reading the state of IO PB.00 pin (mapped to
     JoyStick available on adafruit 1.8" TFT shield). If the state of PB.00
     is high then the adafruit 1.8" TFT shield is available. */
  if(TFT_ShieldDetect() == SHIELD_DETECTED)
  {
    ADAFRUIT_802_JOY_Init(JOY1,  JOY_MODE_EXTI, JOY_ALL);

    /* Initialize the LCD */
    ADAFRUIT_802_LCD_Init(0, LCD_ORIENTATION_PORTRAIT);
    UTIL_LCD_SetFuncDriver(&LCD_Driver);
    Display_DemoDescription();

    while (1)
    {
      JoyState = ADAFRUIT_802_JOY_GetState(JOY1);
      HAL_Delay(100);
      if(JoyState == JOY_SEL)
      {
        BSP_examples[DemoIndex++].DemoFunc();
        HAL_Delay(100);

        if(DemoIndex >= COUNT_OF_EXAMPLE(BSP_examples))
        {
          DemoIndex = 0;
        }
        Display_DemoDescription();
        HAL_Delay(100);
      }
    }
  }
  else /* there is no AdaFruit shield 802 connected */
  {
    /* Initialize Led2 on STM32G4xx-Nucleo ------------------*/
    BSP_LED_Init(LED2);

    /* Infinite loop */
    while(1)
    {
      /* Test on blink speed */
      if(BlinkSpeed == 0)
      {
        BSP_LED_Toggle(LED2);
        /* Wait for 500ms */
        HAL_Delay(500);
      }
      else if(BlinkSpeed == 1)
      {
        BSP_LED_Toggle(LED2);
        /* Wait for 100ms */
        HAL_Delay(100);
      }
      else if(BlinkSpeed == 2)
      {
        BSP_LED_Toggle(LED2);
        /* wait for 50ms */
        HAL_Delay(50);
      }
    }
  }
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
}

/**
  * @brief  Display main demo messages.
  * @param  None
  * @retval None
  */
static void Display_DemoDescription(void)
{
  uint8_t desc[50];
  uint32_t x_size;
  uint32_t y_size;

  ADAFRUIT_802_LCD_GetXSize(0, &x_size);
  ADAFRUIT_802_LCD_GetYSize(0, &y_size);

  UTIL_LCD_SetFont(&Font8);

  /* Clear the LCD */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Set the LCD Text Color */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);

  /* Display LCD messages */
  UTIL_LCD_DisplayStringAt(0, 10, (uint8_t *)"STM32G431RB BSP", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 25, (uint8_t *)"Drivers examples", CENTER_MODE);

  /* Draw Bitmap */
  UTIL_LCD_DrawBitmap((x_size - 80)/2, 35, (uint8_t *)stlogo);

  UTIL_LCD_DisplayStringAt(0, y_size- 15, (uint8_t *)"Copyright (c) STM 2019", CENTER_MODE);

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_FillRect(0, y_size/2 + 15, x_size, 40, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DisplayStringAt(0, y_size/2 + 25, (uint8_t *)"Use User Button to start", CENTER_MODE);
  sprintf((char *)desc,"%s example", BSP_examples[DemoIndex].DemoName);
  UTIL_LCD_DisplayStringAt(0, y_size/2 + 40, (uint8_t *)desc, CENTER_MODE);
}


/**
  * @brief  Check the availability of adafruit 1.8" TFT shield on top of STM32NUCLEO
  *         board. This is done by reading the state of IO PB.00 pin (mapped to
  *         JoyStick available on adafruit 1.8" TFT shield). If the state of PB.00
  *         is high then the adafruit 1.8" TFT shield is available.
  * @param  None
  * @retval SHIELD_DETECTED: 1.8" TFT shield is available
  *         SHIELD_NOT_DETECTED: 1.8" TFT shield is not available
  */
static ShieldStatus TFT_ShieldDetect(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable GPIO clock */
  ADAFRUIT_802_ADCx_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin  = ADAFRUIT_802_ADCx_GPIO_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(ADAFRUIT_802_ADCx_GPIO_PORT , &GPIO_InitStruct);

  if(HAL_GPIO_ReadPin(ADAFRUIT_802_ADCx_GPIO_PORT , ADAFRUIT_802_ADCx_GPIO_PIN ) != 0)
  {
    return SHIELD_DETECTED;
  }
  else
  {
    return SHIELD_NOT_DETECTED;
  }
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
    if(BlinkSpeed == 2)
    {
      BlinkSpeed = 0;
    }
    else
    {
      BlinkSpeed ++;
    }
  }
}

/**
  * @brief  This function handles the test program fail.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  while(1)
  {
    /* LED stays ON*/
    BSP_LED_On(LED2);
  }
}


#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
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


