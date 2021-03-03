/**
  ******************************************************************************
  * @file    trace_emb.h
  * @author  MCD Application Team
  * @brief   This file contains the headers of tracer_emb.h for Cable Attach-Detach
  *          controls.
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
#ifndef __TRACER_EMB_H_
#define __TRACER_EMB_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#ifndef _STDINT
#include "stdint.h"
#endif /* _STDINT */
#include "tracer_emb_conf.h"

/** @addtogroup TRACER_EMB
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/**
  * @}
  */

/* Exported define -----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup TRACER_EMB_Exported_Functions TRACE Exported Functions
  * @{
  */
/**
  * @brief  Initialize the TRACE module
  * @retval None
  */
void            TRACER_EMB_Init(void);

/**
  * @brief  Add information in debug trace buffer
  * @param  Type    Trace Type based on @ref TRACER_EMB_EVENT
  * @param  PortNum Port number value
  * @param  Sop     SOP type
  * @param  Ptr     Pointer on the data to send
  * @param  Size    Size of the data to send
  * @retval None.
  */
void            TRACER_EMB_Add(uint8_t *Ptr, uint32_t Size);

/**
  * @brief  Add one data into the trace buffer
  * @param  pos position
  * @param  data data value
  */
void TRACER_EMB_WriteData(uint16_t pos, uint8_t data);

/**
  * @brief  Lock the trace buffer.
  * @param  None.
  * @retval None.
  */
void TRACER_EMB_Lock(void);

/**
  * @brief  UnLock the trace buffer.
  * @param  None.
  * @retval None.
  */
void TRACER_EMB_UnLock(void);


void TRACER_EMB_SendData(void);

/**
  * @brief  allocate space inside the buffer to push data
  * @param  data size
  * @retval write position inside the buffer is -1 no space available.
  */
int32_t TRACER_EMB_AllocateBufer(uint32_t Size);

/**
  * @brief  enable the overflow detection
  * @param  Data pointer on the overflow string
  * @param  Size of the overflow string
  * @retval 0 if success other value indicate an error.
  */
int32_t TRACER_EMB_EnableOverFlow(const uint8_t *Data, uint8_t Size);


void TRACER_EMB_CALLBACK_TX(void);

#if TRACER_EMB_DMA_MODE == 1UL
void TRACER_EMB_IRQHandlerDMA(void);
#endif  /* TRACER_EMB_DMA_MODE == 1 */

void TRACER_EMB_IRQHandlerUSART(void);


void TRACER_EMB_StartRX(void (*callbackRX)(uint8_t, uint8_t));


/**
  * @brief  function to initialize LPM for emb tracer.
  * @retval Timing
  */
void TRACER_EMB_LowPowerInit(void);

/**
  * @brief  function to manage LPM when sending data.
  * @retval Timing
  */
void TRACER_EMB_LowPowerSendData(void);

/**
  * @brief  function to manage LPM when data send complete.
  * @retval Timing
  */
void TRACER_EMB_LowPowerSendDataComplete(void);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __TRACER_EMB_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
