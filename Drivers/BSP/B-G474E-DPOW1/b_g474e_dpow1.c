/**
  ******************************************************************************
  * @file    b_g474e_dpow1.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to manage
  *          LEDs
  *          Joystick
  *          available on B-G474E-DPOW1 board (MB1428) from STMicroelectronics.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "b_g474e_dpow1.h"

#if (USE_BSP_COM_FEATURE > 0)
  #if (USE_COM_LOG > 0)
    #ifndef __GNUC__
      #include <stdio.h>
    #endif
  #endif
#endif

/** @addtogroup BSP
  * @{
  */

/** @defgroup B-G474E-DPOW1 B-G474E-DPOW1
  * @{
  */

/** @defgroup B-G474E-DPOW1_Common B-G474E-DPOW1 Common
  * @brief This file provides set of firmware functions to manage Leds and push-button
  *        available on B-G474E-DPOW1 Kit from STMicroelectronics.
  * @{
  */


/** @defgroup B-G474E-DPOW1_Private_Variables Private Variables
  * @{
  */
static GPIO_TypeDef* LED_PORT[LEDn] = {LED2_GPIO_PORT,
                                       LED3_GPIO_PORT,
                                       LED4_GPIO_PORT,
                                       LED5_GPIO_PORT};

static const uint32_t LED_PIN[LEDn] = {LED2_PIN,
                                       LED3_PIN,
                                       LED4_PIN,
                                       LED5_PIN};

#if (USE_BSP_COM_FEATURE > 0)
static USART_TypeDef* COM_USART[COMn]   = {COM1_UART};
#if (USE_COM_LOG > 0)
static COM_TypeDef COM_ActiveLogPort = COM1;
#endif
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
static uint32_t IsComMspCbValid[COMn] = {0};
#endif
#endif /* (USE_BSP_COM_FEATURE > 0)*/

#if (USE_BSP_JOY_FEATURE > 0)
static uint32_t JoyPinsMask[JOYn] = {0};

/**
* @brief JOYSTICK variables
*/
GPIO_TypeDef* JOY_PORT[JOY_KEY_NUMBER] = {
  SEL_JOY_GPIO_PORT,
  DOWN_JOY_GPIO_PORT,
  LEFT_JOY_GPIO_PORT,
  RIGHT_JOY_GPIO_PORT,
  UP_JOY_GPIO_PORT
};

const uint16_t JOY_PIN[JOY_KEY_NUMBER] = {
  SEL_JOY_PIN,
  DOWN_JOY_PIN,
  LEFT_JOY_PIN,
  RIGHT_JOY_PIN,
  UP_JOY_PIN
};

const uint8_t JOY_IRQn[JOY_KEY_NUMBER] = {
  SEL_JOY_EXTI_IRQn,
  DOWN_JOY_EXTI_IRQn,
  LEFT_JOY_EXTI_IRQn,
  RIGHT_JOY_EXTI_IRQn,
  UP_JOY_EXTI_IRQn
};

static void JOY1_EXTI_Callback(JOYPin_TypeDef JoyPin);

#endif /* USE_BSP_JOY_FEATURE */
/**
  * @}
  */

/** @defgroup B-G474E-DPOW1_Exported_Variables Exported Variables
  * @{
  */
#if (USE_BSP_JOY_FEATURE > 0)
EXTI_HandleTypeDef hjoy_exti[JOY_KEY_NUMBER];
#endif /* USE_BSP_JOY_FEATURE */

#if (USE_BSP_COM_FEATURE > 0)
UART_HandleTypeDef hcom_uart[COMn];
#endif /* USE_BSP_COM_FEATURE */
/**
  * @}
  */

/** @defgroup B-G474E-DPOW1_Private_FunctionPrototypes Private Functions
  * @{
  */
#if (USE_BSP_COM_FEATURE > 0)
static void USART3_MspInit(UART_HandleTypeDef *huart);
static void USART3_MspDeInit(UART_HandleTypeDef *huart);
#endif /* USE_BSP_COM_FEATURE */

static void SEL_JOY_EXTI_Callback(void);
static void DOWN_JOY_EXTI_Callback(void);
static void LEFT_JOY_EXTI_Callback(void);
static void RIGHT_JOY_EXTI_Callback(void);
static void UP_JOY_EXTI_Callback(void);
/**
  * @}
  */

/** @addtogroup B-G474E-DPOW1_Exported_Functions
  * @{
  */

/**
  * @brief  This method returns the B-G474E-DPOW1 BSP Driver revision
  * @retval version 0xXYZR (8bits for each decimal, R for RC)
  */
int32_t BSP_GetVersion(void)
{
  return (int32_t)B_G474E_DPOW1_BSP_VERSION;
}

/**
  * @brief  This method returns the board name
  * @retval pointer to the board name string
  */
const uint8_t* BSP_GetBoardName(void)
{
  return (const uint8_t*)B_G474E_DPOW1_BSP_BOARD_NAME;
}

/**
  * @brief  This method returns the board ID
  * @retval pointer to the board ID string
  */
const uint8_t* BSP_GetBoardID(void)
{
  return (const uint8_t*)B_G474E_DPOW1_BSP_BOARD_ID;
}

/**
  * @brief  Configures LED on GPIO.
  * @param  Led LED to be configured.
  *          This parameter can be one of the following values:
  *     @arg  LED2
  *     @arg  LED3
  *     @arg  LED4
  *     @arg  LED5
  * @retval BSP status
  */
int32_t BSP_LED_Init(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;

  GPIO_InitTypeDef  gpio_init_structure;

  /* Enable the GPIO_LED clock */
  if (Led == LED2)
  {
   __HAL_RCC_GPIOA_CLK_ENABLE();
  }
  else if ((Led == LED3) || (Led == LED4) || (Led == LED5))
  {
   __HAL_RCC_GPIOB_CLK_ENABLE();
  }
  else
  {
   return BSP_ERROR_WRONG_PARAM;
  }

  /* Configure the GPIO_LED pin */
  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
  gpio_init_structure.Pin = LED_PIN [Led];
  HAL_GPIO_Init(LED_PORT [Led], &gpio_init_structure);
  HAL_GPIO_WritePin(LED_PORT [Led], (uint16_t)LED_PIN[Led], GPIO_PIN_RESET);

  return ret;
}

/**
  * @brief  DeInit LEDs.
  * @param  Led LED to be configured.
  *          This parameter can be one of the following values:
  *     @arg  LED2
  *     @arg  LED3
  *     @arg  LED4
  *     @arg  LED5
  * @retval BSP status
  */
int32_t BSP_LED_DeInit(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;

  GPIO_InitTypeDef  gpio_init_structure;

  if((Led == LED2) || (Led == LED3) || (Led == LED4) || (Led == LED5))
  {
   /* Configure the GPIO_LED pin */
   gpio_init_structure.Pin = LED_PIN [Led];
   HAL_GPIO_WritePin(LED_PORT [Led], (uint16_t)LED_PIN[Led], GPIO_PIN_SET);

   HAL_GPIO_DeInit(LED_PORT [Led], gpio_init_structure.Pin);
  }
  else
  {
   ret = BSP_ERROR_WRONG_PARAM;
  }

  return ret;
}

/**
  * @brief  Turns selected LED On.
  * @param  Led LED to be set on
  *          This parameter can be one of the following values:
  *     @arg  LED2
  *     @arg  LED3
  *     @arg  LED4
  *     @arg  LED5
  * @retval BSP status
  */
int32_t BSP_LED_On(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;

  if((Led == LED2) || (Led == LED3) || (Led == LED4) || (Led == LED5))
  {
    HAL_GPIO_WritePin (LED_PORT [Led], (uint16_t)LED_PIN [Led], GPIO_PIN_SET);
  }
  else
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led LED to be set off
  *          This parameter can be one of the following values:
  *     @arg  LED2
  *     @arg  LED3
  *     @arg  LED4
  *     @arg  LED5
  * @retval BSP status
  */
int32_t BSP_LED_Off(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;

  if((Led == LED2) || (Led == LED3) || (Led == LED4) || (Led == LED5))
  {
    HAL_GPIO_WritePin (LED_PORT [Led], (uint16_t)LED_PIN [Led], GPIO_PIN_RESET);
  }
  else
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led LED to be toggled
  *          This parameter can be one of the following values:
  *     @arg  LED2
  *     @arg  LED3
  *     @arg  LED4
  *     @arg  LED5
  * @retval BSP status
  */
int32_t BSP_LED_Toggle(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;

  if((Led == LED2) || (Led == LED3) || (Led == LED4) || (Led == LED5))
  {
   HAL_GPIO_TogglePin(LED_PORT[Led], (uint16_t)LED_PIN[Led]);
  }
  else
  {
   ret = BSP_ERROR_WRONG_PARAM;
  }

  return ret;
}

/**
  * @brief  Get the selected LED state.
  * @param  Led LED to be get its state
  *          This parameter can be one of the following values:
  *     @arg  LED2
  *     @arg  LED3
  *     @arg  LED4
  *     @arg  LED5
  * @retval LED status
  */
int32_t BSP_LED_GetState(Led_TypeDef Led)
{
  int32_t ret;

  if((Led == LED2) || (Led == LED3) || (Led == LED4) || (Led == LED5))
  {
   ret = (int32_t)((HAL_GPIO_ReadPin (LED_PORT [Led], (uint16_t)LED_PIN [Led]) != GPIO_PIN_SET) ? 0U : 1U);
  }
  else
  {
   ret = BSP_ERROR_WRONG_PARAM;
  }
  return ret;
}

#if (USE_BSP_COM_FEATURE > 0)
/**
  * @brief  Configures COM port.
  * @param  COM COM port to be configured.
  *          This parameter can be COM1
  * @param  COM_Init Pointer to a UART_HandleTypeDef structure that contains the
  *                configuration information for the specified USART peripheral.
  * @retval BSP status
  */
int32_t BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM > COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    hcom_uart[COM].Instance          = COM_USART[COM];

    /* Init the UART Msp */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
    USART3_MspInit(&hcom_uart[COM]);
#else
    if(IsComMspCbValid[COM] == 0U)
    {
      if(BSP_COM_RegisterDefaultMspCallbacks(COM) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_MSP_FAILURE;
      }
    }
#endif

    if(MX_USART3_Init(&hcom_uart[COM], COM_Init) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  DeInit COM port.
  * @param  COM COM port to be configured.
  *          This parameter can be COM1
  * @retval BSP status
  */
int32_t BSP_COM_DeInit(COM_TypeDef COM)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM > COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* USART configuration */
    hcom_uart[COM].Instance = COM_USART[COM];

#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
    USART3_MspDeInit(&hcom_uart[COM]);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 0) */

    if(HAL_UART_DeInit(&hcom_uart[COM]) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  Configures COM port.
  * @param  huart USART handle
  * @param  COM_Init Pointer to a UART_HandleTypeDef structure that contains the
  *                  configuration information for the specified USART peripheral.
  * @retval HAL error code
  */
__weak HAL_StatusTypeDef MX_USART3_Init(UART_HandleTypeDef *huart, MX_UART_InitTypeDef *Init)
{
  /* USART configuration */
  huart->Init.BaudRate     = Init->BaudRate;
  huart->Init.Mode         = UART_MODE_TX_RX;
  huart->Init.Parity       = (uint32_t)Init->Parity;
  huart->Init.WordLength   = Init->WordLength;
  huart->Init.StopBits     = (uint32_t)Init->StopBits;
  huart->Init.HwFlowCtl    = (uint32_t)Init->HwFlowCtl;
  huart->Init.OverSampling = UART_OVERSAMPLING_8;

  return HAL_UART_Init(huart);
}

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
/**
  * @brief Register Default COM Msp Callbacks
  * @param  COM COM port to be configured.
  * @retval BSP status
  */
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    __HAL_UART_RESET_HANDLE_STATE(&hcom_uart[COM]);

    /* Register default MspInit/MspDeInit Callback */
    if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPINIT_CB_ID, USART3_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPDEINIT_CB_ID, USART3_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsComMspCbValid[COM] = 1U;
    }
  }

  /* BSP status */
  return ret;
}

/**
  * @brief  Register COM1 Msp Callback registering
  * @param  COM COM port to be configured.
  * @param  Callbacks     pointer to COM MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *Callback)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    __HAL_UART_RESET_HANDLE_STATE(&hcom_uart[COM]);

    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPINIT_CB_ID, Callback->pMspInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPDEINIT_CB_ID, Callback->pMspDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsComMspCbValid[COM] = 1U;
    }
  }

  /* BSP status */
  return ret;
}
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

#if (USE_COM_LOG > 0)
/**
  * @brief  Select the active COM port.
  * @param  COM COM port to be activated.
  *          This parameter can be COM1
  * @retval BSP status
  */
int32_t BSP_COM_SelectLogPort(COM_TypeDef COM)
{
  if(COM_ActiveLogPort != COM)
  {
    COM_ActiveLogPort = COM;
  }
  return BSP_ERROR_NONE;
}

/**
  * @brief  Redirect console output to COM
  */
 #ifdef __GNUC__
 int __io_putchar (int ch)
 #else
 int fputc (int ch, FILE *f)
 #endif /* __GNUC__ */
{
  HAL_UART_Transmit (&hcom_uart [COM_ActiveLogPort], (uint8_t *) &ch, 1, COM1_POLL_TIMEOUT);
  return ch;
}
#endif /* USE_COM_LOG */
#endif /* USE_BSP_COM_FEATURE*/

#if (USE_BSP_JOY_FEATURE > 0)
/**
  * @brief  Configures joystick GPIO and EXTI modes.
  * @param  JOY Joystick to be initialized
  * @param  JoyMode Button mode.
  *          This parameter can be one of the following values:
  *            @arg  JOY_MODE_GPIO: Joystick pins will be used as simple IOs
  *            @arg  JOY_MODE_EXTI: Joystick pins will be connected to EXTI line
  *                                 with interrupt generation capability
  * @param  JoyPins joystick pins to be initialized
  * @retval BSP status
  */
int32_t BSP_JOY_Init(JOY_TypeDef JOY, JOYMode_TypeDef JoyMode, JOYPin_TypeDef JoyPins)
{
  int32_t ret = BSP_ERROR_NONE;
  GPIO_InitTypeDef GPIO_InitStruct;
  uint8_t key_pressed, joykey;
  static BSP_EXTI_LineCallback JoyCallback[JOY_KEY_NUMBER] = {
   SEL_JOY_EXTI_Callback, DOWN_JOY_EXTI_Callback, LEFT_JOY_EXTI_Callback,
   RIGHT_JOY_EXTI_Callback, UP_JOY_EXTI_Callback};

  static const uint32_t JOY_EXTI_LINE[JOY_KEY_NUMBER] = {
   SEL_JOY_EXTI_LINE, DOWN_JOY_EXTI_LINE,
   LEFT_JOY_EXTI_LINE, RIGHT_JOY_EXTI_LINE, UP_JOY_EXTI_LINE
  };

  if(JOY >= JOYn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
   JoyPinsMask[JOY] |= JoyPins;
   /* Initialized the Joystick. */
   for(joykey = 0; joykey < JOY_KEY_NUMBER ; joykey++)
   {
    key_pressed = 1U << joykey;
    if(key_pressed & JoyPins)
    {
     /* Enable the JOY clock */
     JOY1_GPIO_CLK_ENABLE(key_pressed);

     GPIO_InitStruct.Pin = JOY_PIN[joykey];
     GPIO_InitStruct.Pull = GPIO_PULLUP;
     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

     if (JoyMode == JOY_MODE_GPIO)
     {
       /* Configure Joy pin as input */
       GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
       HAL_GPIO_Init(JOY_PORT[joykey], &GPIO_InitStruct);
     }
     else if (JoyMode == JOY_MODE_EXTI)
     {
       /* Configure Joy pin as input with External interrupt */
       GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
       HAL_GPIO_Init(JOY_PORT[joykey], &GPIO_InitStruct);

      (void)HAL_EXTI_GetHandle(&hjoy_exti[joykey], JOY_EXTI_LINE[joykey]);
      (void)HAL_EXTI_RegisterCallback(&hjoy_exti[joykey],  HAL_EXTI_COMMON_CB_ID, JoyCallback[joykey]);

       /* Enable and set Joy EXTI Interrupt to the lowest priority */
       HAL_NVIC_SetPriority((IRQn_Type)(JOY_IRQn[joykey]), 0x03, 0x00);
       HAL_NVIC_EnableIRQ((IRQn_Type)(JOY_IRQn[joykey]));
     }
     else
     {
       ret = BSP_ERROR_WRONG_PARAM;
     }
    }
   }
  }
  return ret;
}

/**
  * @brief  DeInit joystick GPIOs.
  * @note   DeInit does not disable the GPIO clock
  * @param  JOY Joystick to be initialized
  * @param  JoyPins joystick pins to be de-initialized
  * @retval BSP status
  */
int32_t BSP_JOY_DeInit(JOY_TypeDef JOY, JOYPin_TypeDef JoyPins)
{
  int32_t ret = BSP_ERROR_NONE;
  uint8_t key_pressed, joykey;
  if(JOY >= JOYn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Initialized the Joystick. */
    for(joykey = 0; joykey < JOY_KEY_NUMBER ; joykey++)
    {
       key_pressed = 1U << joykey;
       if(key_pressed & JoyPins)
       {
          /* Disable the JOY clock */
          HAL_GPIO_DeInit(JOY_PORT[joykey], JOY_PIN[joykey]);
          JoyPinsMask[JOY] &= ~key_pressed;
       }
     }
  }
  return ret;
}

/**
  * @brief  Returns the current joystick status.
  * @param  JOY Joystick instance
  * @retval Code of the joystick key pressed
  *          This code can be one of the following values:
  *            @arg  JOY_NONE
  *            @arg  JOY_SEL
  *            @arg  JOY_DOWN
  *            @arg  JOY_LEFT
  *            @arg  JOY_RIGHT
  *            @arg  JOY_UP
  */
int32_t BSP_JOY_GetState(JOY_TypeDef JOY)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t key_pressed, joykey;

  if(JOY >= JOYn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
      for (joykey = 0; joykey < JOY_KEY_NUMBER ; joykey++)
      {
        key_pressed = 1U << joykey;
        if(key_pressed & JoyPinsMask[JOY])
        {
          if(HAL_GPIO_ReadPin(JOY_PORT[joykey], JOY_PIN[joykey]) == GPIO_PIN_RESET)
          {
            /* Return Code Joystick key pressed */
            return key_pressed;
          }
        }
      }
  }
  return ret;
}

/**
  * @brief  BSP Joystick Callback.
  * @param  JOY Joystick instance
  * @param  JoyPin Specifies the pin connected EXTI line
  * @retval None.
  */
__weak void BSP_JOY_Callback(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(JOY);
  UNUSED(JoyPin);

  /* This function should be implemented by the user application.
     It is called into this driver when an event on JoyPin is triggered. */
}
/**
  * @}
  */

/** @defgroup B-G474E-DPOW1_Operations_Functions Operations Functions
  * @{
  */

/**
  * @brief  Key EXTI line detection callbacks.
  * @retval None
  */
static void SEL_JOY_EXTI_Callback(void)
{
  JOY1_EXTI_Callback(JOY_SEL);
}
/**
  * @brief  Key EXTI line detection callbacks.
  * @retval None
  */
static void DOWN_JOY_EXTI_Callback(void)
{
  JOY1_EXTI_Callback(JOY_DOWN);
}
/**
  * @brief  Key EXTI line detection callbacks.
  * @retval None
  */
static void LEFT_JOY_EXTI_Callback(void)
{
  JOY1_EXTI_Callback(JOY_LEFT);
}
/**
  * @brief  Key EXTI line detection callbacks.
  * @retval None
  */
static void RIGHT_JOY_EXTI_Callback(void)
{
  JOY1_EXTI_Callback(JOY_RIGHT);
}
/**
  * @brief  Key EXTI line detection callbacks.
  * @retval None
  */
static void UP_JOY_EXTI_Callback(void)
{
  JOY1_EXTI_Callback(JOY_UP);
}

#endif /* USE_BSP_JOY_FEATURE */

/*******************************************************************************
                            Static Functions
*******************************************************************************/

#if (USE_BSP_JOY_FEATURE > 0)
/**
  * @brief  Joystick EXTI line detection callbacks.
  * @retval None
  */
static void JOY1_EXTI_Callback(JOYPin_TypeDef JoyPin)
{
  if ((JoyPinsMask[JOY1] & JoyPin) == JOY_SEL)
  {
    BSP_JOY_Callback(JOY1, JOY_SEL);
  }
  if ((JoyPinsMask[JOY1] & JoyPin) == JOY_UP)
  {
      BSP_JOY_Callback(JOY1, JOY_UP);
  }
  if ((JoyPinsMask[JOY1] & JoyPin) == JOY_DOWN)
  {
      BSP_JOY_Callback(JOY1, JOY_DOWN);
  }
  if ((JoyPinsMask[JOY1] & JoyPin) == JOY_LEFT)
  {
      BSP_JOY_Callback(JOY1, JOY_LEFT);
  }
  if ((JoyPinsMask[JOY1] & JoyPin) == JOY_RIGHT)
  {
      BSP_JOY_Callback(JOY1, JOY_RIGHT);
  }
}
#endif /* USE_BSP_JOY_FEATURE */

#if (USE_BSP_COM_FEATURE > 0)

/**
  * @brief  Initializes USART3 Msp part.
  * @param  huart UART handle
  * @retval None
  */
static void USART3_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* Enable GPIO clock */
  COM1_TX_GPIO_CLK_ENABLE();
  COM1_RX_GPIO_CLK_ENABLE();

  /* Enable USART clock */
  COM1_CLK_ENABLE();

  /* Configure USART Tx as alternate function */
  gpio_init_structure.Pin = COM1_TX_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Alternate = COM1_TX_AF;
  HAL_GPIO_Init(COM1_TX_GPIO_PORT, &gpio_init_structure);

  /* Configure USART Rx as alternate function */
  gpio_init_structure.Pin = COM1_RX_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = COM1_RX_AF;
  HAL_GPIO_Init(COM1_RX_GPIO_PORT, &gpio_init_structure);
}

/**
  * @brief  de-Initialize USART3 Msp part
  * @param  huart UART handle
  * @retval None
  */
static void USART3_MspDeInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef          gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* COM GPIO pin configuration */
  gpio_init_structure.Pin  = COM1_TX_PIN;
  HAL_GPIO_DeInit(COM1_TX_GPIO_PORT, gpio_init_structure.Pin);

  gpio_init_structure.Pin  = COM1_RX_PIN;
  HAL_GPIO_DeInit(COM1_RX_GPIO_PORT, gpio_init_structure.Pin);

  /* Disable USART clock */
  COM1_CLK_DISABLE();
}
#endif /* #if USE_BSP_COM_FEATURE */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
