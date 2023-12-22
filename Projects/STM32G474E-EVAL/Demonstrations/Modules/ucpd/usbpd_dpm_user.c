/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_dpm_user.c
  * @author  MCD Application Team
  * @brief   USBPD DPM user code
  ******************************************************************************
  *
  * Copyright (c) 2019 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#define USBPD_DPM_USER_C
/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
#include "usbpd_core.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_conf.h"
#include "usbpd_dpm_user.h"
#include "usbpd_pwr_if.h"
#if defined(_TRACE)
#include "usbpd_trace.h"
#endif /* _TRACE */

#if defined(USBPD_REV30_SUPPORT) && _ALERT
#include "usbpd_hw_if.h"
#endif /* USBPD_REV30_SUPPORT && _ALERT */

#if defined(_VDM) || defined (_VCONN_SUPPORT)
#include "usbpd_vdm_user.h"
#endif /*_VDM*/
#include "string.h"
#ifdef _USB_DEVICE
#include "usbd_core.h"
#include "usbd_desc.h"
#if defined(USB_BILLBOARD)
#include "usbd_bb.h"
#else
#include "usbd_msc.h"
#include "usbd_storage.h"
#endif /* USB_BILLBOARD */
#endif /* _USB_DEVICE */
#ifdef _RTOS
#include "cmsis_os.h"
#else
#include "stm32g4xx.h"
#endif
#include "demo_application.h"
#if defined(_FWUPDATE_RESPONDER)||defined(_FWUPDATE_INITIATOR)
#if defined(_FWUPDATE_INITIATOR)
#include "usbpd_pdfu_initiator.h"
#endif /* _FWUPDATE_INITIATOR */
#if defined(_FWUPDATE_RESPONDER)
#include "usbpd_pdfu_responder.h"
#endif /* _FWUPDATE_RESPONDER */
#include "usbpd_pdfu_user.h"
#endif /* _FWUPDATE_RESPONDER)|| _FWUPDATE_INITIATOR */

/** @addtogroup STM32_USBPD_APPLICATION
  * @{
  */

/** @addtogroup STM32_USBPD_APPLICATION_DPM_USER
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN Private_Typedef */
#if defined(_SNK) || defined(_DRP)
/** @brief  Sink Request characteristics Structure definition
  *
  */
typedef struct
{
  uint32_t RequestedVoltageInmVunits;              /*!< Sink request operating voltage in mV units       */
  uint32_t MaxOperatingCurrentInmAunits;           /*!< Sink request Max operating current in mA units   */
  uint32_t OperatingCurrentInmAunits;              /*!< Sink request operating current in mA units       */
  uint32_t MaxOperatingPowerInmWunits;             /*!< Sink request Max operating power in mW units     */
  uint32_t OperatingPowerInmWunits;                /*!< Sink request operating power in mW units         */
} USBPD_DPM_SNKPowerRequestDetails_TypeDef;
#endif /*_SNK || _DRP */
/* USER CODE END Private_Typedef */

/* Private define ------------------------------------------------------------*/
/** @defgroup USBPD_USER_PRIVATE_DEFINES USBPD USER Private Defines
  * @{
  */
/* USER CODE BEGIN Private_Define */

#define DPM_TIMER_ENABLE_MSK      ((uint16_t)0x8000U)       /*!< Enable Timer Mask                                                        */
#define DPM_TIMER_READ_MSK        ((uint16_t)0x7FFFU)       /*!< Read Timer Mask                                                          */

#define DPM_BOX_MESSAGES_MAX      30u
#if defined(USB_BILLBOARD)

/* Timer used to start billboard driver (based on tAMETimeout */
#define DPM_TIMER_AME             1000u  /*!< 1000ms: The time between a Sink attach until a
                                                USB Billboard Device Class interface is
                                                exposed when an Alternate Mode is
                                                not successfully entered*/
#endif /* USB_BILLBOARD */
#if _SRC_CAPA_EXT

/* Timer used to send a GET_SRC_CAPA_EXT*/
#define DPM_TIMER_GET_SRC_CAPA_EXT  300u  /*!< 300ms */
#endif /* GENERATOR_SRC_CAPA_EXT_SUPPORT_YES */
#if _ALERT

/* Timer used to check if need to send an alert */
#define DPM_TIMER_ALERT             100u  /*!< 100ms */
#endif /* GENERATOR_ALERT_SUPPORT_YES */

#define DPM_NO_SRC_PDO_FOUND      0xFFU        /*!< No match found between Received SRC PDO and SNK capabilities                             */

#define DPM_GUI_NOTIF_ISCONNECTED       (1 << 5)
#define DPM_GUI_NOTIF_POWER_EVENT       (1 << 15)

/* USER CODE END Private_Define */
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/** @defgroup USBPD_USER_PRIVATE_MACROS USBPD USER Private Macros
  * @{
  */
/* USER CODE BEGIN Private_Macro */
#if defined(_RTOS)
#define DPM_START_TIMER(_PORT_,_TIMER_,_TIMEOUT_)   do{                                                               \
                                                      DPM_Ports[_PORT_]._TIMER_ = (_TIMEOUT_) |  DPM_TIMER_ENABLE_MSK;\
                                                      osMessagePut(DPMMsgBox,DPM_USER_EVENT_TIMER, 0);                \
                                                    }while(0);
#else
#define DPM_START_TIMER(_PORT_, _TIMER_,_TIMEOUT_)  DPM_Ports[_PORT_]._TIMER_ = (_TIMEOUT_) |  DPM_TIMER_ENABLE_MSK;
#endif /* _RTOS */
#define IS_DPM_TIMER_RUNNING(_PORT_, _TIMER_)       ((DPM_Ports[_PORT_]._TIMER_ & DPM_TIMER_READ_MSK) > 0)
#define IS_DPM_TIMER_EXPIRED(_PORT_, _TIMER_)       (DPM_TIMER_ENABLE_MSK == DPM_Ports[_PORT_]._TIMER_)

#if defined(_DEBUG_TRACE)
#define __DPM_DEBUG_CALLBACK(_PORT_, __MESSAGE__)  USBPD_TRACE_Add(USBPD_TRACE_DEBUG,    (_PORT_), 0u,(__MESSAGE__), sizeof(__MESSAGE__) - 1u)
#else
#define __DPM_DEBUG_CALLBACK(_PORT_, __MESSAGE__)
#endif /* _DEBUG_TRACE */

/* USER CODE END Private_Macro */
/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup USBPD_USER_PRIVATE_VARIABLES USBPD USER Private Variables
  * @{
  */
/* USER CODE BEGIN Private_Variables */
extern USBPD_ParamsTypeDef DPM_Params[USBPD_PORT_COUNT];

#ifdef _RTOS
osMessageQId  DPMMsgBox;
#endif /* _RTOS */

#if defined(_VDM)
extern uint8_t VDM_Mode_On[USBPD_PORT_COUNT];
#endif /* _VDM */

#if defined(_USB_DEVICE)
USBD_HandleTypeDef hUsbDevice[USBPD_PORT_COUNT];
#endif /* _USB_DEVICE */

#if defined(USBPD_REV30_SUPPORT) && _ALERT && _PPS
extern USBPD_PWR_Port_PDO_Storage_TypeDef PWR_Port_PDO_Storage[USBPD_PORT_COUNT];
#endif /* USBPD_REV30_SUPPORT && _ALERT && _PPS */

GUI_NOTIFICATION_POST         DPM_GUI_PostNotificationMessage   = NULL;
GUI_NOTIFICATION_FORMAT_SEND  DPM_GUI_FormatAndSendNotification = NULL;
GUI_SAVE_INFO                 DPM_GUI_SaveInfo                  = NULL;

/* USER CODE END Private_Variables */
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @defgroup USBPD_USER_PRIVATE_FUNCTIONS USBPD USER Private Functions
  * @{
  */
/* USER CODE BEGIN USBPD_USER_PRIVATE_FUNCTIONS_Prototypes */
#if defined(_SNK) || defined(_DRP)
static  void DPM_SNK_BuildRDOfromSelectedPDO(uint8_t PortNum, uint8_t IndexSrcPDO, USBPD_DPM_SNKPowerRequestDetails_TypeDef* PtrRequestPowerDetails,
                                             USBPD_SNKRDO_TypeDef* Rdo, USBPD_CORE_PDO_Type_TypeDef *PtrPowerObject);
static uint32_t DPM_FindVoltageIndex(uint32_t PortNum, USBPD_DPM_SNKPowerRequestDetails_TypeDef* PtrRequestPowerDetails);
#endif /*_SNK || _DRP */
#if defined(_SRC) || defined(_DRP)
static USBPD_StatusTypeDef DPM_TurnOnPower(uint8_t PortNum, USBPD_PortPowerRole_TypeDef Role);
static USBPD_StatusTypeDef DPM_TurnOffPower(uint8_t PortNum, USBPD_PortPowerRole_TypeDef Role);
static void                DPM_AssertRp(uint8_t PortNum);
#endif /* _SRC || _DRP */
#if defined(_DRP)
static void DPM_AssertRd(uint8_t PortNum);
#endif /* _DRP */
#if defined(_RTOS)
static uint32_t CheckDPMTimers(void);
#endif /* _RTOS */

#if defined(USBPD_REV30_SUPPORT)
#if _SRC_CAPA_EXT
static void DPM_ManageExtendedCapa(void);
#endif /* _SRC_CAPA_EXT */
#if _ALERT
static void DPM_ManageAlert(void);
#endif /* _ALERT */
#endif /* USBPD_REV30_SUPPORT */


#if defined(_USB_DEVICE) || defined(_USB_HOST)
static USBPD_StatusTypeDef DPM_USB_Init(void);
static void DPM_USB_Start(uint32_t PortNum);
static void DPM_USB_Stop(uint32_t PortNum);
#endif /* _USB_DEVICE || _USB_HOST */
/* USER CODE END USBPD_USER_PRIVATE_FUNCTIONS_Prototypes */
/**
  * @}
  */

/* Exported functions ------- ------------------------------------------------*/
/** @defgroup USBPD_USER_EXPORTED_FUNCTIONS USBPD USER Exported Functions
  * @{
  */
/* USER CODE BEGIN USBPD_USER_EXPORTED_FUNCTIONS */

/* USER CODE END USBPD_USER_EXPORTED_FUNCTIONS */

/** @defgroup USBPD_USER_EXPORTED_FUNCTIONS_GROUP1 USBPD USER Exported Functions called by DPM CORE
  * @{
  */
/* USER CODE BEGIN USBPD_USER_EXPORTED_FUNCTIONS_GROUP1 */

/* USER CODE END USBPD_USER_EXPORTED_FUNCTIONS_GROUP1 */

/**
  * @brief  Initialize DPM (port power role, PWR_IF, CAD and PE Init procedures)
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_UserInit(void)
{
/* USER CODE BEGIN USBPD_DPM_UserInit */
  /* Demo initialisation */
  DEMO_InitBSP();

#ifdef _USB_DEVICE
  if(USBPD_OK !=  DPM_USB_Init())
  {
      return USBPD_ERROR;
  }
#endif

  /* PWR SET UP */
  if(USBPD_OK !=  USBPD_PWR_IF_Init())
  {
    return USBPD_ERROR;
  }

 /* VDM initialisation */
#ifdef _VDM
  if(USBPD_TRUE == DPM_Settings[USBPD_PORT_0].PE_VDMSupport)
  {
    if (USBPD_OK != USBPD_VDM_UserInit(USBPD_PORT_0))
    {
      return USBPD_ERROR;
    }
  }
#if USBPD_PORT_COUNT == 2
  if(USBPD_TRUE == DPM_Settings[USBPD_PORT_1].PE_VDMSupport)
  {
    if (USBPD_OK != USBPD_VDM_UserInit(USBPD_PORT_1))
    {
      return USBPD_ERROR;
    }
  }
#endif /* USBPD_PORT_COUNT == 2 */
#elif defined(_VCONN_SUPPORT)
  if (USBPD_OK != USBPD_VDM_UserInit(USBPD_PORT_0))
  {
    return USBPD_ERROR;
  }
#if USBPD_PORT_COUNT == 2
  if (USBPD_OK != USBPD_VDM_UserInit(USBPD_PORT_1))
  {
    return USBPD_ERROR;
  }
#endif /* USBPD_PORT_COUNT == 2 */
#endif /* _VDM */

  DEMO_InitTask();

#if defined(_FWUPDATE_RESPONDER)||defined(_FWUPDATE_INITIATOR)
  USBPD_PDFU_UserInit(USBPD_PORT_0, &DPM_Params[USBPD_PORT_0]);
#endif /* _FWUPDATE_RESPONDER)|| _FWUPDATE_INITIATOR */
#ifdef _RTOS
  osMessageQDef(MsgBox, DPM_BOX_MESSAGES_MAX, uint32_t);
  DPMMsgBox = osMessageCreate(osMessageQ(MsgBox), NULL);
  osThreadDef(DPM, USBPD_DPM_UserExecute, osPriorityLow, 0, 300);

  if(NULL == osThreadCreate(osThread(DPM), &DPMMsgBox))
  {
    return USBPD_ERROR;
  }
#else
#endif /* _RTOS */

  return USBPD_OK;
/* USER CODE END USBPD_DPM_UserInit */
}

/**
  * @brief  Function to set the function ptr linked to GUI interface
  * @param  PtrFormatSend Pointer on function to format and send GUI notifications
  * @param  PtrPost       Pointer on function to send GUI notifications
  * @param  PtrSaveInfo   Pointer on function to save information from Port Partner
  * @retval None
  */
void USBPD_DPM_SetNotification_GUI(GUI_NOTIFICATION_FORMAT_SEND PtrFormatSend, GUI_NOTIFICATION_POST PtrPost, GUI_SAVE_INFO PtrSaveInfo)
{
  DPM_GUI_PostNotificationMessage   = PtrPost;
  DPM_GUI_FormatAndSendNotification = PtrFormatSend;
  DPM_GUI_SaveInfo                  = PtrSaveInfo;
}

/**
  * @brief  User delay implementation which is OS dependent
  * @param  Time time in ms
  * @retval None
  */
void USBPD_DPM_WaitForTime(uint32_t Time)
{
/* USER CODE BEGIN USBPD_DPM_WaitForTime */
#ifdef _RTOS
  osDelay(Time);
#else
  HAL_Delay(Time);
#endif
/* USER CODE END USBPD_DPM_WaitForTime */
}

/**
  * @brief  User processing time, it is recommended to avoid blocking task for long time
  * @param  argument  DPM User event
  * @retval None
  */
void USBPD_DPM_UserExecute(void const *argument)
{
/* USER CODE BEGIN USBPD_DPM_UserExecute */
  /* User code implementation */
#ifdef _RTOS
  uint32_t _timing = osWaitForever;
  osMessageQId  queue = *(osMessageQId *)argument;

  do
  {
    osEvent event = osMessageGet(queue, _timing);
    switch (((DPM_USER_EVENT)event.value.v & 0xF))
    {
    case DPM_USER_EVENT_TIMER:
      {
#endif /* _RTOS */

#if defined(USB_DEVICE) || defined(USB_HOST)
        /* This function is used only in the billboard case to start the AME delay */
        #warning "to rework"
        DPM_USB_Start(USBPD_PORT_0);
#endif /* USB_DEVICE || USB_HOST */

#if defined(USBPD_REV30_SUPPORT)
#if _SRC_CAPA_EXT
        /* Manage the extended capa */
        DPM_ManageExtendedCapa();

#endif /* _SRC_CAPA_EXT */
#if _ALERT
        DPM_ManageAlert();

#endif /* _ALERT */
#endif /* USBPD_REV30_SUPPORT */
#ifdef _RTOS
        break;
      }

    default:
      break;
    }
    _timing = CheckDPMTimers();
  }
  while(1);
#else

#if defined(_FWUPDATE_RESPONDER)
  PDFU_Task(USBPD_PORT_0, PDFU_EVENT_NONE);
#endif /* _FWUPDATE_RESPONDER */
#endif /* _RTOS */
/* USER CODE END USBPD_DPM_UserExecute */
}

/**
  * @brief  UserCableDetection reporting events on a specified port from CAD layer.
  * @param  PortNum The handle of the port
  * @param  State   CAD state
  * @retval None
  */
void USBPD_DPM_UserCableDetection(uint8_t PortNum, USBPD_CAD_EVENT State)
{
/* USER CODE BEGIN USBPD_DPM_UserCableDetection */
  DEMO_PostCADMessage(PortNum, State, DPM_Params[PortNum].ActiveCCIs);

  switch(State)
  {
  case USBPD_CAD_EVENT_ATTEMC:
#if defined(_VCONN_SUPPORT)
    /* Save flag to keep information that cable is a EMC. */
    DPM_Ports[PortNum].DPM_CablePDCapable = USBPD_TRUE;
#endif /* _VCONN_SUPPORT */
  case USBPD_CAD_EVENT_ATTACHED:
    /* Format and send a notification to GUI if enabled */
    if (NULL != DPM_GUI_FormatAndSendNotification)
    {
      DPM_GUI_FormatAndSendNotification(PortNum, DPM_GUI_NOTIF_ISCONNECTED, 0);
    }

#ifdef _VDM
    USBPD_VDM_UserReset(PortNum);
#endif /* _VDM */
#if defined(_SRC) || defined(_DRP)
    if(USBPD_PORTPOWERROLE_SRC == DPM_Params[PortNum].PE_PowerRole)
    {
#if !defined(USBPD_TCPM_MODULE_ENABLED)
      USBPD_DPM_WaitForTime(110);
      if (USBPD_OK != USBPD_PWR_IF_VBUSEnable(PortNum))
      {
        /* display the error */
        DEMO_PostMMIMessage(DEMO_MMI_ACTION_ERROR_POWER);

        /* Should not occur */
        USBPD_DPM_WaitForTime(6000);
        NVIC_SystemReset();
      }
#endif /* !USBPD_TCPM_MODULE_ENABLED*/
    }
#endif /* _SRC || _DRP */
    DPM_Ports[PortNum].DPM_IsConnected = 1;

#if defined(_USB_DEVICE) || defined(_USB_HOST)
    DPM_USB_Start(PortNum);
#endif
    break;

  case USBPD_CAD_EVENT_DETACHED :
  case USBPD_CAD_EVENT_EMC :
  default :

#if defined(_USB_DEVICE) || defined(_USB_HOST)
    /* STOP must be executed before the DPM_Ports Reset */
    DPM_USB_Stop(PortNum);
#endif
    /* reset all values received from port partner */
    memset(&DPM_Ports[PortNum], 0, sizeof(DPM_Ports[PortNum]));

    /* Format and send a notification to GUI if enabled */
    if (NULL != DPM_GUI_FormatAndSendNotification)
    {
      DPM_GUI_FormatAndSendNotification(PortNum, DPM_GUI_NOTIF_ISCONNECTED | DPM_GUI_NOTIF_POWER_EVENT, 0);
    }

#ifdef _VDM
    USBPD_VDM_UserReset(PortNum);
#endif /* _VDM */
#if defined(USBPD_TCPM_MODULE_ENABLED)
#else
#if defined(_SRC) || defined(_DRP)
    if(USBPD_PORTPOWERROLE_SRC == DPM_Params[PortNum].PE_PowerRole)
    {
      if (USBPD_OK != USBPD_PWR_IF_VBUSDisable(PortNum))
      {
        /* Should not occur */
        while(1);
      }
    }
#endif /* _SRC || _DRP */
#endif /* !USBPD_TCPM_MODULE_ENABLED*/

#if defined(_VCONN_SUPPORT)
    if(USBPD_TRUE == DPM_Params[PortNum].VconnStatus)
    {
      /* Switch Off Vconn */
      USBPD_DPM_PE_VconnPwr(PortNum, USBPD_DISABLE);
    }
#endif /* _VCONN_SUPPORT */
#if defined(_FWUPDATE_INITIATOR)||defined(_FWUPDATE_RESPONDER)
#if defined(_FWUPDATE_INITIATOR)
    /* Kill the PDFU task*/
    USBPD_PDFU_INIT_TerminateTask(PortNum);
#endif /* _FWUPDATE_INITIATOR */
#if defined(_FWUPDATE_RESPONDER)
    /* Kill the PDFU task*/
    USBPD_PDFU_TerminateTask(PortNum);
#endif /* _FWUPDATE_RESPONDER */
#endif /* _FWUPDATE_INITIATOR || _FWUPDATE_RESPONDER */
    break;
  }
/* USER CODE END USBPD_DPM_UserCableDetection */
}

/**
  * @brief  function used to manage user timer.
  * @param  PortNum Port number
  * @retval None
  */
void USBPD_DPM_UserTimerCounter(uint8_t PortNum)
{
/* USER CODE BEGIN USBPD_DPM_UserTimerCounter */
#ifdef USB_BILLBOARD
  if((DPM_Ports[PortNum].DPM_TimerAME & DPM_TIMER_READ_MSK) > 0)
  {
    DPM_Ports[PortNum].DPM_TimerAME--;
  }
#endif /* USB_BILLBOARD */

#ifdef USBPD_REV30_SUPPORT
#if _SRC_CAPA_EXT
  if((DPM_Ports[PortNum].DPM_TimerSRCExtendedCapa & DPM_TIMER_READ_MSK) > 0)
  {
    DPM_Ports[PortNum].DPM_TimerSRCExtendedCapa--;
  }
#endif /* _SRC_CAPA_EXT */
#if _ALERT
  if((DPM_Ports[PortNum].DPM_TimerAlert & DPM_TIMER_READ_MSK) > 0)
  {
    DPM_Ports[PortNum].DPM_TimerAlert--;
  }
#endif /* _ALERT */
#endif /* USBPD_REV30_SUPPORT */
/* USER CODE END USBPD_DPM_UserTimerCounter */
}

/**
  * @}
  */

/** @defgroup USBPD_USER_EXPORTED_FUNCTIONS_GROUP2 USBPD USER Exported Callbacks functions called by PE
  * @{
  */

/**
  * @brief  Callback function called by PE layer when HardReset message received from PRL
  * @param  PortNum     The current port number
  * @param  CurrentRole the current role
  * @param  Status      status on hard reset event
  * @retval None
  */
void USBPD_DPM_HardReset(uint8_t PortNum, USBPD_PortPowerRole_TypeDef CurrentRole, USBPD_HR_Status_TypeDef Status)
{
/* USER CODE BEGIN USBPD_DPM_HardReset */
#ifdef _VDM
  USBPD_VDM_UserReset(PortNum);
#endif /* _VDM */
#if defined(USBPD_REV30_SUPPORT) && _ALERT
  /* Stop Alert timer */
  DPM_Ports[PortNum].DPM_TimerAlert    = 0;
  DPM_Ports[PortNum].DPM_SendAlert.d32 = 0;
#endif /* USBPD_REV30_SUPPORT && _ALERT */
#if defined(_VCONN_SUPPORT)
  /* Reset flag DPM_CableResetOnGoing */
  DPM_Ports[PortNum].DPM_CableResetOnGoing = USBPD_FALSE;
#endif /* _VCONN_SUPPORT */

  switch (Status)
  {
#if defined(_SRC) || defined(_DRP)
  case USBPD_HR_STATUS_WAIT_VBUS_VSAFE0V:
    if (USBPD_PORTPOWERROLE_SRC == CurrentRole)
    {
      /* Reset the power supply */
      DPM_TurnOffPower(PortNum, USBPD_PORTPOWERROLE_SRC);
    }
    break;
  case USBPD_HR_STATUS_WAIT_VBUS_VSAFE5V:
    if (CurrentRole == USBPD_PORTPOWERROLE_SRC)
    {
#if defined(_VCONN_SUPPORT)
      /* When VCONN is supported, the Hard Reset Shall cause the Port with the Rp resistor asserted to supply VCONN */
      if (USBPD_TRUE == DPM_Ports[PortNum].DPM_CablePDCapable)
      {
        DPM_Params[PortNum].VconnStatus = USBPD_TRUE;
      }
#endif /* _VCONN_SUPPORT */
      /* Power on the power supply */
      DPM_TurnOnPower(PortNum, CurrentRole);
    }
    break;
#endif /* _SRC || _DRP */
  case USBPD_HR_STATUS_COMPLETED:
    break;
  case USBPD_HR_STATUS_FAILED:
    break;
  default:
      break;
  }
/* USER CODE END USBPD_DPM_HardReset */
}

#if defined(_SRC) || defined(_DRP)
/**
  * @brief  Request the DPM to setup the new power level.
  * @param  PortNum The current port number
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_DPM_SetupNewPower(uint8_t PortNum)
{
/* USER CODE BEGIN USBPD_DPM_SetupNewPower */
  return  USBPD_PWR_IF_SetProfile(PortNum);
/* USER CODE END USBPD_DPM_SetupNewPower */
}

#endif /* _SRC || _DRP */
/**
  * @brief  Evaluate the swap request from PE.
  * @param  PortNum The current port number
  * @retval USBPD_ACCEPT, USBPD_WAIT, USBPD_REJECT
  */
USBPD_StatusTypeDef USBPD_DPM_EvaluatePowerRoleSwap(uint8_t PortNum)
{
/* USER CODE BEGIN USBPD_DPM_EvaluatePowerRoleSwap */
  return USBPD_ACCEPT;
/* USER CODE END USBPD_DPM_EvaluatePowerRoleSwap */
}

/**
  * @brief  Callback function called by PE to inform DPM about PE event.
  * @param  PortNum The current port number
  * @param  EventVal @ref USBPD_NotifyEventValue_TypeDef
  * @retval None
  */
void USBPD_DPM_Notification(uint8_t PortNum, USBPD_NotifyEventValue_TypeDef EventVal)
{
/* USER CODE BEGIN USBPD_DPM_Notification */
  DEMO_PostNotificationMessage(PortNum, EventVal);

  switch(EventVal)
  {
    /***************************************************************************
                              Power Notification
    */
    case USBPD_NOTIFY_POWER_EXPLICIT_CONTRACT :
    {
      /* Power ready means an explicit contract has been establish and Power is available */
#if defined(_VDM)
      /* Request VDM identify only if not already entered in VDM mode */
      if ((0 == VDM_Mode_On[PortNum]) && (USBPD_PORTDATAROLE_DFP == DPM_Params[PortNum].PE_DataRole))
      {
        USBPD_PE_SVDM_RequestIdentity(PortNum, USBPD_SOPTYPE_SOP);
      }
#endif /*_VDM*/
#if defined(USBPD_REV30_SUPPORT)
#if _ALERT
#if defined(_SRC) || defined(_DRP)
      if ((USBPD_SPECIFICATION_REV3 == DPM_Params[PortNum].PE_SpecRevision)
       && (USBPD_PORTPOWERROLE_SRC == DPM_Params[PortNum].PE_PowerRole))
      {
        DPM_START_TIMER(PortNum, DPM_TimerAlert, DPM_TIMER_ALERT);
      }
#endif /* _SRC || _DRP */
#endif /* _ALERT */
#if defined(_FWUPDATE_INITIATOR)||defined(_FWUPDATE_RESPONDER)
#if defined(_FWUPDATE_INITIATOR)
      /* Start the PDFU initiator task*/
      USBPD_PDFU_INIT_CreateTask(PortNum);
#endif /* _FWUPDATE_INITIATOR */
#if defined(_FWUPDATE_RESPONDER)
      /* Start the PDFU Responder task*/
      USBPD_PDFU_CreateTask(PortNum);
      /* Send an UVDM message to indicate initiator that CC loader is ready */
      USBPD_DPM_RequestUVDMMessage(PortNum, USBPD_SOPTYPE_SOP);
#endif /* _FWUPDATE_RESPONDER */
#endif /* _FWUPDATE_INITIATOR || _FWUPDATE_RESPONDER */
#endif /* USBPD_REV30_SUPPORT */
      break;
    }
    /*
                              End Power Notification
     ***************************************************************************/
#if defined(_SNK) || defined(_DRP)
    /***************************************************************************
                               REQUEST ANSWER NOTIFICATION
    */
    case USBPD_NOTIFY_REQUEST_ACCEPTED:
      /* Update DPM_RDOPosition only if current role is SNK */
      if (USBPD_PORTPOWERROLE_SNK == DPM_Params[PortNum].PE_PowerRole)
      {
        USBPD_SNKRDO_TypeDef rdo;
        rdo.d32                                   = DPM_Ports[PortNum].DPM_RequestDOMsg;
        DPM_Ports[PortNum].DPM_RDOPosition        = rdo.GenericRDO.ObjectPosition;
        if (NULL != DPM_GUI_SaveInfo)
        {
          DPM_GUI_SaveInfo(PortNum, USBPD_CORE_DATATYPE_RDO_POSITION, (uint8_t*)&DPM_Ports[PortNum].DPM_RDOPosition, 4);
        }
      }
    break;
    /*
                              End REQUEST ANSWER NOTIFICATION
     ***************************************************************************/
#endif /* _SNK || _DRP */
#if defined(_VDM) || defined(_FWUPDATE_INITIATOR)
    case USBPD_NOTIFY_HARDRESET_RX:
    case USBPD_NOTIFY_HARDRESET_TX:
#if defined(_VDM)
      if (USBPD_PORTPOWERROLE_SNK == DPM_Params[PortNum].PE_PowerRole)
      {
          USBPD_VDM_UserReset(PortNum);
      }
#endif /* _VDM */
#if defined(_FWUPDATE_INITIATOR)
      USBPD_PDFU_INIT_TerminateTask(PortNum);
#endif /* _FWUPDATE_INITIATOR */
#if defined(_FWUPDATE_RESPONDER)
      /* Kill the PDFU task*/
      USBPD_PDFU_TerminateTask(PortNum);
#endif /* _FWUPDATE_RESPONDER */
      break;
#endif /* _VDM || _FWUPDATE_INITIATOR */
#if defined(_SNK) || defined(_DRP)
    case USBPD_NOTIFY_STATE_SNK_READY:
      {
#if defined(USBPD_REV30_SUPPORT)
#if _SRC_CAPA_EXT
        /* Send GET_SRC_CAPA_EXT only if current revision is PD3.0 */
        if ((USBPD_SPECIFICATION_REV3 == DPM_Params[PortNum].PE_SpecRevision)
         && (1 == DPM_Ports[PortNum].FlagSendGetSrcCapaExtended))
        {
          DPM_START_TIMER(PortNum, DPM_TimerSRCExtendedCapa, DPM_TIMER_GET_SRC_CAPA_EXT);
          DPM_Ports[PortNum].FlagSendGetSrcCapaExtended = 0;
        }
#endif /* _SRC_CAPA_EXT*/
#endif /* USBPD_REV30_SUPPORT */
      }
      break;

#endif /* _SNK || _DRP */
    case USBPD_NOTIFY_STATE_SRC_DISABLED:
      {
        /* SINK Port Partner is not PD capable. Legacy cable may have been connected
           In this state, VBUS is set to 5V */
      }
      break;

#if defined(_USB_DEVICE) || defined(USB_HOST)
  case USBPD_NOTIFY_DATAROLESWAP_UFP :
  case USBPD_NOTIFY_DATAROLESWAP_DFP :
    {
      DPM_USB_Stop(PortNum);
      DPM_USB_Start(PortNum);
      break;
    }
#endif /* _USB */
#if defined(USBPD_REV30_SUPPORT)
#if _ALERT
  case USBPD_NOTIFY_ALERT_RECEIVED :
    {
#if _BATTERY
      uint32_t index = 0;
      /*!< Battery Status Change Event(Attach/Detach/charging/discharging/idle) */
      if (USBPD_ADO_TYPE_ALERT_BATTERY_STATUS == (DPM_Ports[PortNum].DPM_RcvAlert.b.TypeAlert & USBPD_ADO_TYPE_ALERT_BATTERY_STATUS))
      {
        USBPD_GBSDB_TypeDef get_battery_status;
        uint32_t battery_found = 0;

        /* Set default value for BatteryStatusRef
           (useful when no battery selected Fixed Batteries or Hot Swappable Batteries fields in ALERT message
            as in MQP implementation of Test PROT-SRC3-STATUS */
        get_battery_status.BatteryStatusRef = 0;

        for (index = 0; index < 4; index++)
        {
          if ((1 << index) == (DPM_Ports[PortNum].DPM_RcvAlert.b.FixedBatteries & (1 << index)))
          {
            get_battery_status.BatteryStatusRef = index;
            battery_found = 1;
            break;
          }
        }
        if (!battery_found)
        {
          for (index = 0; index < 4; index++)
          {
            if ((1 << index) == (DPM_Ports[PortNum].DPM_RcvAlert.b.HotSwappableBatteries & (1 << index)))
            {
              get_battery_status.BatteryStatusRef = index + 4;
              battery_found = 1;
              break;
            }
          }
        }

        /* Post a GET_BATTERY_STATUS message to PE */
        USBPD_DPM_RequestGetBatteryStatus(PortNum, (uint8_t*)&get_battery_status);

        /* Reset Battery Status bit */
        DPM_Ports[PortNum].DPM_RcvAlert.b.TypeAlert &= ~USBPD_ADO_TYPE_ALERT_BATTERY_STATUS;
        break;
      }
#endif /* _BATTERY */
      /*!< Over-Current Protection event when set (Source only, for Sink Reserved and Shall be set to zero) */
      /* Bit reserved for a Sink */
      if ((USBPD_PORTPOWERROLE_SNK == DPM_Params[PortNum].PE_PowerRole)
               && (USBPD_ADO_TYPE_ALERT_OCP == (DPM_Ports[PortNum].DPM_RcvAlert.b.TypeAlert & USBPD_ADO_TYPE_ALERT_OCP)))
      {
        /* Reset OCP bit */
        DPM_Ports[PortNum].DPM_RcvAlert.b.TypeAlert &= ~USBPD_ADO_TYPE_ALERT_OCP;
        /* Send a Get_Status */
        goto _ctrl_msg;
      }
      /*!< Over-Temperature Protection event when set. */
      else if (USBPD_ADO_TYPE_ALERT_OTP == (DPM_Ports[PortNum].DPM_RcvAlert.b.TypeAlert & USBPD_ADO_TYPE_ALERT_OTP))
      {
        /* Reset OTP bit */
        DPM_Ports[PortNum].DPM_RcvAlert.b.TypeAlert &= ~USBPD_ADO_TYPE_ALERT_OTP;
        /* Send a Get_Status */
        goto _ctrl_msg;
      }
      /*!< Operating Condition Change when set */
      else if (USBPD_ADO_TYPE_ALERT_OPERATING_COND == (DPM_Ports[PortNum].DPM_RcvAlert.b.TypeAlert & USBPD_ADO_TYPE_ALERT_OPERATING_COND))
      {
        /* Reset OP COND bit */
        DPM_Ports[PortNum].DPM_RcvAlert.b.TypeAlert &= ~USBPD_ADO_TYPE_ALERT_OPERATING_COND;
        /* Send a Get_Status */
        goto _ctrl_msg;
      }
      /*!< Source Input Change Event when set */
      else if (USBPD_ADO_TYPE_ALERT_SRC_INPUT == (DPM_Ports[PortNum].DPM_RcvAlert.b.TypeAlert & USBPD_ADO_TYPE_ALERT_SRC_INPUT))
      {
        /* Reset SRC input bit */
        DPM_Ports[PortNum].DPM_RcvAlert.b.TypeAlert &= ~USBPD_ADO_TYPE_ALERT_SRC_INPUT;
        /* Send a Get_Status */
        goto _ctrl_msg;
      }
      /*!< Over-Voltage Protection event when set (Sink only, for Source Reserved and Shall be set to zero) */
      /* Bit reserved for a Source */
      else
      {
#if defined(_SRC) || defined(_DRP)
        if ((USBPD_PORTPOWERROLE_SRC == DPM_Params[PortNum].PE_PowerRole)
               && (USBPD_ADO_TYPE_ALERT_OVP == (DPM_Ports[PortNum].DPM_RcvAlert.b.TypeAlert & USBPD_ADO_TYPE_ALERT_OVP)))
        {
          /* Reset OVP bit */
          DPM_Ports[PortNum].DPM_RcvAlert.b.TypeAlert &= ~USBPD_ADO_TYPE_ALERT_OVP;
          /* Send a Get_Status */
          goto _ctrl_msg;
        }
#endif /* _SRC || _DRP */
      }

      _ctrl_msg:
        /* Post GET_STATUS message */
        USBPD_DPM_RequestGetStatus(PortNum);
    }
    break;
#endif /* ALERT */
#endif /* USBPD_REV30_SUPPORT */
#if defined(_VCONN_SUPPORT)
    case USBPD_NOTIFY_CABLERESET_REQUESTED :
      {
        /* CABLE_RESET is requested by the PE but port is not in the correct configuration */
        /* Only a DFP Shall generate Cable Reset Signaling. A DFP Shall only generate Cable Reset Signaling within an Explicit Contract. */
        if ((USBPD_PORTDATAROLE_DFP == DPM_Params[PortNum].PE_DataRole) && (USBPD_FALSE == DPM_Params[PortNum].VconnStatus))
        {
          /* DFP is not VCONN source. Should request a VCONN Swap */
          USBPD_DPM_RequestVconnSwap(PortNum);
          DPM_Ports[PortNum].DPM_CableResetOnGoing = USBPD_TRUE;
        }
      }
      break;
    case USBPD_NOTIFY_VCONN_SWAP_COMPLETE :
      {
        /* VCONN Swap received in DFP. Now DFP is VCONN Source. Can send the CABLE_RESET  */
        if ((USBPD_TRUE == DPM_Ports[PortNum].DPM_CableResetOnGoing) && (USBPD_TRUE == DPM_Params[PortNum].VconnStatus))
        {
          /* DFP is now VCONN source. Cable Reset request could be processed */
          USBPD_DPM_RequestCableReset(PortNum);
          DPM_Ports[PortNum].DPM_CableResetOnGoing = USBPD_FALSE;
        }
      }
      break;
#endif /* _VCONN_SUPPORT */
    default :
      break;
  }

  /* Forward PE notifications to GUI if enabled */
  if (NULL != DPM_GUI_PostNotificationMessage)
  {
    DPM_GUI_PostNotificationMessage(PortNum, EventVal);
  }
/* USER CODE END USBPD_DPM_Notification */
}

/**
  * @brief  DPM callback to allow PE to retrieve information from DPM/PWR_IF.
  * @param  PortNum Port number
  * @param  DataId  Type of data to be updated in DPM based on @ref USBPD_CORE_DataInfoType_TypeDef
  * @param  Ptr     Pointer on address where DPM data should be written (u8 pointer)
  * @param  Size    Pointer on nb of u8 written by DPM
  * @retval None
  */
void USBPD_DPM_GetDataInfo(uint8_t PortNum, USBPD_CORE_DataInfoType_TypeDef DataId, uint8_t *Ptr, uint32_t *Size)
{
/* USER CODE BEGIN USBPD_DPM_GetDataInfo */
  /* Check type of information targeted by request */
  switch (DataId)
  {
    /* Case Port Source PDO Data information :
    Case Port SINK PDO Data information :
    Call PWR_IF PDO reading request.
    */
  case USBPD_CORE_DATATYPE_SRC_PDO :
  case USBPD_CORE_DATATYPE_SNK_PDO :
    USBPD_PWR_IF_GetPortPDOs(PortNum, DataId, Ptr, Size);
    *Size *= 4;
    break;

    /* Case Requested voltage value Data information */
  case USBPD_CORE_DATATYPE_REQ_VOLTAGE :
    *Size = 4;
    (void)memcpy((uint8_t*)Ptr, (uint8_t *)&DPM_Ports[PortNum].DPM_RequestedVoltage, *Size);
    break;

#if defined(USBPD_REV30_SUPPORT)
#if _PPS
  case USBPD_CORE_PPS_STATUS :
    {
      /* Get current drawn by sink */
      USBPD_PPSSDB_TypeDef pps_status = {0};

      /* Disable VBUS & IBUS Measurements */
      pps_status.fields.OutputVoltageIn20mVunits  = 0xFFFF;
      pps_status.fields.OutputCurrentIn50mAunits  = 0xFF;
      pps_status.fields.RealTimeFlags             = USBPD_PPS_REALTIMEFLAGS_PTF_NOT_SUPPORTED | USBPD_PPS_REALTIMEFLAGS_OMF_DISABLED;

      *Size = 4;
      (void)memcpy((uint8_t*)Ptr, (uint8_t *)&pps_status.d32, *Size);
    }
    break;
#endif /* _PPS */
#if _SRC_CAPA_EXT && (defined(_SRC)||defined(_DRP))
  case USBPD_CORE_EXTENDED_CAPA :
    {
      *Size = sizeof(USBPD_SCEDB_TypeDef);
      memcpy((uint8_t*)Ptr, (uint8_t *)&DPM_USER_Settings[PortNum].DPM_SRCExtendedCapa, *Size);
    }
    break;
#endif /* _SRC_CAPA_EXT && (_SRC || _DRP) */
#if defined(_SNK)||defined(_DRP)
  case USBPD_CORE_SNK_EXTENDED_CAPA :
    {
      *Size = sizeof(USBPD_SKEDB_TypeDef);
      memcpy((uint8_t*)Ptr, (uint8_t *)&DPM_USER_Settings[PortNum].DPM_SNKExtendedCapa, *Size);
     }
     break;
#endif /* _SNK || _DRP */
#if _STATUS
  case USBPD_CORE_INFO_STATUS :
    {
      USBPD_SDB_TypeDef  infostatus = {
        .InternalTemp = 0,          /*!< Source or Sink internal temperature in degrees centigrade */
        .PresentInput = 0,          /*!< Present Input                                             */
        .PresentBatteryInput = 0,   /*!< Present Battery Input                                     */
        .EventFlags = 0,            /*!< Event Flags                                               */
        .TemperatureStatus = 0,     /*!< Temperature                                               */
        .PowerStatus = 0,           /*!< Power Status based on combination of @ref USBPD_SDB_POWER_STATUS*/
      };

      *Size = sizeof(USBPD_SDB_TypeDef);
      memcpy((uint8_t *)Ptr, &infostatus, *Size);
    }
    break;
#endif /* _STATUS */
#if _MANU_INFO
  case USBPD_CORE_MANUFACTURER_INFO :
    {
      USBPD_MIDB_TypeDef* manu_info;
      /* Manufacturer Info Target must be a range 0..1 */
      /* Manufacturer Info Ref must be a range 0..7    */
      if((DPM_Ports[PortNum].DPM_GetManufacturerInfo.ManufacturerInfoTarget > USBPD_MANUFINFO_TARGET_BATTERY)
      || (DPM_Ports[PortNum].DPM_GetManufacturerInfo.ManufacturerInfoRef > USBPD_MANUFINFO_REF_MAX_VALUES))
      {
        /* If the Manufacturer Info Target field or Manufacturer Info Ref field in the Get_Manufacturer_Info Message is
           unrecognized the field Shall return a null terminated ascii text string "Not Supported".*/
        char *_notsupported = "Not Supported\0";
        *Size = 4 + 14; /* VID (2) + .PID(2) + sizeof("Not Supported\0")*/
        /* Copy Manufacturer Info into data area for transmission */
        manu_info = (USBPD_MIDB_TypeDef*)&DPM_USER_Settings[PortNum].DPM_ManuInfoPort;
        memcpy((uint8_t*)Ptr, (uint8_t *)manu_info, 4);
        memcpy((uint8_t*)(Ptr + 4), (uint8_t *)_notsupported, 14);
      }
      else
      {
        if (USBPD_MANUFINFO_TARGET_PORT_CABLE_PLUG == DPM_Ports[PortNum].DPM_GetManufacturerInfo.ManufacturerInfoTarget)
        {
          /* Manufacturer info requested for the port */
          /* VID(2) + .PID(2) + .ManuString("STMicroelectronics") */
          *Size = 4 + strlen((char*)(DPM_USER_Settings[PortNum].DPM_ManuInfoPort.ManuString));
          /* Copy Manufacturer Info into data area for transmission */
          manu_info = (USBPD_MIDB_TypeDef*)&DPM_USER_Settings[PortNum].DPM_ManuInfoPort;
          memcpy((uint8_t*)Ptr, (uint8_t *)manu_info, *Size);
        }
        else
        {
          /* Manufacturer info requested for the battery (not available yet) */
          /* If the Manufacturer Info Target field or Manufacturer Info Ref field in the Get_Manufacturer_Info Message is
             unrecognized the field Shall return a null terminated ascii text string "Not Supported".*/
          char *_notsupported = "Not Supported\0";
          *Size = 4 + 14; /* VID (2) + .PID(2) + sizeof("Not Supported\0")*/
          /* Copy Manufacturer Info into data area for transmission */
          manu_info = (USBPD_MIDB_TypeDef*)&DPM_USER_Settings[PortNum].DPM_ManuInfoPort;
          memcpy((uint8_t*)Ptr, (uint8_t *)manu_info, 4);
          memcpy((uint8_t*)(Ptr + 4), (uint8_t *)_notsupported, 14);
        }
      }
    }
    break;
#endif /* _MANU_INFO */
#if _BATTERY
  case USBPD_CORE_BATTERY_STATUS:
    {
      USBPD_BSDO_TypeDef  battery_status;

      battery_status.d32 = 0;
#if _SRC_CAPA_EXT && (defined(_SRC)||defined(_DRP))
      if ((DPM_Ports[PortNum].DPM_GetBatteryStatus.BatteryStatusRef < 8)
        && (DPM_Ports[PortNum].DPM_GetBatteryStatus.BatteryStatusRef < DPM_USER_Settings[PortNum].DPM_SRCExtendedCapa.NbBatteries))
      {
        battery_status.b.BatteryPC    = 0xABCDu;
        battery_status.b.BatteryInfo  = USBPD_BSDO_BATT_INFO_BATT_PRESENT | USBPD_BSDO_BATT_INFO_BATT_ISIDLE;
      }
      else
#endif /* _SRC_CAPA_EXT && (_SRC || _DRP) */
      {
        battery_status.b.BatteryPC    = 0xFFFFu;
        battery_status.b.BatteryInfo  = USBPD_BSDO_BATT_INFO_INVALID_REF;
      }
      *Size = 4;
      memcpy((uint8_t *)Ptr, (uint8_t *)&battery_status.d32, *Size);
    }
    break;

  case USBPD_CORE_BATTERY_CAPABILITY:
    {
#if _SRC_CAPA_EXT && (defined(_SRC)||defined(_DRP))
      if (0 != DPM_USER_Settings[PortNum].DPM_SRCExtendedCapa.NbBatteries)
      {
        *Size = sizeof(USBPD_BCDB_TypeDef);
        USBPD_BCDB_TypeDef  battery_capability = {
          .VID = USBPD_VID,                     /*!< Vendor ID (assigned by the USB-IF)         */
          .PID = USBPD_PID,                     /*!< Product ID (assigned by the manufacturer)  */
          .BatteryDesignCapa = 0xAABB,          /*!< Battery Design Capacity                    */
          .BatteryLastFullChargeCapa = 0xCCDD,  /*!< Battery last full charge capacity          */
          .BatteryType= 0,                      /*!< Battery Type                               */
        };
        if (DPM_Ports[PortNum].DPM_GetBatteryCapability.BatteryCapRef >= 8)
        {
          battery_capability.BatteryType  = 1;
        }

        memcpy((uint8_t *)Ptr, &battery_capability, *Size);
      }
      else
#endif /* _SRC_CAPA_EXT && (_SRC || _DRP) */
      {
        /* Set Size to 0 to send a not supported message */
        *Size = 0;
      }
    }
    break;
#endif /* _BATTERY */
#endif /* USBPD_REV30_SUPPORT */
  default :
    *Size = 0;
    break;
  }
/* USER CODE END USBPD_DPM_GetDataInfo */
}

/**
  * @brief  DPM callback to allow PE to update information in DPM/PWR_IF.
  * @param  PortNum Port number
  * @param  DataId  Type of data to be updated in DPM based on @ref USBPD_CORE_DataInfoType_TypeDef
  * @param  Ptr     Pointer on the data
  * @param  Size    Nb of bytes to be updated in DPM
  * @retval None
  */
void USBPD_DPM_SetDataInfo(uint8_t PortNum, USBPD_CORE_DataInfoType_TypeDef DataId, uint8_t *Ptr, uint32_t Size)
{
/* USER CODE BEGIN USBPD_DPM_SetDataInfo */
  uint32_t index;

  /* Check type of information targeted by request */
  switch (DataId)
  {
    /* Case requested DO position Data information :
    */
  case USBPD_CORE_DATATYPE_RDO_POSITION :
    if (Size == 4)
    {
      uint8_t* temp;
      temp = (uint8_t*)&DPM_Ports[PortNum].DPM_RDOPosition;
      (void)memcpy(temp, Ptr, Size);
      DPM_Ports[PortNum].DPM_RDOPositionPrevious = *Ptr;
      temp = (uint8_t*)&DPM_Ports[PortNum].DPM_RDOPositionPrevious;
      (void)memcpy(temp, Ptr, Size);
    }
    break;

    /* Case Received Source PDO values Data information :
    */
  case USBPD_CORE_DATATYPE_RCV_SRC_PDO :
    if (Size <= (USBPD_MAX_NB_PDO * 4))
    {
      uint8_t* rdo;
      DPM_Ports[PortNum].DPM_NumberOfRcvSRCPDO = (Size / 4);
      /* Copy PDO data in DPM Handle field */
      for (index = 0; index < (Size / 4); index++)
      {
        rdo = (uint8_t*)&DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index];
        (void)memcpy(rdo, (Ptr + (index * 4u)), (4u * sizeof(uint8_t)));
      }
    }
    break;

    /* Case Received Sink PDO values Data information :
    */
  case USBPD_CORE_DATATYPE_RCV_SNK_PDO :
    if (Size <= (USBPD_MAX_NB_PDO * 4))
    {
      uint8_t* rdo;
      DPM_Ports[PortNum].DPM_NumberOfRcvSNKPDO = (Size / 4);
      /* Copy PDO data in DPM Handle field */
      for (index = 0; index < (Size / 4); index++)
      {
        rdo = (uint8_t*)&DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index];
        (void)memcpy(rdo, (Ptr + (index * 4u)), (4u * sizeof(uint8_t)));
      }
    }
    break;

    /* Case Received Request PDO Data information :
    */
  case USBPD_CORE_DATATYPE_RCV_REQ_PDO :
    if (Size == 4)
    {
      uint8_t* rdo;
      rdo = (uint8_t*)&DPM_Ports[PortNum].DPM_RcvRequestDOMsg;
      (void)memcpy(rdo, Ptr, Size);
    }
    break;

    /* Case Request message DO (from Sink to Source) Data information :
    */
  case USBPD_CORE_DATATYPE_REQUEST_DO :
    if (Size == 4)
    {
      uint8_t* rdo;
      rdo = (uint8_t*)&DPM_Ports[PortNum].DPM_RcvRequestDOMsg;
      (void)memcpy(rdo, Ptr, Size);
    }
    break;

#if defined(USBPD_REV30_SUPPORT)
#if _STATUS
  case USBPD_CORE_INFO_STATUS :
    {
      uint8_t* info_status;
      info_status = (uint8_t*)&DPM_Ports[PortNum].DPM_RcvStatus;
      memcpy(info_status, Ptr, Size);
    }
    break;
#endif /* _STATUS */
#if _PPS
  case USBPD_CORE_PPS_STATUS :
    {
      uint8_t*  ext_capa;
      ext_capa = (uint8_t*)&DPM_Ports[PortNum].DPM_RcvPPSStatus;
      memcpy(ext_capa, Ptr, Size);
    }
    break;
#endif /* _PPS */
#if _SRC_CAPA_EXT
  case USBPD_CORE_EXTENDED_CAPA :
    {
      uint8_t*  ext_capa;
      ext_capa = (uint8_t*)&DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa;
      memcpy(ext_capa, Ptr, Size);
    }
    break;
#endif /* _SRC_CAPA_EXT */
  case USBPD_CORE_SNK_EXTENDED_CAPA :
    {
      uint8_t*  _snk_ext_capa;
      _snk_ext_capa = (uint8_t*)&DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa;
      memcpy(_snk_ext_capa, Ptr, Size);
    }
    break;
#if _MANU_INFO
  case USBPD_CORE_GET_MANUFACTURER_INFO:
    {
      uint8_t* temp = (uint8_t*)Ptr;
      DPM_Ports[PortNum].DPM_GetManufacturerInfo.ManufacturerInfoTarget = *temp;
      DPM_Ports[PortNum].DPM_GetManufacturerInfo.ManufacturerInfoRef    = *(temp + 1);
    }
    break;
#endif /* _MANU_INFO */
#if _BATTERY
  case USBPD_CORE_BATTERY_STATUS:
    DPM_Ports[PortNum].DPM_BatteryStatus.d32 = *Ptr;
    break;

  case USBPD_CORE_GET_BATTERY_STATUS:
    {
      DPM_Ports[PortNum].DPM_GetBatteryStatus.BatteryStatusRef = *(uint8_t*)Ptr;
    }
    break;

  case USBPD_CORE_GET_BATTERY_CAPABILITY:
    {
      uint8_t* temp = (uint8_t*)Ptr;
      DPM_Ports[PortNum].DPM_GetBatteryCapability.BatteryCapRef= *temp;
    }
    break;
#endif /* _BATTERY */
#if _ALERT
    case USBPD_CORE_ALERT:
      {
        uint8_t*  alert;
        alert = (uint8_t*)&DPM_Ports[PortNum].DPM_RcvAlert.d32;
        memcpy(alert, Ptr, Size);
      }
      break;
#endif /* _ALERT */
#endif /* USBPD_REV30_SUPPORT */

  /* In case of unexpected data type (Set request could not be fulfilled) :
   */
  default :
    break;
  }
  /* Forward info to GUI if enabled */
  if (NULL != DPM_GUI_SaveInfo)
  {
    DPM_GUI_SaveInfo(PortNum, DataId, Ptr, Size);
  }
/* USER CODE END USBPD_DPM_SetDataInfo */
}

#if defined(_SRC) || defined(_DRP)
/**
  * @brief  Evaluate received Request Message from Sink port
  * @param  PortNum         Port number
  * @param  PtrPowerObject  Pointer on the power data object
  * @retval USBPD status : USBPD_ACCEPT, USBPD_REJECT, USBPD_WAIT, USBPD_GOTOMIN
  */
USBPD_StatusTypeDef USBPD_DPM_EvaluateRequest(uint8_t PortNum, USBPD_CORE_PDO_Type_TypeDef *PtrPowerObject)
{
/* USER CODE BEGIN USBPD_DPM_EvaluateRequest */
  USBPD_SNKRDO_TypeDef rdo;
  USBPD_PDO_TypeDef pdo;
  uint32_t pdomaxcurrent = 0;
  uint32_t rdomaxcurrent = 0, rdoopcurrent = 0, rdoobjposition = 0;
  USBPD_HandleTypeDef *pdhandle = &DPM_Ports[PortNum];

  rdo.d32 = pdhandle->DPM_RcvRequestDOMsg;
  rdoobjposition  = rdo.GenericRDO.ObjectPosition;
#if defined(USBPD_REV30_SUPPORT) && defined(_UNCHUNKED_SUPPORT)
  /* Set unchuncked bit if supported by ports */
  DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_FALSE;
  if ((USBPD_TRUE == rdo.GenericRDO.UnchunkedExtendedMessage)
   && (USBPD_TRUE == DPM_Settings[PortNum].PE_PD3_Support.d.PE_UnchunkSupport))
  {
    DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_TRUE;
  }
#endif /* USBPD_REV30_SUPPORT && _UNCHUNKED_SUPPORT */
  pdhandle->DPM_RDOPosition = 0;

  /* Check if RDP can be met within the supported PDOs by the Source port */
  /* USBPD_DPM_EvaluateRequest: Evaluate Sink Request\r */
  /* USBPD_DPM_EvaluateRequest: Check if RDP can be met within the supported PDOs by the Source port\r */

  /* Search PDO in Port Source PDO list, that corresponds to Position provided in Request RDO */
  if (USBPD_PWR_IF_SearchRequestedPDO(PortNum, rdoobjposition, &pdo.d32) != USBPD_OK)
  {
    /* Invalid PDO index */
    /* USBPD_DPM_EvaluateRequest: Invalid PDOs index */
    return USBPD_REJECT;
  }

  switch(pdo.GenericPDO.PowerObject)
  {
  case USBPD_CORE_PDO_TYPE_FIXED:
    {
      pdomaxcurrent = pdo.SRCFixedPDO.MaxCurrentIn10mAunits;
      rdomaxcurrent = rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits;
      rdoopcurrent  = rdo.FixedVariableRDO.OperatingCurrentIn10mAunits;
      DPM_Ports[PortNum].DPM_RequestedCurrent = rdoopcurrent * 10;
      if(rdoopcurrent > pdomaxcurrent)
      {
        /* Sink requests too much operating current */
        /* USBPD_DPM_EvaluateRequest: Sink requests too much operating current*/
        return USBPD_REJECT;
      }

      if(rdomaxcurrent > pdomaxcurrent)
      {
        /* Sink requests too much maximum operating current */
        /* USBPD_DPM_EvaluateRequest: Sink requests too much maximum operating current */
        return USBPD_REJECT;
      }
    }
    break;
#if _PPS
  case USBPD_CORE_PDO_TYPE_APDO:
    {
      uint32_t pdominvoltage, pdomaxvoltage, rdovoltage;
      pdomaxcurrent                           = pdo.SRCSNKAPDO.MaxCurrentIn50mAunits;
      rdoopcurrent                            = rdo.ProgRDO.OperatingCurrentIn50mAunits;
      DPM_Ports[PortNum].DPM_RequestedCurrent = rdoopcurrent * 50;
      if(rdoopcurrent > pdomaxcurrent)
      {
        /* Sink requests too much operating current */
        return USBPD_REJECT;
      }
      pdominvoltage = pdo.SRCSNKAPDO.MinVoltageIn100mV * 100;
      pdomaxvoltage = pdo.SRCSNKAPDO.MaxVoltageIn100mV * 100;
      rdovoltage    = rdo.ProgRDO.OutputVoltageIn20mV * 20;
      if ((rdovoltage < pdominvoltage) || (rdovoltage > pdomaxvoltage))
      {
        /* Sink requests too much maximum operating current */
        return USBPD_REJECT;
      }
    }
    break;
#endif /* _PPS */
  case USBPD_CORE_PDO_TYPE_BATTERY:
  case USBPD_CORE_PDO_TYPE_VARIABLE:
  default:
    {
      return USBPD_REJECT;
    }
  }

  /* Set RDO position and requested voltage in DPM port structure */
  pdhandle->DPM_RequestedVoltage = pdo.SRCFixedPDO.VoltageIn50mVunits * 50;
  pdhandle->DPM_RDOPositionPrevious = pdhandle->DPM_RDOPosition;
  pdhandle->DPM_RDOPosition = rdoobjposition;

  /* Save the power object */
  *PtrPowerObject = pdo.GenericPDO.PowerObject;

  /* Accept the requested power */
  /* USBPD_DPM_EvaluateRequest: Sink requested %d mV %d mA for operating current from %d to %d mA\r",
               pdo.SRCFixedPDO.VoltageIn50mVunits * 50, pdo.SRCFixedPDO.MaxCurrentIn10mAunits * 10,
               rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits * 10, rdo.FixedVariableRDO.OperatingCurrentIn10mAunits * 10 */
  /* USBPD_DPM_EvaluateRequest: Source accepts the requested power */
  return USBPD_ACCEPT;
/* USER CODE END USBPD_DPM_EvaluateRequest */
}
#endif /* _SRC || _DRP */

#if defined(_SNK) || defined(_DRP)
/**
  * @brief  Evaluate received Capabilities Message from Source port and prepare the request message
  * @param  PortNum             Port number
  * @param  PtrRequestData      Pointer on selected request data object
  * @param  PtrPowerObjectType  Pointer on the power data object
  * @retval None
  */
void USBPD_DPM_SNK_EvaluateCapabilities(uint8_t PortNum, uint32_t *PtrRequestData, USBPD_CORE_PDO_Type_TypeDef *PtrPowerObjectType)
{
/* USER CODE BEGIN USBPD_DPM_SNK_EvaluateCapabilities */
  USBPD_PDO_TypeDef  fixed_pdo;
  USBPD_SNKRDO_TypeDef rdo;
  USBPD_HandleTypeDef *pdhandle = &DPM_Ports[PortNum];
  USBPD_USER_SettingsTypeDef *puser = (USBPD_USER_SettingsTypeDef *)&DPM_USER_Settings[PortNum];
  USBPD_DPM_SNKPowerRequestDetails_TypeDef snkpowerrequestdetails;
  uint32_t pdoindex, size;
  uint32_t snkpdolist[USBPD_MAX_NB_PDO];
  USBPD_PDO_TypeDef snk_fixed_pdo;

  /* USBPD_DPM_EvaluateCapabilities: Port Partner Requests Max Voltage */

  /* Find the Pdo index for the requested voltage */
  pdoindex = DPM_FindVoltageIndex(PortNum, &snkpowerrequestdetails);

  /* Initialize RDO */
  rdo.d32 = 0;

  /* If no valid SNK PDO or if no SRC PDO match found (index>=nb of valid received SRC PDOs or function returned DPM_NO_SRC_PDO_FOUND*/
  if (pdoindex >= pdhandle->DPM_NumberOfRcvSRCPDO)
  {
#ifdef _TRACE
    USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *) "PE_EvaluateCapability: could not find desired voltage", sizeof("PE_EvaluateCapability: could not find desired voltage"));
#endif /* _TRACE */
    fixed_pdo.d32 = pdhandle->DPM_ListOfRcvSRCPDO[0];
    /* Read SNK PDO list for retrieving useful data to fill in RDO */
    USBPD_PWR_IF_GetPortPDOs(PortNum, USBPD_CORE_DATATYPE_SNK_PDO, (uint8_t*)&snkpdolist[0], &size);
    /* Store value of 1st SNK PDO (Fixed) in local variable */
    snk_fixed_pdo.d32 = snkpdolist[0];
    rdo.FixedVariableRDO.ObjectPosition = 1;
    rdo.FixedVariableRDO.OperatingCurrentIn10mAunits  =  fixed_pdo.SRCFixedPDO.MaxCurrentIn10mAunits;
    rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits = puser->DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits / 10;
    rdo.FixedVariableRDO.CapabilityMismatch = 1;
    rdo.FixedVariableRDO.USBCommunicationsCapable = snk_fixed_pdo.SNKFixedPDO.USBCommunicationsCapable;
#if defined(USBPD_REV30_SUPPORT) && defined(_UNCHUNKED_SUPPORT)
    if (USBPD_SPECIFICATION_REV2 < DPM_Params[PortNum].PE_SpecRevision)
    {
      rdo.FixedVariableRDO.UnchunkedExtendedMessage = DPM_Settings[PortNum].PE_PD3_Support.d.PE_UnchunkSupport;
      DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_FALSE;
      /* Set unchuncked bit if supported by port partner;*/
      if (USBPD_TRUE == fixed_pdo.SRCFixedPDO.UnchunkedExtendedMessage)
      {
        DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_TRUE;
      }
    }
#endif /* USBPD_REV30_SUPPORT && _UNCHUNKED_SUPPORT */
    DPM_Ports[PortNum].DPM_RequestedCurrent = puser->DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits;

    pdhandle->DPM_RequestDOMsg = rdo.d32;
    *PtrPowerObjectType = USBPD_CORE_PDO_TYPE_FIXED;
    *PtrRequestData     = rdo.d32;
    pdhandle->DPM_RequestedVoltage = 5000;
    return;
  }

  DPM_SNK_BuildRDOfromSelectedPDO(PortNum, pdoindex, &snkpowerrequestdetails,&rdo, PtrPowerObjectType);

#if defined(USBPD_REV30_SUPPORT) && _SRC_CAPA_EXT
  /* Check to send a Get Get_Source_Cap_Extended.
     Can be requested if Peak Current of fixed supply PDOs to 0.
  */
  fixed_pdo.d32 = pdhandle->DPM_ListOfRcvSRCPDO[pdoindex];
  if (USBPD_CORE_PDO_TYPE_FIXED == fixed_pdo.GenericPDO.PowerObject)
  {
    if (USBPD_CORE_PDO_PEAKEQUAL == fixed_pdo.SRCFixedPDO.PeakCurrent)
    {
      DPM_Ports[PortNum].FlagSendGetSrcCapaExtended = 1;
    }
  }
#endif /* USBPD_REV30_SUPPORT && _SRC_CAPA_EXT*/

  *PtrRequestData = pdhandle->DPM_RequestDOMsg;
/* USER CODE END USBPD_DPM_SNK_EvaluateCapabilities */
}
#endif /* _SNK || _DRP */

#if defined(_DRP)
/**
  * @brief  Power role swap status update
  * @param  PortNum Port number
  * @param  CurrentRole the current role
  * @param  Status status on power role swap event
  */
void USBPD_DPM_PowerRoleSwap(uint8_t PortNum, USBPD_PortPowerRole_TypeDef CurrentRole, USBPD_PRS_Status_TypeDef Status)
{
/* USER CODE BEGIN USBPD_DPM_PowerRoleSwap */
    switch (Status)
    {
    case USBPD_PRS_STATUS_VBUS_OFF:
#if defined(_SRC) || defined(_DRP)
      if (CurrentRole == USBPD_PORTPOWERROLE_SRC)
      {
#if defined(_VCONN_SUPPORT)
        /* This variable is used to avoid VCONN disable during HR procedure */
        uint32_t _vconnstate = DPM_Params[PortNum].VconnStatus;
        DPM_Params[PortNum].VconnStatus = USBPD_FALSE;
#endif /* _VCONN_SUPPORT */

        /* In case of power role swap keep VCONN On */
        DPM_TurnOffPower(PortNum, CurrentRole);
#if defined(_VCONN_SUPPORT)
        /* restore vconn status */
        DPM_Params[PortNum].VconnStatus = _vconnstate;
#endif /* _VCONN_SUPPORT */
      }
#endif /* _SRC || _DRP */
      break;
    case USBPD_PRS_STATUS_SRC_RP2RD:
      DPM_AssertRd(PortNum);
      break;
    case USBPD_PRS_STATUS_SNK_RD2RP:
      DPM_AssertRp(PortNum);
      break;
#if defined(_SRC) || defined(_DRP)
    case USBPD_PRS_STATUS_VBUS_ON:
      DPM_TurnOnPower(PortNum, CurrentRole);

#if defined(USBPD_TCPM_MODULE_ENABLED) && defined(_RTOS)
      /* Check if still connected */
      osMessagePut(AlarmMsgBox, (PortNum << 8 | 1), osWaitForever);
#endif /* USBPD_TCPM_MODULE_ENABLED */
      break;
#endif /*_SRC || _DRP */
    default:
      break;
    }
/* USER CODE END USBPD_DPM_PowerRoleSwap */
}
#endif /* _DRP */

#if defined(_VCONN_SUPPORT)
/**
  * @brief  Callback to be used by PE to evaluate a Vconn swap
  * @param  PortNum Port number
  * @retval USBPD_ACCEPT, USBPD_REJECT, USBPD_WAIT
  */
USBPD_StatusTypeDef USBPD_DPM_EvaluateVconnSwap(uint8_t PortNum)
{
/* USER CODE BEGIN USBPD_DPM_EvaluateVconnSwap */
  USBPD_StatusTypeDef status = USBPD_REJECT;
  if (USBPD_TRUE == DPM_USER_Settings[PortNum].PE_VconnSwap)
  {
    status = USBPD_ACCEPT;
  }

  return status;
/* USER CODE END USBPD_DPM_EvaluateVconnSwap */
}

/**
  * @brief  Callback to be used by PE to manage VConn
  * @param  PortNum  Port number
  * @param  State    Enable or Disable VConn on CC lines
  * @retval USBPD_ACCEPT, USBPD_REJECT
  */
USBPD_StatusTypeDef USBPD_DPM_PE_VconnPwr(uint8_t PortNum, USBPD_FunctionalState State)
{
/* USER CODE BEGIN USBPD_DPM_PE_VconnPwr */
  USBPD_StatusTypeDef status = USBPD_ERROR;
  if((USBPD_ENABLE == State) && (CCNONE != DPM_Params[PortNum].VconnCCIs))
  {
    /* In case of VCONN Swap SRC -> SNK. Keep information that CABLE is EMC */
    DPM_Ports[PortNum].DPM_CablePDCapable = USBPD_TRUE;
    status = USBPD_PWR_IF_Enable_VConn(PortNum,DPM_Params[PortNum].VconnCCIs);
#ifndef _RTOS
    /* To be sure than VCONN is ON */
    USBPD_DPM_WaitForTime(2);
#endif /* _RTOS */
  }
  if((USBPD_DISABLE == State) && (CCNONE != DPM_Params[PortNum].VconnCCIs))
  {
    status = USBPD_PWR_IF_Disable_VConn(PortNum,DPM_Params[PortNum].VconnCCIs);
  }
  return status;
/* USER CODE END USBPD_DPM_PE_VconnPwr */
}
#endif /* _VCONN_SUPPORT */

#ifdef USBPD_REV30_SUPPORT
/**
  * @brief  DPM callback to allow PE to forward extended message information.
  * @param  PortNum   Port number
  * @param  MsgType   Type of message to be handled in DPM
  *         This parameter can be one of the following values:
  *           @arg @ref USBPD_EXT_SECURITY_REQUEST Security Request extended message
  *           @arg @ref USBPD_EXT_SECURITY_RESPONSE Security Response extended message
  * @param  ptrData   Pointer on address Extended Message data could be read (u8 pointer)
  * @param  DataSize  Nb of u8 that compose Extended message
  * @retval None
  */
void USBPD_DPM_ExtendedMessageReceived(uint8_t PortNum, USBPD_ExtendedMsg_TypeDef MsgType, uint8_t *ptrData, uint16_t DataSize)
{
/* USER CODE BEGIN USBPD_DPM_ExtendedMessageReceived */
  if (DataSize == 0)
  {
    /* No data received. */
    return;
  }

  switch(MsgType)
  {
#ifdef _SECURITY_MSG
    case USBPD_EXT_SECURITY_REQUEST :
      {
        uint8_t data[60] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60};
        USBPD_PE_SendExtendedMessage(PortNum, USBPD_SOPTYPE_SOP, USBPD_EXT_SECURITY_RESPONSE, data, 60);
      }
      break;
    case USBPD_EXT_SECURITY_RESPONSE :
      break;
#endif /* _SECURITY_MSG */

#if defined(_FWUPDATE_INITIATOR) || defined(_FWUPDATE_RESPONDER)
    case USBPD_EXT_FIRM_UPDATE_REQUEST :
#if defined(_FWUPDATE_RESPONDER)
      USBPD_PDFU_FWUpdateRequestReceived(PortNum, ptrData, DataSize);
#else
      {
        uint8_t data[4] = {0xAA, 0xBB, 0xCC, 0xDD};
        USBPD_PE_SendExtendedMessage(PortNum, USBPD_SOPTYPE_SOP, USBPD_EXT_FIRM_UPDATE_RESPONSE, data, 4);
      }
#endif /* _FWUPDATE_RESPONDER */
      break;
    case USBPD_EXT_FIRM_UPDATE_RESPONSE :
#if defined(_FWUPDATE_INITIATOR)
      USBPD_PDFU_INIT_FWUpdateResponseReceived(PortNum, ptrData, DataSize);
#endif /* _FWUPDATE_INITIATOR */
      break;
#endif /* _FWUPDATE_INITIATOR || _FWUPDATE_RESPONDER */
#ifdef _COUNTRY_MSG
    case USBPD_EXT_COUNTRY_INFO :
    case USBPD_EXT_COUNTRY_CODES :
      break;
#endif /* _COUNTRY_MSG */
    default:
      break;
  }
/* USER CODE END USBPD_DPM_ExtendedMessageReceived */
}
#endif /* USBPD_REV30_SUPPORT */

/**
  * @brief  DPM callback to allow PE to enter ERROR_RECOVERY state.
  * @param  PortNum Port number
  * @retval None
  */
void USBPD_DPM_EnterErrorRecovery(uint8_t PortNum)
{
#if defined(_SRC) || defined(_DRP)
  /* Take care about VBUS, VCONN voltage */
  DPM_TurnOffPower(PortNum, DPM_Params[PortNum].PE_PowerRole);
#endif /*_SRC || _DRP */

  /* Inform CAD to enter recovery mode */
  USBPD_CAD_EnterErrorRecovery(PortNum);
}

/**
  * @brief  DPM callback used to know user choice about Data Role Swap.
  * @param  PortNum Port number
  * @retval USBPD_REJECT, UBPD_ACCEPT
  */
USBPD_StatusTypeDef USBPD_DPM_EvaluateDataRoleSwap(uint8_t PortNum)
{
/* USER CODE BEGIN USBPD_DPM_EvaluateDataRoleSwap */
  USBPD_StatusTypeDef status = USBPD_REJECT;
  if ((USBPD_TRUE == DPM_USER_Settings[PortNum].PE_DataSwap)
    && (((USBPD_TRUE == DPM_USER_Settings[PortNum].PE_DR_Swap_To_DFP) && (USBPD_PORTDATAROLE_UFP == DPM_Params[PortNum].PE_DataRole))
     || ((USBPD_TRUE == DPM_USER_Settings[PortNum].PE_DR_Swap_To_UFP) && (USBPD_PORTDATAROLE_DFP == DPM_Params[PortNum].PE_DataRole))))
  {
    status = USBPD_ACCEPT;
  }
  return status;
/* USER CODE END USBPD_DPM_EvaluateDataRoleSwap */
}

/**
  * @brief  Callback to be used by PE to check is VBUS is ready or present
  * @param  PortNum Port number
  * @param  Vsafe   Vsafe status based on @ref USBPD_VSAFE_StatusTypeDef
  * @retval USBPD_DISABLE or USBPD_ENABLE
  */
USBPD_FunctionalState USBPD_DPM_IsPowerReady(uint8_t PortNum, USBPD_VSAFE_StatusTypeDef Vsafe)
{
/* USER CODE BEGIN USBPD_DPM_IsPowerReady */
  return ((USBPD_OK == USBPD_PWR_IF_SupplyReady(PortNum, Vsafe)) ? USBPD_ENABLE : USBPD_DISABLE);
/* USER CODE END USBPD_DPM_IsPowerReady */
}

/**
  * @}
  */

/** @defgroup USBPD_USER_EXPORTED_FUNCTIONS_GROUP3 USBPD USER Functions PD messages requests
  * @{
  */

/**
  * @brief  Request the PE to send a hard reset
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestHardReset(uint8_t PortNum)
{
  return USBPD_PE_Request_HardReset(PortNum);
}

/**
  * @brief  Request the PE to send a cable reset.
  * @note   Only a DFP Shall generate Cable Reset Signaling. A DFP Shall only generate Cable Reset Signaling within an Explicit Contract.
            The DFP has to be supplying VCONN prior to a Cable Reset
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestCableReset(uint8_t PortNum)
{
  return USBPD_PE_Request_CableReset(PortNum);
}

/**
  * @brief  Request the PE to send a GOTOMIN message
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestGotoMin(uint8_t PortNum)
{
  return USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GOTOMIN, USBPD_SOPTYPE_SOP);
}

/**
  * @brief  Request the PE to send a PING message
  * @note   In USB-PD stack, only ping management for P3.0 is implemented.
  *         If PD2.0 is used, PING timer needs to be implemented on user side.
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestPing(uint8_t PortNum)
{
  return USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_PING, USBPD_SOPTYPE_SOP);
}

/**
  * @brief  Request the PE to send a request message.
  * @param  PortNum     The current port number
  * @param  IndexSrcPDO Index on the selected SRC PDO (value between 1 to 7)
  * @param  RequestedVoltage Requested voltage (in MV and use mainly for APDO)
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestMessageRequest(uint8_t PortNum, uint8_t IndexSrcPDO, uint16_t RequestedVoltage)
{
  USBPD_StatusTypeDef status = USBPD_ERROR;
#if defined(_SNK) || defined(_DRP)
  uint32_t voltage, allowablepower;
  USBPD_SNKRDO_TypeDef rdo;
  USBPD_PDO_TypeDef  pdo;
  USBPD_CORE_PDO_Type_TypeDef pdo_object;
  USBPD_USER_SettingsTypeDef *puser = (USBPD_USER_SettingsTypeDef *)&DPM_USER_Settings[PortNum];
  USBPD_DPM_SNKPowerRequestDetails_TypeDef request_details;
  rdo.d32 = 0;

  /* selected SRC PDO */
  pdo.d32 = DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[(IndexSrcPDO - 1)];
  voltage = RequestedVoltage;
  allowablepower = (puser->DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits * RequestedVoltage) / 1000;

  if (USBPD_TRUE == USBPD_DPM_SNK_EvaluateMatchWithSRCPDO(PortNum, pdo.d32, &voltage, &allowablepower))
  {
    /* Check that voltage has been correctly selected */
    if (RequestedVoltage == voltage)
    {
      request_details.RequestedVoltageInmVunits    = RequestedVoltage;
      request_details.OperatingCurrentInmAunits    = (1000 * allowablepower)/RequestedVoltage;
      request_details.MaxOperatingCurrentInmAunits = puser->DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits;
      request_details.MaxOperatingPowerInmWunits   = puser->DPM_SNKRequestedPower.MaxOperatingPowerInmWunits;
      request_details.OperatingPowerInmWunits      = puser->DPM_SNKRequestedPower.OperatingPowerInmWunits;

      DPM_SNK_BuildRDOfromSelectedPDO(PortNum, (IndexSrcPDO - 1), &request_details, &rdo, &pdo_object);

      status = USBPD_PE_Send_Request(PortNum, rdo.d32, pdo_object);
    }
  }
#endif /* _SNK || _DRP */

  return status;
}

/**
  * @brief  Request the PE to send a GET_SRC_CAPA message
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestGetSourceCapability(uint8_t PortNum)
{
  return USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_SRC_CAP, USBPD_SOPTYPE_SOP);
}

/**
  * @brief  Request the PE to send a GET_SNK_CAPA message
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestGetSinkCapability(uint8_t PortNum)
{
  return USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_SNK_CAP, USBPD_SOPTYPE_SOP);
}

/**
  * @brief  Request the PE to perform a Data Role Swap.
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestDataRoleSwap(uint8_t PortNum)
{
  return USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_DR_SWAP, USBPD_SOPTYPE_SOP);
}

/**
  * @brief  Request the PE to perform a Power Role Swap.
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestPowerRoleSwap(uint8_t PortNum)
{
#if defined(_DRP)
  return USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_PR_SWAP, USBPD_SOPTYPE_SOP);
#else
  return USBPD_ERROR;
#endif

}

/**
  * @brief  Request the PE to perform a VCONN Swap.
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestVconnSwap(uint8_t PortNum)
{
  return USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_VCONN_SWAP, USBPD_SOPTYPE_SOP);
}

/**
  * @brief  Request the PE to send a soft reset
  * @param  PortNum The current port number
  * @param  SOPType SOP Type based on @ref USBPD_SOPType_TypeDef
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestSoftReset(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType)
{
  return USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_SOFT_RESET, SOPType);
}

/**
  * @brief  Request the PE to send a Source Capability message.
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestSourceCapability(uint8_t PortNum)
{
  /* PE will directly get the PDO saved in structure @ref PWR_Port_PDO_Storage */
  return USBPD_PE_Request_DataMessage(PortNum, USBPD_DATAMSG_SRC_CAPABILITIES, NULL);
}

#if defined(_VDM) || defined(_VCONN_SUPPORT)
/**
  * @brief  Request the PE to send a VDM discovery identity
  * @param  PortNum The current port number
  * @param  SOPType SOP Type
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestVDM_DiscoveryIdentify(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType)
{
  USBPD_StatusTypeDef _status = USBPD_ERROR;
  __DPM_DEBUG_CALLBACK(PortNum, "USBPD_PE_SVDM_RequestIdentity");

  if ((USBPD_SOPTYPE_SOP == SOPType)
#if defined(_VCONN_SUPPORT)
    || (USBPD_TRUE == DPM_Ports[PortNum].DPM_CablePDCapable)
#endif /* _VCONN_SUPPORT */
    )
  {
    _status = USBPD_PE_SVDM_RequestIdentity(PortNum, SOPType);
  }

  return _status;
}

/**
  * @brief  Request the PE to send a VDM discovery SVID
  * @param  PortNum The current port number
  * @param  SOPType SOP Type
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestVDM_DiscoverySVID(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType)
{
  __DPM_DEBUG_CALLBACK(PortNum, "USBPD_PE_SVDM_RequestSVID");
  return USBPD_PE_SVDM_RequestSVID(PortNum, SOPType);
}

/**
  * @brief  Request the PE to perform a VDM Discovery mode message on one SVID.
  * @param  PortNum The current port number
  * @param  SOPType SOP Type
  * @param  SVID    SVID used for discovery mode message
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestVDM_DiscoveryMode(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID)
{
  __DPM_DEBUG_CALLBACK(PortNum, "USBPD_PE_SVDM_RequestMode");
  return USBPD_PE_SVDM_RequestMode(PortNum, SOPType, SVID);
}

/**
  * @brief  Request the PE to perform a VDM mode enter.
  * @param  PortNum   The current port number
  * @param  SOPType   SOP Type
  * @param  SVID      SVID used for discovery mode message
  * @param  ModeIndex Index of the mode to be entered
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestVDM_EnterMode(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID, uint8_t ModeIndex)
{
  __DPM_DEBUG_CALLBACK(PortNum, "USBPD_PE_SVDM_RequestModeEnter");
  return USBPD_PE_SVDM_RequestModeEnter(PortNum, SOPType, SVID, ModeIndex);
}

/**
  * @brief  Request the PE to perform a VDM mode exit.
  * @param  PortNum   The current port number
  * @param  SOPType   SOP Type
  * @param  SVID      SVID used for discovery mode message
  * @param  ModeIndex Index of the mode to be exit
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestVDM_ExitMode(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID, uint8_t ModeIndex)
{
  __DPM_DEBUG_CALLBACK(PortNum, "USBPD_PE_SVDM_RequestModeExit");
  return USBPD_PE_SVDM_RequestModeExit(PortNum, SOPType, SVID, ModeIndex);
}

/**
  * @brief  Request the PE to send a Display Port status
  * @param  PortNum   The current port number
  * @param  SOPType   SOP Type
  * @param  SVID      Used SVID
  * @param  pDPStatus Pointer on DP Status data (32 bit)
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestDisplayPortStatus(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID, uint32_t *pDPStatus)
{
  __DPM_DEBUG_CALLBACK(PortNum, "USBPD_PE_SVDM_RequestSpecific(DP_STATUS)");
  USBPD_VDM_FillDPStatus(PortNum, (USBPD_DPStatus_TypeDef*)pDPStatus);
  return USBPD_PE_SVDM_RequestSpecific(PortNum, SOPType, SVDM_SPECIFIC_1, SVID);
}
/**
  * @brief  Request the PE to send a Display Port Config
  * @param  PortNum   The current port number
  * @param  SOPType   SOP Type
  * @param  SVID      Used SVID
  * @param  pDPConfig Pointer on DP Config data (32 bit)
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestDisplayPortConfig(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID, uint32_t *pDPConfig)
{
  __DPM_DEBUG_CALLBACK(PortNum, "USBPD_PE_SVDM_RequestSpecific(DP_CONFIG)");
  USBPD_VDM_FillDPConfig(PortNum, (USBPD_DPConfig_TypeDef*)pDPConfig);
  return USBPD_PE_SVDM_RequestSpecific(PortNum, SOPType, SVDM_SPECIFIC_2, SVID);
}

#endif /* _VDM || _VCONN_SUPPORT */
#if defined(_VDM)
/**
  * @brief  Request the PE to perform a VDM Attention.
  * @param  PortNum The current port number
  * @param  SOPType SOP Type
  * @param  SVID    Used SVID
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestAttention(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID)
{
  __DPM_DEBUG_CALLBACK(PortNum, "USBPD_PE_SVDM_RequestAttention");
  return USBPD_PE_SVDM_RequestAttention(PortNum, SOPType, SVID);
}
#endif /* _VDM */
#if defined(_UVDM)
/**
  * @brief  Called by DPM to request the PE to send a UVDM message.
  * @param  PortNum Index of current used port
  * @param  SOPType Received message type based on @ref USBPD_SOPType_TypeDef
  * @retval USBPD status : USBPD_BUSY, USBPD_OK, USBPD_FAIL
  */
USBPD_StatusTypeDef USBPD_DPM_RequestUVDMMessage(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType)
{
  return USBPD_PE_UVDM_RequestMessage(PortNum, SOPType);
}

#endif /* _UVDM */
#ifdef USBPD_REV30_SUPPORT
/**
  * @brief  Request the PE to send an ALERT to port partner
  * @param  PortNum The current port number
  * @param  Alert   Alert based on @ref USBPD_ADO_TypeDef
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestAlert(uint8_t PortNum, USBPD_ADO_TypeDef Alert)
{
#if _ALERT
  return USBPD_PE_Request_DataMessage(PortNum, USBPD_DATAMSG_ALERT, (uint32_t*)&Alert.d32);
#else
  return USBPD_ERROR;
#endif /* _ALERT */
}

/**
  * @brief  Request the PE to get a source capability extended
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestGetSourceCapabilityExt(uint8_t PortNum)
{
#if _SRC_CAPA_EXT
  return USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_SRC_CAPEXT, USBPD_SOPTYPE_SOP);
#else
  return USBPD_ERROR;
#endif /* _SRC_CAPA_EXT*/
}

/**
  * @brief  Request the PE to get a sink capability extended
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestGetSinkCapabilityExt(uint8_t PortNum)
{
  return USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_SNK_CAPEXT, USBPD_SOPTYPE_SOP);
}

/**
  * @brief  Request the PE to get a manufacturer info
  * @param  PortNum The current port number
  * @param  SOPType SOP Type
  * @param  pManuInfoData Pointer on manufacturer info based on @ref USBPD_GMIDB_TypeDef
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestGetManufacturerInfo(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint8_t* pManuInfoData)
{
#if _MANU_INFO
  return USBPD_PE_SendExtendedMessage(PortNum, SOPType, USBPD_EXT_GET_MANUFACTURER_INFO, (uint8_t*)pManuInfoData, sizeof(USBPD_GMIDB_TypeDef));
#else
  return USBPD_ERROR;
#endif /* _MANU_INFO */
}

/**
  * @brief  Request the PE to request a GET_PPS_STATUS
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestGetPPS_Status(uint8_t PortNum)
{
#if _PPS
  return USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_PPS_STATUS, USBPD_SOPTYPE_SOP);
#else
  return USBPD_ERROR;
#endif /* _PPS */
}

/**
  * @brief  Request the PE to request a GET_STATUS
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestGetStatus(uint8_t PortNum)
{
#if _STATUS
  return USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_STATUS, USBPD_SOPTYPE_SOP);
#else
  return USBPD_ERROR;
#endif /* _STATUS*/
}

/**
  * @brief  Request the PE to perform a Fast Role Swap.
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestFastRoleSwap(uint8_t PortNum)
{
  return USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_FR_SWAP, USBPD_SOPTYPE_SOP);
}

/**
  * @brief  Request the PE to send a GET_COUNTRY_CODES message
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestGetCountryCodes(uint8_t PortNum)
{
  return USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_COUNTRY_CODES, USBPD_SOPTYPE_SOP);
}

/**
  * @brief  Request the PE to send a GET_COUNTRY_INFO message
  * @param  PortNum     The current port number
  * @param  CountryCode Country code (1st character and 2nd of the Alpha-2 Country)
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestGetCountryInfo(uint8_t PortNum, uint16_t CountryCode)
{
  return USBPD_PE_Request_DataMessage(PortNum, USBPD_DATAMSG_GET_COUNTRY_INFO, (uint32_t*)&CountryCode);
}

/**
  * @brief  Request the PE to send a GET_BATTERY_CAPA
  * @param  PortNum         The current port number
  * @param  pBatteryCapRef  Pointer on the Battery Capability reference
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestGetBatteryCapability(uint8_t PortNum, uint8_t *pBatteryCapRef)
{
#if _BATTERY
  return USBPD_PE_SendExtendedMessage(PortNum, USBPD_SOPTYPE_SOP, USBPD_EXT_GET_BATTERY_CAP, (uint8_t*)pBatteryCapRef, 1);
#else
  return USBPD_ERROR;
#endif /* _BATTERY */
}

/**
  * @brief  Request the PE to send a GET_BATTERY_STATUS
  * @param  PortNum           The current port number
  * @param  pBatteryStatusRef Pointer on the Battery Status reference
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestGetBatteryStatus(uint8_t PortNum, uint8_t *pBatteryStatusRef)
{
#if _BATTERY
  return USBPD_PE_SendExtendedMessage(PortNum, USBPD_SOPTYPE_SOP, USBPD_EXT_GET_BATTERY_STATUS, (uint8_t*)pBatteryStatusRef, 1);
#else
  return USBPD_ERROR;
#endif /* _BATTERY */
}

/**
  * @brief  Request the PE to send a SECURITY_REQUEST
  * @param  PortNum The current port number
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestSecurityRequest(uint8_t PortNum)
{
#if defined(_SECURITY_MSG)
  uint8_t data[60] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60};
  return USBPD_PE_SendExtendedMessage(PortNum, USBPD_SOPTYPE_SOP, USBPD_EXT_SECURITY_REQUEST, data, 60);
#else
  return USBPD_ERROR;
#endif /* _SECURITY_MSG */
}

/**
  * @brief  Request the PE to send a FIRWMARE_UPDATE_REQUEST or FIRWMARE_UPDATE_RESPONSE
  * @param  PortNum     The current port number
  * @param  MessageType Type of the message (REQUEST or RESPONSE)
  * @param  pPayload    Pointer of the Payload to send to port partner
  * @param  Size        Size of the payload
  * @retval USBPD Status
  */
USBPD_StatusTypeDef USBPD_DPM_RequestFirwmwareUpdate(uint8_t PortNum, USBPD_ExtendedMsg_TypeDef MessageType, uint8_t *pPayload, uint16_t Size)
{
#if defined(_FWUPDATE_INITIATOR) || defined(_FWUPDATE_RESPONDER)
  return USBPD_PE_SendExtendedMessage(PortNum, USBPD_SOPTYPE_SOP, MessageType, pPayload, Size);
#else
  return USBPD_ERROR;
#endif /* _FWUPDATE_INITIATOR || _FWUPDATE_RESPONDER */
}
#endif /*USBPD_REV30_SUPPORT*/

/**
  * @}
  */

/** @addtogroup USBPD_USER_PRIVATE_FUNCTIONS
  * @{
  */

/* USER CODE BEGIN USBPD_USER_PRIVATE_FUNCTIONS */
#if defined(_SNK) || defined(_DRP)
/**
  * @brief  Examinate a given SRC PDO to check if matching with SNK capabilities.
  * @param  PortNum             Port number
  * @param  SrcPDO              Selected SRC PDO (32 bits)
  * @param  PtrRequestedVoltage Pointer on Voltage value that could be reached if SRC PDO is requested (only valid if USBPD_TRUE is returned) in mV
  * @param  PtrRequestedPower   Pointer on Power value that could be reached if SRC PDO is requested (only valid if USBPD_TRUE is returned) in mW
  * @retval USBPD_FALSE of USBPD_TRUE (USBPD_TRUE returned in SRC PDO is considered matching with SNK profile)
  */
uint32_t USBPD_DPM_SNK_EvaluateMatchWithSRCPDO(uint8_t PortNum, uint32_t SrcPDO, uint32_t* PtrRequestedVoltage, uint32_t* PtrRequestedPower)
{
  USBPD_PDO_TypeDef  srcpdo, snkpdo;
  uint32_t match = USBPD_FALSE;
  uint32_t nbsnkpdo;
  uint32_t snkpdo_array[USBPD_MAX_NB_PDO];
  uint16_t i, srcvoltage50mv, srcmaxvoltage50mv, srcminvoltage50mv, srcmaxcurrent10ma;
  uint16_t snkvoltage50mv, snkmaxvoltage50mv, snkminvoltage50mv, snkopcurrent10ma;
  uint32_t maxrequestedpower, currentrequestedpower;
  uint32_t maxrequestedvoltage, currentrequestedvoltage;
  uint32_t snkoppower250mw, srcmaxpower250mw;

  /* Retrieve SNK PDO list from PWR_IF storage : PDO values + nb of u32 written by PWR_IF (nb of PDOs) */
  USBPD_PWR_IF_GetPortPDOs(PortNum, USBPD_CORE_DATATYPE_SNK_PDO, (uint8_t*)snkpdo_array, &nbsnkpdo);

  if (0 == nbsnkpdo)
  {
    return(USBPD_FALSE);
  }

  /* Set default output values */
  maxrequestedpower    = 0;
  maxrequestedvoltage  = 0;

  /* Check SRC PDO value according to its type */
  srcpdo.d32 = SrcPDO;
  switch(srcpdo.GenericPDO.PowerObject)
  {
    /* SRC Fixed Supply PDO */
    case USBPD_CORE_PDO_TYPE_FIXED:
      srcvoltage50mv = srcpdo.SRCFixedPDO.VoltageIn50mVunits;
      srcmaxcurrent10ma = srcpdo.SRCFixedPDO.MaxCurrentIn10mAunits;

      /* Loop through SNK PDO list */
      for (i=0; i<nbsnkpdo; i++)
      {
        currentrequestedpower = 0;
        currentrequestedvoltage = 0;

        /* Retrieve SNK PDO value according to its type */
        snkpdo.d32 = snkpdo_array[i];
        switch(snkpdo.GenericPDO.PowerObject)
        {
          /* SNK Fixed Supply PDO */
          case USBPD_CORE_PDO_TYPE_FIXED:
          {
            snkvoltage50mv = snkpdo.SNKFixedPDO.VoltageIn50mVunits;
            snkopcurrent10ma = snkpdo.SNKFixedPDO.OperationalCurrentIn10mAunits;

            /* Match if :
                 SNK Voltage = SRC Voltage
                 &&
                 SNK Op Current <= SRC Max Current

               Requested Voltage : SNK Voltage
               Requested Op Current : SNK Op Current
               Requested Max Current : SNK Op Current
            */
            if (  (snkvoltage50mv == srcvoltage50mv)
                &&(snkopcurrent10ma <= srcmaxcurrent10ma))
            {
              currentrequestedpower = (snkvoltage50mv * snkopcurrent10ma) / 2; /* to get value in mw */
              currentrequestedvoltage = snkvoltage50mv;
            }
            break;
          }
          /* SNK Variable Supply (non-battery) PDO */
          case USBPD_CORE_PDO_TYPE_VARIABLE:
            snkmaxvoltage50mv = snkpdo.SNKVariablePDO.MaxVoltageIn50mVunits;
            snkminvoltage50mv = snkpdo.SNKVariablePDO.MinVoltageIn50mVunits;
            snkopcurrent10ma  = snkpdo.SNKVariablePDO.OperationalCurrentIn10mAunits;

            /* Match if :
                 SNK Max voltage >= SRC Voltage
                 &&
                 SNK Min voltage <= SRC Voltage
                 &&
                 SNK Op current <= SRC Max current

               Requested Voltage : SRC Voltage
               Requested Op Current : SNK Op Current
               Requested Max Current : SNK Op Current
            */
            if (  (snkmaxvoltage50mv >= srcvoltage50mv)
                &&(snkminvoltage50mv <= srcvoltage50mv)
                &&(snkopcurrent10ma  <= srcmaxcurrent10ma))
            {
              currentrequestedpower = (srcvoltage50mv * snkopcurrent10ma) / 2; /* to get value in mw */
              currentrequestedvoltage = srcvoltage50mv;
            }
            break;

            /* SNK Battery Supply PDO */
          case USBPD_CORE_PDO_TYPE_BATTERY:
            snkmaxvoltage50mv = snkpdo.SNKBatteryPDO.MaxVoltageIn50mVunits;
            snkminvoltage50mv = snkpdo.SNKBatteryPDO.MinVoltageIn50mVunits;
            snkoppower250mw   = snkpdo.SNKBatteryPDO.OperationalPowerIn250mWunits;

            /* Match if :
                 SNK Max voltage >= SRC Voltage
                 &&
                 SNK Min voltage <= SRC Voltage
                 &&
                 SNK Op power <= SRC Max current * SRC Voltage

               Requested Voltage : SRC Voltage
               Requested Op Current : SNK Op Power/ SRC Voltage
               Requested Max Current : SNK Op Power/ SRC Voltage
            */
            if (  (snkmaxvoltage50mv >= srcvoltage50mv)
                &&(snkminvoltage50mv <= srcvoltage50mv)
                &&(snkoppower250mw <= ((srcvoltage50mv * srcmaxcurrent10ma)/500)))  /* to get value in 250 mw units */
            {
              currentrequestedvoltage = srcvoltage50mv;
              currentrequestedpower = snkoppower250mw;
            }
            break;

#if _PPS
            /* SNK Augmented Power Data Object (APDO) */
          case USBPD_CORE_PDO_TYPE_APDO:
            break;
#endif /* _PPS */

          default:
            break;
        }

        if (currentrequestedpower > maxrequestedpower)
        {
          match = USBPD_TRUE;
          maxrequestedpower   = currentrequestedpower;
          maxrequestedvoltage = currentrequestedvoltage;
        }
      }
      break;

    /* SRC Variable Supply (non-battery) PDO */
    case USBPD_CORE_PDO_TYPE_VARIABLE:
      srcmaxvoltage50mv = srcpdo.SRCVariablePDO.MaxVoltageIn50mVunits;
      srcminvoltage50mv = srcpdo.SRCVariablePDO.MinVoltageIn50mVunits;
      srcmaxcurrent10ma = srcpdo.SRCVariablePDO.MaxCurrentIn10mAunits;

      /* Loop through SNK PDO list */
      for (i=0; i<nbsnkpdo; i++)
      {
        currentrequestedpower = 0;
        currentrequestedvoltage = 0;

        /* Retrieve SNK PDO value according to its type */
        snkpdo.d32 = snkpdo_array[i];
        switch(snkpdo.GenericPDO.PowerObject)
        {
          /* SNK Fixed Supply PDO */
          case USBPD_CORE_PDO_TYPE_FIXED:
            /* No match */
            break;

            /* SNK Variable Supply (non-battery) PDO */
          case USBPD_CORE_PDO_TYPE_VARIABLE:
            snkmaxvoltage50mv = snkpdo.SNKVariablePDO.MaxVoltageIn50mVunits;
            snkminvoltage50mv = snkpdo.SNKVariablePDO.MinVoltageIn50mVunits;
            snkopcurrent10ma  = snkpdo.SNKVariablePDO.OperationalCurrentIn10mAunits;

            /* Match if :
                 SNK Max voltage >= SRC Max Voltage
                 &&
                 SNK Min voltage <= SRC Min Voltage
                 &&
                 SNK Op current <= SRC Max current

               Requested Voltage : Any value between SRC Min Voltage and SRC Max Voltage
               Requested Op Current : SNK Op Current
               Requested Max Current : SNK Op Current
            */
            if (  (snkmaxvoltage50mv >= srcmaxvoltage50mv)
                &&(snkminvoltage50mv <= srcminvoltage50mv)
                &&(snkopcurrent10ma <= srcmaxcurrent10ma))
            {
              currentrequestedpower = (srcmaxvoltage50mv * snkopcurrent10ma) / 2; /* to get value in mw */
              currentrequestedvoltage = srcmaxvoltage50mv;
            }
            break;

            /* SNK Battery Supply PDO */
          case USBPD_CORE_PDO_TYPE_BATTERY:
            snkmaxvoltage50mv = snkpdo.SNKBatteryPDO.MaxVoltageIn50mVunits;
            snkminvoltage50mv = snkpdo.SNKBatteryPDO.MinVoltageIn50mVunits;
            snkoppower250mw   = snkpdo.SNKBatteryPDO.OperationalPowerIn250mWunits;

            /* Match if :
                 SNK Max voltage >= SRC Max Voltage
                 &&
                 SNK Min voltage <= SRC Min Voltage
                 &&
                 SNK Op power <= SRC Max current * SRC Max Voltage

               Requested Voltage : Any value between SRC Min Voltage and SRC Max Voltage, that fulfill
                                   SNK Op power <= Voltage * SRC Max Current
               Requested Op Current : SNK Op Power/ SRC Voltage
               Requested Max Current : SNK Op Power/ SRC Voltage
            */
            if (  (snkmaxvoltage50mv >= srcmaxvoltage50mv)
                &&(snkminvoltage50mv <= srcminvoltage50mv)
                &&(snkoppower250mw <= ((srcmaxvoltage50mv * srcmaxcurrent10ma)/500)))  /* to get value in 250 mw units */
            {
              currentrequestedpower   = snkoppower250mw * 250; /* to get value in mw */
              currentrequestedvoltage = srcmaxvoltage50mv;
            }
            break;

#if _PPS
            /* SNK Augmented Power Data Object (APDO) */
          case USBPD_CORE_PDO_TYPE_APDO:
            break;
#endif /* _PPS */

          default:
            break;
        }

        if (currentrequestedpower > maxrequestedpower)
        {
          match = USBPD_TRUE;
          maxrequestedpower   = currentrequestedpower;
          maxrequestedvoltage = currentrequestedvoltage;
        }
      }
      break;

    /* SRC Battery Supply PDO */
    case USBPD_CORE_PDO_TYPE_BATTERY:
      srcmaxvoltage50mv = srcpdo.SRCBatteryPDO.MaxVoltageIn50mVunits;
      srcminvoltage50mv = srcpdo.SRCBatteryPDO.MinVoltageIn50mVunits;
      srcmaxpower250mw  = srcpdo.SRCBatteryPDO.MaxAllowablePowerIn250mWunits;

      /* Loop through SNK PDO list */
      for (i=0; i<nbsnkpdo; i++)
      {
        currentrequestedpower = 0;
        currentrequestedvoltage = 0;

        /* Retrieve SNK PDO value according to its type */
        snkpdo.d32 = snkpdo_array[i];
        switch(snkpdo.GenericPDO.PowerObject)
        {
          /* SNK Fixed Supply PDO */
          case USBPD_CORE_PDO_TYPE_FIXED:
            /* No match */
            break;

            /* SNK Variable Supply (non-battery) PDO */
          case USBPD_CORE_PDO_TYPE_VARIABLE:
            snkmaxvoltage50mv = snkpdo.SNKVariablePDO.MaxVoltageIn50mVunits;
            snkminvoltage50mv = snkpdo.SNKVariablePDO.MinVoltageIn50mVunits;
            snkopcurrent10ma  = snkpdo.SNKVariablePDO.OperationalCurrentIn10mAunits;

            /* Match if :
                 SNK Max voltage >= SRC Max Voltage
                 &&
                 SNK Min voltage <= SRC Min Voltage
                 &&
                 SNK Op current * SRC Max Voltage <= SRC Max Power

               Requested Voltage : Any value between SRC Min Voltage and SRC Max Voltage : SRC Max Voltage
               Requested Op Current : SNK Op Current
               Requested Max Current : SNK Op Current
            */
            if (  (snkmaxvoltage50mv >= srcmaxvoltage50mv)
                &&(snkminvoltage50mv <= srcminvoltage50mv)
                &&(srcmaxvoltage50mv * snkopcurrent10ma <= srcmaxpower250mw))
            {
              currentrequestedpower = (srcmaxvoltage50mv * snkopcurrent10ma) / 2; /* to get value in mw */
              currentrequestedvoltage = srcmaxvoltage50mv;
            }
            break;

            /* SNK Battery Supply PDO */
          case USBPD_CORE_PDO_TYPE_BATTERY:
            snkmaxvoltage50mv = snkpdo.SNKBatteryPDO.MaxVoltageIn50mVunits;
            snkminvoltage50mv = snkpdo.SNKBatteryPDO.MinVoltageIn50mVunits;
            snkoppower250mw   = snkpdo.SNKBatteryPDO.OperationalPowerIn250mWunits;

            /* Match if :
                 SNK Max voltage >= SRC Max Voltage
                 &&
                 SNK Min voltage <= SRC Min Voltage
                 &&
                 SNK Op power <= SRC Max power

               Requested Voltage : Any value between SRC Min Voltage and SRC Max Voltage, that fulfill
                                   SNK Op power <= Voltage * SRC Max Current
               Requested Op Current : SNK Op Power/ SRC Voltage
               Requested Max Current : SNK Op Power/ SRC Voltage
            */
            if (  (snkmaxvoltage50mv >= srcmaxvoltage50mv)
                &&(snkminvoltage50mv <= srcminvoltage50mv)
                &&(snkoppower250mw <= srcmaxpower250mw))
            {
              currentrequestedpower   = snkoppower250mw * 250; /* to get value in mw */
              currentrequestedvoltage = srcmaxvoltage50mv;
            }
            break;

#if _PPS
            /* SNK Augmented Power Data Object (APDO) */
          case USBPD_CORE_PDO_TYPE_APDO:
            break;
#endif /* _PPS */

          default:
            break;
        }

        if (currentrequestedpower > maxrequestedpower)
        {
          match = USBPD_TRUE;
          maxrequestedpower   = currentrequestedpower;
          maxrequestedvoltage = currentrequestedvoltage;
        }
      }
      break;

#if _PPS
    /* Augmented Power Data Object (APDO) */
    case USBPD_CORE_PDO_TYPE_APDO:
      {
        uint16_t srcmaxvoltage100mv, srcmaxcurrent50ma;
        srcmaxvoltage100mv = srcpdo.SRCSNKAPDO.MaxVoltageIn100mV;
        srcmaxcurrent50ma = srcpdo.SRCSNKAPDO.MaxCurrentIn50mAunits;

        /* Loop through SNK PDO list */
        for (i=0; i<nbsnkpdo; i++)
        {
          currentrequestedpower = 0;
          currentrequestedvoltage = 0;

          /* Retrieve SNK PDO value according to its type */
          snkpdo.d32 = snkpdo_array[i];
          switch(snkpdo.GenericPDO.PowerObject)
          {
            case USBPD_CORE_PDO_TYPE_FIXED:
            case USBPD_CORE_PDO_TYPE_VARIABLE:
            case USBPD_CORE_PDO_TYPE_BATTERY:
              /* No match */
              break;
            /* SNK Augmented Power Data Object (APDO) */
            case USBPD_CORE_PDO_TYPE_APDO:
              {
                uint16_t snkmaxvoltage100mv, snkminvoltage100mv, snkmaxcurrent50ma;

                snkminvoltage100mv = snkpdo.SRCSNKAPDO.MinVoltageIn100mV;
                snkmaxvoltage100mv = snkpdo.SRCSNKAPDO.MaxVoltageIn100mV;
                snkmaxcurrent50ma = snkpdo.SRCSNKAPDO.MaxCurrentIn50mAunits;

                /* Match if voltage matches with the APDO voltage range */
                if ((PWR_DECODE_100MV(snkminvoltage100mv) <= (*PtrRequestedVoltage))
                 && ((*PtrRequestedVoltage) <= PWR_DECODE_100MV(snkmaxvoltage100mv))
                 && (snkmaxcurrent50ma <= srcmaxcurrent50ma))
                {
                  if (0 != *PtrRequestedPower)
                  {
                    currentrequestedpower = (*PtrRequestedVoltage * PWR_DECODE_50MA(snkmaxcurrent50ma)) / 1000; /* to get value in mw */
                    currentrequestedvoltage = (*PtrRequestedVoltage / 50);
                  }
                  else
                  {
                    *PtrRequestedVoltage = MIN(PWR_DECODE_100MV(srcmaxvoltage100mv), PWR_DECODE_100MV(snkmaxvoltage100mv));
                    currentrequestedpower = (*PtrRequestedVoltage * PWR_DECODE_50MA(snkmaxcurrent50ma)) / 1000; /* to get value in mw */
                    currentrequestedvoltage = (*PtrRequestedVoltage / 50);
                  }
                }
              }
              break;

            default:
              break;
          }

          if (currentrequestedpower > maxrequestedpower)
          {
            match = USBPD_TRUE;
            maxrequestedpower   = currentrequestedpower;
            maxrequestedvoltage = currentrequestedvoltage;
          }
        }
      }
      break;
#endif /* _PPS */

    default:
      return(USBPD_FALSE);
  }

  if (maxrequestedpower > 0)
  {
    *PtrRequestedPower   = maxrequestedpower;
    *PtrRequestedVoltage = maxrequestedvoltage * 50; /* value in mV */
  }
  return(match);
}

/**
  * @brief  Find PDO index that offers the most amount of power and in accordance with SNK capabilities.
  * @param  PortNum Port number
  * @param  PtrRequestPowerDetails  Sink requested power details structure pointer
  * @retval Index of PDO within source capabilities message (DPM_NO_SRC_PDO_FOUND indicating not found)
  */
static uint32_t DPM_FindVoltageIndex(uint32_t PortNum, USBPD_DPM_SNKPowerRequestDetails_TypeDef* PtrRequestPowerDetails)
{
  uint32_t *ptpdoarray;
  USBPD_PDO_TypeDef  pdo;
  uint32_t voltage, reqvoltage, nbpdo, allowablepower, maxpower;
  uint32_t curr_index = DPM_NO_SRC_PDO_FOUND, temp_index;
  USBPD_USER_SettingsTypeDef *puser = (USBPD_USER_SettingsTypeDef *)&DPM_USER_Settings[PortNum];

  allowablepower = 0;
  maxpower       = 0;
  reqvoltage     = 0;
  voltage        = 0;

  /* Search PDO index among Source PDO of Port */
  nbpdo = DPM_Ports[PortNum].DPM_NumberOfRcvSRCPDO;
  ptpdoarray = DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO;

  /* search the better PDO in the list of source PDOs */
  for(temp_index = 0; temp_index < nbpdo; temp_index++)
  {
    pdo.d32 = ptpdoarray[temp_index];
    /* check if the received source PDO is matching any of the SNK PDO */
    allowablepower = 0;
    if (USBPD_TRUE == USBPD_DPM_SNK_EvaluateMatchWithSRCPDO(PortNum, pdo.d32, &voltage, &allowablepower))
    {
      /* choose the "better" PDO, in this case only the distance in absolute value from the target voltage */
      if (allowablepower >= maxpower)
      {
        /* Add additional check for compatibility of this SRC PDO with port characteristics (defined in DPM_USER_Settings) */
        if (  (voltage >= puser->DPM_SNKRequestedPower.MinOperatingVoltageInmVunits)
            &&(voltage <= puser->DPM_SNKRequestedPower.MaxOperatingVoltageInmVunits)
            &&(allowablepower <= puser->DPM_SNKRequestedPower.MaxOperatingPowerInmWunits))
        {
          /* consider the current PDO the better one until now */
          curr_index = temp_index;
          maxpower   = allowablepower;
          reqvoltage = voltage;
        }
      }
    }
  }

  if (curr_index != DPM_NO_SRC_PDO_FOUND)
  {
    PtrRequestPowerDetails->MaxOperatingCurrentInmAunits = puser->DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits;
    PtrRequestPowerDetails->OperatingCurrentInmAunits    = (1000 * maxpower)/voltage;
    PtrRequestPowerDetails->MaxOperatingPowerInmWunits   = puser->DPM_SNKRequestedPower.MaxOperatingPowerInmWunits;
    PtrRequestPowerDetails->OperatingPowerInmWunits      = maxpower;
    PtrRequestPowerDetails->RequestedVoltageInmVunits    = reqvoltage;
  }

  return curr_index;
}

/**
  * @brief  Build RDO to be used in Request message according to selected PDO from received SRC Capabilities
  * @param  PortNum           Port number
  * @param  IndexSrcPDO       Index on the selected SRC PDO (value between 0 to 6)
  * @param  PtrRequestPowerDetails  Sink requested power details structure pointer
  * @param  Rdo               Pointer on the RDO
  * @param  PtrPowerObject    Pointer on the selected power object
  * @retval None
  */
void DPM_SNK_BuildRDOfromSelectedPDO(uint8_t PortNum, uint8_t IndexSrcPDO, USBPD_DPM_SNKPowerRequestDetails_TypeDef* PtrRequestPowerDetails,
                                     USBPD_SNKRDO_TypeDef* Rdo, USBPD_CORE_PDO_Type_TypeDef *PtrPowerObject)
{
  uint32_t mv = 0, mw = 0, ma = 0, size;
  USBPD_PDO_TypeDef  pdo;
  USBPD_SNKRDO_TypeDef rdo;
  USBPD_HandleTypeDef *pdhandle = &DPM_Ports[PortNum];
  USBPD_USER_SettingsTypeDef *puser = (USBPD_USER_SettingsTypeDef *)&DPM_USER_Settings[PortNum];
  uint32_t snkpdolist[USBPD_MAX_NB_PDO];
  USBPD_PDO_TypeDef snk_fixed_pdo;

  /* Initialize RDO */
  rdo.d32 = 0;

  /* Read SNK PDO list for retrieving useful data to fill in RDO */
  USBPD_PWR_IF_GetPortPDOs(PortNum, USBPD_CORE_DATATYPE_SNK_PDO, (uint8_t*)&snkpdolist[0], &size);

  /* Store value of 1st SNK PDO (Fixed) in local variable */
  snk_fixed_pdo.d32 = snkpdolist[0];

  /* Set common fields in RDO */
  pdo.d32 = pdhandle->DPM_ListOfRcvSRCPDO[0];
  rdo.GenericRDO.USBCommunicationsCapable     = snk_fixed_pdo.SNKFixedPDO.USBCommunicationsCapable;
#if defined(USBPD_REV30_SUPPORT) && defined(_UNCHUNKED_SUPPORT)
  if (USBPD_SPECIFICATION_REV2 < DPM_Params[PortNum].PE_SpecRevision)
  {
    rdo.FixedVariableRDO.UnchunkedExtendedMessage = DPM_Settings[PortNum].PE_PD3_Support.d.PE_UnchunkSupport;
    DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_FALSE;
    /* Set unchuncked bit if supported by port partner;*/
    if (USBPD_TRUE == pdo.SRCFixedPDO.UnchunkedExtendedMessage)
    {
      DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_TRUE;
    }
  }
#endif /* USBPD_REV30_SUPPORT && _UNCHUNKED_SUPPORT */

  /* If no valid SNK PDO or if no SRC PDO match found (index>=nb of valid received SRC PDOs */
  if ((size < 1) || (IndexSrcPDO >= pdhandle->DPM_NumberOfRcvSRCPDO))
  {
    /* USBPD_DPM_EvaluateCapabilities: Mismatch, could not find desired pdo index */
#ifdef _TRACE
    USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t*)"DPM_SNK_BuildRDOfromSelectedPDO: Pb in SRC PDO selection", sizeof("DPM_SNK_BuildRDOfromSelectedPDO: Pb in SRC PDO selection"));
#endif /* _TRACE */
    rdo.FixedVariableRDO.ObjectPosition = 1;
    rdo.FixedVariableRDO.OperatingCurrentIn10mAunits  = pdo.SRCFixedPDO.MaxCurrentIn10mAunits;
    rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits = puser->DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits / 10;
    rdo.FixedVariableRDO.CapabilityMismatch           = 1;
    rdo.FixedVariableRDO.USBCommunicationsCapable     = snk_fixed_pdo.SNKFixedPDO.USBCommunicationsCapable;
    DPM_Ports[PortNum].DPM_RequestedCurrent           = puser->DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits;
    /* USBPD_DPM_EvaluateCapabilities: Mismatch, could not find desired pdo index */

    pdhandle->DPM_RequestDOMsg = rdo.d32;
    return;
  }

  /* Set the Object position */
  rdo.GenericRDO.ObjectPosition               = IndexSrcPDO + 1;
  rdo.GenericRDO.NoUSBSuspend                 = 1;

  /* Extract power information from Power Data Object */
  pdo.d32 = pdhandle->DPM_ListOfRcvSRCPDO[IndexSrcPDO];

  *PtrPowerObject = pdo.GenericPDO.PowerObject;

  /* Retrieve request details from SRC PDO selection */
  mv = PtrRequestPowerDetails->RequestedVoltageInmVunits;
  ma = PtrRequestPowerDetails->OperatingCurrentInmAunits;

  switch(pdo.GenericPDO.PowerObject)
  {
  case USBPD_CORE_PDO_TYPE_FIXED:
  case USBPD_CORE_PDO_TYPE_VARIABLE:
    {
      /* USBPD_DPM_EvaluateCapabilities: Mismatch, less power offered than the operating power */
      ma = USBPD_MIN(ma, puser->DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits);
      mw = (ma * mv)/1000; /* mW */
      DPM_Ports[PortNum].DPM_RequestedCurrent           = ma;
      rdo.FixedVariableRDO.OperatingCurrentIn10mAunits  = ma / 10;
      rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits = ma / 10;
      if(mw < puser->DPM_SNKRequestedPower.OperatingPowerInmWunits)
      {
        /* USBPD_DPM_EvaluateCapabilities: Mismatch, less power offered than the operating power */
        rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits = puser->DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits / 10;
        rdo.FixedVariableRDO.CapabilityMismatch = 1;
      }
    }
    break;

  case USBPD_CORE_PDO_TYPE_BATTERY:
    {
      /* USBPD_DPM_EvaluateCapabilities: Battery Request Data Object */
      mw = USBPD_MIN(PtrRequestPowerDetails->OperatingPowerInmWunits, puser->DPM_SNKRequestedPower.MaxOperatingPowerInmWunits); /* mW */
      ma = (1000 * mw) / mv; /* mA */
      ma = USBPD_MIN(ma, puser->DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits);
      DPM_Ports[PortNum].DPM_RequestedCurrent       = ma;
      mw = (ma * mv)/1000; /* mW */
      rdo.BatteryRDO.OperatingPowerIn250mWunits     = mw / 250;
      rdo.BatteryRDO.MaxOperatingPowerIn250mWunits  = mw / 250;
      if(mw < puser->DPM_SNKRequestedPower.OperatingPowerInmWunits)
      {
        /* Mismatch, less power offered than the operating power */
        rdo.BatteryRDO.CapabilityMismatch = 1;
      }
    }
    break;

#if _PPS
  case USBPD_CORE_PDO_TYPE_APDO:
    {
      DPM_Ports[PortNum].DPM_RequestedCurrent    = ma;
      rdo.ProgRDO.ObjectPosition                 = IndexSrcPDO + 1;
      rdo.ProgRDO.OperatingCurrentIn50mAunits    = ma / 50;
      rdo.ProgRDO.OutputVoltageIn20mV            = mv / 20;
    }
    break;
#endif /* _PPS */
  default:
    break;
  }

  pdhandle->DPM_RequestDOMsg = rdo.d32;
  pdhandle->DPM_RDOPosition  = rdo.GenericRDO.ObjectPosition;

  Rdo->d32 = pdhandle->DPM_RequestDOMsg;
  /* Get the requested voltage */
  pdhandle->DPM_RequestedVoltage = mv;
}
#endif /* _SNK || _DRP */

#if defined(_SRC) || defined(_DRP)
/**
  * @brief  Turn Off power supply.
  * @param  PortNum The current port number
  * @param  Role    Port power role
  * @retval USBPD_OK, USBPD_ERROR
  */
static USBPD_StatusTypeDef DPM_TurnOffPower(uint8_t PortNum, USBPD_PortPowerRole_TypeDef Role)
{
  USBPD_StatusTypeDef status = USBPD_OK;

  status = USBPD_PWR_IF_VBUSDisable(PortNum);
  return status;
}

/**
  * @brief  Turn On power supply.
  * @param  PortNum The current port number
  * @param  Role    Port power role
  * @retval USBPD_ACCEPT, USBPD_WAIT, USBPD_REJECT
  */
static USBPD_StatusTypeDef DPM_TurnOnPower(uint8_t PortNum, USBPD_PortPowerRole_TypeDef Role)
{
  USBPD_StatusTypeDef status = USBPD_OK;
  /* Enable the output */
  status = USBPD_PWR_IF_VBUSEnable(PortNum);
  if(USBPD_PORTPOWERROLE_SRC == Role)
  {
    /* Enable the output */
    USBPD_DPM_WaitForTime(30);
  }
  else
  {
    /* stop current sink */
  }

  return status;
}

/**
  * @brief  Assert Rp resistor.
  * @param  PortNum The current port number
  * @retval None
  */
static void DPM_AssertRp(uint8_t PortNum)
{
#if defined(USBPD_TCPM_MODULE_ENABLED)
  USBPD_TCPM_set_cc(PortNum, TYPEC_CC_RP, USBPD_ENABLE);
#else
  USBPD_CAD_AssertRp(PortNum);
#endif /* USBPD_TCPM_MODULE_ENABLED*/
}
#endif /* _SRC || _DRP */

#if defined(_DRP)
/**
  * @brief  Assert Rd resistor.
  * @param  PortNum The current port number
  * @retval None
  */
static void DPM_AssertRd(uint8_t PortNum)
{
#if defined(USBPD_TCPM_MODULE_ENABLED)
  USBPD_TCPM_set_cc(PortNum, TYPEC_CC_RD, USBPD_ENABLE);
#else
  USBPD_CAD_AssertRd(PortNum);
#endif /* USBPD_TCPM_MODULE_ENABLED*/
}

#endif /* _DRP  */

#ifdef _RTOS
static uint32_t CheckDPMTimers(void)
{
  uint32_t _timing = osWaitForever;

#if defined(USBPD_REV30_SUPPORT) && (_SRC_CAPA_EXT || _ALERT) || defined(USBPD_BILLBOARD)
  uint32_t _current_timing;

  /* Calculate the minimum timers to wake-up DPM tasks */
  for(uint8_t instance = 0; instance < USBPD_PORT_COUNT; instance++)
  {
#ifdef USB_BILLBOARD
    _current_timing = DPM_Ports[instance].DPM_TimerAME & DPM_TIMER_READ_MSK;
    if(_current_timing > 0)
    {
      if (_current_timing < _timing)
      {
        _timing = _current_timing;
      }
    }
#endif /* USB_BILLBOARD */
#if _SRC_CAPA_EXT
    _current_timing = DPM_Ports[instance].DPM_TimerSRCExtendedCapa & DPM_TIMER_READ_MSK;
    if(_current_timing > 0)
    {
      if (_current_timing < _timing)
      {
        _timing = _current_timing;
      }
    }
#endif /* _SRC_CAPA_EXT */
#if _ALERT
    _current_timing = DPM_Ports[instance].DPM_TimerAlert & DPM_TIMER_READ_MSK;
    if(_current_timing > 0)
    {
      if (_current_timing < _timing)
      {
        _timing = _current_timing;
      }
    }
#endif /* _ALERT */
  }
#endif /* USBPD_REV30_SUPPORT */
  return _timing;
}
#endif /* _RTOS */

#if defined(USBPD_REV30_SUPPORT)
#if _SRC_CAPA_EXT
/**
  * @brief  Manage the send of the message GET EXTENDED CAPA.
  * @retval none
  */
void DPM_ManageExtendedCapa(void)
{
  for(uint8_t _instance = 0; _instance < USBPD_PORT_COUNT; _instance++)
  {
    /* -------------------------------------------------  */
    /* Check if send SRC extended capa timer is expired   */
    /* -------------------------------------------------  */
    if (DPM_TIMER_ENABLE_MSK == DPM_Ports[_instance].DPM_TimerSRCExtendedCapa)
    {
      DPM_Ports[_instance].DPM_TimerSRCExtendedCapa = 0;
      USBPD_DPM_RequestGetSourceCapabilityExt(_instance);
    }
  }
}

#endif /* _SRC_CAPA_EXT */
#if _ALERT
/**
  * @brief  Manage the ALERT.
  * @retval none
  */
void DPM_ManageAlert(void)
{
  for(uint8_t _instance = 0; _instance < USBPD_PORT_COUNT; _instance++)
  {
    /* check if Alert timer is expired */
    if (DPM_TIMER_ENABLE_MSK == DPM_Ports[_instance].DPM_TimerAlert)
    {
      /* Restart alert timer */
      DPM_START_TIMER(_instance, DPM_TimerAlert, DPM_TIMER_ALERT);
      DPM_Ports[_instance].DPM_MeasuredCurrent = HW_IF_PWR_GetCurrent(_instance);
      if (DPM_Ports[_instance].DPM_MeasuredCurrent > 3600)
      {
        USBPD_DPM_RequestHardReset(_instance);
      }
      else
      {
        if (DPM_Ports[_instance].DPM_MeasuredCurrent > 3400)
        {
          if (0 == (DPM_Ports[_instance].DPM_SendAlert.b.TypeAlert & USBPD_ADO_TYPE_ALERT_OCP))
          {
            USBPD_ADO_TypeDef alert = {0};
            alert.b.TypeAlert = USBPD_ADO_TYPE_ALERT_OCP;
            USBPD_DPM_RequestAlert(_instance, alert);
            DPM_Ports[_instance].DPM_SendAlert.b.TypeAlert |= alert.b.TypeAlert;
          }
        }
        else
        {
          /* Reset of the OCP bit */
          DPM_Ports[_instance].DPM_SendAlert.b.TypeAlert &= ~USBPD_ADO_TYPE_ALERT_OCP;
#if _PPS && (defined(_SRC) || defined(_DRP))
          /*
            The Operating Condition Change bit Shall be set when a Source or Sink detects its
            Operating Condition enters or exits either the <warning> or <over temperature> temperature states.
            The Operating Condition Change bit Shall be set when the Source operating in the
            Programmable Power Supply mode detects it has changed its operating condition between Constant Voltage (CV)
            and Current Limit (CL).
          */
          USBPD_PDO_TypeDef pdo;
          pdo.d32 = PWR_Port_PDO_Storage[_instance].SourcePDO.ListOfPDO[DPM_Ports[_instance].DPM_RDOPosition];
          if (USBPD_CORE_PDO_TYPE_APDO == pdo.GenericPDO.PowerObject)
          {
            if (DPM_Ports[_instance].DPM_MeasuredCurrent > PWR_DECODE_50MA(pdo.SRCSNKAPDO.MaxCurrentIn50mAunits))
            {
              if (0 == (DPM_Ports[_instance].DPM_SendAlert.b.TypeAlert & USBPD_ADO_TYPE_ALERT_OPERATING_COND))
              {
                USBPD_ADO_TypeDef alert = {0};
                alert.b.TypeAlert = USBPD_ADO_TYPE_ALERT_OPERATING_COND;
                USBPD_DPM_RequestAlert(_instance, alert);
                DPM_Ports[_instance].DPM_SendAlert.b.TypeAlert |= alert.b.TypeAlert;
              }
            }
            else
            {
              /* Reset alert bits */
              DPM_Ports[_instance].DPM_SendAlert.b.TypeAlert = 0;
            }
          }
          else
          {
            /* Reset alert bits */
            DPM_Ports[_instance].DPM_SendAlert.b.TypeAlert = 0;
          }
#endif /* _PPS && (_SRC || _DRP) */
        }
      }
    }
  }
}

#endif /* _ALERT */
#endif /* USBPD_REV30_SUPPORT */

#if defined (_USB_DEVICE) || defined (_USB_HOST)
/**
  * @brief  Initialize billboard driver
  * @param  None
  * @retval status
  */
static USBPD_StatusTypeDef DPM_USB_Init(void)
{
#if defined (USB_BILLBOARD)
#if (USBD_BOS_ENABLED == 1)
  USBD_BB_AlternateModeTypeDef AlternateMode;

  /* Add Display Port alternate mode */
  AlternateMode.VendorID = 0xFF01;
  AlternateMode.bAlternateMode = 0x01;
  AlternateMode.iAlternateModeString = 0x00;
  if(USBD_OK != USBD_BB_AddAlternateMode(hUSBDBOSDesc, &AlternateMode))
  {
    return USBPD_ERROR;
  }

  /* Setup Alternate mode with Index=0x00 as the preferred alternate mode */
  if(USBD_OK != USBD_BB_SetupPreferedAltMode (hUSBDBOSDesc, 0x00))
  {
    return USBPD_ERROR;
  }

  /* Setup the state of the alternate mode with index=0x00 */
  if(USBD_OK != USBD_BB_SetupAltModeState (hUSBDBOSDesc, 0x00, CONFIGURATION_NOT_ATTEMPTED))
  {
    return USBPD_ERROR;
  }
#endif
  /* Init Device Library,Add Supported Class and Start the library*/
  if(USBD_OK != USBD_Init(&hUsbDevice[USBPD_PORT_0], &BB_Desc, 0))
  {
    return USBPD_ERROR;
  }

  if(USBD_OK != USBD_RegisterClass(&hUsbDevice[USBPD_PORT_0], &USBD_BB))
  {
    return USBPD_ERROR;
  }
#else
  /* Init MSC Application */
  USBD_Init(&hUsbDevice[USBPD_PORT_0], &MSC_Desc, 0);

  /* initialize the MSC class */
  /* Add Supported Class */
  USBD_RegisterClass(&hUsbDevice[USBPD_PORT_0], USBD_MSC_CLASS);

  /* Add Storage callbacks for MSC Class */
  USBD_MSC_RegisterStorage(&hUsbDevice[USBPD_PORT_0], &USBD_DISK_fops);
#endif

  for(uint8_t index = 0; index < USBPD_PORT_COUNT; index++)
  {
    DPM_Ports[index].DPM_USBState = 0;
  }
  return USBPD_OK;
}

/**
  * @brief  check if BILLBAORD usb driver must be started.
  * @retval none
  */
void DPM_USB_Start(uint32_t PortNum)
{
  /* check if tAMETimeout is expired */
  /* Check if VDM failed */
  if(DPM_Ports[PortNum].DPM_USBState == 0)
  {
    switch(DPM_Params[PortNum].PE_DataRole)
    {
    case USBPD_PORTDATAROLE_UFP :
      {
#if defined(USB_BILLBOARD) || defined(USB_MSC)
        /* Start Billboard driver */
        USBD_Start(&hUsbDevice[PortNum]);
        DPM_Ports[PortNum].DPM_USBState = 1;
#endif /* USB_BILLBOARD */
        break;
      }
    case USBPD_PORTDATAROLE_DFP :
    default :
      {
        break;
      }
    }
  }
}

void DPM_USB_Stop(uint32_t PortNum)
{
  if(DPM_Ports[PortNum].DPM_USBState == 1)
  {
#if defined(USB_BILLBOARD) || defined(USB_MSC)
    USBD_Stop(&hUsbDevice[0]);
    DPM_Ports[PortNum].DPM_USBState = 0;
#endif
  }

  if(DPM_Ports[PortNum].DPM_USBState == 2)
  {
    /* Stop the host */
  }
}

#endif /* _USB */

/* USER CODE END USBPD_USER_PRIVATE_FUNCTIONS */
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


