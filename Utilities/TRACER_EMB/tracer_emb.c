/**
  ******************************************************************************
  * @file    tracer_emb.c
  * @author  MCD Application Team
  * @brief   This file contains embedded tracer control functions.
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
#include "string.h"

/** @addtogroup TRACER_EMB
  * @{
  */

/** @addtogroup TRACER_EMB
  * @{
  */

/** @addtogroup TRACER_EMB
  * @{
  */

/* Private enums -------------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/
/** @defgroup TRACER_EMB_Private_TypeDef EMB TRACER Private typedef
  * @{
  */
typedef enum
{
  TRACER_OVERFLOW_NONE,
  TRACER_OVERFLOW_DETECTED,
  TRACER_OVERFLOW_SENT
} TRACER_OverFlowTypedef;
/**
  * @}
  */


/** @defgroup TRACER_EMB_Private_TypeDef EMB TRACER Private typedef
  * @{
  */
typedef enum
{
  TRACER_OK,
  TRACER_KO
} TRACER_StatusTypedef;
/**
  * @}
  */

/** @defgroup TRACER_EMB_Private_TypeDef EMB TRACER Private typedef
  * @{
  */
typedef struct
{
  uint32_t PtrTx_Read;
  uint32_t PtrTx_Write;
  uint32_t SizeSent;
  uint32_t LowPower_Counter;
  __IO  uint8_t Counter;
  const uint8_t *OverFlow_Data;
  uint8_t OverFlow_Size;
  uint8_t discontinue;
  TRACER_OverFlowTypedef OverFlow_Status;
  uint8_t PtrDataTx[TRACER_EMB_BUFFER_SIZE];
} TRACER_ContextTypedef_t;
/**
  * @}
  */

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/** @defgroup TRACER_EMB_Private_Macros TRACE Private Macros
  * @{
  */

#define TRACER_WRITE_DATA(_POSITION_,_DATA_) \
  TracerContext.PtrDataTx[(_POSITION_) % ((int32_t)TRACER_EMB_BUFFER_SIZE)] = (_DATA_);\
  (_POSITION_) = ((_POSITION_) + 1) % ((int32_t)TRACER_EMB_BUFFER_SIZE);

#define TRACER_ENTER_CRITICAL_SECTION()  uint32_t primask= __get_PRIMASK();\
  __disable_irq();

#define TRACER_LEAVE_CRITICAL_SECTION() __set_PRIMASK(primask)


/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup TRACER_EMB_Private_Functions TRACE Private Functions
  * @{
  */
uint32_t TRACER_CallbackRX(uint8_t character, uint8_t error);

/* Function prototypes -----------------------------------------------*/
uint8_t TRACER_EMB_ReadData(void);

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup TRACER_EMB_Private_Variables TRACE Private Variables
  * @{
  */
static TRACER_ContextTypedef_t TracerContext;
/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/

/** @addtogroup TRACER_EMB_Exported_Functions
  * @{
  */
void TRACER_EMB_Init(void)
{
  /* initialize the Ptr for Read/Write */
  (void)memset(&TracerContext, 0, sizeof(TRACER_ContextTypedef_t));

  /* Initialize trace BUS */
  HW_TRACER_EMB_Init();

  /* Initialize the lowpower aspect */
  TRACER_EMB_LowPowerInit();
}

void TRACER_EMB_Add(uint8_t *Ptr, uint32_t Size)
{
  int32_t _writepos;
  uint8_t *data_to_write = Ptr;
  uint32_t index;

  /* Data processing */
  TRACER_EMB_Lock();
  _writepos = TRACER_EMB_AllocateBufer(Size);

  /* if allocation is ok, write data into the buffer */
  if (_writepos != -1)
  {
    /* initialize the Ptr for Read/Write */
    for (index = 0U; index < Size; index++)
    {
      TRACER_WRITE_DATA(_writepos, data_to_write[index]);
    }
  }
  TRACER_EMB_UnLock();

  /* Tx processing */
  TRACER_EMB_SendData();
}

#if TRACER_EMB_DMA_MODE == 1UL
void TRACER_EMB_IRQHandlerDMA(void)
{
  HW_TRACER_EMB_IRQHandlerDMA();
}
#endif /* TRACER_EMB_DMA_MODE == 1 */

void TRACER_EMB_IRQHandlerUSART(void)
{
  HW_TRACER_EMB_IRQHandlerUSART();
}

void TRACER_EMB_WriteData(uint16_t pos, uint8_t data)
{
  TracerContext.PtrDataTx[pos % TRACER_EMB_BUFFER_SIZE] = data;
}

void TRACER_EMB_StartRX(void (*callbackRX)(uint8_t, uint8_t))
{
  HW_TRACER_EMB_RegisterRxCallback(callbackRX);
  HW_TRACER_EMB_StartRX();
}

int32_t TRACER_EMB_EnableOverFlow(const uint8_t *Data, uint8_t Size)
{
  if (Size != 0U)
  {
    TracerContext.OverFlow_Data = Data;
    TracerContext.OverFlow_Size = Size;
    return 0;
  }
  return -1;
}

uint8_t TRACER_EMB_ReadData()
{
  return HW_TRACER_EMB_ReadData();
}


/**
  * @}
  */

/** @defgroup TRACER_EMB_Private_Functions TRACE Private Functions
  * @{
  */

/**
  * @brief  callback called to end a transfer.
  * @param  None.
  * @retval None.
  */
void TRACER_EMB_CALLBACK_TX(void)
{
  TRACER_ENTER_CRITICAL_SECTION();
  TracerContext.PtrTx_Read = (TracerContext.PtrTx_Read + TracerContext.SizeSent) % TRACER_EMB_BUFFER_SIZE;

  if ((TracerContext.OverFlow_Data != NULL) && (TracerContext.OverFlow_Status == TRACER_OVERFLOW_DETECTED)
      && (TracerContext.discontinue == 0U))
  {
    TracerContext.OverFlow_Status = TRACER_OVERFLOW_SENT;
    HW_TRACER_EMB_SendData(TracerContext.OverFlow_Data, TracerContext.OverFlow_Size);
    TRACER_LEAVE_CRITICAL_SECTION();
  }
  else
  {
    TracerContext.LowPower_Counter--;
    TRACER_LEAVE_CRITICAL_SECTION();
    TRACER_EMB_UnLock();
    TRACER_EMB_SendData();
  }
}

/**
  * @brief  Lock the trace buffer.
  * @param  None.
  * @retval None.
  */
void TRACER_EMB_Lock(void)
{
  TRACER_ENTER_CRITICAL_SECTION();
  TracerContext.Counter++;
  TRACER_LEAVE_CRITICAL_SECTION();
}

/**
  * @brief  UnLock the trace buffer.
  * @param  None.
  * @retval None.
  */
void TRACER_EMB_UnLock(void)
{
  TRACER_ENTER_CRITICAL_SECTION();
  TracerContext.Counter--;
  TRACER_LEAVE_CRITICAL_SECTION();
}

/**
  * @brief  if buffer is not locked return a Begin / end @ to transfer over the media.
  * @param  address begin of the data
  * @param  address end of the data
  * @retval None.
  */
void TRACER_EMB_SendData(void)
{
  uint32_t _begin;
  uint32_t _end;

  TRACER_ENTER_CRITICAL_SECTION();

  if (0u == TracerContext.Counter)
  {
    _begin = TracerContext.PtrTx_Read;
    _end = TracerContext.PtrTx_Write;

    if (_begin != _end)
    {
      TRACER_EMB_Lock();
      /*  */
      if (_end > _begin)
      {
        TracerContext.SizeSent = _end - _begin;
        TracerContext.discontinue = 0;
      }
      else  /* _begin > _end */
      {
        TracerContext.SizeSent = TRACER_EMB_BUFFER_SIZE - _begin;
        TracerContext.discontinue = 1;
      }
      TRACER_EMB_LowPowerSendData();
      TracerContext.LowPower_Counter++;
      HW_TRACER_EMB_SendData((const uint8_t *)(&(TracerContext.PtrDataTx[_begin])), TracerContext.SizeSent);
    }
  }

  /* Low power processing */
  switch (TracerContext.LowPower_Counter)
  {
    case 1:
      TRACER_EMB_LowPowerSendData();
      break;
    case 0:
      TRACER_EMB_LowPowerSendDataComplete();
      break;
    default:
      break;
  }

  TRACER_LEAVE_CRITICAL_SECTION();
}

/**
  * @brief  allocate space inside the buffer to push data
  * @param  data size
  * @retval write position inside the buffer is -1 no space available.
  */
int32_t TRACER_EMB_AllocateBufer(uint32_t Size)
{
  uint32_t _freesize;
  int32_t _pos = -1;

  TRACER_ENTER_CRITICAL_SECTION();

  if (TracerContext.PtrTx_Write == TracerContext.PtrTx_Read)
  {
    /* Need to add buffer full management */
    _freesize = TRACER_EMB_BUFFER_SIZE;
  }
  else
  {
    if (TracerContext.PtrTx_Write > TracerContext.PtrTx_Read)
    {
      _freesize = TRACER_EMB_BUFFER_SIZE - TracerContext.PtrTx_Write + TracerContext.PtrTx_Read;
    }
    else
    {
      _freesize = TracerContext.PtrTx_Read - TracerContext.PtrTx_Write;
    }
  }

  if (_freesize > Size)
  {
    _pos = (int32_t)TracerContext.PtrTx_Write;
    TracerContext.PtrTx_Write = (TracerContext.PtrTx_Write + Size) % TRACER_EMB_BUFFER_SIZE;
    if (TRACER_OVERFLOW_SENT == TracerContext.OverFlow_Status)
    {
      TracerContext.OverFlow_Status = TRACER_OVERFLOW_NONE;
    }
  }
  else
  {
    if (TRACER_OVERFLOW_NONE == TracerContext.OverFlow_Status)
    {
      TracerContext.OverFlow_Status = TRACER_OVERFLOW_DETECTED;
    }
  }

  TRACER_LEAVE_CRITICAL_SECTION();
  return _pos;
}

__weak void TRACER_EMB_LowPowerInit(void)
{
}

__weak void TRACER_EMB_LowPowerSendData(void)
{
}

__weak void TRACER_EMB_LowPowerSendDataComplete(void)
{
}

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

