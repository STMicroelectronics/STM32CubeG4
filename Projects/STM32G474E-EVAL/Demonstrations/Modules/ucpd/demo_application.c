/**
  ******************************************************************************
  * @file    demo_application.c
  * @author  MCD Application Team
  * @brief   UCPD demo application code
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

/* Includes ------------------------------------------------------------------*/
#include "usbpd_core.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_conf.h"
#include "usbpd_dpm_user.h"
#include "usbpd_devices_conf.h"
#include "usbpd_pwr_if.h"
#include "logo_STM32_G4.h"
#include "demo_application.h"
#include "string.h"
#include "stdio.h"
#if defined(_RTOS)
#include "cmsis_os.h"
#endif
#if defined(_TRACE)
#include "usbpd_trace.h"
#endif
#include "stm32_lcd.h"
#include "stm32g474e_eval.h"
#include "stm32g474e_eval_lcd.h"
#include "stm32g474e_eval_io.h"
#include "stm32g474e_eval_usbpd_pwr.h"

#define LCD_INSTANCE  0

/** @addtogroup STM32_USBPD_APPLICATION
  * @{
  */

/** @addtogroup STM32_USBPD_APPLICATION_DEMO
  * @{
  */

/* Exported variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LCD_ALARMBOX_MESSAGES_MAX         30u

#define DEMO_MAIN_LCD                     LCD_INSTANCE

/* Format of the different kind of message */
/*   - 31-28  4 bit for the message type   */
#define DEMO_MSG_TYPE_POS                 28u
#define DEMO_MSG_TYPE_MSK                 (0xFu << DEMO_MSG_TYPE_POS)
#define DEMO_MSG_MMI                      (0u   << DEMO_MSG_TYPE_POS)
#define DEMO_MSG_CAD                      (1u   << DEMO_MSG_TYPE_POS)
#define DEMO_MSG_PENOTIFY                 (2u   << DEMO_MSG_TYPE_POS)

/*  MMI
     - 15-0 id event
#define DEMO_MMI_ACTION_Pos                   (0U)
#define DEMO_MMI_ACTION_Msk                   (0xFF << DEMO_MMI_ACTION_Pos)
#define DEMO_MMI_ACTION                       DPM_USER_ACTION_Msk
#define DEMO_MMI_ACTION_NONE                  (0u)
#define DEMO_MMI_ACTION_TAMPER_PRESS          (1u << DEMO_MMI_ACTION_Pos)
#define DEMO_MMI_ACTION_RIGHT_PRESS           (2u << DEMO_MMI_ACTION_Pos)
#define DEMO_MMI_ACTION_LEFT_PRESS            (3u << DEMO_MMI_ACTION_Pos)
#define DEMO_MMI_ACTION_UP_PRESS              (4u << DEMO_MMI_ACTION_Pos)
#define DEMO_MMI_ACTION_DOWN_PRESS            (5u << DEMO_MMI_ACTION_Pos)
#define DEMO_MMI_ACTION_SEL_PRESS             (6u << DEMO_MMI_ACTION_Pos)
#define DEMO_MMI_ACTION_DISPLAY_VBUS_IBUS     (7u << DEMO_MMI_ACTION_Pos)
#define DEMO_MMI_ACTION_JOY_PRESS             (8u << DEMO_MMI_ACTION_Pos)
*/
/*  CAD
     - 27-26 2 bit for CC Pin
     - 25-24 2 bit port num
     - 15-00 16 bit CAD event
*/
#define DEMO_CAD_CC_NUM_Pos               (26u)
#define DEMO_CAD_CC_NUM_Msk               (0x3u << DEMO_CAD_CC_NUM_Pos)
#define DEMO_CAD_CC_NUM_NONE              (0u   << DEMO_CAD_CC_NUM_Pos)
#define DEMO_CAD_CC_NUM_CC1               (1u   << DEMO_CAD_CC_NUM_Pos)
#define DEMO_CAD_CC_NUM_CC2               (2u   << DEMO_CAD_CC_NUM_Pos)

#define DEMO_CAD_PORT_NUM_Pos             (24u)
#define DEMO_CAD_PORT_NUM_Msk             (0x3u << DEMO_CAD_PORT_NUM_Pos)
#define DEMO_CAD_PORT_NUM_0               (0x0u << DEMO_CAD_PORT_NUM_Pos)
#define DEMO_CAD_PORT_NUM_1               (0x1u << DEMO_CAD_PORT_NUM_Pos)

#define DEMO_CAD_STATE_Pos                (0u)
#define DEMO_CAD_STATE_Msk                (0xFFu << DEMO_CAD_STATE_Pos)

/*  PE
     - 27-26 2 bit none
     - 25-24 2 bit port num
     - 15-00 16 bit PE event
*/
#define DEMO_PE_PORT_NUM_Pos             (24u)
#define DEMO_PE_PORT_NUM_Msk             (0x3u << DEMO_PE_PORT_NUM_Pos)
#define DEMO_PE_PORT_NUM_0               (0x0u << DEMO_PE_PORT_NUM_Pos)
#define DEMO_PE_PORT_NUM_1               (0x1u << DEMO_PE_PORT_NUM_Pos)

#define DEMO_PE_STATE_Pos                (0u)
#define DEMO_PE_STATE_Msk                (0xFFu << DEMO_PE_STATE_Pos)

#define MAX_LINE_PDO      4u
#define MAX_LINE_COMMAND  4u
#define MAX_LINE_EXTCAPA  5u

/* DEMO Error messages max size
*/
#define DEMO_ERROR_MAX_MSG_SIZE  25U


typedef enum {
  MENU_EMPTY,
  MENU_COMMAND,
  MENU_SOURCECAPA_RECEIVED,
  MENU_SINKCAPA_RECEIVED,
  MENU_EXTCAPA_RECEIVED,
  MENU_SNKEXTCAPA_RECEIVED,
  MENU_BUILD,
  MENU_CABLE,
  MENU_MAX
} DEMO_MENU;

typedef enum {
  COMMAND_NONE,
  COMMAND_HARDRESET,
  COMMAND_CONTROLMSG_GOTOMIN,
  COMMAND_CONTROLMSG_GET_SRC_CAP,
  COMMAND_CONTROLMSG_GET_SNK_CAP,
  COMMAND_CONTROLMSG_DR_SWAP,
  COMMAND_CONTROLMSG_PR_SWAP,
  COMMAND_CONTROLMSG_VCONN_SWAP,
  COMMAND_CONTROLMSG_SOFT_RESET,
#if defined(_VDM)
  COMMAND_REQUEST_VDM_DISCOVERY,
#endif
#if defined(USBPD_REV30_SUPPORT)
  COMMAND_CONTROLMSG_GET_SRC_CAPEXT,
  COMMAND_CONTROLMSG_GET_STATUS,
  COMMAND_CONTROLMSG_GET_SNK_CAPEXT,
#endif /* USBPD_REV30_SUPPORT */

} DEMO_COMMAND;

enum {
  COMMANDROLE_SRC   = 1,
  COMMANDROLE_SNK   = 2,
  COMMANDROLE_DRP   = 4,
  COMMANDROLE_VCONN = 8
};

typedef struct
{
  uint8_t commandid;
  uint8_t commandstr[23];
  uint8_t commandconfig;
} typedef_COMMANDE;

/* Private macro -------------------------------------------------------------*/
#define DEMO_MAX_COMMAND  13u

/* Private variables ---------------------------------------------------------*/
const typedef_COMMANDE g_tab_command_global[DEMO_MAX_COMMAND] = {
  { COMMAND_HARDRESET                   ,"HARD RESET            ", COMMANDROLE_SRC | COMMANDROLE_SNK  },
  { COMMAND_CONTROLMSG_GOTOMIN          ,"GOTO MIN              ", COMMANDROLE_SRC },
  { COMMAND_CONTROLMSG_GET_SRC_CAP      ,"GET SOURCE CAPA       ", COMMANDROLE_SRC | COMMANDROLE_DRP  },
  { COMMAND_CONTROLMSG_GET_SNK_CAP      ,"GET SINK   CAPA       ", COMMANDROLE_SRC },
  { COMMAND_CONTROLMSG_DR_SWAP          ,"DATA ROLE SWAP        ", COMMANDROLE_SRC | COMMANDROLE_SNK  },
  { COMMAND_CONTROLMSG_PR_SWAP          ,"POWER ROLE SWAP       ", COMMANDROLE_DRP },
  { COMMAND_CONTROLMSG_VCONN_SWAP       ,"VCONN SWAP            ", COMMANDROLE_VCONN },
  { COMMAND_CONTROLMSG_SOFT_RESET       ,"SOFT RESET            ", COMMANDROLE_SRC | COMMANDROLE_SNK },
#if defined(_VDM)
  { COMMAND_REQUEST_VDM_DISCOVERY       ,"REQUEST VDM DISCOVERY " ,COMMANDROLE_SRC | COMMANDROLE_SNK },
#endif
#if defined(USBPD_REV30_SUPPORT)
  { COMMAND_CONTROLMSG_GET_SRC_CAPEXT   ,"GET SOURCE EXT CAPA   ", COMMANDROLE_SNK },
  { COMMAND_CONTROLMSG_GET_STATUS       ,"GET_STATUS            ", 0               },
  { COMMAND_CONTROLMSG_GET_SNK_CAPEXT   ,"GET SINK EXT CAPA     ", COMMANDROLE_SRC },
#endif /* USBPD_REV30_SUPPORT */
  { COMMAND_NONE                        ,""                      , 0}
};

const DEMO_MENU g_tab_menu_next[2][MENU_MAX] = {
  { MENU_EMPTY,  MENU_SOURCECAPA_RECEIVED, MENU_SINKCAPA_RECEIVED, MENU_EXTCAPA_RECEIVED, MENU_SNKEXTCAPA_RECEIVED, MENU_CABLE, MENU_COMMAND, MENU_BUILD },
  { MENU_EMPTY,  MENU_SOURCECAPA_RECEIVED, MENU_EXTCAPA_RECEIVED,  MENU_EMPTY,            MENU_BUILD,               MENU_EMPTY, MENU_COMMAND, MENU_EMPTY }
};

const DEMO_MENU g_tab_menu_prev[2][MENU_MAX] = {
  { MENU_EMPTY, MENU_CABLE, MENU_COMMAND, MENU_SOURCECAPA_RECEIVED, MENU_SINKCAPA_RECEIVED,   MENU_EXTCAPA_RECEIVED,  MENU_COMMAND,           MENU_SNKEXTCAPA_RECEIVED },
  { MENU_EMPTY, MENU_BUILD, MENU_COMMAND, MENU_EMPTY,               MENU_SOURCECAPA_RECEIVED, MENU_EMPTY,             MENU_EXTCAPA_RECEIVED,  MENU_EMPTY }
};

static typedef_COMMANDE g_tab_command_PORT[2][DEMO_MAX_COMMAND];

/*  contains the id of the selected item inside a menu*/
static uint8_t   g_tab_menu_sel[2] = { 0, 0};
/*  contains the menu selected position  */
static  int8_t   g_tab_menu_pos[2] = { 0, 0};

/* avoid multiple joystick event */
static __IO uint8_t joyevent = 0;

const uint8_t g_tab_error_strings[DEMO_ERROR_TYPE_MAXNBITEMS][DEMO_ERROR_MAX_MSG_SIZE] = {
  "  POWER SUPPLY ERROR  ",    /* Power Supply error */
  " FLASH SETTINGS ERROR ",    /* Error in settings stored in flash */
};

/* Private variables ---------------------------------------------------------*/

/* Counter for Sel Joystick pressed*/
#if defined(_RTOS)
osMessageQId  LCDMsgBox;
#endif
static int8_t portSel =0;
USBPD_DiscoveryIdentity_TypeDef pIdentity;

/* Private function prototypes -----------------------------------------------*/
static void Display_Selected_port(void);
static void Display_border(void);
static void Display_clear_info(void);
static void Display_debug_port(uint8_t PortNum, uint8_t *msg);

static void Display_sinkcapa_menu(uint8_t PortNum);
static void Display_sinkcapa_menu_nav(uint8_t PortNum, int8_t Or);

static void Display_sourcecapa_menu_nav(uint8_t PortNum, int8_t Or);
static uint8_t  Display_sourcecapa_menu_exec(uint8_t PortNum);

static void Display_extcapa_menu_nav(uint8_t PortNum, int8_t Or);
static void Display_sinkextcapa_menu_nav(uint8_t PortNum, int8_t Orientation);
static void Display_command_menu(uint8_t PortNum);
static void Display_command_menu_nav(uint8_t PortNum, int8_t Or);
static void Display_command_menu_exec(uint8_t PortNum);

static void Display_cableinfo_menu(uint8_t PortNum);

void DEMO_Task(void const *argument);
static void string_completion(uint8_t *Str, uint8_t SizeMax);

/**
  * @brief  Demo BSP initialisation
  * @retval DEMO_ErrorCode status
  */
DEMO_ErrorCode DEMO_InitBSP(void)
{
  UTIL_LCD_SetFont(&Font12);

  /* Clear the LCD */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  BSP_LCD_DrawBitmap(DEMO_MAIN_LCD, 0, 0, (uint8_t *)header_data_logo);
  Display_Selected_port();
  Display_border();

  /* Initialize Joystick */
  BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL);

  /* Initialize Push Button  */
  BSP_PB_Init(BUTTON_USER , BUTTON_MODE_EXTI);

  return DEMO_OK;
}

/**
  * @brief  Demo Task initialisation
  * @retval DEMO_ErrorCode status
  */
DEMO_ErrorCode DEMO_InitTask()
{
#if defined(_RTOS)
  osMessageQDef(MsgBox, LCD_ALARMBOX_MESSAGES_MAX, uint32_t);

#if defined(_GUI_INTERFACE)
  osThreadDef(LCD, DEMO_Task, osPriorityLow, 0, 400);
#else
  osThreadDef(LCD, DEMO_Task, osPriorityLow, 0, 280);
#endif /*_GUI_INTERFACE*/

  LCDMsgBox = osMessageCreate(osMessageQ(MsgBox), NULL);
  if(NULL == osThreadCreate(osThread(LCD), &LCDMsgBox))
  {
    return DEMO_ERROR;
  }
#endif /* _RTOS */

  return DEMO_OK;
}

#if defined(_RTOS)
/**
  * @brief  UserCableDetection reporting events on a specified port from CAD layer.
  * @param  PortNum  The handle of the port
  * @param  State    CAD state
  * @param  Cc       The Communication Channel for the USBPD communication
  * @retval None
  */
void DEMO_PostCADMessage(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc)
{
  uint32_t event = (DEMO_MSG_CAD |  (Cc << DEMO_CAD_CC_NUM_Pos | (PortNum << DEMO_CAD_PORT_NUM_Pos) | State ));
  osMessagePut(LCDMsgBox, event, 0);
}

/**
  * @brief  Callback function called by PE to inform DPM about PE event.
  * @param  PortNum   The handle of the port
  * @param  EventVal  @ref USBPD_NotifyEventValue_TypeDef
  * @retval None
  */
void DEMO_PostNotificationMessage(uint8_t PortNum, USBPD_NotifyEventValue_TypeDef EventVal)
{
  uint32_t event = DEMO_MSG_PENOTIFY | PortNum << DEMO_PE_PORT_NUM_Pos | EventVal;
  osMessagePut(LCDMsgBox, event, 0);
}


/**
  * @brief  Callback function called by PE to inform DPM about PE event.
  * @param  EventVal @ref USBPD_NotifyEventValue_TypeDef
  * @retval None
  */
void DEMO_PostMMIMessage(uint32_t EventVal)
{
  uint32_t event = DEMO_MSG_MMI | EventVal;
  if (joyevent == 0 )
  {
    joyevent = 1;
    (void)osMessagePut(LCDMsgBox, event, 0);
  }
}
#endif /* _RTOS */

/**
  * @brief  Function used to save cable information .
  * @param  PortNum        The handle of the port
  * @param  pDiscoIdentify Pointer on the VDM discovery identify
  * @retval None
  */
void DEMO_SetCableInfo(uint8_t PortNum, USBPD_DiscoveryIdentity_TypeDef *pDiscoIdentify)
{
  memcpy(&pIdentity, pDiscoIdentify, sizeof(USBPD_DiscoveryIdentity_TypeDef));
}

/**
  * @brief  Display power
  * @retval None
  */
static void Display_power(void)
{
  uint32_t vsense = 0;
  int32_t isense = 0;
  char  pstr[27]={0};
  static uint8_t counter = 0;

  counter++;

  if(counter == 5)
  {
    counter = 0;

    UTIL_LCD_SetFont(&Font12);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);

    /* Port 0 */
    if(DPM_Ports[USBPD_PORT_0].DPM_IsConnected)
    {
      BSP_USBPD_PWR_VBUSGetVoltage(USBPD_PORT_0, &vsense);
      BSP_USBPD_PWR_VBUSGetCurrent(USBPD_PORT_0, &isense);
      if(isense < 0)
      {
        isense = -isense;
        sprintf(pstr,"%2ld.%02ldV -%1ld.%02ldA",(vsense / 1000), (vsense % 1000)/10, (isense/1000), (isense % 1000)/10);
      }
      else
      {
        sprintf(pstr,"%2ld.%02ldV %1ld.%02ldA",(vsense / 1000), (vsense % 1000)/10, (isense/1000), (isense % 1000)/10);
      }

      UTIL_LCD_DisplayStringAt(64, Font12.Height * 4, (uint8_t*)pstr,LEFT_MODE);
    }
    else
    {
      UTIL_LCD_DisplayStringAt(64, Font12.Height * 4, (uint8_t*)"                ",LEFT_MODE);
    }
  }
}

/**
  * @brief  Display contract info
  * @param  PortNum The handle of the port
  * @retval None
  */
static void Display_contract_port(uint8_t PortNum)
{
  uint32_t pos;

  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);

  pos = (PortNum == 0)? 54:181;
  pos = pos +  Font16.Width;

  /* PDX */
  if(DPM_Ports[PortNum].DPM_IsConnected)
  {
    if (USBPD_SPECIFICATION_REV3 == DPM_Params[PortNum].PE_SpecRevision)
    {
      UTIL_LCD_DisplayStringAt(pos, 10+ Font16.Height, (uint8_t*)"PD3", LEFT_MODE);
    }
    else
    {
      UTIL_LCD_DisplayStringAt(pos, 10+Font16.Height, (uint8_t*)"PD2", LEFT_MODE);
    }

    /* UFP/DFP */
    if (USBPD_PORTDATAROLE_UFP == DPM_Params[PortNum].PE_DataRole)
    {
      UTIL_LCD_DisplayStringAt(pos + 38, 10+ Font16.Height, (uint8_t*)"UFP", LEFT_MODE);
    }
    else
    {
      UTIL_LCD_DisplayStringAt(pos + 38, 10 + Font16.Height, (uint8_t*)"DFP", LEFT_MODE);
    }

  }
  else
  {
    UTIL_LCD_DisplayStringAt(pos, 10+Font16.Height, (uint8_t*)"          ", LEFT_MODE);
  }
}

/**
  * @brief  Display error message
  * @param  PortNum   Port Number
  * @param  ErrorType Demo error type
  * @retval None
  */
void DEMO_Display_Error(uint8_t PortNum, uint8_t ErrorType)
{
  uint8_t *pmsg;

  Display_clear_info();

  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_RED);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_LIGHT);

  /* Display error info */
  if (ErrorType < DEMO_ERROR_TYPE_MAXNBITEMS)
  {
    pmsg = (uint8_t *) g_tab_error_strings[ErrorType];
    UTIL_LCD_DisplayStringAt(0, 1 + 6 * Font16.Height, pmsg, CENTER_MODE);
  }
}

/**
  * @brief  Display CC info
  * @param  PortNum The handle of the port
  * @param  cc      CC Pin value
  * @retval None
  */
static void Display_cc_port(uint8_t PortNum, CCxPin_TypeDef cc)
{
  uint32_t pos;

  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);

  pos = (PortNum == 0)? 54+30:181+30;
  pos = pos +  5*Font16.Width;

  switch(cc)
  {
  case CCNONE :
    UTIL_LCD_DisplayStringAt(pos, 10+ Font16.Height, (uint8_t*)"    ", LEFT_MODE);
    break;
  case CC1 :
    UTIL_LCD_DisplayStringAt(pos, 10+ Font16.Height, (uint8_t*)"CC1", LEFT_MODE);
    break;
  case CC2 :
    UTIL_LCD_DisplayStringAt(pos, 10+ Font16.Height, (uint8_t*)"CC2", LEFT_MODE);
    break;
  }
}

/**
  * @brief  Display border
  * @retval None
  */
static void Display_border(void)
{
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_CYAN);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);

  UTIL_LCD_DrawLine(0, 5 * Font12.Height, 320, 5 * Font12.Height, UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_DrawLine(0, 16 * Font12.Height -1, 320, 16 * Font12.Height - 1, UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_DrawLine(160, 16 * Font12.Height - 1, 160, 240, UTIL_LCD_COLOR_ST_BLUE_DARK);
}

/**
  * @brief  Display clear info
  * @retval None
  */
static void Display_clear_info(void)
{
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetFont(&Font12);

  /* Fill a blank rectangle to clear the display information panel */
  UTIL_LCD_FillRect(0, LINE(6), 320, 9*Font12.Height, UTIL_LCD_COLOR_WHITE);
}

/**
  * @brief  Display build info
  * @retval None
  */
static void Display_build_info(void)
{
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetFont(&Font16);

  UTIL_LCD_DisplayStringAt(0, 1 + (5 * Font16.Height), (uint8_t*)"USBPD Demo for", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 1 + (6 * Font16.Height), (uint8_t*)"STM32G474E-EVAL", CENTER_MODE);
#if defined(_GUI_INTERFACE) && defined(_TRACE)
  UTIL_LCD_DisplayStringAt(0, 1 + (8 * Font16.Height), (uint8_t*)"GUI TRACE", CENTER_MODE);
#elif defined(_GUI_INTERFACE) && !defined(_TRACE)
  UTIL_LCD_DisplayStringAt(0, 1 + (8 * Font16.Height), (uint8_t*)"GUI", CENTER_MODE);
#elif !defined(_GUI_INTERFACE) && defined(_TRACE)
  UTIL_LCD_DisplayStringAt(0, 1 + (8 * Font16.Height), (uint8_t*)"TRACE", CENTER_MODE);
#else /* TRACE */
  //UTIL_LCD_DisplayStringAt(0, 1 + (8 * Font16.Height), (uint8_t*)"GUI", CENTER_MODE);
#endif
#ifdef _VDM
  UTIL_LCD_DisplayStringAt(0, 1 + (9 * Font16.Height), (uint8_t*)"VDM", CENTER_MODE);
#endif

  char tab[27] = {0};
  sprintf(tab,"FW:%6x-STACK:%8x", USBPD_FW_VERSION, _LIB_ID);
  UTIL_LCD_DisplayStringAt(0, 1 + (10 * Font16.Height), (uint8_t*)tab, CENTER_MODE);
}

/**
  * @brief  Menu selection management
  * @param  PortNum     The handle of the port
  * @param  IndexMax    MAX index
  * @param  LineMax     MAX line
  * @param  Start       Pointer on Start
  * @param  End         Pointer on end
  * @param  Orientation Orientation
  * @retval None
  */
static void Menu_manage_selection(uint8_t PortNum, uint8_t IndexMax, uint8_t LineMax, uint8_t *Start, uint8_t *End, int8_t Orientation)
{
  *Start = 0;
  *End   = *Start + LineMax;

  if(( Orientation == 0 ) || (IndexMax == 0))
  {
    *Start = 0;
    *End =(LineMax > IndexMax)? IndexMax: LineMax;
    g_tab_menu_pos[PortNum] = g_tab_menu_sel[PortNum] = 0;
    return;
  }

  if( -1 == Orientation)
  {
    if(0 == g_tab_menu_sel[PortNum])
    {
      /* got at the end of the selection */
      g_tab_menu_pos[PortNum] = LineMax > IndexMax? IndexMax-1: LineMax-1;
      g_tab_menu_sel[PortNum] = IndexMax-1;
      *End = IndexMax;
      if( IndexMax > LineMax ) {
        *Start = IndexMax - LineMax;
      } else {
        *Start = 0;
      }
    }
    else
    {
      if(0 == g_tab_menu_pos[PortNum])
      {
        g_tab_menu_sel[PortNum]--;
        *Start =  g_tab_menu_sel[PortNum];
        if(IndexMax > LineMax )
          *End = *Start + LineMax;
        else
          *End = *Start + IndexMax;
      }
      else
      {
      *Start = g_tab_menu_sel[PortNum] - g_tab_menu_pos[PortNum];
      if(IndexMax > LineMax )
        *End   = g_tab_menu_sel[PortNum] + LineMax - g_tab_menu_pos[PortNum];
      else
        *End   = g_tab_menu_sel[PortNum] + IndexMax - g_tab_menu_pos[PortNum];

      g_tab_menu_pos[PortNum]--;
      g_tab_menu_sel[PortNum]--;
      }
    }
  }
  else  /* +1 == orientation */
  {
    if(((LineMax-1) == g_tab_menu_pos[PortNum]) || (((IndexMax-1) == g_tab_menu_pos[PortNum]) && (LineMax >= IndexMax)))
    {      /* at the end of selection, so two case there is others data or at the end of the data */
      if(g_tab_menu_sel[PortNum] == (IndexMax-1))
      {
        /* restart at the begin of the list */
        g_tab_menu_pos[PortNum] = 0;
        g_tab_menu_sel[PortNum] = 0;
        *Start = 0;
        *End   = LineMax > IndexMax? IndexMax : LineMax;
      }
      else
      {
        *Start = g_tab_menu_sel[PortNum] - (LineMax-1) + 1;
        *End = *Start+LineMax;
        g_tab_menu_sel[PortNum]++;
      }
    }
    else
    {
      *Start = g_tab_menu_sel[PortNum] - g_tab_menu_pos[PortNum];
      if(IndexMax > LineMax)
        *End   = *Start + LineMax;
      else
        *End   = *Start + IndexMax;
      g_tab_menu_sel[PortNum]++;
      g_tab_menu_pos[PortNum]++;
    }
  }
}

/**
  * @brief  get next menu
  * @param  PortNum     The handle of the port
  * @param  MenuId      ID menu
  * @retval menuid
  */
static DEMO_MENU Menu_manage_next(uint8_t PortNum, uint8_t MenuId)
{
  return g_tab_menu_next[PortNum][MenuId];
}

/**
  * @brief  get prev menu
  * @param  PortNum     The handle of the port
  * @param  MenuId      ID menu
  * @param  MenuId
  * @retval menuid
  */
static DEMO_MENU Menu_manage_prev(uint8_t PortNum, uint8_t MenuId)
{
  return g_tab_menu_prev[PortNum][MenuId];
}


/**
  * @brief  cable info memu
  * @param  PortNum     The handle of the port
  * @retval None
  */
static void Display_cableinfo_menu(uint8_t PortNum)
{
  uint8_t str[30];
  char *ss[4]        = {"2.0","GEN1","GEN1&2","NA  "};
  char *vbusc[4]     = {"NA","3A","5A","NA"};
  char *MaxVoltage[] = {"20V  ", "30V  ", "40V  ", "50V  "};
  char *Term[]    =    {"     ", "VCONN", "VCONN", "VCONN"};
  char *Latency[]    = {"<10ns", "<20ns", "<30ns", "<40ns",
                        "<50ns", "<60ns", "<70ns", ">70ns"
                        "1000 ", "2000 "};

  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetFont(&Font16);

  /* Display menu sink capa */
  UTIL_LCD_DisplayStringAt(0, 1 + 6 * Font12.Height, (uint8_t*)"Cable information :", LEFT_MODE);

  if(pIdentity.CableVDO_Presence == 0)
  {
    sprintf((char *)str, "no information available");
    UTIL_LCD_DisplayStringAt(0, 1 + 8 * Font12.Height + Font16.Height , str, LEFT_MODE);
  }
  else
  {
    sprintf((char *)str, "USBSS: %s  VBUS cap:%s", ss[pIdentity.CableVDO.b.USB_SS_Support],vbusc[pIdentity.CableVDO.b.VBUS_CurrentHandCap]);
    UTIL_LCD_DisplayStringAt(0, 1 + 7 * Font12.Height + Font16.Height , str, LEFT_MODE);
    sprintf((char *)str, "Max V: %s  TType:%s", MaxVoltage[pIdentity.CableVDO.b.CableMaxVoltage], Term[pIdentity.CableVDO.b.CableTermType]);
    UTIL_LCD_DisplayStringAt(0, 1 + 8 * Font12.Height + Font16.Height , str, LEFT_MODE);
    sprintf((char *)str, "Laten; %s CableType:0x%x", Latency[pIdentity.CableVDO.b.CableLatency],pIdentity.CableVDO.b.CableToType);
    UTIL_LCD_DisplayStringAt(0, 1 + 9 * Font12.Height + Font16.Height , str, LEFT_MODE);
    sprintf((char *)str, "VDO:0x%x FW:0x%x  HW:0x%x", pIdentity.CableVDO.b.VDO_Version,pIdentity.CableVDO.b.CableFWVersion,pIdentity.CableVDO.b.CableHWVersion );
    UTIL_LCD_DisplayStringAt(0, 1 + 10 * Font12.Height + Font16.Height , str, LEFT_MODE);
  }
}

/**
  * @brief  snk capa menu display
  * @param  PortNum     The handle of the port
  * @retval None
  */
static void Display_sinkcapa_menu(uint8_t PortNum)
{
  uint8_t str[32];

  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetFont(&Font16);

  /* Display menu sink capa */
  UTIL_LCD_DisplayStringAt(0, 1 + 6 * Font12.Height, (uint8_t *)"Sink capa :", LEFT_MODE);

  UTIL_LCD_SetFont(&Font12);
  sprintf((char *)str, "FRS  DRD  USB  EPW  HCPA  DRP ");
#if defined(USBPD_REV30_SUPPORT)
  if( DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[PortNum] & USBPD_PDO_SNK_FIXED_FRS_SUPPORT_Msk)           str[3] = '*';
#endif
  if( DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[PortNum] & USBPD_PDO_SNK_FIXED_DRD_SUPPORT_Msk)           str[8] = '*';
  if( DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[PortNum] & USBPD_PDO_SNK_FIXED_USBCOMM_Msk)               str[13] = '*';
  if( DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[PortNum] & USBPD_PDO_SNK_FIXED_EXT_POWER_Msk)             str[18] = '*';
  if( DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[PortNum] & USBPD_PDO_SNK_FIXED_HIGHERCAPAB_Msk)           str[24] = '*';
  if( DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[PortNum] & USBPD_PDO_SNK_FIXED_DRP_SUPPORT_Msk)           str[29] = '*';
  UTIL_LCD_DisplayStringAt(0, 1 + 6 * Font12.Height + Font16.Height , str, LEFT_MODE);

  Display_sinkcapa_menu_nav(PortNum, 0);
}

/**
  * @brief  snk capa menu navigation
  * @param  PortNum     The handle of the port
  * @param  Nav         Navigation
  * @retval None
  */
static void Display_sinkcapa_menu_nav(uint8_t PortNum, int8_t Nav)
{
  uint8_t _str[30];
  uint8_t _max = DPM_Ports[portSel].DPM_NumberOfRcvSNKPDO;
  uint8_t _start, _end;

  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetFont(&Font16);

  Menu_manage_selection(PortNum, _max, MAX_LINE_PDO, &_start, &_end, Nav);

  for(int8_t index=_start; index < _end; index++)
  {
    switch((uint32_t)((DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index] & USBPD_PDO_TYPE_Msk) >> USBPD_PDO_TYPE_Pos))
    {
    case USBPD_CORE_PDO_TYPE_FIXED :
      {
        uint32_t maxcurrent = ((DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index] & USBPD_PDO_SRC_FIXED_MAX_CURRENT_Msk) >> USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos)*10;
        uint32_t maxvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index] & USBPD_PDO_SRC_FIXED_VOLTAGE_Msk) >> USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)*50;
        sprintf((char*)_str, " FIXED:       %2ldV %2ld.%ldA", maxvoltage/1000, maxcurrent/1000, (maxcurrent % 1000) /100);
      }
      break;
    case USBPD_CORE_PDO_TYPE_BATTERY :
      {
        uint32_t maxvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index] & USBPD_PDO_SRC_BATTERY_MAX_VOLTAGE_Msk) >> USBPD_PDO_SRC_BATTERY_MAX_VOLTAGE_Pos) * 50;
        uint32_t minvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index] & USBPD_PDO_SRC_BATTERY_MIN_VOLTAGE_Msk) >> USBPD_PDO_SRC_BATTERY_MIN_VOLTAGE_Pos) * 50;
        uint32_t maxpower = ((DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index] & USBPD_PDO_SRC_BATTERY_MAX_POWER_Msk) >> USBPD_PDO_SRC_BATTERY_MAX_POWER_Pos) * 250;
        sprintf((char*)_str, " BATT:%2ld.%1ld-%2ld.%1ldV %2ld.%ldW", (minvoltage/1000),(minvoltage/100)%10, (maxvoltage/1000),(maxvoltage/100)%10, (maxpower/1000), (maxpower%1000)/100);
      }
      break;
    case USBPD_CORE_PDO_TYPE_VARIABLE :
      {
        uint32_t maxvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index] & USBPD_PDO_SRC_VARIABLE_MAX_VOLTAGE_Msk) >> USBPD_PDO_SRC_VARIABLE_MAX_VOLTAGE_Pos) * 50;
        uint32_t minvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index] & USBPD_PDO_SRC_VARIABLE_MIN_VOLTAGE_Msk) >> USBPD_PDO_SRC_VARIABLE_MIN_VOLTAGE_Pos) * 50;
        uint32_t maxcurrent = ((DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index] & USBPD_PDO_SRC_VARIABLE_MAX_CURRENT_Msk) >> USBPD_PDO_SRC_VARIABLE_MAX_CURRENT_Pos) * 10;
        sprintf((char*)_str, " VAR: %2ld.%1ld-%2ld.%1ldV %2ld.%ldA ", (minvoltage/1000),(minvoltage/100)%10, (maxvoltage/1000),(maxvoltage/100)%10, (maxcurrent/1000), ((maxcurrent % 1000) /100));
      }
      break;
#if _PPS
    case USBPD_CORE_PDO_TYPE_APDO :
      {
        uint32_t minvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index] & USBPD_PDO_SRC_APDO_MIN_VOLTAGE_Msk) >> USBPD_PDO_SRC_APDO_MIN_VOLTAGE_Pos) * 100;
        uint32_t maxvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index] & USBPD_PDO_SRC_APDO_MAX_VOLTAGE_Msk) >> USBPD_PDO_SRC_APDO_MAX_VOLTAGE_Pos) * 100;
        uint32_t maxcurrent = ((DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index] & USBPD_PDO_SRC_APDO_MAX_CURRENT_Msk) >> USBPD_PDO_SRC_APDO_MAX_CURRENT_Pos) * 50;
        sprintf((char*)_str, " APDO:%2ld.%1ld-%2ld.%1ldV %2ld.%ldA ", (minvoltage/1000),(minvoltage/100)%10, (maxvoltage/1000),(maxvoltage/100)%10, (maxcurrent/1000), ((maxcurrent % 1000) /100));
      }
      break;
#endif /* _PPS */
    default :
      sprintf((char*)_str,"Not yet managed by demo");
      break;
    }

    if((index - _start) == g_tab_menu_pos[PortNum])
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    }
    string_completion(_str, sizeof(_str));
    UTIL_LCD_DisplayStringAt(0, 1 + 8 * Font12.Height  + (index + 1 - _start) * Font16.Height,  (uint8_t*)_str, CENTER_MODE);

    if((index - _start) == g_tab_menu_pos[PortNum])
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
    }
  }
}

/**
  * @brief  src capa menu display
  * @param  PortNum     The handle of the port
  * @retval None
  */
static void Display_sourcecapa_menu(uint8_t PortNum)
{
  uint8_t str[32];

  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetFont(&Font16);

  /* Display menu source capa */
  sprintf((char *)str, "Source capa : %ld",DPM_Ports[PortNum].DPM_NumberOfRcvSRCPDO);
  UTIL_LCD_DisplayStringAt(0, 1 + 6 * Font12.Height, str, LEFT_MODE);

  UTIL_LCD_SetFont(&Font12);
  sprintf((char *)str, "UNCK  DRD  USB  EPW  SUSP  DRP ");
#if defined(USBPD_REV30_SUPPORT)
  if( DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[0] & USBPD_PDO_SRC_FIXED_UNCHUNK_SUPPORT_Msk)       str[4] = '*';
#endif
  if( DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[0] & USBPD_PDO_SRC_FIXED_DRD_SUPPORT_Msk)           str[9] = '*';
  if( DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[0] & USBPD_PDO_SRC_FIXED_USBCOMM_Msk)               str[14] = '*';
  if( DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[0] & USBPD_PDO_SRC_FIXED_EXT_POWER_Msk)             str[19] = '*';
  if( DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[0] & USBPD_PDO_SRC_FIXED_USBSUSPEND_Msk)            str[25] = '*';
  if( DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[0] & USBPD_PDO_SRC_FIXED_DRP_SUPPORT_Msk)           str[30] = '*';
  UTIL_LCD_DisplayStringAt(0, 1 + 6 * Font12.Height + Font16.Height , str, LEFT_MODE);

  Display_sourcecapa_menu_nav(PortNum, 0);
}

/**
  * @brief  src capa menu navigation
  * @param  PortNum     The handle of the port
  * @param  Nav         Navigation
  * @retval None
  */
static void Display_sourcecapa_menu_nav(uint8_t PortNum, int8_t Nav)
{
  uint8_t _str[25];
  uint8_t _max = DPM_Ports[portSel].DPM_NumberOfRcvSRCPDO;
  uint8_t _start, _end;

  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetFont(&Font16);

  Menu_manage_selection(PortNum, _max, MAX_LINE_PDO, &_start, &_end, Nav);

  for(int8_t index=_start; index < _end; index++)
  {
    switch((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_TYPE_Msk) >> USBPD_PDO_TYPE_Pos)
    {
    case USBPD_CORE_PDO_TYPE_FIXED :
      {
        uint32_t maxcurrent = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_FIXED_MAX_CURRENT_Msk) >> USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos)*10;
        uint32_t maxvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_FIXED_VOLTAGE_Msk) >> USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)*50;
        sprintf((char*)_str, " FIXED:      %2ldV %2ld.%ldA", (maxvoltage/1000), (maxcurrent/1000), ((maxcurrent % 1000) /100));
      }
      break;
    case USBPD_CORE_PDO_TYPE_BATTERY :
      {
        uint32_t maxvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_BATTERY_MAX_VOLTAGE_Msk) >> USBPD_PDO_SRC_BATTERY_MAX_VOLTAGE_Pos) * 50;
        uint32_t minvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_BATTERY_MIN_VOLTAGE_Msk) >> USBPD_PDO_SRC_BATTERY_MIN_VOLTAGE_Pos) * 50;
        uint32_t maxpower = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_BATTERY_MAX_POWER_Msk) >> USBPD_PDO_SRC_BATTERY_MAX_POWER_Pos) * 250;
        sprintf((char*)_str, " BATT:%2ld.%1ld-%2ld.%1ldV %2ld.%ldW", (minvoltage/1000),(minvoltage/100)%10, (maxvoltage/1000),(maxvoltage/100)%10, (maxpower/1000), (maxpower%1000)/100);
      }
      break;
    case USBPD_CORE_PDO_TYPE_VARIABLE :
      {
        uint32_t maxvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_VARIABLE_MAX_VOLTAGE_Msk) >> USBPD_PDO_SRC_VARIABLE_MAX_VOLTAGE_Pos) * 50;
        uint32_t minvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_VARIABLE_MIN_VOLTAGE_Msk) >> USBPD_PDO_SRC_VARIABLE_MIN_VOLTAGE_Pos) * 50;
        uint32_t maxcurrent = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_VARIABLE_MAX_CURRENT_Msk) >> USBPD_PDO_SRC_VARIABLE_MAX_CURRENT_Pos) * 10;
        sprintf((char*)_str, " VAR: %2ld.%1ld-%2ld.%1ldV %2ld.%ldA", (minvoltage/1000),(minvoltage/100)%10, (maxvoltage/1000),(maxvoltage/100)%10, (maxcurrent/1000), ((maxcurrent % 1000) /100));
      }
      break;
#if _PPS
    case USBPD_CORE_PDO_TYPE_APDO :
      {
        uint32_t minvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_APDO_MIN_VOLTAGE_Msk) >> USBPD_PDO_SRC_APDO_MIN_VOLTAGE_Pos) * 100;
        uint32_t maxvoltage = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_APDO_MAX_VOLTAGE_Msk) >> USBPD_PDO_SRC_APDO_MAX_VOLTAGE_Pos) * 100;
        uint32_t maxcurrent = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] & USBPD_PDO_SRC_APDO_MAX_CURRENT_Msk) >> USBPD_PDO_SRC_APDO_MAX_CURRENT_Pos) * 50;
        sprintf((char*)_str, " APDO:%2d.%1d-%2d.%1dV %2d.%dA", (minvoltage/1000),(minvoltage/100)%10, (maxvoltage/1000),(maxvoltage/100)%10, (maxcurrent/1000), ((maxcurrent % 1000) /100));
      }
      break;
#endif /* _PPS */
    default :
      sprintf((char*)_str,"Not yet managed by demo");
      break;
    }
    string_completion(_str, sizeof(_str));

    if((index - _start) == g_tab_menu_pos[PortNum])
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    }

    UTIL_LCD_DisplayStringAt(Font12.Width * 2, 1 + 8 * Font12.Height  + (index + 1 - _start) * Font16.Height,  (uint8_t*)_str, LEFT_MODE);

    if((index - _start) == g_tab_menu_pos[PortNum])
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
    }
  }
}

/**
  * @brief  src capa menu exec
  * @param  PortNum     The handle of the port
  * @retval status      0 if ok
  */
uint8_t Display_sourcecapa_menu_exec(uint8_t PortNum)
{
#if defined(_SNK) || defined(_DRP)
  USBPD_SNKRDO_TypeDef rdo;
  USBPD_PDO_TypeDef  pdo;
#if defined(USBPD_REV30_SUPPORT) && defined(_UNCHUNKED_SUPPORT)
  USBPD_PDO_TypeDef fixed_pdo;
#endif /* USBPD_REV30_SUPPORT && _UNCHUNKED_SUPPORT */
  uint32_t voltage, allowablepower, size;
  uint32_t snkpdolist[USBPD_MAX_NB_PDO];
  USBPD_PDO_TypeDef snk_fixed_pdo;

  /* Read SNK PDO list for retrieving useful data to fill in RDO */
  USBPD_PWR_IF_GetPortPDOs(PortNum, USBPD_CORE_DATATYPE_SNK_PDO, (uint8_t*)&snkpdolist[0], &size);
  /* Store value of 1st SNK PDO (Fixed) in local variable */
  snk_fixed_pdo.d32 = snkpdolist[0];

  /* selected SRC PDO */
  pdo.d32 = DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[g_tab_menu_sel[PortNum]];
  voltage = 0;
  allowablepower = 0;
  /* check if the selected source PDO is matching any of the SNK PDO */
  if (USBPD_TRUE == USBPD_DPM_SNK_EvaluateMatchWithSRCPDO(PortNum, pdo.d32, &voltage, &allowablepower))
  {
    /* consider the current PDO as correct */
  }
  else
  {
    Display_debug_port(PortNum, (uint8_t *)"Invalid PDO for SINK");
    return(1); /* error */
  }

  rdo.d32 = 0;
  rdo.FixedVariableRDO.CapabilityMismatch = 0;
  rdo.FixedVariableRDO.GiveBackFlag = 0;
  rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits = ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[g_tab_menu_sel[PortNum]] & USBPD_PDO_SRC_FIXED_MAX_CURRENT_Msk) >> USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos);
  rdo.FixedVariableRDO.NoUSBSuspend = 0;
  rdo.FixedVariableRDO.ObjectPosition = g_tab_menu_sel[PortNum] + 1;
  rdo.FixedVariableRDO.USBCommunicationsCapable = snk_fixed_pdo.SNKFixedPDO.USBCommunicationsCapable;
#if defined(USBPD_REV30_SUPPORT) && defined(_UNCHUNKED_SUPPORT)
  if (USBPD_SPECIFICATION_REV2 < DPM_Params[PortNum].PE_SpecRevision)
  {
    rdo.FixedVariableRDO.UnchunkedExtendedMessage = DPM_Settings[PortNum].PE_PD3_Support.d.PE_UnchunkSupport;
    fixed_pdo.d32 = DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[0];
    DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_FALSE;
    /* Set unchuncked bit if supported by port partner;*/
    if (USBPD_TRUE == fixed_pdo.SRCFixedPDO.UnchunkedExtendedMessage)
    {
      DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_TRUE;
    }
  }
#endif /* USBPD_REV30_SUPPORT && _UNCHUNKED_SUPPORT */

  if ((DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[g_tab_menu_sel[PortNum]] & USBPD_PDO_TYPE_Msk) == USBPD_PDO_TYPE_FIXED)
  {
    if( USBPD_OK == USBPD_DPM_RequestMessageRequest(PortNum, rdo.GenericRDO.ObjectPosition, voltage))
    {
      Display_debug_port(PortNum, (uint8_t *)"request sent");
      return(0); /* ok */
    }
    else
    {
      Display_debug_port(PortNum, (uint8_t *)"request not sent");
    }
  }
  else
  {
    Display_debug_port(PortNum, (uint8_t *)"not supported by demo");
  }
  return 1;
#else
  return(1); /* error */
#endif /* _SNK) || _DRP */
}

#ifdef USBPD_REV30_SUPPORT
/**
  * @brief  ext capa menu display
  * @param  PortNum     The handle of the port
  * @retval None
  */
static void Display_extcapa_menu(uint8_t PortNum)
{
  uint8_t str[30];

  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetFont(&Font16);

  /* Display menu source capa */
  sprintf((char *)str, "Extended capa : ");
  UTIL_LCD_DisplayStringAt(0, 1 + 6 * Font12.Height, str, LEFT_MODE);

  Display_extcapa_menu_nav(PortNum, 0);
}

/**
  * @brief  ext capa menu navigation
  * @param  PortNum     The handle of the port
  * @param  Orientation Orientation
  * @retval None
  */
static void Display_extcapa_menu_nav(uint8_t PortNum, int8_t Orientation)
{
#if _SRC_CAPA_EXT
  uint8_t _str[16][20];
  uint8_t _max = 16;  /* Nb field inside extended capa */
  uint8_t _start, _end;

  memset(_str, ' ', sizeof(_str));

  sprintf((char *)_str[0], "VID:0x%x",      DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.VID);
  sprintf((char *)_str[1], "PID:0x%x",      DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.PID);
  sprintf((char *)_str[2], "XID:0x%x",      DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.XID);
  sprintf((char *)_str[3], "F rev:0x%x",    DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.FW_revision);
  sprintf((char *)_str[4], "H rev:0x%x",    DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.HW_revision);
  sprintf((char *)_str[5], "V reg:%d",      DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.Voltage_regulation);
  sprintf((char *)_str[6], "Htime:%d",      DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.Holdup_time);
  sprintf((char *)_str[7], "Compliance:%d", DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.Compliance);
  sprintf((char *)_str[8], "Tcurre:%d",     DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.TouchCurrent);
  sprintf((char *)_str[9], "Peak1:%d",      DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.PeakCurrent1);
  sprintf((char *)_str[10],"Peak2:%d",      DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.PeakCurrent2);
  sprintf((char *)_str[11],"Peak3:%d" ,     DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.PeakCurrent3);
  sprintf((char *)_str[12],"Ttemp:%d",      DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.Touchtemp);
  sprintf((char *)_str[13],"SRCin:%d",      DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.Source_inputs);
  sprintf((char *)_str[14],"Nbbatt:%d",     DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.NbBatteries);
  sprintf((char *)_str[15],"PDP:%d",        DPM_Ports[PortNum].DPM_RcvSRCExtendedCapa.SourcePDP);

  for(int8_t i = 0; i < _max; i++)
  {
    uint8_t _size = strlen((char *)_str[i]);
    if(_size < 19 )
    {
      _str[i][_size] = ' ';
      _str[i][19] = '\0';
    }
  }

  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetFont(&Font16);

  Menu_manage_selection(PortNum, _max, MAX_LINE_EXTCAPA, &_start, &_end, Orientation);

  for(int8_t index=_start; index < _end; index++)
  {
    if((index - _start) == g_tab_menu_pos[PortNum])
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    }

    UTIL_LCD_DisplayStringAt(0, 1 + 6 * Font12.Height  + (index + 1 - _start) * Font16.Height,  (uint8_t*)_str[index], CENTER_MODE);

    if((index - _start) == g_tab_menu_pos[PortNum])
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
    }
  }
#endif /* _SRC_CAPA_EXT */
}

/**
  * @brief  Sink ext capa menu display
  * @param  PortNum     The handle of the port
  * @retval None
  */
static void Display_sinkextcapa_menu(uint8_t PortNum)
{
  uint8_t str[30];

  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetFont(&Font16);

  /* Display menu source capa */
  sprintf((char *)str, "Sink Extended capa : ");
  UTIL_LCD_DisplayStringAt(0, 1 + 6 * Font12.Height, str, LEFT_MODE);

  Display_sinkextcapa_menu_nav(PortNum, 0);
}

/**
  * @brief  Sink ext capa menu navigation
  * @param  PortNum     The handle of the port
  * @param  Orientation Orientation
  * @retval None
  */
static void Display_sinkextcapa_menu_nav(uint8_t PortNum, int8_t Orientation)
{
  uint8_t _str[15][22];
  uint8_t _max = 15;  /* Nb field inside sink extended capa */
  uint8_t _start, _end;

  memset(_str, ' ', sizeof(_str));

  sprintf((char *)_str[0],  "VID:0x%x",            DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa.VID);
  sprintf((char *)_str[1],  "PID:0x%x",            DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa.PID);
  sprintf((char *)_str[2],  "XID:0x%lx",           DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa.XID);
  sprintf((char *)_str[3],  "F rev:0x%x",          DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa.FW_revision);
  sprintf((char *)_str[4],  "H rev:0x%x",          DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa.HW_revision);
  sprintf((char *)_str[5],  "SKEDB_Ver:0x%x",      DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa.SKEDB_Version);
  sprintf((char *)_str[6],  "LoadStep:0x%x",       DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa.LoadStep);
  sprintf((char *)_str[7],  "SinkLoadCharac:0x%x", DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa.SinkLoadCharac.Value);
  sprintf((char *)_str[8],  "Compliance:%d",       DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa.Compliance);
  sprintf((char *)_str[9],  "Ttemp:%d",            DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa.Touchtemp);
  sprintf((char *)_str[10], "BatteryInfo:0x%x",    DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa.BatteryInfo);
  sprintf((char *)_str[11], "SinkModes:0x%x",      DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa.SinkModes);
  sprintf((char *)_str[12], "SinkMinPDP:%d",       DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa.SinkMinimumPDP);
  sprintf((char *)_str[13], "SinkOpePDP:%d",       DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa.SinkOperationalPDP);
  sprintf((char *)_str[14], "SinkMaxPDP:%d",       DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa.SinkMaximumPDP);

  for(int8_t i = 0; i < _max; i++)
  {
    uint8_t _size = strlen((char *)_str[i]);
    if(_size < 19 )
    {
      _str[i][_size] = ' ';
      _str[i][19] = '\0';
    }
  }

  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetFont(&Font16);

  Menu_manage_selection(PortNum, _max, MAX_LINE_EXTCAPA, &_start, &_end, Orientation);

  for(int8_t index=_start; index < _end; index++)
  {
    if((index - _start) == g_tab_menu_pos[PortNum])
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    }

    UTIL_LCD_DisplayStringAt(0, 1 + 6 * Font12.Height  + (index + 1 - _start) * Font16.Height,  (uint8_t*)_str[index], CENTER_MODE);

    if((index - _start) == g_tab_menu_pos[PortNum])
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
    }
  }
}
#else
static void Display_extcapa_menu(uint8_t PortNum)
{}

static void Display_extcapa_menu_nav(uint8_t PortNum, int8_t Orientation)
{}
#endif

/**
  * @brief  command menu display
  * @param  PortNum     The handle of the port
  * @retval None
  */
static void Display_command_menu(uint8_t PortNum)
{
  uint8_t _config = 0;
  uint8_t _countcommand = 0;

  /* Menu command display */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetFont(&Font16);

  if(USBPD_PORTPOWERROLE_SRC == DPM_Params[PortNum].PE_PowerRole) { _config = COMMANDROLE_SRC; }
  else { _config = COMMANDROLE_SNK; }
  if(USBPD_TRUE == DPM_Settings[PortNum].PE_RoleSwap ) _config|= COMMANDROLE_DRP;
  if(USBPD_TRUE == DPM_USER_Settings[PortNum].PE_VconnSwap) _config|= COMMANDROLE_VCONN;

  memset(g_tab_command_PORT[PortNum], 0, sizeof(typedef_COMMANDE)*DEMO_MAX_COMMAND);

  for(uint8_t index = 0; index < DEMO_MAX_COMMAND ;index++)
  {
     if(_config & g_tab_command_global[index].commandconfig)
     {
       memcpy((void *)&g_tab_command_PORT[PortNum][_countcommand], &g_tab_command_global[index], sizeof(typedef_COMMANDE));
       _countcommand++;
     }
  }

  /* Display menu command */
  UTIL_LCD_DisplayStringAt(0, 1 + 6 * Font12.Height, (uint8_t *)"Commands :       ", LEFT_MODE);

  Display_command_menu_nav(PortNum, 0);
}

/**
  * @brief  command menu navigation
  * @param  PortNum     The handle of the port
  * @param  Nav         (+1 -1 or 0)
  * @retval None
  */
void Display_command_menu_nav(uint8_t PortNum, int8_t Nav)
{
  uint8_t _max = 0;
  uint8_t _start;
  uint8_t _end;

  /* count the max value */
  for(uint8_t index = 0; index < DEMO_MAX_COMMAND; index++)
  {
     if(COMMAND_NONE != g_tab_command_PORT[PortNum][index].commandid )
     {
       _max++;
     }
  }

  Menu_manage_selection(PortNum, _max, MAX_LINE_COMMAND, &_start, &_end, Nav);

  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetFont(&Font16);


  for(int8_t index=_start; index < _end; index++)
  {
    if((index - _start) == g_tab_menu_pos[PortNum])
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    }

    UTIL_LCD_DisplayStringAt(0, 1 + 7 * Font12.Height  + (index + 1 - _start) * Font16.Height,  (uint8_t*)g_tab_command_PORT[PortNum][index].commandstr, CENTER_MODE);

    if((index - _start) == g_tab_menu_pos[PortNum])
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
    }
  }
}

/**
  * @brief  command menu execution
  * @param  PortNum     The handle of the port
  * @retval None
  */
static void Display_command_menu_exec(uint8_t PortNum)
{
  switch(g_tab_command_PORT[PortNum][g_tab_menu_sel[PortNum]].commandid)
  {
  case COMMAND_HARDRESET :
    USBPD_PE_Request_HardReset(PortNum);
    break;
  case COMMAND_CONTROLMSG_GOTOMIN :
    if( USBPD_OK == USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GOTOMIN, USBPD_SOPTYPE_SOP))
      Display_debug_port(PortNum, (uint8_t*)"request gotomin");
    else
      Display_debug_port(PortNum, (uint8_t*)"request not sent");
    break;
  case COMMAND_CONTROLMSG_GET_SRC_CAP :
    if( USBPD_OK == USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_SRC_CAP, USBPD_SOPTYPE_SOP))
      Display_debug_port(PortNum, (uint8_t*)"request srccapa");
    else
      Display_debug_port(PortNum, (uint8_t*)"request not sent");
    break;
  case COMMAND_CONTROLMSG_GET_SNK_CAP :
    if( USBPD_OK == USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_SNK_CAP, USBPD_SOPTYPE_SOP))
      Display_debug_port(PortNum, (uint8_t*)"request snkcapa");
    else
      Display_debug_port(PortNum, (uint8_t*)"request not sent");
    break;
  case COMMAND_CONTROLMSG_DR_SWAP :
    if( USBPD_OK == USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_DR_SWAP, USBPD_SOPTYPE_SOP))
      Display_debug_port(PortNum, (uint8_t*)"request drswap");
    else
      Display_debug_port(PortNum, (uint8_t*)"request not sent");
    break;
  case COMMAND_CONTROLMSG_PR_SWAP :
    if( USBPD_OK == USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_PR_SWAP, USBPD_SOPTYPE_SOP))
      Display_debug_port(PortNum, (uint8_t*)"request prswap");
    else
      Display_debug_port(PortNum, (uint8_t*)"request not sent");
    break;
  case COMMAND_CONTROLMSG_VCONN_SWAP :
    if( USBPD_OK == USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_VCONN_SWAP, USBPD_SOPTYPE_SOP))
      Display_debug_port(PortNum, (uint8_t*)"request vconnswap");
    else
      Display_debug_port(PortNum, (uint8_t*)"request not sent");
    break;
  case COMMAND_CONTROLMSG_SOFT_RESET :
    if( USBPD_OK == USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_SOFT_RESET, USBPD_SOPTYPE_SOP))
      Display_debug_port(PortNum, (uint8_t*)"request swreset");
    else
      Display_debug_port(PortNum, (uint8_t*)"request not sent");
    break;
#if defined(USBPD_REV30_SUPPORT)
  case COMMAND_CONTROLMSG_GET_SRC_CAPEXT :
    if( USBPD_OK == USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_SRC_CAPEXT, USBPD_SOPTYPE_SOP))
      Display_debug_port(PortNum, (uint8_t*)"request src extcapa");
    else
      Display_debug_port(PortNum, (uint8_t*)"request not sent");
    break;
  case COMMAND_CONTROLMSG_GET_SNK_CAPEXT :
    if( USBPD_OK == USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_SNK_CAPEXT, USBPD_SOPTYPE_SOP))
      Display_debug_port(PortNum, (uint8_t*)"request snk extcapa");
    else
      Display_debug_port(PortNum, (uint8_t*)"request not sent");
    break;
  case COMMAND_CONTROLMSG_GET_STATUS :
    if( USBPD_OK == USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_STATUS, USBPD_SOPTYPE_SOP))
      Display_debug_port(PortNum, (uint8_t*)"request status");
    else
      Display_debug_port(PortNum, (uint8_t*)"request not sent");
    break;
#if defined(_VDM)
  case COMMAND_REQUEST_VDM_DISCOVERY :
    USBPD_DPM_RequestVDM_DiscoverySVID(PortNum, USBPD_SOPTYPE_SOP);
    break;
#endif
#endif /* USBPD_REV30_SUPPORT */
  default :
    break;
  }
}

/**
  * @brief  manage menu display
  * @param  PortNum     The handle of the port
  * @param  MenuSel     Selection of the menu
  * @retval None
  */
static void Display_menuupdate_info(uint8_t PortNum, DEMO_MENU MenuSel)
{
  Display_clear_info();
  switch(MenuSel)
  {
  default :
  case MENU_EMPTY : /* No Port connected so nothing to display */
/*    Display_build_info();*/
    break;
  case MENU_COMMAND : /* Display menu command */
    Display_command_menu(PortNum);
    break;
  case MENU_SOURCECAPA_RECEIVED : /* Display menu source capa */
    Display_sourcecapa_menu(PortNum);
    break;
  case MENU_SINKCAPA_RECEIVED : /* Display menu source capa */
    Display_sinkcapa_menu(PortNum);
    break;
  case MENU_BUILD :
    Display_build_info();
    break;
  case MENU_EXTCAPA_RECEIVED :
    Display_extcapa_menu(PortNum);
    break;
  case MENU_SNKEXTCAPA_RECEIVED :
    Display_sinkextcapa_menu(PortNum);
    break;
  case MENU_CABLE:
    Display_cableinfo_menu(PortNum);
    break;
  }
}

/**
  * @brief  manage menu selection
  * @param  PortNum     The handle of the port
  * @param  MenuSel     Selection of the menu
  * @param  Nav         (+1, -1, 0)
  * @retval None
  */
static void Display_menunav_info(uint8_t PortNum, uint8_t MenuSel, int8_t Nav)
{
  switch(MenuSel)
  {
  default :
  case MENU_EMPTY : /* No Port connected so nothing to display */
    break;
  case MENU_COMMAND : /* Display menu command */
    Display_command_menu_nav(PortNum, Nav);
    break;
  case MENU_SOURCECAPA_RECEIVED : /* Display menu source capa */
    Display_sourcecapa_menu_nav(PortNum, Nav);
    break;
  case MENU_SINKCAPA_RECEIVED : /* Display menu source capa */
    Display_sinkcapa_menu_nav(PortNum, Nav);
    break;
  case MENU_EXTCAPA_RECEIVED : /* Display menu source capa */
    Display_extcapa_menu_nav(PortNum, Nav);
    break;
  case MENU_SNKEXTCAPA_RECEIVED : /* Display menu sink capa */
    Display_sinkextcapa_menu_nav(PortNum, Nav);
    break;
  }
}

/**
  * @brief  manage menu execution
  * @param  PortNum     The handle of the port
  * @param  MenuSel     Selection of the menu
  * @retval None
  */
static void Display_menuexec_info(uint8_t PortNum, uint8_t MenuSel)
{
  switch(MenuSel)
  {
  default :
  case MENU_EMPTY : /* No Port connected so nothing to display */
    break;
  case MENU_COMMAND : /* Display menu command */
    Display_command_menu_exec(PortNum);
    break;
  case MENU_SOURCECAPA_RECEIVED : /* Display menu source capa */
    Display_sourcecapa_menu_exec(PortNum);
    break;
  }
}

/**
  * @brief  manage the debug port info display
  * @param  PortNum     The handle of the port
  * @param  msg         string to display
  * @retval None
  */
static void Display_debug_port(uint8_t PortNum, uint8_t *msg)
{
  uint32_t pos;
  static uint8_t TxtMessage[2][4][22];

  if (strlen((char*)msg) > 22)
  {
    /* Display not possible */
    sprintf((char*)TxtMessage[PortNum][3], "Display error\n");
    return;
  }

  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PURPLE);

  pos = (PortNum == 0)? 0:160;
  pos = pos + 5;

  /* move line 2 inside 1 */
  strncpy((char*)TxtMessage[PortNum][0], (char*)TxtMessage[PortNum][1], 21);
  strncpy((char*)TxtMessage[PortNum][1], (char*)TxtMessage[PortNum][2], 21);
  /* move line 3 inside 2 */
  strncpy((char*)TxtMessage[PortNum][2], (char*)TxtMessage[PortNum][3], 21);
  /* move line enter inside line 3 */
  memset(TxtMessage[PortNum][3], 0, 20*sizeof(uint8_t));
  sprintf((char*)TxtMessage[PortNum][3], "%21s", msg);

  for(uint8_t index = 0; index < 4; index++)
  {
    UTIL_LCD_DisplayStringAt(pos, (index + 16) * Font12.Height, TxtMessage[PortNum][index], LEFT_MODE);
  }
}

/**
  * @brief  main demo function to manage all the application event and to update MMI
  * @retval None
  */
static void Display_Selected_port(void)
{
#if (USBPD_PORT_COUNT > 1)
  char pstr[20]={0};
#endif /* USBPD_PORT_COUNT > 1 */
  UTIL_LCD_SetFont(&Font16);

  if (portSel == 0)
  {
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  }
  else
  {
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  }

  /* Port 0 */
  if(DPM_Ports[0].DPM_IsConnected)
  {
    if (USBPD_PORTPOWERROLE_SRC == DPM_Params[0].PE_PowerRole)
      UTIL_LCD_DisplayStringAt(54 + Font16.Width , 10, (uint8_t*)"P1:SRC", LEFT_MODE);
    else
      UTIL_LCD_DisplayStringAt(54 + Font16.Width , 10, (uint8_t*)"P1:SNK", LEFT_MODE);
  }
  else
  {
    if(USBPD_TRUE == DPM_Settings[0].CAD_RoleToggle)
    {
      UTIL_LCD_DisplayStringAt(54 + Font16.Width, 10, (uint8_t*)"P1:DRP", LEFT_MODE);
    }
    else
    {
      if (USBPD_PORTPOWERROLE_SRC == DPM_Settings[0].PE_DefaultRole)
        UTIL_LCD_DisplayStringAt(54 + Font16.Width , 10, (uint8_t*)"P1:SRC", LEFT_MODE);
      else
        UTIL_LCD_DisplayStringAt(54 + Font16.Width , 10, (uint8_t*)"P1:SNK", LEFT_MODE);
    }
  }
}

/**
  * @brief  main demo function to manage all the application event and to update MMI
  * @param  Event Event value
  * @retval None
  */
static void DEMO_Manage_event(uint32_t Event)
{
  static DEMO_MENU _tab_menu_val[2] = { MENU_EMPTY, MENU_EMPTY};
  static uint8_t   _tab_connect_status[2] = { 0, 0};

  switch(Event & DEMO_MSG_TYPE_MSK)
  {
  case DEMO_MSG_MMI:
    {
      switch(Event & DEMO_MMI_ACTION_Msk)
      {
      case DEMO_MMI_ACTION_NONE:
        Display_power();
        return;
      case DEMO_MMI_ACTION_USER_PRESS:     /* change the selected default port */
        Display_menuupdate_info(portSel,_tab_menu_val[portSel]);
        break;
      case  DEMO_MMI_ACTION_RIGHT_PRESS:
        _tab_menu_val[portSel] = Menu_manage_next(portSel, _tab_menu_val[portSel]);
        Display_menuupdate_info(portSel,_tab_menu_val[portSel]);
        break;
      case  DEMO_MMI_ACTION_LEFT_PRESS:
        _tab_menu_val[portSel] = Menu_manage_prev(portSel, _tab_menu_val[portSel]);
        Display_menuupdate_info(portSel,_tab_menu_val[portSel]);
        break;
      case  DEMO_MMI_ACTION_UP_PRESS:       /* up /down */
        Display_menunav_info(portSel, _tab_menu_val[portSel], -1);
        break;
      case  DEMO_MMI_ACTION_DOWN_PRESS:
        Display_menunav_info(portSel, _tab_menu_val[portSel], +1);
        break;
      case DEMO_MMI_ACTION_SEL_PRESS :
        Display_menuexec_info(portSel, _tab_menu_val[portSel]);
        /* delay add to avoid multiple press event */
        osDelay(100);
        break;
      case DEMO_MMI_ACTION_ERROR_POWER :
        DEMO_Display_Error(portSel, DEMO_ERROR_TYPE_POWER);
        break;
      }
    }
    break;
  case DEMO_MSG_CAD :
    {
       uint8_t _portnum = (Event &  DEMO_CAD_PORT_NUM_Msk) >> DEMO_CAD_PORT_NUM_Pos;
       uint8_t _cc = (Event &  DEMO_CAD_CC_NUM_Msk) >> DEMO_CAD_CC_NUM_Pos;
       Display_cc_port(_portnum, (CCxPin_TypeDef)_cc);

       switch((Event & DEMO_CAD_STATE_Msk)>> DEMO_CAD_STATE_Pos)
       {
       case USBPD_CAD_EVENT_ATTEMC :
         _tab_connect_status[_portnum] = 1;
         Display_debug_port(_portnum, (uint8_t *)"attach emc");
         break;
       case USBPD_CAD_EVENT_ATTACHED :
         _tab_connect_status[_portnum] = 1;
         Display_debug_port(_portnum, (uint8_t *)"attach");
           break;
       case USBPD_CAD_EVENT_DETACHED :

         /* Reset cable information : vconn available only for port 0 */
         if (_portnum == 0 )
         {
           pIdentity.CableVDO_Presence = 0;
         }

         _tab_connect_status[_portnum] = 0;
         _tab_menu_val[_portnum] = MENU_EMPTY;
         Display_contract_port(_portnum);
         Display_power();
         Display_debug_port(_portnum, (uint8_t *)"detach");
         if(portSel == _portnum) Display_menuupdate_info(_portnum, _tab_menu_val[_portnum]);
         break;
       }
    }
  break;
  case DEMO_MSG_PENOTIFY :
    {
      uint8_t _portnum = (Event &  DEMO_PE_PORT_NUM_Msk) >> DEMO_PE_PORT_NUM_Pos;

      switch((Event & DEMO_PE_STATE_Msk)>> DEMO_PE_STATE_Pos)
      {
      case USBPD_NOTIFY_POWER_EXPLICIT_CONTRACT :
        Display_Selected_port();
        Display_contract_port(_portnum);
        Display_power();
        Display_debug_port(_portnum, (uint8_t *)"Explicit contract");

        if (_tab_connect_status[_portnum] == 1)
        {
          if (DPM_Params[_portnum].PE_PowerRole == USBPD_PORTPOWERROLE_SNK)
            _tab_menu_val[_portnum] = MENU_SOURCECAPA_RECEIVED;
          else
            _tab_menu_val[_portnum] = MENU_COMMAND;

          if(_portnum == portSel)
          {
            Display_menuupdate_info(_portnum, _tab_menu_val[_portnum]);
          }

          _tab_connect_status[_portnum] = 2;
        }
        break;
      case USBPD_NOTIFY_HARDRESET_TX :
        Display_debug_port(_portnum, (uint8_t *)"send hardreset");
        break;
      case USBPD_NOTIFY_HARDRESET_RX :
        Display_debug_port(_portnum, (uint8_t *)"receive hardreset");
        break;
      case USBPD_NOTIFY_SNK_GOTOMIN :
        Display_debug_port(_portnum, (uint8_t *)"receive gotomin");
        break;
      case USBPD_NOTIFY_SNK_GOTOMIN_READY :
        Display_debug_port(_portnum, (uint8_t *)"receive gotomin ready");
        break;
      case USBPD_NOTIFY_SOFTRESET_RECEIVED :
        Display_debug_port(_portnum, (uint8_t *)"receive soft reset");
        break;
      case USBPD_NOTIFY_PING_RECEIVED :
        Display_debug_port(_portnum, (uint8_t *)"receive ping");
        break;
      case USBPD_NOTIFY_GETSRCCAP_ACCEPTED :
        Display_debug_port(_portnum, (uint8_t *)"receive source capa");
        break;
      case USBPD_NOTIFY_GETSRCCAP_REJECTED :
        Display_debug_port(_portnum, (uint8_t *)"reject source capa");
        break;
      case USBPD_NOTIFY_GETSNKCAP_TIMEOUT :
        Display_debug_port(_portnum, (uint8_t *)"timeout source capa");
        break;
      case USBPD_NOTIFY_REQUEST_ACCEPTED :
        Display_debug_port(_portnum, (uint8_t *)"request accepted");
        break;
      case USBPD_NOTIFY_REQUEST_REJECTED :
        Display_debug_port(_portnum, (uint8_t *)"request rejected");
        break;
      case USBPD_NOTIFY_REQUEST_WAIT :
        Display_debug_port(_portnum, (uint8_t *)"request wait");
        break;
      case USBPD_NOTIFY_REQUEST_GOTOMIN :
        Display_debug_port(_portnum, (uint8_t *)"request gotomin");
        break;
      case USBPD_NOTIFY_POWER_SWAP_TO_SNK_DONE :
        Display_debug_port(_portnum, (uint8_t *)"swap SNK done");
        break;
      case USBPD_NOTIFY_POWER_SWAP_TO_SRC_DONE :
        Display_debug_port(_portnum, (uint8_t *)"swap SRC done");
        break;
      case USBPD_NOTIFY_POWER_SWAP_REJ :
        Display_debug_port(_portnum, (uint8_t *)"power swap reject");
        break;
      case USBPD_NOTIFY_DATAROLESWAP_UFP:
      case USBPD_NOTIFY_DATAROLESWAP_DFP:
        Display_contract_port(_portnum);
      break;
      case USBPD_NOTIFY_REQUEST_ENTER_MODE :
        Display_debug_port(_portnum, (uint8_t *)"send vdm enter mode");
        break;
      case USBPD_NOTIFY_REQUEST_ENTER_MODE_ACK :
        Display_debug_port(_portnum, (uint8_t *)"vdm enter mode ACK");
        break;
      case USBPD_NOTIFY_REQUEST_ENTER_MODE_NAK :
        Display_debug_port(_portnum, (uint8_t *)"vdm enter mode NAK");
        break;
      case USBPD_NOTIFY_REQUEST_ENTER_MODE_BUSY :
        Display_debug_port(_portnum, (uint8_t *)"vdm enter mode BSY");
        break;
      case USBPD_NOTIFY_GET_SNK_CAP_EXT_RECEIVED :
        Display_debug_port(_portnum, (uint8_t *)"rec get SNK cap ext");
        break;
      case USBPD_NOTIFY_SNK_CAP_EXT_SENT :
        Display_debug_port(_portnum, (uint8_t *)"SNK capa ext sent");
        break;
      case USBPD_NOTIFY_SNK_CAP_EXT_RECEIVED :
        Display_debug_port(_portnum, (uint8_t *)"rec SNK cap ext");
        break;
      }
    }
  break;

  }
  Display_Selected_port();
}

/**
  * @brief  demo task function
  * @param  queue_id ID of the queue
  * @retval None
  */
void DEMO_Task(void const *queue_id)
{
#if defined(_RTOS)
  osMessageQId  queue = *(osMessageQId *)queue_id;
#endif

  Display_build_info();

  for (;;)
  {
    osEvent event = osMessageGet(queue, 1000);
    DEMO_Manage_event(DEMO_MSG_MMI | DEMO_MMI_ACTION_NONE);
    if(osEventMessage == event.status)
    {
      DEMO_Manage_event(event.value.v);
      joyevent = 0;
    }
  }
}

/**
  * @brief  strin completion add space to fit the string size
  * @param  Str       Pointer on the string
  * @param  SizeMax   Max size of the string
  * @retval None
  */
static void string_completion(uint8_t *Str, uint8_t SizeMax)
{
  uint8_t size = strlen((char *)Str);

  if(size > SizeMax) Str[SizeMax] = '\0';
  else
  {
    /* add space */
    for(uint8_t index = size; index < SizeMax; index++)
    {
      Str[index] = ' ';
    }
    Str[SizeMax] = '\0';
  }
}

void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    signed char *pcTaskName );
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    signed char *pcTaskName )
{
  while(1);
}



