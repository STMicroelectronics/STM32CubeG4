/**
  ******************************************************************************
  * @file    usbpd_hw_if_it.c
  * @author  MCD Application Team
  * @brief   This file contains HW interface interrupt routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbpd_devices_conf.h"
#include "usbpd_core.h"
#include "usbpd_hw_if.h"
#include "usbpd_trace.h"
#if defined(_LOW_POWER)
#include "usbpd_lowpower.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void PORTx_IRQHandler(uint8_t PortNum);

void USBPD_PORT0_IRQHandler(void)
{
  PORTx_IRQHandler(USBPD_PORT_0);
}

void PORTx_IRQHandler(uint8_t PortNum)
{
  UCPD_TypeDef *hucpd = Ports[PortNum].husbpd;
  uint32_t _interrupt = LL_UCPD_ReadReg(hucpd, SR);
  static uint8_t ovrflag = 0;

  if ((hucpd->IMR & _interrupt) != 0u)
  {
    /* TXIS no need to enable it all the transfer are done by DMA */
    if (UCPD_SR_TXMSGDISC == (_interrupt & UCPD_SR_TXMSGDISC))
    {
      /* the message has been discarded */
      LL_UCPD_ClearFlag_TxMSGDISC(hucpd);
      CLEAR_BIT(Ports[PortNum].hdmatx->CCR, DMA_CCR_EN);
      Ports[PortNum].cbs.USBPD_HW_IF_TxCompleted(PortNum, 1);
      return;
    }

    if (UCPD_SR_TXMSGSENT == (_interrupt & UCPD_SR_TXMSGSENT))
    {
      /* Message has been fully transferred */
      LL_UCPD_ClearFlag_TxMSGSENT(hucpd);
      CLEAR_BIT(Ports[PortNum].hdmatx->CCR, DMA_CCR_EN);
      Ports[PortNum].cbs.USBPD_HW_IF_TxCompleted(PortNum, 0);

#if defined(_LOW_POWER)
      UTIL_LPM_SetStopMode(0 == PortNum?LPM_PE_0:LPM_PE_1,UTIL_LPM_ENABLE);
#endif
      return;
    }

    if (UCPD_SR_TXMSGABT == (_interrupt & UCPD_SR_TXMSGABT))
    {
      LL_UCPD_ClearFlag_TxMSGABT(hucpd);
      CLEAR_BIT(Ports[PortNum].hdmatx->CCR, DMA_CCR_EN);
      Ports[PortNum].cbs.USBPD_HW_IF_TxCompleted(PortNum, 2);
      return;
    }

    /* HRSTDISC : the sent of the hrad reset has been discarded */
    if (UCPD_SR_HRSTDISC == (_interrupt & UCPD_SR_HRSTDISC))
    {
      LL_UCPD_ClearFlag_TxHRSTDISC(hucpd);
      return;
    }

    /* TXUND : tx underrun detected */
    if (UCPD_SR_HRSTSENT == (_interrupt & UCPD_SR_HRSTSENT))
    {
      /* Mhum, the stack is not expected to get this answer */
      LL_UCPD_ClearFlag_TxHRSTSENT(hucpd);
      Ports[PortNum].cbs.USBPD_HW_IF_TX_HardResetCompleted(PortNum, USBPD_SOPTYPE_HARD_RESET);
      return;
    }

    /* TXUND : tx underrun detected */
    if (UCPD_SR_TXUND == (_interrupt & UCPD_SR_TXUND))
    {
      /* nothing to do the port partner check message integrity with CRC so PRL will repeat the sent
         can be used for debug purpose */
      LL_UCPD_ClearFlag_TxUND(hucpd);
      return;
    }

    /* RXNE : not needed the stack only perform transfer by DMA */
    /* RXORDDET: not needed so stack will not enabled this interrupt */
    if (UCPD_SR_RXORDDET == (_interrupt & UCPD_SR_RXORDDET))
    {
      if(LL_UCPD_RXORDSET_CABLE_RESET == hucpd->RX_ORDSET)
      {
        /* Cable reset detected */
        Ports[PortNum].cbs.USBPD_HW_IF_RX_ResetIndication(PortNum, USBPD_SOPTYPE_CABLE_RESET);
      }
      LL_UCPD_ClearFlag_RxOrderSet(hucpd);
#if defined(_LOW_POWER)
      UTIL_LPM_SetStopMode(0 == PortNum?LPM_PE_0:LPM_PE_1,UTIL_LPM_DISABLE);
#endif
      return;
    }

    /* check RXHRSTDET */
    if (UCPD_SR_RXHRSTDET == (_interrupt & UCPD_SR_RXHRSTDET))
    {
      Ports[PortNum].cbs.USBPD_HW_IF_RX_ResetIndication(PortNum, USBPD_SOPTYPE_HARD_RESET);
      LL_UCPD_ClearFlag_RxHRST(hucpd);
      return;
    }

    /* check RXOVR */
    if (UCPD_SR_RXOVR == (_interrupt & UCPD_SR_RXOVR))
    {
      /* nothing to do the message will be discarded and the port partner retry the send */
      ovrflag = 1;
      LL_UCPD_ClearFlag_RxOvr(hucpd);
      return;
    }

    /* check RXMSGEND an Rx message has been received */
    if (UCPD_SR_RXMSGEND == (_interrupt & UCPD_SR_RXMSGEND))
    {
      /* for DMA mode add a control to check if the number of data received is corresponding with the number of
         data receive by USBPD */
      LL_UCPD_ClearFlag_RxMsgEnd(hucpd);

      /* disable DMA */
      CLEAR_BIT(Ports[PortNum].hdmarx->CCR, DMA_CCR_EN);

#if defined(_LOW_POWER)
      UTIL_LPM_SetOffMode(0 == PortNum?LPM_PE_0:LPM_PE_1, UTIL_LPM_ENABLE);
#endif

      if (((_interrupt & UCPD_SR_RXERR) == 0u) && (ovrflag == 0u))
      {
        /* Rx message has been received without error */
        Ports[PortNum].cbs.USBPD_HW_IF_RX_Completed(PortNum, hucpd->RX_ORDSET & UCPD_RX_ORDSET_RXORDSET);
      }
      ovrflag = 0;

      /* Ready for next transaction */
      WRITE_REG(Ports[PortNum].hdmarx->CMAR, (uint32_t)Ports[PortNum].ptr_RxBuff);
      WRITE_REG(Ports[PortNum].hdmarx->CNDTR, SIZE_MAX_PD_TRANSACTION_UNCHUNK);

      SET_BIT(Ports[PortNum].hdmarx->CCR, DMA_CCR_EN);
      return;
    }

    /* check TYPECEVT1IE/TYPECEVT1IE || check TYPECEVT2IE/TYPECEVT2IE */
    if ((UCPD_SR_TYPECEVT1 == (_interrupt & UCPD_SR_TYPECEVT1)) || (UCPD_SR_TYPECEVT2 == (_interrupt & UCPD_SR_TYPECEVT2)))
    {
      /* clear both interrupt */
      LL_UCPD_ClearFlag_TypeCEventCC1(hucpd);
      LL_UCPD_ClearFlag_TypeCEventCC2(hucpd);
      Ports[PortNum].USBPD_CAD_WakeUp();
      /* Wakeup CAD to check the detection event */
      return;
    }

    /* check FRSEVTIE */
    if (UCPD_SR_FRSEVT == (_interrupt & UCPD_SR_FRSEVT))
    {
      LL_UCPD_ClearFlag_FRS(hucpd);
      if (USBPD_PORTPOWERROLE_SNK == Ports[PortNum].params->PE_PowerRole)
      {
        /* Confirm the FRS by checking if an RP is always present on the current CC line
           we should wait min 6us to refresh the type C state machine */
        for(uint32_t delay=0; delay < 30; delay++){ __DSB(); }       
        
        if (0 != (hucpd->SR & (UCPD_SR_TYPEC_VSTATE_CC1 | UCPD_SR_TYPEC_VSTATE_CC2)))
        {
          Ports[PortNum].cbs.USBPD_HW_IF_TX_FRSReception(PortNum);
        }
      }
    }
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

