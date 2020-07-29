/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_dpm_core.c
  * @author  MCD Application Team
  * @brief   USBPD dpm core file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#define __USBPD_DPM_CORE_C

/* Includes ------------------------------------------------------------------*/
#include "usbpd_core.h"
#include "usbpd_trace.h"
#include "usbpd_dpm_core.h"
#if !defined(USBPDCORE_LIB_NO_PD) || defined(USBPD_TYPE_STATE_MACHINE)
#include "usbpd_dpm_user.h"
#endif /* !USBPDCORE_LIB_NO_PD || USBPD_TYPE_STATE_MACHINE */
#include "usbpd_dpm_conf.h"

#if defined(USBPD_TCPM_MODULE_ENABLED)
#if defined(USBPD_STUSB1605)
#include "p-nucleo-usb002.h"
#else
#include "usbpd_tcpci.h"
#endif /*USBPD_STUSB1605*/
#endif /* USBPD_TCPM_MODULE_ENABLED */

#ifdef _RTOS
#include "cmsis_os.h"
#if (osCMSIS >= 0x20000U)
#include "task.h"
#endif /* osCMSIS >= 0x20000U */
#else
#if defined(USE_STM32_UTILITY_OS)
#include "utilities_conf.h"
#endif /* USE_STM32_UTILITY_OS */
#if defined(USBPD_TCPM_MODULE_ENABLED)
#include "usbpd_timersserver.h"
#endif /* USBPD_TCPM_MODULE_ENABLED */
#endif /* _RTOS */

#if defined(_FWUPDATE_RESPONDER)
#include "usbpd_pdfu_responder.h"
#endif /* _FWUPDATE_RESPONDER */

#if defined(_LOW_POWER)
#include "usbpd_lowpower.h"
#endif

/* Generic STM32 prototypes */
extern uint32_t HAL_GetTick(void);

/* Private function prototypes -----------------------------------------------*/
#ifdef _RTOS
#if (osCMSIS < 0x20000U)
#if !defined(USBPDCORE_LIB_NO_PD)
void USBPD_PE_Task(void const *argument);
#endif /* !USBPDCORE_LIB_NO_PD */
#if defined(USBPD_TCPM_MODULE_ENABLED)
void USBPD_ALERT_Task(void const *argument);
#else
#if !defined(USBPDCORE_LIB_NO_PD) || defined(USBPD_TYPE_STATE_MACHINE)
void USBPD_CAD_Task(void const *argument);
#endif /* !USBPDCORE_LIB_NO_PD || USBPD_TYPE_STATE_MACHINE */
#endif /* USBPD_TCPM_MODULE_ENABLED */
#if defined(_TRACE) || defined(_GUI_INTERFACE)
void USBPD_TRACE_TX_Task(void const *argument);
#endif /* _TRACE || _GUI_INTERFACE */

#else /* osCMSIS >= 0x20000U */

#if !defined(USBPDCORE_LIB_NO_PD)
void USBPD_PE_Task_P0(void *argument);
void USBPD_PE_Task_P1(void *argument);
static void PE_Task(uint32_t PortNum);
#endif /* !USBPDCORE_LIB_NO_PD */
#if defined(USBPD_TCPM_MODULE_ENABLED)
void USBPD_ALERT_Task(void *argument);
#else
#if !defined(USBPDCORE_LIB_NO_PD) || defined(USBPD_TYPE_STATE_MACHINE)
void USBPD_CAD_Task(void *argument);
#endif /* !USBPDCORE_LIB_NO_PD || USBPD_TYPE_STATE_MACHINE */
#endif /* USBPD_TCPM_MODULE_ENABLED */
#if defined(_TRACE) || defined(_GUI_INTERFACE)
void USBPD_TRACE_TX_Task(void *argument);
#endif /* _TRACE || _GUI_INTERFACE */

#endif /* osCMSIS < 0x20000U */
#else /* !_RTOS */

#if !defined(USBPDCORE_LIB_NO_PD) || defined(USBPD_TYPE_STATE_MACHINE)
void USBPD_CAD_Task(void);
#if defined(USE_STM32_UTILITY_OS)
void TimerCADfunction(void *);
#endif /* USE_STM32_UTILITY_OS */
#endif /* !USBPDCORE_LIB_NO_PD || USBPD_TYPE_STATE_MACHINE */

#if defined(_TRACE)
void USBPD_TRACE_TX_Task(void);
#if defined(USE_STM32_UTILITY_OS)
void TimerTRACEfunction(void *pArg);
#endif /* USE_STM32_UTILITY_OS */
#endif /* _TRACE */

#if !defined(USBPDCORE_LIB_NO_PD)
void USBPD_PE_Task_P0(void);
void USBPD_PE_Task_P1(void);
#if defined(USE_STM32_UTILITY_OS)
void TimerPE0function(void *pArg);
void TimerPE1function(void *pArg);
#endif /* USE_STM32_UTILITY_OS */
#endif /* !USBPDCORE_LIB_NO_PD  */

void USBPD_TaskUser(void);


#endif /* _RTOS */

/* Private typedef -----------------------------------------------------------*/
#ifdef _RTOS
#if (osCMSIS < 0x20000U)
#define DPM_STACK_SIZE_ADDON_FOR_CMSIS              1
#else
#define DPM_STACK_SIZE_ADDON_FOR_CMSIS              4
#endif /* osCMSIS < 0x20000U */
#if !defined(USBPDCORE_LIB_NO_PD)
#define FREERTOS_PE_PRIORITY                    osPriorityAboveNormal
#if defined(_VDM)
#define FREERTOS_PE_STACK_SIZE                  (350 * DPM_STACK_SIZE_ADDON_FOR_CMSIS)
#elif defined(__AUTHENTICATION__)
#define FREERTOS_PE_STACK_SIZE                  (350 * DPM_STACK_SIZE_ADDON_FOR_CMSIS)
#else
#define FREERTOS_PE_STACK_SIZE                  (200 * DPM_STACK_SIZE_ADDON_FOR_CMSIS)
#endif /*_VDM*/
#endif /* !USBPDCORE_LIB_NO_PD */
#if defined(USBPD_TCPM_MODULE_ENABLED)
#define FREERTOS_ALERT_PRIORITY                 osPriorityRealtime
#if defined(_TRACE)
#define FREERTOS_ALERT_STACK_SIZE               (240 * DPM_STACK_SIZE_ADDON_FOR_CMSIS)
#else
#define FREERTOS_ALERT_STACK_SIZE               (120 * DPM_STACK_SIZE_ADDON_FOR_CMSIS)
#endif /* _TRACE */
#else
#define FREERTOS_CAD_PRIORITY                   osPriorityRealtime
#define FREERTOS_CAD_STACK_SIZE                 (300 * DPM_STACK_SIZE_ADDON_FOR_CMSIS)
#endif /*USBPD_TCPM_MODULE_ENABLED*/
#if defined(_TRACE) || defined(_GUI_INTERFACE)
#define FREERTOS_TRACE_PRIORITY                 osPriorityLow
#define FREERTOS_TRACE_STACK_SIZE               (300 * DPM_STACK_SIZE_ADDON_FOR_CMSIS)
#endif /* _TRACE || _GUI_INTERFACE */

#if (osCMSIS < 0x20000U)
#if !defined(USBPDCORE_LIB_NO_PD)
osThreadDef(PE_0, USBPD_PE_Task, FREERTOS_PE_PRIORITY, 0, FREERTOS_PE_STACK_SIZE);
osThreadDef(PE_1, USBPD_PE_Task, FREERTOS_PE_PRIORITY, 0, FREERTOS_PE_STACK_SIZE);
osMessageQDef(queuePE, 1, uint16_t);
#endif /* !USBPDCORE_LIB_NO_PD */
#if defined(USBPD_TCPM_MODULE_ENABLED)
osThreadDef(ALERTTask, USBPD_ALERT_Task, FREERTOS_ALERT_PRIORITY, 0, FREERTOS_ALERT_STACK_SIZE);
#else
#if !defined(USBPDCORE_LIB_NO_PD) || defined(USBPD_TYPE_STATE_MACHINE)
osThreadDef(CAD, USBPD_CAD_Task, FREERTOS_CAD_PRIORITY, 0, FREERTOS_CAD_STACK_SIZE);
osMessageQDef(queueCAD, 2, uint16_t);
#endif /* !USBPDCORE_LIB_NO_PD || USBPD_TYPE_STATE_MACHINE */
#endif /* USBPD_TCPM_MODULE_ENABLED */
#if defined(_TRACE) || defined(_GUI_INTERFACE)
osThreadDef(TRA_TX, USBPD_TRACE_TX_Task, FREERTOS_TRACE_PRIORITY, 0, FREERTOS_TRACE_STACK_SIZE);
osMessageQDef(queueTRACE, 1, uint16_t);
#endif /* _TRACE || _GUI_INTERFACE */

#else /* osCMSIS >= 0x20000U */

#if !defined(USBPDCORE_LIB_NO_PD)
osThreadAttr_t PE0_Thread_Atrr = {
  .name       = "PE_0",
  .priority   = FREERTOS_PE_PRIORITY, /*osPriorityAboveNormal,*/
  .stack_size = FREERTOS_PE_STACK_SIZE
};
osThreadAttr_t PE1_Thread_Atrr = {
  .name       = "PE_1",
  .priority   = FREERTOS_PE_PRIORITY,
  .stack_size = FREERTOS_PE_STACK_SIZE
};
#endif /* !USBPDCORE_LIB_NO_PD */

#if defined(USBPD_TCPM_MODULE_ENABLED)
osThreadAttr_t ALERT_Thread_Atrr = {
  .name       = "ALERTTask",
  .priority   = FREERTOS_ALERT_PRIORITY, /*osPriorityRealtime,*/
  .stack_size = FREERTOS_ALERT_STACK_SIZE
};
#else
#if !defined(USBPDCORE_LIB_NO_PD) || defined(USBPD_TYPE_STATE_MACHINE)
osThreadAttr_t CAD_Thread_Atrr = {
  .name       = "CAD",
  .priority   = FREERTOS_CAD_PRIORITY, /*osPriorityRealtime,*/
  .stack_size = FREERTOS_CAD_STACK_SIZE
};
#endif /* !USBPDCORE_LIB_NO_PD || USBPD_TYPE_STATE_MACHINE */
#endif /* USBPD_TCPM_MODULE_ENABLED */

#if defined(_TRACE) || defined(_GUI_INTERFACE)
osThreadAttr_t TRA_Thread_Atrr = {
  .name       = "TRA_TX",
  .priority   = FREERTOS_TRACE_PRIORITY, /*osPriorityLow,*/
  .stack_size = FREERTOS_TRACE_STACK_SIZE
};
#endif /* _TRACE || _GUI_INTERFACE */
#endif /* osCMSIS < 0x20000U */

#else /* !_RTOS */

#if defined(USE_STM32_UTILITY_OS)
#if !defined(USBPDCORE_LIB_NO_PD) || defined(USBPD_TYPE_STATE_MACHINE)
UTIL_TIMER_Object_t TimerCAD;
#endif /* !USBPDCORE_LIB_NO_PD || USBPD_TYPE_STATE_MACHINE */
#if defined(_TRACE)
UTIL_TIMER_Object_t TimerTRACE;
#endif /* _TRACE */
#if !defined(USBPDCORE_LIB_NO_PD)
UTIL_TIMER_Object_t TimerPE0,TimerPE1;
#endif /* !USBPDCORE_LIB_NO_PD */
#endif /* USE_STM32_UTILITY_OS */

#endif /* _RTOS */

/* Private define ------------------------------------------------------------*/
#ifdef _RTOS
#if (osCMSIS < 0x20000U)
#define OSTHREAD_PE(__PORT__)       (((__PORT__) == USBPD_PORT_0) ? osThread(PE_0) : osThread(PE_1))
#else
#define OSTHREAD_PE(__PORT__)       (((__PORT__) == USBPD_PORT_0) ? USBPD_PE_Task_P0 : USBPD_PE_Task_P1)
#define OSTHREAD_PE_ATTR(__PORT__)  (((__PORT__) == USBPD_PORT_0) ? &PE0_Thread_Atrr : &PE1_Thread_Atrr)
#endif /* osCMSIS < 0x20000U */
#endif /* _RTOS */

/* Private macro -------------------------------------------------------------*/
#define CHECK_PE_FUNCTION_CALL(_function_)  _retr = _function_;                  \
                                            if(USBPD_OK != _retr) {return _retr;}
#define CHECK_CAD_FUNCTION_CALL(_function_) if(USBPD_CAD_OK != _function_) {return USBPD_ERROR;}

#if defined(_DEBUG_TRACE)
#define DPM_CORE_DEBUG_TRACE(_PORTNUM_, __MESSAGE__)  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, _PORTNUM_, 0u, (uint8_t *)(__MESSAGE__), sizeof(__MESSAGE__) - 1u);
#else
#define DPM_CORE_DEBUG_TRACE(_PORTNUM_, __MESSAGE__)
#endif /* _DEBUG_TRACE */

/* Private variables ---------------------------------------------------------*/
#ifdef _RTOS
#if defined(USBPDCORE_LIB_NO_PD)
#if defined(USBPD_TYPE_STATE_MACHINE)
static osMessageQId CADQueueId;
#endif /* USBPD_TYPE_STATE_MACHINE */
#else
static osThreadId DPM_PEThreadId_Table[USBPD_PORT_COUNT];
#if defined(USBPD_TCPM_MODULE_ENABLED)
osMessageQId  AlarmMsgBox;
osThreadId ThreadAlert;
#else
static osMessageQId CADQueueId;
#endif /* USBPD_TCPM_MODULE_ENABLED */
static osMessageQId PEQueueId[USBPD_PORT_COUNT];
#endif /* USBPDCORE_LIB_NO_PD */
#if defined(_TRACE) || defined(_GUI_INTERFACE)
osMessageQId TraceQueueId;
#endif /* _TRACE || _GUI_INTERFACE */
#else
#if !defined(USE_STM32_UTILITY_OS)
#if defined(_TRACE)
#define OFFSET_TRACE 1U
#else
#define OFFSET_TRACE 0U
#endif /* _TRACE */
#if defined(USBPD_TCPM_MODULE_ENABLED)
#define OFFSET_CAD 0U
#else
#define OFFSET_CAD 1U
#endif /* USBPD_TCPM_MODULE_ENABLED */
#if !defined(USBPDCORE_LIB_NO_PD) || defined(USBPD_TYPE_STATE_MACHINE) || defined(_TRACE) || defined(_GUI_INTERFACE)
static uint32_t DPM_Sleep_time[USBPD_PORT_COUNT + OFFSET_CAD + OFFSET_TRACE];
static uint32_t DPM_Sleep_start[USBPD_PORT_COUNT + OFFSET_CAD + OFFSET_TRACE];
#endif /* !USBPDCORE_LIB_NO_PD || USBPD_TYPE_STATE_MACHINE ||_TRACE || _GUI_INTERFACE */
#endif /* !USE_STM32_UTILITY_OS */
#endif /* _RTOS */


USBPD_ParamsTypeDef   DPM_Params[USBPD_PORT_COUNT];

/* Private function prototypes -----------------------------------------------*/
#if !defined(USBPDCORE_LIB_NO_PD)
static void USBPD_PE_TaskWakeUp(uint8_t PortNum);
static void DPM_ManageAttachedState(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc);
#endif /* USBPDCORE_LIB_NO_PD */
#if !defined(USBPDCORE_LIB_NO_PD)||defined(USBPD_TYPE_STATE_MACHINE)
void USBPD_DPM_CADCallback(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc);
#if !defined(USBPD_TCPM_MODULE_ENABLED)
static void USBPD_DPM_CADTaskWakeUp(void);
#endif /* !USBPD_TCPM_MODULE_ENABLED */
#endif /* !USBPDCORE_LIB_NO_PD||USBPD_TYPE_STATE_MACHINE */
#if defined(_TRACE)
void USBPD_DPM_TraceWakeUp(void);
#endif /* _TRACE */

/**
  * @brief  Initialize the core stack (port power role, PWR_IF, CAD and PE Init procedures)
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_DPM_InitCore(void)
{
  /* variable to get dynamique memory allocated by usbpd stack */
  uint32_t stack_dynamemsize;
  USBPD_StatusTypeDef _retr = USBPD_OK;

#if defined(USBPDCORE_LIB_NO_PD)

#if defined(USBPD_TYPE_STATE_MACHINE)
  static const USBPD_CAD_Callbacks CAD_cbs = { USBPD_DPM_CADCallback, USBPD_DPM_CADTaskWakeUp };
#endif /* USBPD_TYPE_STATE_MACHINE */

  /* Check the lib selected */
  if (USBPD_TRUE != USBPD_PE_CheckLIB(_LIB_ID))
  {
    return USBPD_ERROR;
  }

  /* to get how much memory are dynamically allocated by the stack
     the memory return is corresponding to 2 ports so if the application
     managed only one port divide the value return by 2                   */
  stack_dynamemsize = USBPD_PE_GetMemoryConsumption();

  /* done to avoid warning */
  (void)stack_dynamemsize;

#if defined(_TRACE) || defined(_GUI_INTERFACE)
  /* Initialise the TRACE */
  USBPD_TRACE_Init();
#endif /* _TRACE || _GUI_INTERFACE */

  DPM_Params[USBPD_PORT_0].PE_PowerRole     = DPM_Settings[USBPD_PORT_0].PE_DefaultRole;

#if defined(USBPD_TYPE_STATE_MACHINE)
  /* Init CAD */
  CHECK_CAD_FUNCTION_CALL(USBPD_CAD_Init(USBPD_PORT_0, &CAD_cbs, (USBPD_SettingsTypeDef *)&DPM_Settings[USBPD_PORT_0], &DPM_Params[USBPD_PORT_0]));

  /* Enable CAD on Port 0 */
  USBPD_CAD_PortEnable(USBPD_PORT_0, USBPD_CAD_ENABLE);
#else
   /* Init CAD */
  CHECK_CAD_FUNCTION_CALL(USBPD_CAD_Init(USBPD_PORT_0, NULL, (USBPD_SettingsTypeDef *)&DPM_Settings[USBPD_PORT_0], &DPM_Params[USBPD_PORT_0]));
#endif /* USBPD_TYPE_STATE_MACHINE */

#else /* !USBPDCORE_LIB_NO_PD */

  static const USBPD_PE_Callbacks dpmCallbacks =
  {
#if defined(_SRC) || defined(_DRP)
    USBPD_DPM_SetupNewPower,
#else
    NULL,
#endif /*_SRC || DRP */
    USBPD_DPM_HardReset,
    USBPD_DPM_EvaluatePowerRoleSwap,
    USBPD_DPM_Notification,
#ifdef USBPD_REV30_SUPPORT
    USBPD_DPM_ExtendedMessageReceived,
#else
    NULL,
#endif /* USBPD_REV30_SUPPORT */
    USBPD_DPM_GetDataInfo,
    USBPD_DPM_SetDataInfo,
#if defined(_SRC) || defined(_DRP)
    USBPD_DPM_EvaluateRequest,
#else
    NULL,
#endif /*_SRC || DRP */
#if defined(_SNK) || defined(_DRP)
    USBPD_DPM_SNK_EvaluateCapabilities,
#else
    NULL,
#endif /*_SNK || DRP */
#if defined(_DRP)
    USBPD_DPM_PowerRoleSwap,
#else
    NULL,
#endif /*  _DRP  */
    USBPD_PE_TaskWakeUp,
#if defined(_VCONN_SUPPORT)
    USBPD_DPM_EvaluateVconnSwap,
    USBPD_DPM_PE_VconnPwr,
#else
    NULL,
    NULL,
#endif /* _VCONN_SUPPORT */
#ifdef _ERROR_RECOVERY
    USBPD_DPM_EnterErrorRecovery,
#else
    NULL,
#endif /* _ERROR_RECOVERY */
    USBPD_DPM_EvaluateDataRoleSwap,
    USBPD_DPM_IsPowerReady
  };

#if defined(USBPD_TCPM_MODULE_ENABLED)
static const USBPD_CAD_Callbacks CAD_cbs = { USBPD_DPM_CADCallback, NULL };
#else
  static const USBPD_CAD_Callbacks CAD_cbs = { USBPD_DPM_CADCallback, USBPD_DPM_CADTaskWakeUp };
#endif /* USBPD_TCPM_MODULE_ENABLED */

#if !defined(_SIMULATOR)
  /* Check the lib selected */
  if (USBPD_TRUE != USBPD_PE_CheckLIB(_LIB_ID))
  {
    return USBPD_ERROR;
  }
#endif

  /* to get how much memory are dynamically allocated by the stack
     the memory return is corresponding to 2 ports so if the application
     managed only one port divide the value return by 2                   */
  stack_dynamemsize = USBPD_PE_GetMemoryConsumption();

  /* done to avoid warning */
  (void)stack_dynamemsize;

  /* Variable to be sure that DPM is correctly initialized */
  DPM_Params[USBPD_PORT_0].DPM_Initialized = USBPD_FALSE;
#if USBPD_PORT_COUNT == 2
  DPM_Params[USBPD_PORT_1].DPM_Initialized = USBPD_FALSE;
#endif /* USBPD_PORT_COUNT == 2 */

  /* check the stack settings */
  DPM_Params[USBPD_PORT_0].PE_SpecRevision  = DPM_Settings[USBPD_PORT_0].PE_SpecRevision;
  DPM_Params[USBPD_PORT_0].PE_PowerRole     = DPM_Settings[USBPD_PORT_0].PE_DefaultRole;
  DPM_Params[USBPD_PORT_0].PE_SwapOngoing   = USBPD_FALSE;
  DPM_Params[USBPD_PORT_0].ActiveCCIs       = CCNONE;
  DPM_Params[USBPD_PORT_0].VconnCCIs        = CCNONE;
  DPM_Params[USBPD_PORT_0].VconnStatus      = USBPD_FALSE;
#if USBPD_PORT_COUNT == 2
  DPM_Params[USBPD_PORT_1].PE_SpecRevision  = DPM_Settings[USBPD_PORT_1].PE_SpecRevision;
  DPM_Params[USBPD_PORT_1].PE_PowerRole     = DPM_Settings[USBPD_PORT_1].PE_DefaultRole;
  DPM_Params[USBPD_PORT_1].PE_SwapOngoing   = USBPD_FALSE;
  DPM_Params[USBPD_PORT_1].ActiveCCIs       = CCNONE;
  DPM_Params[USBPD_PORT_1].VconnCCIs        = CCNONE;
  DPM_Params[USBPD_PORT_1].VconnStatus      = USBPD_FALSE;
#endif /* USBPD_PORT_COUNT == 2 */

#if defined(_TRACE) || defined(_GUI_INTERFACE)
  /* Initialise the TRACE */
  USBPD_TRACE_Init();
#endif /* _TRACE || _GUI_INTERFACE */

#if defined(USBPD_TCPM_MODULE_ENABLED)
#ifndef _RTOS
  USBPD_TIM_Init();
#endif /* !_RTOS*/

#if !defined(USBPD_STUSB1605)
  USBPD_TCPCI_Init();
#endif /* !USBPD_STUSB1605 */
  TCPC_DrvTypeDef *tcpc_driver;
  USBPD_TCPCI_GetDevicesDrivers(USBPD_PORT_0, &tcpc_driver);
  USBPD_TCPM_HWInit(USBPD_PORT_0, DPM_Settings[USBPD_PORT_0].CAD_RoleToggle, &DPM_Params[USBPD_PORT_0], (USBPD_CAD_Callbacks *)&CAD_cbs, tcpc_driver);
#if USBPD_PORT_COUNT == 2
  USBPD_TCPCI_GetDevicesDrivers(USBPD_PORT_1, &tcpc_driver);
  USBPD_TCPM_HWInit(USBPD_PORT_1, DPM_Settings[USBPD_PORT_1].CAD_RoleToggle, &DPM_Params[USBPD_PORT_1], (USBPD_CAD_Callbacks *)&CAD_cbs, tcpc_driver);
#endif /* USBPD_PORT_COUNT == 2 */
#else
  /* CAD SET UP : Port 0 */
  CHECK_CAD_FUNCTION_CALL(USBPD_CAD_Init(USBPD_PORT_0, (USBPD_CAD_Callbacks *)&CAD_cbs, (USBPD_SettingsTypeDef *)&DPM_Settings[USBPD_PORT_0], &DPM_Params[USBPD_PORT_0]));
#if USBPD_PORT_COUNT == 2
  CHECK_CAD_FUNCTION_CALL(USBPD_CAD_Init(USBPD_PORT_1, (USBPD_CAD_Callbacks *)&CAD_cbs, (USBPD_SettingsTypeDef *)&DPM_Settings[USBPD_PORT_1], &DPM_Params[USBPD_PORT_1]));
#endif /* USBPD_PORT_COUNT == 2 */
#endif /* USBPD_TCPM_MODULE_ENABLED */

  /* PE SET UP : Port 0 */
  CHECK_PE_FUNCTION_CALL(USBPD_PE_Init(USBPD_PORT_0, (USBPD_SettingsTypeDef *)&DPM_Settings[USBPD_PORT_0], &DPM_Params[USBPD_PORT_0], &dpmCallbacks));
#if USBPD_PORT_COUNT == 2
  CHECK_PE_FUNCTION_CALL(USBPD_PE_Init(USBPD_PORT_1, (USBPD_SettingsTypeDef *)&DPM_Settings[USBPD_PORT_1], &DPM_Params[USBPD_PORT_1], &dpmCallbacks));
#endif /* USBPD_PORT_COUNT == 2 */

  /* DPM is correctly initialized */
  DPM_Params[USBPD_PORT_0].DPM_Initialized = USBPD_TRUE;
#if USBPD_PORT_COUNT == 2
  DPM_Params[USBPD_PORT_1].DPM_Initialized = USBPD_TRUE;
#endif /* USBPD_PORT_COUNT == 2 */

#if defined(USBPD_TCPM_MODULE_ENABLED)
#else
  /* Enable CAD on Port 0 */
  USBPD_CAD_PortEnable(USBPD_PORT_0, USBPD_CAD_ENABLE);
#if USBPD_PORT_COUNT == 2
  USBPD_CAD_PortEnable(USBPD_PORT_1, USBPD_CAD_ENABLE);
#endif /* USBPD_PORT_COUNT == 2 */
#endif /* USBPD_TCPM_MODULE_ENABLED */

#endif /* USBPDCORE_LIB_NO_PD */

#if !defined(_RTOS)
#if defined(USE_STM32_UTILITY_OS)
  /* initialise timer server */
  UTIL_TIMER_Init();

  /* initialize the sequencer */
  UTIL_SEQ_Init();
#endif /* USE_STM32_UTILITY_OS */
#endif /* !_RTOS */

#ifdef _LOW_POWER
  USBPD_LOWPOWER_Init();
#endif /* _LOW_POWER */

  return _retr;
}

/**
  * @brief  Initialize the OS parts (task, queue,... )
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_DPM_InitOS(void)
{
#ifdef _RTOS
#if defined(USBPD_TCPM_MODULE_ENABLED)
#if (osCMSIS < 0x20000U)
  osMessageQDef(MsgBox, TCPM_ALARMBOX_MESSAGES_MAX, uint16_t);
  AlarmMsgBox = osMessageCreate(osMessageQ(MsgBox), NULL);
  if (NULL == (ThreadAlert = osThreadCreate(osThread(ALERTTask), &AlarmMsgBox)))
#else
  AlarmMsgBox = osMessageQueueNew (TCPM_ALARMBOX_MESSAGES_MAX, sizeof(uint16_t), NULL);
  if (NULL == osThreadNew(USBPD_ALERT_Task, &AlarmMsgBox, &Alert_Thread_Atrr))
#endif /* osCMSIS < 0x20000U */
  {
    return USBPD_ERROR;
  }
#else
#if !defined(USBPDCORE_LIB_NO_PD)
#if (osCMSIS < 0x20000U)
  CADQueueId = osMessageCreate(osMessageQ(queueCAD), NULL);
  if (osThreadCreate(osThread(CAD), NULL) == NULL)
#else
  CADQueueId = osMessageQueueNew (2, sizeof(uint16_t), NULL);
  if (NULL == osThreadNew(USBPD_CAD_Task, &CADQueueId, &CAD_Thread_Atrr))
#endif /* osCMSIS < 0x20000U */
  {
    return USBPD_ERROR;
  }
#endif /* !USBPDCORE_LIB_NO_PD */
#endif /* USBPD_TCPM_MODULE_ENABLED */

#if defined(_TRACE) || defined(_GUI_INTERFACE)
#if (osCMSIS < 0x20000U)
  TraceQueueId = osMessageCreate(osMessageQ(queueTRACE), NULL);
  if (NULL == osThreadCreate(osThread(TRA_TX), NULL))
#else
  TraceQueueId = osMessageQueueNew (1, sizeof(uint16_t), NULL);
  if (NULL == osThreadNew(USBPD_TRACE_TX_Task, &TraceQueueId, &TRA_Thread_Atrr))
#endif /* osCMSIS < 0x20000U */
  {
    return USBPD_ERROR;
  }
#endif /* _TRACE || _GUI_INTERFACE */

#if !defined(USBPDCORE_LIB_NO_PD)
  /* Create the queue corresponding to PE task */
#if (osCMSIS < 0x20000U)
  PEQueueId[0] = osMessageCreate(osMessageQ(queuePE), NULL);
#if USBPD_PORT_COUNT == 2
  PEQueueId[1] = osMessageCreate(osMessageQ(queuePE), NULL);
#endif /* USBPD_PORT_COUNT == 2 */
#else
  PEQueueId[0] = osMessageQueueNew (1, sizeof(uint16_t), NULL);
#if USBPD_PORT_COUNT == 2
  PEQueueId[1] = osMessageQueueNew (1, sizeof(uint16_t), NULL);
#endif /* USBPD_PORT_COUNT == 2 */
#endif /* osCMSIS < 0x20000U */

  /* PE task to be created on attachment */
  DPM_PEThreadId_Table[USBPD_PORT_0] = NULL;
#if USBPD_PORT_COUNT == 2
  DPM_PEThreadId_Table[USBPD_PORT_1] = NULL;
#endif /* USBPD_PORT_COUNT == 2 */
#endif /* !USBPDCORE_LIB_NO_PD */
#endif /* _RTOS */
#if defined(USBPD_TCPM_MODULE_ENABLED)
  USBPD_TCPI_AlertInit();
#endif /* USBPD_TCPM_MODULE_ENABLED */

  return USBPD_OK;
}

/**
  * @brief  Initialize the OS parts (port power role, PWR_IF, CAD and PE Init procedures)
  * @retval None
  */
#ifdef _RTOS
void USBPD_DPM_Run(void)
{
#if (osCMSIS >= 0x20000U)
  osKernelInitialize();
#endif /* osCMSIS >= 0x20000U */
  osKernelStart();
}
#else /* NRTOS */
#if !defined(USBPDCORE_LIB_NO_PD) || defined(USBPD_TYPE_STATE_MACHINE)
#if defined(USE_STM32_UTILITY_OS)
/**
  * @brief  Task for CAD processing
  * @retval None
  */
void USBPD_CAD_Task(void)
{
  UTIL_TIMER_Stop(&TimerCAD);
  uint32_t _timing = USBPD_CAD_Process();
  UTIL_TIMER_SetPeriod(&TimerCAD, _timing);
  UTIL_TIMER_Start(&TimerCAD);
}

/**
  * @brief  timer function to wakeup CAD Task
  * @param pArg Pointer on an argument
  * @retval None
  */
void TimerCADfunction(void *pArg)
{
  UTIL_SEQ_SetTask(TASK_CAD, 0);
}

#if !defined(USBPDCORE_LIB_NO_PD)
/**
  * @brief  timer function to wakeup PE_0 Task
  * @param pArg Pointer on an argument
  * @retval None
  */
void TimerPE0function(void *pArg)
{
  UTIL_SEQ_SetTask(TASK_PE_0, 0);
}

/**
  * @brief  timer function to wakeup PE_1 Task
  * @param pArg Pointer on an argument
  * @retval None
  */
void TimerPE1function(void *pArg)
{
  UTIL_SEQ_SetTask(TASK_PE_1, 0);
}

/**
  * @brief  Task for PE_0 processing
  * @retval None
  */
void USBPD_PE_Task_P0(void)
{
  UTIL_TIMER_Stop(&TimerPE0);
  uint32_t _timing =
#ifdef _DRP
  USBPD_PE_StateMachine_DRP(USBPD_PORT_0);
#elif _SRC
  USBPD_PE_StateMachine_SRC(USBPD_PORT_0);
#elif _SNK
  USBPD_PE_StateMachine_SNK(USBPD_PORT_0);
#endif /* _DRP */
  if(_timing != 0xFFFFFFFF)
  {
    UTIL_TIMER_SetPeriod(&TimerPE0, _timing);
    UTIL_TIMER_Start(&TimerPE0);
  }
}

/**
  * @brief  Task for PE_1 processing
  * @retval None
  */
void USBPD_PE_Task_P1(void)
{
  UTIL_TIMER_Stop(&TimerPE1);
  uint32_t _timing =
#ifdef _DRP
  USBPD_PE_StateMachine_DRP(USBPD_PORT_1);
#elif _SRC
  USBPD_PE_StateMachine_SRC(USBPD_PORT_1);
#elif _SNK
  USBPD_PE_StateMachine_SNK(USBPD_PORT_1);
#endif /* _DRP */
  if(_timing != 0xFFFFFFFF)
  {
    UTIL_TIMER_SetPeriod(&TimerPE1, _timing);
    UTIL_TIMER_Start(&TimerPE1);
  }
}
#endif /* !USBPDCORE_LIB_NO_PD */

#if defined(_TRACE)
/**
  * @brief  Task for TRACE processing
  * @retval None
  */
void USBPD_TRACE_TX_Task(void)
{
  uint32_t  _timing =  USBPD_TRACE_TX_Process();
  if(_timing != 0xFFFFFFFF)
  {
    UTIL_TIMER_SetPeriod(&TimerTRACE, _timing);
    UTIL_TIMER_Start(&TimerTRACE);
  }
}

/**
  * @brief  timer function to wakeup TRACE Task
  * @param pArg Pointer on an argument
  * @retval None
  */
void TimerTRACEfunction(void *pArg)
{
  UTIL_SEQ_SetTask(TASK_TRACE,  0);
}
#endif /* _TRACE */

/**
  * @brief  Task for DPM_USER processing
  * @retval None
  */
void USBPD_TaskUser(void)
{
  USBPD_DPM_UserExecute(NULL);
}
#endif /* USE_STM32_UTILITY_OS */
#endif /* !USBPDCORE_LIB_NO_PD || USBPD_TYPE_STATE_MACHINE */

void USBPD_DPM_Run(void)
{
#if defined(USE_STM32_UTILITY_OS)
#if !defined(USBPD_TCPM_MODULE_ENABLED)
#if !defined(USBPDCORE_LIB_NO_PD) || defined(USBPD_TYPE_STATE_MACHINE)
  UTIL_SEQ_RegTask(TASK_CAD,  0, USBPD_CAD_Task);
  UTIL_SEQ_SetTask(TASK_CAD,  0);
  UTIL_TIMER_Create(&TimerCAD, 10, UTIL_TIMER_ONESHOT, TimerCADfunction, NULL);
#endif /* !USBPDCORE_LIB_NO_PD || USBPD_TYPE_STATE_MACHINE */
#endif /* !USBPD_TCPM_MODULE_ENABLED */

#if !defined(USBPDCORE_LIB_NO_PD)
  UTIL_SEQ_RegTask(TASK_PE_0, 0,  USBPD_PE_Task_P0);
  UTIL_SEQ_PauseTask(TASK_PE_0);
  UTIL_TIMER_Create(&TimerPE0, 10, UTIL_TIMER_ONESHOT, TimerPE0function, NULL);
#if USBPD_PORT_COUNT == 2
  UTIL_SEQ_RegTask(TASK_PE_1, 0,  USBPD_PE_Task_P1);
  UTIL_SEQ_PauseTask(TASK_PE_1);
  UTIL_TIMER_Create(&TimerPE1, 10, UTIL_TIMER_ONESHOT, TimerPE1function, NULL);
#endif /* USBPD_PORT_COUNT == 2 */
#endif /* !USBPDCORE_LIB_NO_PD */

#if defined(_TRACE) || defined(_GUI_INTERFACE)
  UTIL_SEQ_RegTask(TASK_TRACE, 0, USBPD_TRACE_TX_Task);
  UTIL_SEQ_SetTask(TASK_TRACE,  0);
  UTIL_TIMER_Create(&TimerTRACE, 350, UTIL_TIMER_PERIODIC, TimerTRACEfunction, NULL);
  UTIL_TIMER_Start(&TimerTRACE);
#endif /* _TRACE || _GUI_INTERFACE */

  UTIL_SEQ_RegTask(TASK_USER, 0 , USBPD_TaskUser);
  UTIL_SEQ_SetTask(TASK_USER,  0);

  do
  {
    UTIL_SEQ_Run(~0);
  }
  while (1u == 1u);
#else /* !USE_STM32_UTILITY_OS */
  do
  {
#if !defined(USBPD_TCPM_MODULE_ENABLED)
#if !defined(USBPDCORE_LIB_NO_PD) || defined(USBPD_TYPE_STATE_MACHINE)
    if ((HAL_GetTick() - DPM_Sleep_start[USBPD_PORT_COUNT]) >= DPM_Sleep_time[USBPD_PORT_COUNT])
    {
      DPM_Sleep_time[USBPD_PORT_COUNT] = USBPD_CAD_Process();
      DPM_Sleep_start[USBPD_PORT_COUNT] = HAL_GetTick();
    }
#endif /* !USBPDCORE_LIB_NO_PD || USBPD_TYPE_STATE_MACHINE */
#endif /* USBPD_TCPM_MODULE_ENABLED */

#if !defined(USBPDCORE_LIB_NO_PD)
    uint32_t port = 0;

    for (port = 0; port < USBPD_PORT_COUNT; port++)
    {
      if ((HAL_GetTick() - DPM_Sleep_start[port]) >= DPM_Sleep_time[port])
      {
        DPM_Sleep_time[port] =
#ifdef _DRP
          USBPD_PE_StateMachine_DRP(port);
#elif _SRC
          USBPD_PE_StateMachine_SRC(port);
#elif _SNK
          USBPD_PE_StateMachine_SNK(port);
#endif /* _DRP */
        DPM_Sleep_start[port] = HAL_GetTick();
      }
    }
#endif /* USBPDCORE_LIB_NO_PD */

#if !defined(USBPDCORE_LIB_NO_PD) || defined(USBPD_TYPE_STATE_MACHINE)
    USBPD_DPM_UserExecute(NULL);
#endif /* !USBPDCORE_LIB_NO_PD || USBPD_TYPE_STATE_MACHINE */

#if defined(_TRACE) || defined(_GUI_INTERFACE)
    if ((HAL_GetTick() - DPM_Sleep_start[USBPD_PORT_COUNT + OFFSET_CAD]) >= DPM_Sleep_time[USBPD_PORT_COUNT + OFFSET_CAD])
    {
      DPM_Sleep_time[USBPD_PORT_COUNT + OFFSET_CAD] = USBPD_TRACE_TX_Process();
      DPM_Sleep_start[USBPD_PORT_COUNT + OFFSET_CAD] = HAL_GetTick();
    }
#endif /* _TRACE || _GUI_INTERFACE */

#if defined(_SIMULATOR)
    return;
#endif
  }
  while (1u == 1u);
#endif /* USE_STM32_UTILITY_OS */
}

#endif /* _RTOS */


#if !defined(USBPDCORE_LIB_NO_PD)
/**
  * @brief  Initialize DPM (port power role, PWR_IF, CAD and PE Init procedures)
  * @retval USBPD status
  */
void USBPD_DPM_TimerCounter(void)
{
  /* Call PE/PRL timers functions only if DPM is initialized */
  if (USBPD_TRUE == DPM_Params[USBPD_PORT_0].DPM_Initialized)
  {
    USBPD_DPM_UserTimerCounter(USBPD_PORT_0);
    USBPD_PE_TimerCounter(USBPD_PORT_0);
    USBPD_PRL_TimerCounter(USBPD_PORT_0);
#if defined(_FWUPDATE_RESPONDER)
    USBPD_PDFU_TimerCounter(USBPD_PORT_0);
#endif /* _FWUPDATE_RESPONDER */
  }
#if USBPD_PORT_COUNT==2
  if (USBPD_TRUE == DPM_Params[USBPD_PORT_1].DPM_Initialized)
  {
    USBPD_DPM_UserTimerCounter(USBPD_PORT_1);
    USBPD_PE_TimerCounter(USBPD_PORT_1);
    USBPD_PRL_TimerCounter(USBPD_PORT_1);
#if defined(_FWUPDATE_RESPONDER)
    USBPD_PDFU_TimerCounter(USBPD_PORT_1);
#endif /* _FWUPDATE_RESPONDER */
  }
#endif /* USBPD_PORT_COUNT == 2 */

#ifdef _RTOS
#if (osCMSIS >= 0x20000U)
  /* SysTick Handler now fully handled on CMSIS OS V2 side */
#else
  /* check to avoid count before OSKernel Start */
  if (uxTaskGetNumberOfTasks() != 0)
  {
    osSystickHandler();
  }
#endif /* osCMSIS >= 0x20000U */
#endif /* _RTOS */
}

/**
  * @brief  WakeUp PE task
  * @param  PortNum port number
  * @retval None
  */
static void USBPD_PE_TaskWakeUp(uint8_t PortNum)
{
#ifdef _RTOS
#if (osCMSIS < 0x20000U)
  (void)osMessagePut(PEQueueId[PortNum], 0xFFFF, 0);
#else
  uint32_t event = 0xFFFFU;
  (void)osMessageQueuePut(PEQueueId[PortNum], &event, 0U, 0U);
#endif /* osCMSIS < 0x20000U */
#else
#if defined(USE_STM32_UTILITY_OS)
  UTIL_SEQ_SetTask(PortNum == 0? TASK_PE_0: TASK_PE_1, 0);
#else
  DPM_Sleep_time[PortNum] = 0;
#endif /* USE_STM32_UTILITY_OS */
#endif /* _RTOS */
}
#endif /* USBPDCORE_LIB_NO_PD */

#if !defined(USBPDCORE_LIB_NO_PD) || defined(USBPD_TYPE_STATE_MACHINE)
#if !defined(USBPD_TCPM_MODULE_ENABLED)
/**
  * @brief  WakeUp CAD task
  * @retval None
  */
static void USBPD_DPM_CADTaskWakeUp(void)
{
#ifdef _RTOS
#if (osCMSIS < 0x20000U)
  (void)osMessagePut(CADQueueId, 0xFFFF, 0);
#else
  uint32_t event = 0xFFFFU;
  (void)osMessageQueuePut(CADQueueId, &event, 0U, 0U);
#endif /* osCMSIS < 0x20000U */
#else
#if defined(USE_STM32_UTILITY_OS)
  UTIL_SEQ_SetTask(TASK_CAD, 0);
#else
  DPM_Sleep_time[USBPD_PORT_COUNT] = 0;
#endif /* USE_STM32_UTILITY_OS */
#endif /* _RTOS */
}
#endif /* !USBPD_TCPM_MODULE_ENABLED */
#endif /* !USBPDCORE_LIB_NO_PD||USBPD_TYPE_STATE_MACHINE */

#ifdef _RTOS
#if !defined(USBPDCORE_LIB_NO_PD)
#if (osCMSIS < 0x20000U)
/**
  * @brief  Main task for PE layer
  * @param  argument Not used
  * @retval None
  */
void USBPD_PE_Task(void const *argument)
{
  uint8_t _port = (uint32_t)argument;
  uint32_t _timing;

#ifdef _LOW_POWER
  LPM_SetOffMode((LPM_Id_t)(LPM_PE_0 + _port), LPM_Disable);
#endif

  for (;;)
  {
#if defined(_DRP) || (defined(_SRC) && defined(_SNK))
#if defined(USBPDCORE_VPD)
    if((USBPD_FALSE == DPM_Params[_port].PE_PowerRole)
       && (USBPD_FALSE == DPM_Params[_port].PE_PowerRole)
         && (USBPD_TRUE == DPM_Settings[_port].VPDSupport))
    {
      _timing = USBPD_PE_StateMachine_SNKwVPD(_port);
    }
    else
#endif
    {
      _timing = USBPD_PE_StateMachine_DRP(_port);
    }
#elif defined(_SRC)
    _timing = USBPD_PE_StateMachine_SRC(_port);
#elif defined(_SNK)
#if defined(USBPDCORE_VPD)
    if (USBPD_TRUE == DPM_Settings[_port].VPDSupport)
    {
      _timing = USBPD_PE_StateMachine_SNKwVPD(_port);
    }
    else
#endif
    {
      _timing = USBPD_PE_StateMachine_SNK(_port);
    }
#endif
    osMessageGet(PEQueueId[_port],_timing);
#if defined(USBPD_TCPM_MODULE_ENABLED)
    /* During SRC tests, VBUS is disabled by the FUSB but the detection is not
       well done */
    if ((DPM_Params[_port].PE_SwapOngoing == 0) && (USBPD_ERROR == USBPD_TCPM_VBUS_IsVsafe5V(_port)))
    {
      (void)osMessagePut(AlarmMsgBox, (_port << 8 | 2), osWaitForever);
    }
#endif /* USBPD_TCPM_MODULE_ENABLED */
  }
}

#else /* osCMSIS > 0x20000U */

/**
  * @brief  Main task for PE layer on Port0
  * @param  argument Not used
  * @retval None
  */
void USBPD_PE_Task_P0(void *argument)
{
  PE_Task(USBPD_PORT_0);
}

/**
  * @brief  Main task for PE layer on Port1
  * @param  argument Not used
  * @retval None
  */
void USBPD_PE_Task_P1(void *argument)
{
  PE_Task(USBPD_PORT_1);
}

/**
  * @brief  Main task for PE layer
  * @param  argument Not used
  * @retval None
  */
static void PE_Task(uint32_t PortNum)
{
#ifdef _LOW_POWER
  LPM_SetOffMode((LPM_Id_t)(LPM_PE_0 + PortNum), LPM_Disable);
#endif

  for (;;)
  {
    uint32_t event;
    (void)osMessageQueueGet(PEQueueId[PortNum], &event, NULL,
#if defined(_DRP)
    USBPD_PE_StateMachine_DRP(PortNum));
#elif defined(_SRC)
    USBPD_PE_StateMachine_SRC(PortNum));
#elif defined(_SNK)
    USBPD_PE_StateMachine_SNK(PortNum));
#endif
#if defined(USBPD_TCPM_MODULE_ENABLED)
    /* During SRC tests, VBUS is disabled by the FUSB but the detection is not
       well done */
    if ((DPM_Params[PortNum].PE_SwapOngoing == 0) && (USBPD_ERROR == USBPD_TCPM_VBUS_IsVsafe5V(PortNum)))
    {
      uint32_t event = (_port << 8 | 2);
      (void)osMessageQueuePut(AlarmMsgBox, &event, 0U, osWaitForever);
    }
#endif /* USBPD_TCPM_MODULE_ENABLED */
  }
}
#endif /* osCMSIS < 0x20000U */
#endif /* !USBPDCORE_LIB_NO_PD */

#if defined(USBPD_TCPM_MODULE_ENABLED)
/**
  * @brief  Main task for ALERT layer
  * @param  argument: Not used
  * @retval None
  */
#if (osCMSIS < 0x20000U)
void USBPD_ALERT_Task(void const *queue_id)
#else
void USBPD_ALERT_Task(void *queue_id)
#endif /* osCMSIS < 0x20000U */
{
  osMessageQId  queue = *(osMessageQId *)queue_id;
  uint8_t port;
  for (;;)
  {
#if (osCMSIS < 0x20000U)
    osEvent event = osMessageGet(queue, osWaitForever);
    port = (event.value.v >> 8);
#else
    (void)osMessageQueueGet(queue, &event, NULL, osWaitForever);
    port = (event >> 8);
#endif /* osCMSIS < 0x20000U */
#if defined(_TRACE)
    USBPD_TRACE_Add(USBPD_TRACE_DEBUG, port, ((event.value.v) & 0x00FF), (uint8_t*)"ALERT_TASK", sizeof("ALERT_TASK") - 1);
#endif /* _TRACE */
#if (osCMSIS < 0x20000U)
    USBPD_TCPM_alert(event.value.v);
#else
    USBPD_TCPM_alert(event);
#endif /* osCMSIS < 0x20000U */
    HAL_NVIC_EnableIRQ(ALERT_GPIO_IRQHANDLER(port));
  }
}
#else /* !USBPD_TCPM_MODULE_ENABLED */
#if !defined(USBPDCORE_LIB_NO_PD)||defined(USBPD_TYPE_STATE_MACHINE)
/**
  * @brief  Main task for CAD layer
  * @param  argument Not used
  * @retval None
  */
#if (osCMSIS < 0x20000U)
void USBPD_CAD_Task(void const *argument)
#else
void USBPD_CAD_Task(void *argument)
#endif /* osCMSIS < 0x20000U */
{
#ifdef _LOW_POWER
  LPM_SetOffMode(LPM_CAD,LPM_Disable);
#endif
  for (;;)
  {
#if (osCMSIS < 0x20000U)
    osMessageGet(CADQueueId, USBPD_CAD_Process());
#else
    uint32_t event;
    (void)osMessageQueueGet(CADQueueId, &event, NULL, USBPD_CAD_Process());
#endif /* osCMSIS < 0x20000U */
  }
}
#endif /* !USBPDCORE_LIB_NO_PD || USBPD_TYPE_STATE_MACHINE */
#endif /* USBPD_TCPM_MODULE_ENABLED */

#if defined(_TRACE) || defined(_GUI_INTERFACE)
/**
  * @brief  Main task for TRACE TX layer
  * @param  argument Not used
  * @retval None
  */
#if (osCMSIS < 0x20000U)
void USBPD_TRACE_TX_Task(void const *argument)
#else
void USBPD_TRACE_TX_Task(void *argument)
#endif /* osCMSIS < 0x20000U */
{
  for (;;)
  {
#if (osCMSIS < 0x20000U)
    osMessageGet(TraceQueueId,USBPD_TRACE_TX_Process());
#else
    uint32_t event;
    (void)osMessageQueueGet(TraceQueueId, &event, NULL, USBPD_TRACE_TX_Process());
#endif /* osCMSIS < 0x20000U */
  }
}
#endif /* _TRACE || _GUI_INTERFACE */
#endif /* _RTOS */

#if defined(_TRACE)
/**
  * @brief  WakeUp TRACE task
  * @retval None
  */
void USBPD_DPM_TraceWakeUp(void)
{
#ifdef _RTOS
  if (NULL != TraceQueueId)
  {
#if (osCMSIS < 0x20000U)
    (void)osMessagePut(TraceQueueId, 0xFFFF, 0);
#else
    uint32_t event = 0xFFFFU;
    (void)osMessageQueuePut(TraceQueueId, &event, 0U, 0U);
#endif /* osCMSIS < 0x20000U */
  }
#else
#if defined(USE_STM32_UTILITY_OS)
  /* Wake up the non RTOS application to evacuate the trace */
  UTIL_SEQ_SetTask(TASK_TRACE, 0);
#else
  DPM_Sleep_time[USBPD_PORT_COUNT + OFFSET_CAD] = 0;
#endif /* USE_STM32_UTILITY_OS */
#endif /* _RTOS */
}
#endif /* _TRACE */

/**
  * @brief  CallBack reporting events on a specified port from CAD layer.
  * @param  PortNum   The handle of the port
  * @param  State     CAD state
  * @param  Cc        The Communication Channel for the USBPD communication
  * @retval None
  */
#if defined(USBPDCORE_LIB_NO_PD)
#if defined(USBPD_TYPE_STATE_MACHINE)
void USBPD_DPM_CADCallback(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc)
{
#ifdef _TRACE
  USBPD_TRACE_Add(USBPD_TRACE_CADEVENT, PortNum, (uint8_t)State, NULL, 0);
#endif /* _TRACE */

  switch (State)
  {
#if defined(USBPDCORE_VPD)
  case USPPD_CAD_EVENT_VPD :
#endif
  case USBPD_CAD_EVENT_ATTEMC :
  case USBPD_CAD_EVENT_ATTACHED :
    {
      DPM_Params[PortNum].ActiveCCIs = Cc;
      USBPD_DPM_UserCableDetection(PortNum, State);
      break;
    }
  case USBPD_CAD_EVENT_DETACHED :
  case USBPD_CAD_EVENT_EMC :
    {
      USBPD_DPM_UserCableDetection(PortNum, State);
      DPM_Params[PortNum].ActiveCCIs = CCNONE;
      break;
    }
  default :
    /* nothing to do */
    break;
  }
}
#endif /* USBPD_TYPE_STATE_MACHINE */
#else
void USBPD_DPM_CADCallback(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc)
{
#ifdef _TRACE
  USBPD_TRACE_Add(USBPD_TRACE_CADEVENT, PortNum, (uint8_t)State, NULL, 0);
#endif /* _TRACE */

  switch (State)
  {
#if defined(USBPDCORE_VPD)
    case USPPD_CAD_EVENT_VPD    :
#endif
    case USBPD_CAD_EVENT_ATTEMC :
    {
#ifdef _VCONN_SUPPORT
      DPM_Params[PortNum].VconnStatus = USBPD_TRUE;
#endif /* _VCONN_SUPPORT */
      DPM_ManageAttachedState(PortNum, State, Cc);
#ifdef _VCONN_SUPPORT
      DPM_CORE_DEBUG_TRACE(PortNum, "Note: VconnStatus=TRUE");
#endif /* _VCONN_SUPPORT */
      break;
    }

    case USBPD_CAD_EVENT_ATTACHED :
      DPM_ManageAttachedState(PortNum, State, Cc);
      break;
    case USBPD_CAD_EVENT_DETACHED :
    case USBPD_CAD_EVENT_EMC :
    {
      /* The ufp is detached */
      (void)USBPD_PE_IsCableConnected(PortNum, 0);
      /* Terminate PE task */
#ifdef _RTOS
      if (DPM_PEThreadId_Table[PortNum] != NULL)
      {
#ifdef _LOW_POWER
        LPM_SetStopMode((LPM_Id_t)(LPM_PE_0 + PortNum), LPM_Enable);
        LPM_SetOffMode((LPM_Id_t)(LPM_PE_0 + PortNum), LPM_Enable);
#endif
        osThreadTerminate(DPM_PEThreadId_Table[PortNum]);
        DPM_PEThreadId_Table[PortNum] = NULL;
      }
#else
#if defined(USE_STM32_UTILITY_OS)
      UTIL_SEQ_PauseTask(PortNum == 0? TASK_PE_0: TASK_PE_1);
#else
      DPM_Sleep_time[PortNum] = 0xFFFFFFFFU;
#endif /* USE_STM32_UTILITY_OS */
#endif /* _RTOS */
      DPM_Params[PortNum].PE_SwapOngoing = USBPD_FALSE;
      DPM_Params[PortNum].ActiveCCIs = CCNONE;
      DPM_Params[PortNum].PE_Power   = USBPD_POWER_NO;
#ifdef _VCONN_SUPPORT
      DPM_Params[PortNum].VconnCCIs = CCNONE;
      DPM_Params[PortNum].VconnStatus = USBPD_FALSE;
      DPM_CORE_DEBUG_TRACE(PortNum, "Note: VconnStatus=FALSE");
#endif /* _VCONN_SUPPORT */
      USBPD_DPM_UserCableDetection(PortNum, State);
      break;
    }
    default :
      /* nothing to do */
      break;
  }
}

static void DPM_ManageAttachedState(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc)
{
#ifdef _VCONN_SUPPORT
  if (CC1 == Cc)
  {
    DPM_Params[PortNum].VconnCCIs = CC2;
  }
  if (CC2 == Cc)
  {
    DPM_Params[PortNum].VconnCCIs = CC1;
  }
#endif /* _VCONN_SUPPORT */
  DPM_Params[PortNum].ActiveCCIs = Cc;
  (void)USBPD_PE_IsCableConnected(PortNum, 1);

  USBPD_DPM_UserCableDetection(PortNum, State);
#if defined(USBPD_TCPM_MODULE_ENABLED)
  /* Add a delay to postpone the 1st send of SRC capa
  FUS305 seems not react correctly if it sent too quickly */
  USBPD_DPM_WaitForTime(6);
#endif /* USBPD_TCPM_MODULE_ENABLED */

#ifdef _RTOS
  /* Create PE task */
  if (DPM_PEThreadId_Table[PortNum] == NULL)
  {
#if (osCMSIS < 0x20000U)
    DPM_PEThreadId_Table[PortNum] = osThreadCreate(OSTHREAD_PE(PortNum), (void *)((uint32_t)PortNum));
#else
    DPM_PEThreadId_Table[PortNum] = osThreadNew(OSTHREAD_PE(PortNum), NULL, OSTHREAD_PE_ATTR(PortNum));
#endif /* osCMSIS < 0x20000U */
    if (DPM_PEThreadId_Table[PortNum] == NULL)
    {
      /* should not occurr. May be an issue with FreeRTOS heap size too small */
      while (1);
    }
  }
#else
#if defined(USE_STM32_UTILITY_OS)
   /* Resume the task */
   UTIL_SEQ_ResumeTask(PortNum == 0? TASK_PE_0: TASK_PE_1);
   /* Enable task execution */
   UTIL_SEQ_SetTask(PortNum == 0? TASK_PE_0: TASK_PE_1, 0);
#else
   DPM_Sleep_time[PortNum] = 0U;
#endif /* USE_STM32_UTILITY_OS */
#endif /* _RTOS */
}
#endif /* USBPDCORE_LIB_NO_PD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
