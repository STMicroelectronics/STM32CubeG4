/**
  ******************************************************************************
  * @file    tracer_emb_hw.c
  * @author  MCD Application Team
  * @brief   This file contains the low level interface to control trace system.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
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
#include "tracer_emb.h"
#include "tracer_emb_hw.h"

#if TRACER_EMB_DMA_MODE == 1UL && TRACER_EMB_IT_MODE == 1UL
#error "tracer emb hw please select DMA or IT mode"
#endif /* TRACER_EMB_DMA_MODE == 1 && TRACER_EMB_IT_MODE == 1 */

/* Private typedef -----------------------------------------------------------*/
/* Private Defines -----------------------------------------------------------*/
#if (TRACER_EMB_IS_INSTANCE_LPUART_TYPE == 1UL)

/* LPUART entry point */
#define TRACER_EMB_ENABLEDIRECTIONRX    LL_LPUART_EnableDirectionRx
#define TRACER_EMB_RECEIVE_DATA8        LL_LPUART_ReceiveData8
#define TRACER_EMB_TRANSMIT_DATA8       LL_LPUART_TransmitData8
#define TRACER_EMB_DMA_GETREGADDR       LL_LPUART_DMA_GetRegAddr
#define TRACER_EMB_ENABLEDMAREQ_TX      LL_LPUART_EnableDMAReq_TX

#define TRACER_EMB_ENABLE_IT_RXNE       LL_LPUART_EnableIT_RXNE
#define TRACER_EMB_ENABLE_IT_ERROR      LL_LPUART_EnableIT_ERROR
#define TRACER_EMB_ENABLE_IT_TXE        LL_LPUART_EnableIT_TXE
#define TRACER_EMB_ENABLE_IT_TC         LL_LPUART_EnableIT_TC

#define TRACER_EMB_DISABLEIT_TXE        LL_LPUART_DisableIT_TXE
#define TRACER_EMB_DISABLEIT_TC         LL_LPUART_DisableIT_TC

#define TRACER_EMB_ISACTIVEFLAG_TXE     LL_LPUART_IsActiveFlag_TXE
#define TRACER_EMB_ISACTIVEFLAG_PE      LL_LPUART_IsActiveFlag_PE
#define TRACER_EMB_ISACTIVEFLAG_RXNE    LL_LPUART_IsActiveFlag_RXNE
#define TRACER_EMB_ISACTIVEFLAG_FE      LL_LPUART_IsActiveFlag_FE
#define TRACER_EMB_ISACTIVEFLAG_ORE     LL_LPUART_IsActiveFlag_ORE
#define TRACER_EMB_ISACTIVEFLAG_NE      LL_LPUART_IsActiveFlag_NE
#define TRACER_EMB_ISACTIVEFLAG_TC      LL_LPUART_IsActiveFlag_TC

#define TRACER_EMB_ISENABLEDIT_TXE      LL_LPUART_IsEnabledIT_TXE
#define TRACER_EMB_ISENABLEDIT_RXNE     LL_LPUART_IsEnabledIT_RXNE
#define TRACER_EMB_ISENABLEDIT_PE       LL_LPUART_IsEnabledIT_PE
#define TRACER_EMB_ISENABLEDIT_ERROR    LL_LPUART_IsEnabledIT_ERROR
#define TRACER_EMB_ISENABLEDIT_TC       LL_LPUART_IsEnabledIT_TC

#define TRACER_EMB_CLEARFLAG_PE         LL_LPUART_ClearFlag_PE
#define TRACER_EMB_CLEARFLAG_FE         LL_LPUART_ClearFlag_FE
#define TRACER_EMB_CLEARFLAG_ORE        LL_LPUART_ClearFlag_ORE
#define TRACER_EMB_CLEARFLAG_NE         LL_LPUART_ClearFlag_NE
#define TRACER_EMB_CLEARFLAG_TC         LL_LPUART_ClearFlag_TC

#define TRACER_EMB_READREG              LL_LPUART_ReadReg

#else

/* UART entry point */
#define TRACER_EMB_ENABLEDIRECTIONRX    LL_USART_EnableDirectionRx
#define TRACER_EMB_RECEIVE_DATA8        LL_USART_ReceiveData8
#define TRACER_EMB_TRANSMIT_DATA8       LL_USART_TransmitData8
#define TRACER_EMB_DMA_GETREGADDR       LL_USART_DMA_GetRegAddr
#define TRACER_EMB_ENABLEDMAREQ_TX      LL_USART_EnableDMAReq_TX

#define TRACER_EMB_ENABLE_IT_RXNE       LL_USART_EnableIT_RXNE
#define TRACER_EMB_ENABLE_IT_ERROR      LL_USART_EnableIT_ERROR
#define TRACER_EMB_ENABLE_IT_TXE        LL_USART_EnableIT_TXE
#define TRACER_EMB_ENABLE_IT_TC         LL_USART_EnableIT_TC

#define TRACER_EMB_DISABLEIT_TXE        LL_USART_DisableIT_TXE
#define TRACER_EMB_DISABLEIT_TC         LL_USART_DisableIT_TC

#define TRACER_EMB_ISACTIVEFLAG_TXE     LL_USART_IsActiveFlag_TXE
#define TRACER_EMB_ISACTIVEFLAG_PE      LL_USART_IsActiveFlag_PE
#define TRACER_EMB_ISACTIVEFLAG_RXNE    LL_USART_IsActiveFlag_RXNE
#define TRACER_EMB_ISACTIVEFLAG_FE      LL_USART_IsActiveFlag_FE
#define TRACER_EMB_ISACTIVEFLAG_ORE     LL_USART_IsActiveFlag_ORE
#define TRACER_EMB_ISACTIVEFLAG_NE      LL_USART_IsActiveFlag_NE
#define TRACER_EMB_ISACTIVEFLAG_TC      LL_USART_IsActiveFlag_TC
#define TRACER_EMB_ISACTIVEFLAG_RTO     LL_USART_IsActiveFlag_RTO

#define TRACER_EMB_ISENABLEDIT_TXE      LL_USART_IsEnabledIT_TXE
#define TRACER_EMB_ISENABLEDIT_RXNE     LL_USART_IsEnabledIT_RXNE
#define TRACER_EMB_ISENABLEDIT_PE       LL_USART_IsEnabledIT_PE
#define TRACER_EMB_ISENABLEDIT_ERROR    LL_USART_IsEnabledIT_ERROR
#define TRACER_EMB_ISENABLEDIT_TC       LL_USART_IsEnabledIT_TC
#define TRACER_EMB_ISENABLEDIT_RTO      LL_USART_IsEnabledIT_RTO

#define TRACER_EMB_CLEARFLAG_PE         LL_USART_ClearFlag_PE
#define TRACER_EMB_CLEARFLAG_FE         LL_USART_ClearFlag_FE
#define TRACER_EMB_CLEARFLAG_ORE        LL_USART_ClearFlag_ORE
#define TRACER_EMB_CLEARFLAG_NE         LL_USART_ClearFlag_NE
#define TRACER_EMB_CLEARFLAG_TC         LL_USART_ClearFlag_TC
#define TRACER_EMB_CLEARFLAG_RTO        LL_USART_ClearFlag_RTO

#define TRACER_EMB_READREG              LL_USART_ReadReg

#endif /* TRACER_EMB_IS_INSTANCE_LPUART_TYPE == 1 */

#if defined(USART_ISR_RXNE_RXFNE)
#define TRACER_EMB_FLAG_RXNE            USART_ISR_RXNE_RXFNE
#elif defined(USART_ISR_RXNE)
#define TRACER_EMB_FLAG_RXNE            USART_ISR_RXNE
#else
#define TRACER_EMB_FLAG_RXNE            USART_SR_RXNE
#endif  /* USART_ISR_RXNE_RXFNE */
#if defined(USART_CR1_TXEIE_TXFNFIE)
#define TRACER_EMB_FLAG_TXE             USART_CR1_TXEIE_TXFNFIE
#elif defined(USART_ISR_TXE)
#define TRACER_EMB_FLAG_TXE             USART_ISR_TXE
#else
#define TRACER_EMB_FLAG_TXE             USART_SR_TXE
#endif  /* USART_CR1_TXEIE_TXFNFIE */
#if defined(USART_ISR_PE)
#define TRACER_EMB_FLAG_PE              USART_ISR_PE
#define TRACER_EMB_FLAG_FE              USART_ISR_FE
#define TRACER_EMB_FLAG_ORE             USART_ISR_ORE
#define TRACER_EMB_FLAG_NE              USART_ISR_NE
#define TRACER_EMB_FLAG_TC              USART_ISR_TC
#else
#define TRACER_EMB_FLAG_PE              USART_SR_PE
#define TRACER_EMB_FLAG_FE              USART_SR_FE
#define TRACER_EMB_FLAG_ORE             USART_SR_ORE
#define TRACER_EMB_FLAG_NE              USART_SR_NE
#define TRACER_EMB_FLAG_TC              USART_SR_TC
#endif  /* USART_ISR_RXNE_RXFNE */

/* Private Variables ---------------------------------------------------------*/
static void (*fptr_rx)(uint8_t, uint8_t) = NULL;
#if TRACER_EMB_IT_MODE == 1UL
uint8_t *txData = NULL;
uint32_t txSize = 0;
#endif /* TRACER_EMB_IT_MODE == 1 */

/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Trace init
  * @param  callbackTX
  * @param  callbackRX
  * @retval none
  */
void HW_TRACER_EMB_Init(void)
{
  /* Enable the peripheral clock of GPIO Port */
  TRACER_EMB_TX_GPIO_ENABLE_CLOCK();
  TRACER_EMB_RX_GPIO_ENABLE_CLOCK();

  /* Configure Tx Pin as : Alternate function, High Speed, Push pull, Pull up */
  TRACER_EMB_TX_AF_FUNCTION(TRACER_EMB_TX_GPIO, TRACER_EMB_TX_PIN, TRACER_EMB_TX_AF);
  LL_GPIO_SetPinMode(TRACER_EMB_TX_GPIO, TRACER_EMB_TX_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(TRACER_EMB_TX_GPIO, TRACER_EMB_TX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(TRACER_EMB_TX_GPIO, TRACER_EMB_TX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(TRACER_EMB_TX_GPIO, TRACER_EMB_TX_PIN, LL_GPIO_PULL_UP);

  /* Configure Rx Pin as : Alternate function, High Speed, Push pull, Pull up */
  TRACER_EMB_RX_AF_FUNCTION(TRACER_EMB_RX_GPIO, TRACER_EMB_RX_PIN, TRACER_EMB_RX_AF);
  LL_GPIO_SetPinMode(TRACER_EMB_RX_GPIO, TRACER_EMB_RX_PIN, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(TRACER_EMB_RX_GPIO, TRACER_EMB_RX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(TRACER_EMB_RX_GPIO, TRACER_EMB_RX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(TRACER_EMB_RX_GPIO, TRACER_EMB_RX_PIN, LL_GPIO_PULL_UP);

  /* Enable the peripheral clock for USART */
  TRACER_EMB_ENABLE_CLK_USART();

  /* Set clock source */
  TRACER_EMB_SET_CLK_SOURCE_USART();


  if (IS_USART_INSTANCE(TRACER_EMB_USART_INSTANCE))
  {
    /* Configure USART */

    LL_USART_InitTypeDef usart_initstruct;

    /* Disable USART prior modifying configuration registers */
    LL_USART_Disable(TRACER_EMB_USART_INSTANCE);


    /* Set fields of initialization structure                   */
    /*  - Prescaler           : LL_USART_PRESCALER_DIV1         */
    /*  - BaudRate            : TRACE_BAUDRATE                  */
    /*  - DataWidth           : LL_USART_DATAWIDTH_8B           */
    /*  - StopBits            : LL_USART_STOPBITS_1             */
    /*  - Parity              : LL_USART_PARITY_NONE            */
    /*  - TransferDirection   : LL_USART_DIRECTION_TX           */
    /*  - HardwareFlowControl : LL_USART_HWCONTROL_NONE         */
    /*  - OverSampling        : LL_USART_OVERSAMPLING_16        */
#if defined(USART_PRESC_PRESCALER)
    usart_initstruct.PrescalerValue      = LL_USART_PRESCALER_DIV1;
#endif /* USART_PRESC_PRESCALER */
    usart_initstruct.BaudRate            = TRACER_EMB_BAUDRATE;
    usart_initstruct.DataWidth           = LL_USART_DATAWIDTH_8B;
    usart_initstruct.StopBits            = LL_USART_STOPBITS_1;
    usart_initstruct.Parity              = LL_USART_PARITY_NONE;
    usart_initstruct.TransferDirection   = LL_USART_DIRECTION_TX;
    usart_initstruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    usart_initstruct.OverSampling        = LL_USART_OVERSAMPLING_16;

    /* Initialize USART instance according to parameters defined in initialization structure */
    (void)LL_USART_Init(TRACER_EMB_USART_INSTANCE, &usart_initstruct);

    LL_USART_Enable(TRACER_EMB_USART_INSTANCE);

#if defined(USART_ISR_TEACK)
    uint32_t _temp1 = LL_USART_IsActiveFlag_TEACK(TRACER_EMB_USART_INSTANCE);
    /* Polling USART initialisation */
    while (_temp1 == 0U)
    {
      _temp1 = LL_USART_IsActiveFlag_TEACK(TRACER_EMB_USART_INSTANCE);
    }
#endif /* USART_ISR_TEACK */
  }
#if (TRACER_EMB_IS_INSTANCE_LPUART_TYPE == 1UL)
  else
  {
    /* Configure USART */
    LL_LPUART_InitTypeDef lpuart_initstruct;

    /* Disable USART prior modifying configuration registers */
    LL_LPUART_Disable(TRACER_EMB_USART_INSTANCE);


    /* Set fields of initialization structure                   */
    /*  - Prescaler           : LL_USART_PRESCALER_DIV1         */
    /*  - BaudRate            : TRACE_BAUDRATE                  */
    /*  - DataWidth           : LL_USART_DATAWIDTH_8B           */
    /*  - StopBits            : LL_USART_STOPBITS_1             */
    /*  - Parity              : LL_USART_PARITY_NONE            */
    /*  - TransferDirection   : LL_USART_DIRECTION_TX           */
    /*  - HardwareFlowControl : LL_USART_HWCONTROL_NONE         */
    /*  - OverSampling        : LL_USART_OVERSAMPLING_16        */
#if defined(USART_PRESC_PRESCALER)
    lpuart_initstruct.PrescalerValue      = LL_USART_PRESCALER_DIV1;
#endif /* USART_PRESC_PRESCALER */
    lpuart_initstruct.BaudRate            = TRACER_EMB_BAUDRATE;
    lpuart_initstruct.DataWidth           = LL_USART_DATAWIDTH_8B;
    lpuart_initstruct.StopBits            = LL_USART_STOPBITS_1;
    lpuart_initstruct.Parity              = LL_USART_PARITY_NONE;
    lpuart_initstruct.TransferDirection   = LL_USART_DIRECTION_TX;
    lpuart_initstruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;

    /* Initialize USART instance according to parameters defined in initialization structure */
    LL_LPUART_Init(TRACER_EMB_USART_INSTANCE, &lpuart_initstruct);

    LL_LPUART_Enable(TRACER_EMB_USART_INSTANCE);

    uint32_t _temp1 = LL_LPUART_IsActiveFlag_TEACK(TRACER_EMB_USART_INSTANCE);
    /* Polling USART initialisation */
    while (_temp1 == 0U)
    {
      _temp1 = LL_LPUART_IsActiveFlag_TEACK(TRACER_EMB_USART_INSTANCE);
    }
  }
#endif /* TRACER_EMB_IS_INSTANCE_LPUART_TYPE */

#if TRACER_EMB_DMA_MODE == 1UL
  /* Configure TX DMA */
  TRACER_EMB_ENABLE_CLK_DMA();

  /* (3) Configure the DMA functional parameters for transmission */
#if defined(GPDMA1)
  LL_DMA_ConfigTransfer(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL,
                        LL_DMA_SRC_INCREMENT          |
                        LL_DMA_DEST_FIXED             |
                        LL_DMA_SRC_DATAWIDTH_BYTE     |
                        LL_DMA_DEST_DATAWIDTH_BYTE);

  LL_DMA_SetPeriphRequest(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL, TRACER_EMB_TX_DMA_REQUEST);

#elif defined(DMA_SxCR_CHSEL)
  LL_DMA_SetChannelSelection(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_STREAM, TRACER_EMB_TX_DMA_CHANNEL);
  LL_DMA_ConfigTransfer(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_STREAM,
                        LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
                        LL_DMA_PRIORITY_HIGH              |
                        LL_DMA_MODE_NORMAL                |
                        LL_DMA_PERIPH_NOINCREMENT         |
                        LL_DMA_MEMORY_INCREMENT           |
                        LL_DMA_PDATAALIGN_BYTE            |
                        LL_DMA_MDATAALIGN_BYTE);
#else
  LL_DMA_ConfigTransfer(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL,
                        LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
                        LL_DMA_PRIORITY_HIGH              |
                        LL_DMA_MODE_NORMAL                |
                        LL_DMA_PERIPH_NOINCREMENT         |
                        LL_DMA_MEMORY_INCREMENT           |
                        LL_DMA_PDATAALIGN_BYTE            |
                        LL_DMA_MDATAALIGN_BYTE);

#if defined(DMAMUX_CxCR_DMAREQ_ID)
  LL_DMA_SetPeriphRequest(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL, TRACER_EMB_TX_DMA_REQUEST);
#endif /* DMAMUX_CxCR_DMAREQ_ID */

#endif /* GPDMA1 */

#if defined(DMA_SxCR_CHSEL)
  LL_DMA_EnableIT_TC(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_STREAM);
#else
  LL_DMA_EnableIT_TC(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL);
#endif /* DMA_SxCR_CHSEL */

#endif /* TRACER_EMB_DMA_MODE == 1*/

  /* Configure the interrupt for TX */
#if TRACER_EMB_DMA_MODE == 1UL
#ifdef TRACER_EMB_TX_DMA_PRIORITY
  NVIC_SetPriority(TRACER_EMB_TX_DMA_IRQ, TRACER_EMB_TX_DMA_PRIORITY);
  NVIC_EnableIRQ(TRACER_EMB_TX_DMA_IRQ);
#else
  NVIC_SetPriority(TRACER_EMB_TX_DMA_IRQ, 0);
  NVIC_EnableIRQ(TRACER_EMB_TX_DMA_IRQ);
#endif /* TRACER_EMB_TX_DMA_PRIORITY */
#endif /* TRACER_EMB_DMA_MODE == 1 */
#ifdef TRACER_EMB_TX_IRQ_PRIORITY
  NVIC_SetPriority(TRACER_EMB_USART_IRQ, TRACER_EMB_TX_IRQ_PRIORITY);
  NVIC_EnableIRQ(TRACER_EMB_USART_IRQ);
#else
  NVIC_SetPriority(TRACER_EMB_USART_IRQ, 3);
  NVIC_EnableIRQ(TRACER_EMB_USART_IRQ);
#endif /* TRACER_EMB_TX_IRQ_PRIORITY */

  /* Disable the UART */
  if (fptr_rx == NULL)
  {
    TRACER_EMB_DISABLE_CLK_USART();
  }
  return;
}

/**
  * @brief  Trace Deinit
  * @retval none
  */
void HW_TRACER_EMB_DeInit(void)
{
  TRACER_EMB_DISABLE_CLK_USART();
  return;
}

/**
  * @brief  Allow to update the RX callback
  * @param  callbackRX
  * @retval none
  */
void HW_TRACER_EMB_RegisterRxCallback(void (*callbackRX)(uint8_t, uint8_t))
{
  fptr_rx = callbackRX;
  TRACER_EMB_ENABLE_CLK_USART();
}

/**
  * @brief  Start RX reception only when OSKernel have been started
  * @retval none
  */
void HW_TRACER_EMB_StartRX(void)
{
  /* Enable USART IT for RX */
  TRACER_EMB_ENABLE_IT_RXNE(TRACER_EMB_USART_INSTANCE);
  TRACER_EMB_ENABLE_IT_ERROR(TRACER_EMB_USART_INSTANCE);

  /* Enable RX/TX */
  TRACER_EMB_ENABLEDIRECTIONRX(TRACER_EMB_USART_INSTANCE);

  /* Configure the interrupt for RX */
  NVIC_SetPriority(TRACER_EMB_USART_IRQ, 3);
  NVIC_EnableIRQ(TRACER_EMB_USART_IRQ);
}

#if TRACER_EMB_DMA_MODE == 1UL
/**
  * @brief  Function to handle reception in DMA mode
  * @retval none
  */
void HW_TRACER_EMB_IRQHandlerDMA(void)
{
  if (TRACER_EMB_TX_DMA_ACTIVE_FLAG(TRACER_EMB_DMA_INSTANCE) == 1UL)
  {
    TRACER_EMB_ENABLE_IT_TC(TRACER_EMB_USART_INSTANCE);
    TRACER_EMB_TX_DMA_CLEAR_FLAG(TRACER_EMB_DMA_INSTANCE);
  }
}
#endif /* TRACER_EMB_DMA_MODE == 1 */

/**
  * @brief  USART/LPUART IRQ Handler
  * @retval none
  */
void HW_TRACER_EMB_IRQHandlerUSART(void)
{
  uint32_t _temp1;
  uint32_t  _temp2;
  uint32_t _isrflags;
  uint32_t _errorflags;
  uint8_t _error = 0U;

  /* Load ISR in local variable */
#if defined(USART_ISR_TC)
  _isrflags = TRACER_EMB_READREG(TRACER_EMB_USART_INSTANCE, ISR);
#else
  _isrflags = TRACER_EMB_READREG(TRACER_EMB_USART_INSTANCE, SR);
#endif  /* USART_ISR_TC */

  /* Ready to read received char if any */
  _temp1 = TRACER_EMB_ISENABLEDIT_RXNE(TRACER_EMB_USART_INSTANCE);

  /* If Reception is ongoing */
  if (_temp1 == 1UL)
  {
    /* Reception is active : check if new byte is received */
    __IO uint32_t received_char;

    _errorflags = ((_isrflags) & (TRACER_EMB_FLAG_PE | TRACER_EMB_FLAG_FE | TRACER_EMB_FLAG_NE | TRACER_EMB_FLAG_ORE));

    /* Check if RXNE flag is set : new byte is stored in DR */
    if (((TRACER_EMB_FLAG_RXNE) == ((_isrflags) & (TRACER_EMB_FLAG_RXNE)))
        && (_errorflags == 0UL))
    {
      /* Read Received character. RXNE flag is cleared by reading of RDR register */
      received_char = TRACER_EMB_RECEIVE_DATA8(TRACER_EMB_USART_INSTANCE);

      if (fptr_rx != NULL)
      {
        fptr_rx(received_char, 0U);
      }
      return;
    }
    else if (_errorflags != 0UL)
    {
      /* Error has been detected */

      /* Check if RXNE flag is set : new byte is stored in DR */
      if ((TRACER_EMB_FLAG_RXNE) == ((_isrflags) & (TRACER_EMB_FLAG_RXNE)))
      {
        /* Read Received character. RXNE flag is cleared by reading of RDR register */
        received_char = TRACER_EMB_RECEIVE_DATA8(TRACER_EMB_USART_INSTANCE);

        if (fptr_rx != NULL)
        {
          fptr_rx(received_char, 0U);
        }
        return;
      }

      /* Parity error    */
      _temp2 = TRACER_EMB_ISENABLEDIT_PE(TRACER_EMB_USART_INSTANCE);
      if ((_temp2 == 1U)
          && (TRACER_EMB_FLAG_PE == ((_errorflags) & (TRACER_EMB_FLAG_PE))))
      {
        /* Flags clearing */
        TRACER_EMB_CLEARFLAG_PE(TRACER_EMB_USART_INSTANCE);
        _error = 1U;
      }

      /* Generic Errors*/
      _temp2 = TRACER_EMB_ISENABLEDIT_ERROR(TRACER_EMB_USART_INSTANCE);
      if (_temp2 == 1U)
      {
        /* Framing error   */
        if (TRACER_EMB_FLAG_FE == ((_errorflags) & (TRACER_EMB_FLAG_FE)))
        {
          /* Flags clearing */
          TRACER_EMB_CLEARFLAG_FE(TRACER_EMB_USART_INSTANCE);
          _error = 1U;
        }

        /* Overrun error   */
        if (TRACER_EMB_FLAG_ORE == ((_errorflags) & (TRACER_EMB_FLAG_ORE)))
        {
          /* Flags clearing */
          TRACER_EMB_CLEARFLAG_ORE(TRACER_EMB_USART_INSTANCE);
          _error = 1U;
        }

        /* Noise detection */
        if (TRACER_EMB_FLAG_NE == ((_errorflags) & (TRACER_EMB_FLAG_NE)))
        {
          /* Flags clearing */
          TRACER_EMB_CLEARFLAG_NE(TRACER_EMB_USART_INSTANCE);
          _error = 1U;
        }
      }

      if ((_error == 1U) && (fptr_rx != NULL))
      {
        fptr_rx(1U, 1U);  /* 1 indicate a reception error */
      }
    }
    else
    {
      /* no error, no reception */
    }
  }

#if TRACER_EMB_IT_MODE == 1UL
  /* Ready to handle TXE interrupt */
  _temp2 = TRACER_EMB_ISENABLEDIT_TXE(TRACER_EMB_USART_INSTANCE);
  if (((TRACER_EMB_FLAG_TXE) == ((_isrflags) & (TRACER_EMB_FLAG_TXE))) && (_temp2 == 1UL))
  {
    /* Write data inside the data register */
    TRACER_EMB_TRANSMIT_DATA8(TRACER_EMB_USART_INSTANCE, *txData);
    txData++;
    txSize--;
    if (txSize == 0)
    {
      TRACER_EMB_DISABLEIT_TXE(TRACER_EMB_USART_INSTANCE);

      if (NULL == fptr_rx)
      {
        TRACER_EMB_DISABLE_CLK_USART();
      }

      TRACER_EMB_CALLBACK_TX();
    }
  }
#endif /* TRACER_EMB_IT_MODE == 1 */

#if  (TRACER_EMB_DMA_MODE == 1UL)
  /* Ready to handle TC interrupt */
  _temp2 = TRACER_EMB_ISENABLEDIT_TC(TRACER_EMB_USART_INSTANCE);
  if (((TRACER_EMB_FLAG_TC) == ((_isrflags) & (TRACER_EMB_FLAG_TC))) && (_temp2 == 1UL))
  {
    TRACER_EMB_DISABLEIT_TC(TRACER_EMB_USART_INSTANCE);
    TRACER_EMB_CLEARFLAG_TC(TRACER_EMB_USART_INSTANCE);
#if defined(DMA_SxCR_CHSEL)
    TRACER_EMB_DISABLESTREAM(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_STREAM);
#else
    TRACER_EMB_DISABLECHANNEL(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL);
#endif /* DMA_SxCR_CHSEL */
    if (fptr_rx == NULL)
    {
      TRACER_EMB_DISABLE_CLK_USART();
    }
    /* call the callback */
    TRACER_EMB_CALLBACK_TX();
  }
#endif /* TRACER_EMB_DMA_MODE == 1 */

}

/**
  * @brief  USART/LPUART send data
  * @param data pointer
  * @param data size
  * @retval none
  */
void HW_TRACER_EMB_SendData(const uint8_t *pData, uint32_t Size)
{
  /* enable the USART */
  TRACER_EMB_ENABLE_CLK_USART();

#if TRACER_EMB_DMA_MODE == 1UL
#if defined(GPDMA1)
  LL_DMA_ConfigAddresses(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL,
                         (uint32_t)pData,
                         TRACER_EMB_DMA_GETREGADDR(TRACER_EMB_USART_INSTANCE, LL_USART_DMA_REG_DATA_TRANSMIT));
  LL_DMA_SetBlkDataLength(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL, Size);

  /* Enable DMA TX Interrupt */
  TRACER_EMB_ENABLEDMAREQ_TX(TRACER_EMB_USART_INSTANCE);

  /* Enable DMA Channel Tx */
  LL_DMA_EnableChannel(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL);
  TRACER_EMB_ENABLECHANNEL(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL);
#elif defined(DMA_SxCR_CHSEL)
  LL_DMA_ConfigAddresses(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_STREAM,
                         (uint32_t)pData,
                         TRACER_EMB_DMA_GETREGADDR(TRACER_EMB_USART_INSTANCE),
                         LL_DMA_GetDataTransferDirection(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_STREAM));
  LL_DMA_SetDataLength(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_STREAM, Size);

  /* Enable DMA TX Interrupt */
  TRACER_EMB_ENABLEDMAREQ_TX(TRACER_EMB_USART_INSTANCE);

  /* Enable DMA Channel Tx */
  TRACER_EMB_ENABLESTREAM(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_STREAM);
#else
  LL_DMA_ConfigAddresses(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL,
                         (uint32_t)pData,
                         TRACER_EMB_DMA_GETREGADDR(TRACER_EMB_USART_INSTANCE, LL_USART_DMA_REG_DATA_TRANSMIT),
                         LL_DMA_GetDataTransferDirection(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL));
  LL_DMA_SetDataLength(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL, Size);

  /* Enable DMA TX Interrupt */
  TRACER_EMB_ENABLEDMAREQ_TX(TRACER_EMB_USART_INSTANCE);

  /* Enable DMA Channel Tx */
  TRACER_EMB_ENABLECHANNEL(TRACER_EMB_DMA_INSTANCE, TRACER_EMB_TX_DMA_CHANNEL);
#endif /* GPDMA1 */
#endif /* TRACER_EMB_DMA_MODE == 1 */

#if TRACER_EMB_IT_MODE == 1UL
  /* keep information about the data to transfer */
  txData = (uint8_t *)pData;
  txSize = Size;

  /* Enable the TXEIE */
  TRACER_EMB_ENABLE_IT_TXE(TRACER_EMB_USART_INSTANCE);
#endif /* TRACER_EMB_IT_MODE == 1 */
}

/**
  * @brief  USART/LPUART read data
  * @retval return a data
  */
uint8_t HW_TRACER_EMB_ReadData(void)
{
  return TRACER_EMB_RECEIVE_DATA8(TRACER_EMB_USART_INSTANCE);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
