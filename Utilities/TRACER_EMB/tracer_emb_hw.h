/**
  ******************************************************************************
  * @file    usbpd_bsp_trace.h
  * @author  MCD Application Team
  * @brief   This file contains bsp interface control functions.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef HW_TRACE_H
#define HW_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported functions ---------------------------------------------------------*/

void  HW_TRACER_EMB_Init(void);
void  HW_TRACER_EMB_DeInit(void);
void  HW_TRACER_EMB_RegisterRxCallback(void (*callbackRX)(uint8_t, uint8_t));
#if TRACER_EMB_DMA_MODE == 1UL
void  HW_TRACER_EMB_IRQHandlerDMA(void);
#endif  /* TRACER_EMB_DMA_MODE == 1 */
void  HW_TRACER_EMB_IRQHandlerUSART(void);
void  HW_TRACER_EMB_StartRX(void);
void  HW_TRACER_EMB_SendData(const uint8_t *data, uint32_t size);
uint8_t HW_TRACER_EMB_ReadData(void);

#ifdef __cplusplus
}
#endif

#endif /* HW_TRACE_H */
