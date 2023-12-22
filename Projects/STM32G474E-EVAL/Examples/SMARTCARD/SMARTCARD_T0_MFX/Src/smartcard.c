/**
  ******************************************************************************
  * @file    SMARTCARD/SMARTCARD_T0_MFX/Src/smartcard.c
  * @author  MCD Application Team
  * @brief   This file provides all the Smartcard firmware functions.
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

/** @addtogroup STM32G4xx_HAL_Examples
  * @{
  */

/** @addtogroup SMARTCARD_T0
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "smartcard.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SC_3_5V_PIN   IO_PIN_9
#define SC_RESET_PIN  IO_PIN_12
#define SC_CMDVCC_PIN IO_PIN_11
#define SC_OFF_PIN    IO_PIN_10

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables definition and initialization ----------------------------*/
/* Smartcard Interface IO pins
 * (IT related to SC_OFF configured in SC_ConfigScOffIT()) */
BSP_IO_Init_t         SC_3_5V   = {SC_3_5V_PIN,   IO_MODE_OUTPUT_PP,      IO_PULLUP};
BSP_IO_Init_t         SC_RESET  = {SC_RESET_PIN,  IO_MODE_OUTPUT_PP,      IO_PULLUP};
BSP_IO_Init_t         SC_CMDVCC = {SC_CMDVCC_PIN, IO_MODE_OUTPUT_PP,      IO_PULLUP};
BSP_IO_Init_t         SC_OFF    = {SC_OFF_PIN,    IO_MODE_IT_RISING_EDGE, IO_PULLUP};

/* Status of the smart card */
__IO uint32_t CardInserted = 0;

/* Smartcard data */
SC_ATR SC_A2R;
uint8_t SC_ATR_Table[40] = {0};
static __IO uint8_t SCData = 0;
static uint32_t F_Table[16] = {372, 372, 558, 744, 1116, 1488, 1860, 0,
                               0, 512, 768, 1024, 1536, 2048, 0, 0};
static uint32_t D_Table[8] = {0, 1, 2, 4, 8, 16, 32, 64};

/* Smartcard handle */
extern SMARTCARD_HandleTypeDef hsmartcard3;

/* Private function prototypes -----------------------------------------------*/
extern void Error_Handler(void);

/* Transport Layer -----------------------------------------------------------*/
/*----------- APDU ------------*/
static void SC_SendData(SC_ADPU_Commands *SC_ADPU, SC_ADPU_Response *SC_ResponseStatus);

/*------------ ATR ------------*/
static void SC_AnswerReq(SC_State *SCState, uint8_t *card, uint8_t length);  /* Ask ATR */
static uint8_t SC_decode_Answer2reset(uint8_t *card);  /* Decode ATR */

/* Physical Port Layer -------------------------------------------------------*/
static void SC_PowerCmd(FunctionalState NewState);
static void SC_Reset(uint32_t ResetState);
static void SC_Init(void);
static void SC_DeInit(void);
static void SC_VoltageConfig(uint32_t SC_Voltage);
static void SC_ConfigScOffIT(void);
static uint32_t SC_Detect(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Handles all Smartcard states and serves to send and receive all
  *         communication data between Smartcard and reader.
  * @param  SCState: pointer to an SC_State enumeration that will contain the
  *         Smartcard state.
  * @param  SC_ADPU: pointer to an SC_ADPU_Commands structure that will be initialized.
  * @param  SC_Response: pointer to a SC_ADPU_Response structure which will be initialized.
  * @retval None
  */
void SC_Handler(SC_State *SCState, SC_ADPU_Commands *SC_ADPU, SC_ADPU_Response *SC_Response)
{
  uint32_t i = 0;
  switch(*SCState)
  {
  case SC_POWER_ON:
    if (SC_ADPU->Header.INS == SC_GET_A2R)
    {
      /* Smartcard initialization ---------------------------------------------*/
      SC_Init();

      /* Reset Data from SC buffer -------------------------------------------*/
      for (i = 0; i < 40; i++)
      {
        SC_ATR_Table[i] = 0;
      }

      /* Reset SC_A2R Structure ----------------------------------------------*/
      SC_A2R.TS = 0;
      SC_A2R.T0 = 0;
      for (i = 0; i < SETUP_LENGTH; i++)
      {
        SC_A2R.T[i] = 0;
      }
      for (i = 0; i < HIST_LENGTH; i++)
      {
        SC_A2R.H[i] = 0;
      }
      SC_A2R.Tlength = 0;
      SC_A2R.Hlength = 0;

      /* Next State ----------------------------------------------------------*/
      *SCState = SC_RESET_LOW;
    }
    break;

  case SC_RESET_LOW:
    if(SC_ADPU->Header.INS == SC_GET_A2R)
    {
      /* If card is detected then Power ON, Card Reset and wait for an answer) */
      if (SC_Detect())
      {
        while(((*SCState) != SC_POWER_OFF) && ((*SCState) != SC_ACTIVE))
        {
          SC_AnswerReq(SCState, (uint8_t *)&SC_ATR_Table, 40); /* Check for answer to reset */
        }
      }
      else
      {
        (*SCState) = SC_POWER_OFF;
      }
    }
    break;

  case SC_ACTIVE:
    if (SC_ADPU->Header.INS == SC_GET_A2R)
    {
      if(SC_decode_Answer2reset(&SC_ATR_Table[0]) == T0_PROTOCOL)
      {
        (*SCState) = SC_ACTIVE_ON_T0;
      }
      else
      {
        (*SCState) = SC_POWER_OFF;
      }
    }
    break;

  case SC_ACTIVE_ON_T0:
    SC_SendData(SC_ADPU, SC_Response);
    break;

  case SC_POWER_OFF:
    SC_DeInit(); /* Disable Smartcard interface */
    break;

  default: (*SCState) = SC_POWER_OFF;
  }
}

/**
* @brief  Enables or disables the power to the Smartcard.
* @param  NewState: new state of the Smartcard power supply.
*         This parameter can be: ENABLE or DISABLE.
* @retval None
*/
static void SC_PowerCmd(FunctionalState NewState)
{
  BSP_IO_WritePin(0, SC_CMDVCC_PIN, (NewState!= DISABLE)?IO_PIN_RESET:IO_PIN_SET);
}

/**
  * @brief  Sets or clears the Smartcard reset pin.
  * @param  ResetState: this parameter specifies the state of the Smartcard
  *         reset pin. BitVal must be one of the GPIO_PinState enum values:
  *                 @arg IO_PIN_RESET: to clear the port pin.
  *                 @arg IO_PIN_SET: to set the port pin.
  * @retval None
  */
static void SC_Reset(uint32_t ResetState)
{
  /* RST active high */
  BSP_IO_WritePin(0, SC_RESET_PIN, (ResetState==IO_PIN_SET)?IO_PIN_SET:IO_PIN_RESET);
}

/**
  * @brief  Resends the byte that failed to be received (by the Smartcard) correctly.
  * @param  None
  * @retval None
  */
void SC_ParityErrorHandler(void)
{
  HAL_SMARTCARD_Transmit(&hsmartcard3, (uint8_t *)&SCData, 1, SC_TRANSMIT_TIMEOUT);
}

/**
  * @brief  Configures the IO speed (BaudRate) communication.
  * @param  None
  * @retval None
  */
void SC_PTSConfig(void)
{
  uint32_t workingbaudrate = 0, apbclock = 0;
  uint8_t PPSConfirmStatus = 1;
  uint8_t SC_PPSS[4] = {0};

  /* Reconfigure the USART Baud Rate -----------------------------------------*/
  apbclock = HAL_RCC_GetPCLK1Freq();
  apbclock /= ((USART3->GTPR & (uint16_t)0x00FF) * 2);

  if((SC_A2R.T0 & (uint8_t)0x10) == 0x10)
  {
    if(SC_A2R.T[0] != 0x11)
    {
      /* PPSS identifies the PPS request or response and is equal to 0xFF */
      SC_PPSS[0] = 0xFF;
      /* PPS0 indicates by the bits b5, b6, b7 equal to 1 the presence of the optional
      bytes PPSI1, PPS2, PPS3 respectively */
      SC_PPSS[1] = 0x10;   /* only send PPS1 */
      /* PPS1 allows the interface device to propose value of F and D to the card */
      SC_PPSS[2] = SC_A2R.T[0];
      /* PCK check character */
      SC_PPSS[3] = (uint8_t)0xFF^(uint8_t)0x10^(uint8_t)SC_A2R.T[0];

      if(HAL_SMARTCARD_Transmit(&hsmartcard3, (uint8_t *)SC_PPSS, 4, SC_TRANSMIT_TIMEOUT) != HAL_OK)
      {
         Error_Handler();
      }

      if(HAL_SMARTCARD_Receive(&hsmartcard3, SC_PPSS, 4, SC_RECEIVE_TIMEOUT) == HAL_OK)
      {
        if((SC_PPSS[0] != 0xFF) ||
           (SC_PPSS[1] != 0x10) ||
           (SC_PPSS[2] != SC_A2R.T[0]) ||
           (SC_PPSS[3] != ((uint8_t)0xFF^(uint8_t)0x10^(uint8_t)SC_A2R.T[0])))
        {
          /* PCK exchange unsuccessful */
          PPSConfirmStatus = 0x00;
        }
      }
      else
      {
        /* PPS exchange unsuccessful */
        PPSConfirmStatus = 0x00;
      }

      /* PPS exchange successful */
      if(PPSConfirmStatus == 0x01)
      {
        workingbaudrate = apbclock * D_Table[(SC_A2R.T[0] & (uint8_t)0x0F)];
        workingbaudrate /= F_Table[((SC_A2R.T[0] >> 4) & (uint8_t)0x0F)];

        /* Update the baud rate, the other fields were initialised in MX_USART2_SMARTCARD_Init. */
        hsmartcard3.Init.BaudRate = workingbaudrate;
        if(HAL_SMARTCARD_Init(&hsmartcard3) != HAL_OK)
        {
          Error_Handler();
        }
      }
    }
  }
}

/**
  * @brief  Manages the Smartcard transport layer: send APDU commands and receives
  *         the APDU response.
  * @param  SC_ADPU: pointer to a SC_ADPU_Commands structure which will be initialized.
  * @param  SC_Response: pointer to a SC_ADPU_Response structure which will be initialized.
  * @retval None
  */
static void SC_SendData(SC_ADPU_Commands *SC_ADPU, SC_ADPU_Response *SC_ResponseStatus)
{
  uint32_t i = 0;
  uint8_t SC_Command[5] = {0};
  uint8_t locData = 0;

  /* Reset response buffer ---------------------------------------------------*/
  for(i = 0; i < LC_MAX; i++)
  {
    SC_ResponseStatus->Data[i] = 0;
  }

  SC_ResponseStatus->SW1 = 0;
  SC_ResponseStatus->SW2 = 0;

  /* Initialize header -------------------------------------------------------*/
  SC_Command[0] = SC_ADPU->Header.CLA;
  SC_Command[1] = SC_ADPU->Header.INS;
  SC_Command[2] = SC_ADPU->Header.P1;
  SC_Command[3] = SC_ADPU->Header.P2;

  /* Initialize body length to/from SC ---------------------------------------*/
  if(SC_ADPU->Body.LC)
  {
    SC_Command[4] = SC_ADPU->Body.LC;
  }
  else if(SC_ADPU->Body.LE)
  {
    SC_Command[4] = SC_ADPU->Body.LE;
  }

  /* Send header and body length to/from SC ----------------------------------*/
  if(HAL_SMARTCARD_Transmit(&hsmartcard3, (uint8_t *)SC_Command, 5, SC_TRANSMIT_TIMEOUT) != HAL_OK)
  {
    Error_Handler();
  }

  /* Flush the SC_USART DR */
  __HAL_SMARTCARD_FLUSH_DRREGISTER(&hsmartcard3);
  /* --------------------------------------------------------
    Wait Procedure byte from card:
    1 - ACK
    2 - NULL
    3 - SW1; SW2
   -------------------------------------------------------- */
  if(HAL_SMARTCARD_Receive(&hsmartcard3, (uint8_t *)&locData, 1, SC_RECEIVE_TIMEOUT) == HAL_OK)
  {
    if(((locData & (uint8_t)0xF0) == 0x60) || ((locData & (uint8_t)0xF0) == 0x90))
    {
      /* SW1 received */
      SC_ResponseStatus->SW1 = locData;

      if(HAL_SMARTCARD_Receive(&hsmartcard3, (uint8_t *)&locData, 1, SC_RECEIVE_TIMEOUT) == HAL_OK)
      {
        /* SW2 received */
        SC_ResponseStatus->SW2 = locData;
      }
    }
    else if (((locData & (uint8_t)0xFE) == (((uint8_t)~(SC_ADPU->Header.INS)) & \
             (uint8_t)0xFE))||((locData & (uint8_t)0xFE) == (SC_ADPU->Header.INS & (uint8_t)0xFE)))
    {
      SC_ResponseStatus->Data[0] = locData;/* ACK received */
    }
  }
  else
  {
    Error_Handler();
  }

  /* If no status bytes received ---------------------------------------------*/
  if(SC_ResponseStatus->SW1 == 0x00)
  {
    /* Send body data to SC---------------------------------------------------*/
    if (SC_ADPU->Body.LC)
    {
      if(HAL_SMARTCARD_Transmit(&hsmartcard3, (uint8_t *)SC_ADPU->Body.Data, SC_ADPU->Body.LC, SC_TRANSMIT_TIMEOUT) != HAL_OK)
      {
        Error_Handler();
      }
      /* Flush the SC_USART DR */
      __HAL_SMARTCARD_FLUSH_DRREGISTER(&hsmartcard3);
    }

    /* Or receive body data from SC ------------------------------------------*/
    else if (SC_ADPU->Body.LE)
    {
      if(HAL_SMARTCARD_Receive(&hsmartcard3, (uint8_t *)SC_ResponseStatus->Data, SC_ADPU->Body.LE, SC_RECEIVE_TIMEOUT) != HAL_OK)
      {
        Error_Handler();
      }
    }
    /* Wait SW1 --------------------------------------------------------------*/
    i = 0;
    while(i < 10)
    {
      if(HAL_SMARTCARD_Receive(&hsmartcard3, (uint8_t *)&locData, 1, SC_RECEIVE_TIMEOUT) == HAL_OK)
      {
        SC_ResponseStatus->SW1 = locData;
        i = 11;
      }
      else
      {
        i++;
      }
    }
    /* Wait SW2 --------------------------------------------------------------*/
    i = 0;
    while(i < 10)
    {
      if(HAL_SMARTCARD_Receive(&hsmartcard3, (uint8_t *)&locData, 1, SC_RECEIVE_TIMEOUT) == HAL_OK)
      {
        SC_ResponseStatus->SW2 = locData;
        i = 11;
      }
      else
      {
        i++;
      }
    }
  }
}

/**
  * @brief  Requests the reset answer from card.
  * @param  SCState: pointer to an SC_State enumeration that will contain the Smartcard state.
  * @param  card: pointer to a buffer which will contain the card ATR.
  * @param  length: maximum ATR length
  * @retval None
  */
static void SC_AnswerReq(SC_State *SCState, uint8_t *card, uint8_t length)
{
  switch(*SCState)
  {
  case SC_RESET_LOW:
    /* Check response with reset low -----------------------------------------*/
    HAL_SMARTCARD_Receive(&hsmartcard3, card, length, SC_RECEIVE_TIMEOUT);

    if(card[0] != 0x00)
    {
      (*SCState) = SC_ACTIVE;
    }
    else
    {
      (*SCState) = SC_RESET_HIGH;
    }
    break;

  case SC_RESET_HIGH:
    /* Check response with reset high ----------------------------------------*/
    SC_Reset(IO_PIN_SET); /* Reset High */

    HAL_SMARTCARD_Receive(&hsmartcard3, card, length, SC_RECEIVE_TIMEOUT);

    if(card[0])
    {
      (*SCState) = SC_ACTIVE;
    }
    else
    {
      (*SCState) = SC_POWER_OFF;
    }
    break;

  case SC_ACTIVE:
    break;

  case SC_POWER_OFF:
    /* Close Connection if no answer received --------------------------------*/
    SC_Reset(IO_PIN_SET); /* Reset high - a bit is used as level shifter from 3.3 to 5 V */
    SC_PowerCmd(DISABLE);
    break;

  default:
    (*SCState) = SC_RESET_LOW;
    break;
  }
}

/**
  * @brief  Decodes the Answer to reset received from card.
  * @param  card: pointer to the buffer containing the card ATR.
  * @retval None
  */
static uint8_t SC_decode_Answer2reset(uint8_t *card)
{
 uint32_t i = 0, flag = 0, buf = 0, protocol = 0;

  SC_A2R.TS = card[0];  /* Initial character */
  SC_A2R.T0 = card[1];  /* Format character */

  SC_A2R.Hlength = SC_A2R.T0 & (uint8_t)0x0F;

  if ((SC_A2R.T0 & (uint8_t)0x80) == 0x80)
  {
    flag = 1;
  }

  for (i = 0; i < 4; i++)
  {
    SC_A2R.Tlength = SC_A2R.Tlength + (((SC_A2R.T0 & (uint8_t)0xF0) >> (4 + i)) & (uint8_t)0x1);
  }

  for (i = 0; i < SC_A2R.Tlength; i++)
  {
    SC_A2R.T[i] = card[i + 2];
  }

  protocol = SC_A2R.T[SC_A2R.Tlength - 1] & (uint8_t)0x0F;

  while (flag)
  {
    if ((SC_A2R.T[SC_A2R.Tlength - 1] & (uint8_t)0x80) == 0x80)
    {
      flag = 1;
    }
    else
    {
      flag = 0;
    }

    buf = SC_A2R.Tlength;
    SC_A2R.Tlength = 0;

    for (i = 0; i < 4; i++)
    {
      SC_A2R.Tlength = SC_A2R.Tlength + (((SC_A2R.T[buf - 1] & (uint8_t)0xF0) >> (4 + i)) & (uint8_t)0x1);
    }

    for (i = 0;i < SC_A2R.Tlength; i++)
    {
      SC_A2R.T[buf + i] = card[i + 2 + buf];
    }
    SC_A2R.Tlength += (uint8_t)buf;
  }

  for (i = 0; i < SC_A2R.Hlength; i++)
  {
    SC_A2R.H[i] = card[i + 2 + SC_A2R.Tlength];
  }

  return (uint8_t)protocol;
}

/**
  * @brief  Initializes all peripheral used for Smartcard interface.
  * @param  None
  * @retval None
  */
static void SC_Init(void)
{
  /* SC_USART configuration --------------------------------------------------*/
  /* SC_USART configured as follow:
  - Word Length = 9 Bits
  - 1.5 Stop Bit
  - Even parity
  - BaudRate = 10752 baud
  - Hardware flow control disabled (RTS and CTS signals)
  - Tx and Rx enabled
  - USART Clock enabled
  */
  hsmartcard3.Init.BaudRate = 10752;
  if(HAL_SMARTCARD_Init(&hsmartcard3) != HAL_OK)
  {
    Error_Handler();
  }

  /* Set RSTIN HIGH */
  SC_Reset(IO_PIN_SET);

  /* Select 5V */
  SC_VoltageConfig(SC_VOLTAGE_5V);
  HAL_Delay(1);

  /* Enable CMDVCC */
  SC_PowerCmd(ENABLE);

  /* Set RSTIN LOW */
  HAL_Delay(1);
  SC_Reset(IO_PIN_RESET);
}

/**
  * @brief  Deinitializes all resources used by the Smartcard interface.
  * @param  None
  * @retval None
  */
static void SC_DeInit(void)
{
  /* Disable CMDVCC */
  SC_PowerCmd(DISABLE);

  /* Deinitializes hsmartcard3 */
  HAL_SMARTCARD_DeInit(&hsmartcard3);
}

/**
  * @brief  Configures the card power voltage.
  * @param  SC_Voltage: specifies the card power voltage.
  *         This parameter can be one of the following values:
  *              @arg SC_VOLTAGE_5V: 5V cards.
  *              @arg SC_VOLTAGE_3V: 3V cards.
  * @retval None
  */
static void SC_VoltageConfig(uint32_t SC_Voltage)
{
  if(SC_Voltage == SC_VOLTAGE_5V)
  {
    /* Select Smartcard 5V */
    BSP_IO_WritePin(0, SC_3_5V_PIN, IO_PIN_SET);
  }
  else
  {
    /* Select Smartcard 3V */
    BSP_IO_WritePin(0, SC_3_5V_PIN, IO_PIN_RESET);
  }
}

/**
  * @brief  Configures GPIO hardware resources used for Smartcard.
  * @param  None
  * @retval None
  */
void SC_IOConfig(void)
{
  /* USART GPIOs were configured by MX_GPIO_Init. */

  /* Configure Smartcard 3/5V pin */
  BSP_IO_Init(0, &SC_3_5V);
  /* Configure Smartcard CMDVCC pin */
  BSP_IO_Init(0, &SC_CMDVCC);
  /* Configure Smartcard RESET pin */
  BSP_IO_Init(0, &SC_RESET);
  /* Configure Smartcard OFF presence pin */
  BSP_IO_Init(0, &SC_OFF);
  SC_ConfigScOffIT();

  /* Initialize output pins */
  BSP_IO_WritePin(0, SC_CMDVCC_PIN, IO_PIN_SET);
  BSP_IO_WritePin(0, SC_RESET_PIN,  IO_PIN_SET);

  /* Initialize CardInserted */
  if(BSP_IO_ReadPin(0, SC_OFF_PIN) == IO_PIN_SET)
  {
    /* Toggle LED2..4 */
    BSP_LED_Toggle(LED2);
    BSP_LED_Toggle(LED3);
    BSP_LED_Toggle(LED4);

    CardInserted = 1;
  }
  else
  {
    CardInserted = 0;
  }
}

/**
  * @brief  Configures Interrupt mode for the smart card OFF pin.
  * @param  None
  * @retval None
  */
static void SC_ConfigScOffIT(void)
{
  /* Configure IO interrupt acquisition mode */
  if(BSP_IO_ReadPin(0, SC_OFF_PIN) == IO_PIN_RESET)
  {
    SC_OFF.Mode = IO_MODE_IT_RISING_EDGE;
  }
  else
  {
    SC_OFF.Mode = IO_MODE_IT_FALLING_EDGE;
  }
  BSP_IO_Init(0, &SC_OFF);
}

/**
  * @brief  Detects whether the Smartcard is present or not.
  * @param  None.
  * @retval 0 - Smartcard not inserted
  *         1 - Smartcard inserted
  */
static uint32_t SC_Detect(void)
{
  return (BSP_IO_ReadPin(0, SC_OFF_PIN) == IO_PIN_RESET)?0:1;
}

/**
  * @brief MFX EXTI line detection callbacks
  * @param None
  * @retval None
  */
void SC_MFX_EXTI_Callback(void)
{
  if(BSP_IO_GetIT(0, SC_OFF_PIN) == (int32_t)IO_PIN_IT_SET)
  {
    /* Clear IT status of SC_OFF_PIN */
    if( BSP_IO_ClearIT(0, SC_OFF_PIN) < 0)
    {
      /* Nothing to do */
    }

    /* Toggle LED2..4 */
    BSP_LED_Toggle(LED2);
    BSP_LED_Toggle(LED3);
    BSP_LED_Toggle(LED4);

    if (CardInserted == 0)
    {
      /* Smartcard detected */
      CardInserted = 1;

      /* Reset the card */
      SC_Reset(GPIO_PIN_RESET);
    }
    else
    {
      /* Smartcard removed */
      CardInserted = 0;

      /* Enable CMDVCC */
      SC_PowerCmd(DISABLE);
    }

    /* Update the configuration of SC_OFF_PIN */
    SC_ConfigScOffIT();
  }
}

/**
  * @brief Indicate the status of the smart card: inserted or not
  * @param None
  * @retval None
  */
uint32_t SC_IsCardInserted(void)
{
  return CardInserted;
}

/**
  * @}
  */

/**
  * @}
  */


