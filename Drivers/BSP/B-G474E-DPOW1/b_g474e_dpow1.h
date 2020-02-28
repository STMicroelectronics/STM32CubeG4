/**
  ******************************************************************************
  * @file    b_g474e_dpow1.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for B-G474E-DPOW1:
  *          LEDs
  *          Joystick
  *          Hardware resources.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef B_G474E_DPOW1_H
#define B_G474E_DPOW1_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "b_g474e_dpow1_conf.h"
#include "b_g474e_dpow1_errno.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup B-G474E-DPOW1
  * @{
  */

/** @addtogroup B-G474E-DPOW1_Common
  * @{
  */

/** @defgroup B-G474E-DPOW1_Exported_Types Exported Types
  * @{
  */
typedef void (* BSP_EXTI_LineCallback) (void);

/**
  * @brief Define for B-G474E-DPOW1 board
  */

/**
 * @brief B-G474E-DPOW1 BSP Driver version number V1.1.1
   */
#define	B_G474E_DPOW1_BSP_VERSION_MAIN   (uint32_t)(0x01) /*!< [31:24] main version */
#define	B_G474E_DPOW1_BSP_VERSION_SUB1   (uint32_t)(0x01) /*!< [23:16] sub1 version */
#define	B_G474E_DPOW1_BSP_VERSION_SUB2   (uint32_t)(0x01) /*!< [15:8]  sub2 version */
#define	B_G474E_DPOW1_BSP_VERSION_RC     (uint32_t)(0x00) /*!< [7:0]  release candidate */
#define	B_G474E_DPOW1_BSP_VERSION        ((B_G474E_DPOW1_BSP_VERSION_MAIN << 24)\
                                         |(B_G474E_DPOW1_BSP_VERSION_SUB1 << 16)\
                                         |(B_G474E_DPOW1_BSP_VERSION_SUB2 << 8 )\
                                         |(B_G474E_DPOW1_BSP_VERSION_RC))

#if !defined (B_G474E_DPOW1_BSP)
 #define B_G474E_DPOW1_BSP
#endif
#ifndef USE_BSP_COM_FEATURE
   #define USE_BSP_COM_FEATURE                  0U
#endif

#ifndef USE_COM_LOG
  #define USE_COM_LOG                           0U
#endif

#ifndef USE_BSP_JOY_FEATURE
   #define USE_BSP_JOY_FEATURE                  1U
#endif

#define B_G474E_DPOW1_BSP_BOARD_NAME  "B-G474E-DPOW1"
#define B_G474E_DPOW1_BSP_BOARD_ID    "MB1428B"

typedef enum
{
  LED2 = 0U,
  LED_BLUE = LED2,
  LED3 = 1U,
  LED_ORANGE = LED3,
  LED4 = 2U,
  LED_GREEN = LED4,
  LED5 = 3U,
  LED_RED = LED5,
}Led_TypeDef;

typedef enum
{
  BUTTON_USER = 0U,
}Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0U,
  BUTTON_MODE_EXTI = 1U
}ButtonMode_TypeDef;

#if (USE_BSP_COM_FEATURE > 0)
typedef enum
{
  COM1 = 0U,
  COMn,
  }COM_TypeDef;

typedef enum
{
  COM_STOPBITS_1     =   UART_STOPBITS_1,
  COM_STOPBITS_2     =   UART_STOPBITS_2,
}COM_StopBitsTypeDef;

typedef enum
{
  COM_PARITY_NONE     =  UART_PARITY_NONE,
  COM_PARITY_EVEN     =  UART_PARITY_EVEN,
  COM_PARITY_ODD      =  UART_PARITY_ODD,
}COM_ParityTypeDef;

typedef enum
{
  COM_HWCONTROL_NONE    =  UART_HWCONTROL_NONE,
  COM_HWCONTROL_RTS     =  UART_HWCONTROL_RTS,
  COM_HWCONTROL_CTS     =  UART_HWCONTROL_CTS,
  COM_HWCONTROL_RTS_CTS =  UART_HWCONTROL_RTS_CTS,
}COM_HwFlowCtlTypeDef;

typedef struct
{
  uint32_t             BaudRate;
  uint32_t             WordLength;
  COM_StopBitsTypeDef  StopBits;
  COM_ParityTypeDef    Parity;
  COM_HwFlowCtlTypeDef HwFlowCtl;
}COM_InitTypeDef;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
typedef struct
{
  void (* pMspInitCb)(UART_HandleTypeDef *);
  void (* pMspDeInitCb)(UART_HandleTypeDef *);
}BSP_COM_Cb_t;
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 1) */
#endif /* (USE_BSP_COM_FEATURE > 0) */

#if (USE_BSP_JOY_FEATURE > 0)
typedef enum
{
  JOY_NONE  = 0x00,
  JOY_SEL   = 0x01,
  JOY_DOWN  = 0x02,
  JOY_LEFT  = 0x04,
  JOY_RIGHT = 0x08,
  JOY_UP    = 0x10,
  JOY_ALL   = 0x1F,
} JOYPin_TypeDef;

typedef enum
{
  JOY_MODE_GPIO = 0U,
  JOY_MODE_EXTI = 1U
}JOYMode_TypeDef;

typedef enum
{
  JOY1 = 0U,
  JOYn,
}JOY_TypeDef;
#endif/* USE_BSP_JOY_FEATURE */

/**
  * @}
  */

/** @defgroup B-G474E-DPOW1_Exported_Constants Exported Constants
  * @{
  */

/** @defgroup B-G474E-DPOW1_LED LED Constants
  * @{
  */
#define LEDn                             4U

#define LED5_GPIO_PORT                   GPIOB
#define LED5_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED5_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()
#define LED5_PIN                         GPIO_PIN_5

#define LED3_GPIO_PORT                   GPIOB
#define LED3_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED3_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()
#define LED3_PIN                         GPIO_PIN_1

#define LED4_GPIO_PORT                   GPIOB
#define LED4_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED4_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()
#define LED4_PIN                         GPIO_PIN_7

#define LED2_GPIO_PORT                   GPIOA
#define LED2_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()
#define LED2_PIN                         GPIO_PIN_15
/**
  * @}
  */

#if (USE_BSP_COM_FEATURE > 0)
/** @defgroup B-G474E-DPOW1_COM COM Constants
  * @{
  */
/**
 * @brief Definition for COM port, connected to USART3
 */
#define COM1_UART                     USART3
#define COM1_CLK_ENABLE()             __HAL_RCC_USART3_CLK_ENABLE()
#define COM1_CLK_DISABLE()            __HAL_RCC_USART3_CLK_DISABLE()

#define COM1_TX_PIN                   GPIO_PIN_10
#define COM1_TX_GPIO_PORT             GPIOC
#define COM1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define COM1_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOC_CLK_DISABLE()
#define COM1_TX_AF                    GPIO_AF7_USART3

#define COM1_RX_PIN                   GPIO_PIN_11
#define COM1_RX_GPIO_PORT             GPIOC
#define COM1_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define COM1_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOC_CLK_DISABLE()
#define COM1_RX_AF                    GPIO_AF7_USART3
#define COM1_POLL_TIMEOUT              1000

#define MX_UART_InitTypeDef           COM_InitTypeDef
/**
  * @}
  */
#endif /* #if (USE_BSP_COM_FEATURE > 0) */

#if (USE_BSP_JOY_FEATURE > 0)
/** @defgroup B-G474E-DPOW1_JOYSTICK JOYSTICK Constants
  * @{
  */
#define JOYn                             1U
#define JOY_KEY_NUMBER                   5U
#define JOY_SEL_NUMBER                   0U
#define JOY_DOWN_NUMBER                  1U
#define JOY_LEFT_NUMBER                  2U
#define JOY_RIGHT_NUMBER                 3U
#define JOY_UP_NUMBER                    4U


#define SEL_JOY_PIN                      GPIO_PIN_13
#define SEL_JOY_GPIO_PORT                GPIOC
#define SEL_JOY_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define SEL_JOY_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOC_CLK_DISABLE()
#define SEL_JOY_EXTI_IRQn                EXTI15_10_IRQn
#define SEL_JOY_EXTI_LINE                EXTI_LINE_13

#define DOWN_JOY_PIN                     GPIO_PIN_5
#define DOWN_JOY_GPIO_PORT               GPIOC
#define DOWN_JOY_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define DOWN_JOY_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOC_CLK_DISABLE()
#define DOWN_JOY_EXTI_IRQn               EXTI9_5_IRQn
#define DOWN_JOY_EXTI_LINE               EXTI_LINE_5

#define LEFT_JOY_PIN                     GPIO_PIN_4
#define LEFT_JOY_GPIO_PORT               GPIOC
#define LEFT_JOY_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define LEFT_JOY_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOC_CLK_DISABLE()
#define LEFT_JOY_EXTI_IRQn               EXTI4_IRQn
#define LEFT_JOY_EXTI_LINE               EXTI_LINE_4

#define RIGHT_JOY_PIN                    GPIO_PIN_2
#define RIGHT_JOY_GPIO_PORT              GPIOB
#define RIGHT_JOY_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define RIGHT_JOY_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOB_CLK_DISABLE()
#define RIGHT_JOY_EXTI_IRQn              EXTI2_IRQn
#define RIGHT_JOY_EXTI_LINE              EXTI_LINE_2

#define UP_JOY_PIN                       GPIO_PIN_10
#define UP_JOY_GPIO_PORT                 GPIOB
#define UP_JOY_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define UP_JOY_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()
#define UP_JOY_EXTI_IRQn                 EXTI15_10_IRQn
#define UP_JOY_EXTI_LINE                 EXTI_LINE_10

#define JOY1_GPIO_CLK_ENABLE(__JOY__)    do { if((__JOY__) == JOY_SEL) SEL_JOY_GPIO_CLK_ENABLE(); else \
                                          if((__JOY__) == JOY_DOWN) DOWN_JOY_GPIO_CLK_ENABLE(); else \
                                          if((__JOY__) == JOY_LEFT) LEFT_JOY_GPIO_CLK_ENABLE(); else \
                                          if((__JOY__) == JOY_RIGHT) RIGHT_JOY_GPIO_CLK_ENABLE(); else \
                                          if((__JOY__) == JOY_UP) UP_JOY_GPIO_CLK_ENABLE();} while(0)

#define JOY1_GPIO_CLK_DISABLE(__JOY__)   (((__JOY__) == JOY_SEL) ? SEL_JOY_GPIO_CLK_DISABLE() :\
                                          ((__JOY__) == JOY_DOWN) ? DOWN_JOY_GPIO_CLK_DISABLE() :\
                                          ((__JOY__) == JOY_LEFT) ? LEFT_JOY_GPIO_CLK_DISABLE() :\
                                          ((__JOY__) == JOY_RIGHT) ? RIGHT_JOY_GPIO_CLK_DISABLE() :\
                                          ((__JOY__) == JOY_UP) ? UP_JOY_GPIO_CLK_DISABLE() : 0 )
/**
  * @}
  */
#endif /* USE_BSP_JOY_FEATURE */

/**
  * @}
  */

/** @addtogroup B-G474E-DPOW1_Exported_Variables
  * @{
  */
#if (USE_BSP_JOY_FEATURE > 0)
extern EXTI_HandleTypeDef hjoy_exti[JOY_KEY_NUMBER];
#endif /* USE_BSP_JOY_FEATURE > 0 */

#if (USE_BSP_COM_FEATURE > 0)
extern UART_HandleTypeDef hcom_uart[COMn];
#endif /* #if (USE_BSP_COM_FEATURE > 0) */

/**
  * @}
  */

/** @defgroup B-G474E-DPOW1_Exported_Functions Exported Functions
  * @{
  */
int32_t  BSP_GetVersion(void);
const uint8_t* BSP_GetBoardName(void);
const uint8_t* BSP_GetBoardID(void);

/* Legacy APIs aliases */
#define BSP_GetHWBoardVersionName    BSP_GetBoardName
#define BSP_GetPDTypeName            BSP_GetBoardID

int32_t  BSP_LED_Init(Led_TypeDef Led);
int32_t  BSP_LED_DeInit(Led_TypeDef Led);
int32_t  BSP_LED_On(Led_TypeDef Led);
int32_t  BSP_LED_Off(Led_TypeDef Led);
int32_t  BSP_LED_Toggle(Led_TypeDef Led);
int32_t  BSP_LED_GetState(Led_TypeDef Led);
#if (USE_BSP_COM_FEATURE > 0)
int32_t  BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init);
int32_t  BSP_COM_DeInit(COM_TypeDef COM);
#if( USE_COM_LOG > 0)
int32_t  BSP_COM_SelectLogPort (COM_TypeDef COM);
#endif /* USE_COM_LOG */
HAL_StatusTypeDef MX_USART3_Init(UART_HandleTypeDef *huart, COM_InitTypeDef *COM_Init);

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM);
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *Callback);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
#endif /* USE_BSP_COM_FEATURE */

#if (USE_BSP_JOY_FEATURE > 0)
int32_t  BSP_JOY_Init(JOY_TypeDef JOY, JOYMode_TypeDef JoyMode, JOYPin_TypeDef JoyPins);
int32_t  BSP_JOY_DeInit(JOY_TypeDef JOY, JOYPin_TypeDef JoyPins);
int32_t  BSP_JOY_GetState(JOY_TypeDef JOY);
void     BSP_JOY_Callback(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin);
#endif /* USE_BSP_JOY_FEATURE */
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

#ifdef __cplusplus
}
#endif
#endif /* B_G474E_DPOW1_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
