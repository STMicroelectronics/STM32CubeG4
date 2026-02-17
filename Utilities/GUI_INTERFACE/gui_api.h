/**
  ******************************************************************************
  * @file    gui_api.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for GUI
  *          interface.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _GUI_API_H
#define _GUI_API_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usbpd_def.h"

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_GUI_API
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup USBPD_GUI_API_Exported_Defines USBPD GUI API exported Defines
  * @{
  */
#define GUI_PE_PORT_NUM_Pos             (24u)
#define GUI_PE_PORT_NUM_Msk             (0x3u << GUI_PE_PORT_NUM_Pos)
#define GUI_PE_PORT_NUM_0               (0x0u << GUI_PE_PORT_NUM_Pos)
#define GUI_PE_PORT_NUM_1               (0x1u << GUI_PE_PORT_NUM_Pos)
#define GUI_PE_NOTIF_Pos                (4u)
#define GUI_PE_NOTIF_Msk                (0xFFu << GUI_PE_NOTIF_Pos)

#define GUI_NOTIF_MEASUREMENT           (USBPD_NOTIFY_ALL + 1)
#define GUI_NOTIF_MEASUREMENT_STEP      40u

/**
  * @brief  USB PD GUI STATE MACHINE STATE
  */
typedef enum
{
  GUI_STATE_INIT,     /*!< GUI init state */
  GUI_STATE_RUNNING,  /*!< GUI running state */
  GUI_STATE_RESET,    /*!< GUI reset state */
} USBPD_GUI_State;

/**
  * @brief  USB PD GUI TAG TYPE
  */
typedef enum
{
  DPM_RESET_REQ            = 0x01,
  DPM_INIT_REQ             = 0x02,
  DPM_INIT_CNF             = 0x03,
  DPM_CONFIG_GET_REQ       = 0x04,
  DPM_CONFIG_GET_CNF       = 0x05,
  DPM_CONFIG_SET_REQ       = 0x06,
  DPM_CONFIG_SET_CNF       = 0x07,
  DPM_CONFIG_REJ           = 0x08,
  DPM_MESSAGE_REQ          = 0x09,
  DPM_MESSAGE_CNF          = 0x0A,
  DPM_MESSAGE_REJ          = 0x0B,
  DPM_MESSAGE_IND          = 0x0C,
  DPM_MESSAGE_RSP          = 0x0D,
  DPM_REGISTER_READ_REQ    = 0x0E,
  DPM_REGISTER_READ_CNF    = 0x0F,
  DPM_REGISTER_WRITE_REQ   = 0x10,
  DPM_REGISTER_WRITE_CNF   = 0x11,
  DEBUG_STACK_MESSAGE      = 0x12,
} USBPD_GUI_Tag_TypeDef;

/** @defgroup USBPD_CORE_GUI_TYPE_NOTIFICATION USBPD CORE TRACE  TYPE NOTIFICATION
  * @{
  */
#define  GUI_NOTIF_NUMBEROFRCVSNKPDO                    (1UL << 0)
#define  GUI_NOTIF_RDOPOSITION                          (1UL << 1)
#define  GUI_NOTIF_LISTOFRCVSRCPDO                      (1UL << 2)
#define  GUI_NOTIF_NUMBEROFRCVSRCPDO                    (1UL << 3)
#define  GUI_NOTIF_LISTOFRCVSNKPDO                      (1UL << 4)
#define  GUI_NOTIF_ISCONNECTED                          (1UL << 5)
#define  GUI_NOTIF_DATAROLE                             (1UL << 6)
#define  GUI_NOTIF_POWERROLE                            (1UL << 7)
#define  GUI_NOTIF_CCDEFAULTCURRENTADVERTISED           (1UL << 8)
#define  GUI_NOTIF_VCONNON                              (1UL << 9)
#define  GUI_NOTIF_VCONNSWAPED                          (1UL << 10)
#define  GUI_NOTIF_MEASUREREPORTING                     (1UL << 11)
#define  GUI_NOTIF_CC                                   (1UL << 12)
#define  GUI_NOTIF_PE_EVENT                             (1UL << 13)
#define  GUI_NOTIF_TIMESTAMP                            (1UL << 14)
#define  GUI_NOTIF_POWER_EVENT                          (1UL << 15)

/**
  * @}
  */

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup USBPD_GUI_API_Exported_TypeDef USBPD GUI API exported TypeDef
  * @{
  */

typedef struct
{
  uint16_t VBUS_Level               :16;  /*!< VBUS Level */
  uint16_t IBUS_Level               :16;  /*!< IBUS Level */
  /* Measurement Reporting */
  union
  {
    uint8_t MeasurementReporting;
    struct
    {
      uint8_t MeasReportValue       :7;   /*!< Enable Measure reporting every tr x 40 ms  */
      uint8_t MeasReportActivation  :1;   /*!< Enable or Disable Measure reporting        */
    } d;
  } u;
  uint8_t Reserved                  :8;   /*!< Reserved bits */
} GUI_USER_ParamsTypeDef;

typedef struct
{
  uint32_t PE_DataSwap                                    : 1;  /*!< support data swap                                     */
  uint32_t PE_VconnSwap                                   : 1;  /*!< support VCONN swap                                    */
  uint32_t PE_DR_Swap_To_DFP                              : 1U; /*!< If supported, DR Swap to DFP can be accepted or not by the user else directly rejected */
  uint32_t PE_DR_Swap_To_UFP                              : 1U; /*!< If supported, DR Swap to UFP can be accepted or not by the user else directly rejected */
  uint32_t Reserved1                                      : 28U;  /*!< Reserved bits */
#if defined(_SNK) || defined(_DRP)
  USBPD_SNKPowerRequest_TypeDef DPM_SNKRequestedPower;          /*!< Requested Power by the sink board                     */
#else
  uint32_t            Reserved_ReqPower[6];                       /*!< Reserved bits to match with Resquested power information            */
#endif /* _SNK || _DRP */
#if defined(USBPD_REV30_SUPPORT)
#if _MANU_INFO
  USBPD_MIDB_TypeDef  DPM_ManuInfoPort;                         /*!< Manufacturer information used for the port            */
  uint16_t            ReservedManu;                             /*!< Reserved bits to match with Manufacturer information            */
#else
  uint32_t            ReservedManu[7];                          /*!< Reserved bits to match with Manufacturer information            */
#endif /* _MANU_INFO */
#if _SRC_CAPA_EXT && (defined(_SRC)||defined(_DRP))
  USBPD_SCEDB_TypeDef DPM_SRCExtendedCapa;                      /*!< SRC Extended Capability                               */
#else
  uint32_t            ReservedSrcCapa[6];                       /*!< Reserved bits to match with SrcCapa information            */
#endif /* _SRC_CAPA_EXT && (_SRC || _DRP) */
#if defined(_SNK)||defined(_DRP)
  USBPD_SKEDB_TypeDef DPM_SNKExtendedCapa;                      /*!< SNK Extended Capability                                */
  uint8_t             ReservedSnkCapa[3];                       /*!< Reserved bits to match with SnkCapaExt information     */
#else
  uint32_t            ReservedSnkCapa[6];                       /*!< Reserved bits to match with SnkCapaExt information     */
#endif /* _SNK || _DRP */
#else
  uint32_t            ReservedRev3[13];                         /*!< Reserved bits to match with PD3.0 information            */
#endif /* USBPD_REV30_SUPPORT */
  uint32_t PWR_AccessoryDetection                         : 1; /*!< It enables or disables powered accessory detection */
  uint32_t PWR_AccessoryTransition                        : 1; /*!< It enables or disables transition from Powered.accessory to Try.SNK */
  USBPD_CORE_PDO_ExtPowered_TypeDef PWR_UnconstrainedPower: 1; /*!< UUT has an external power source available that is sufficient to adequately power the system while charging external devices or the UUT primary function is to charge external devices. */
  CAD_SNK_Source_Current_Adv_Typedef PWR_RpResistorValue  : 2; /*!< RP resistor value based on @ref CAD_SNK_Source_Current_Adv_Typedef */
  USBPD_CORE_PDO_USBCommCapable_TypeDef USB_Support       : 1; /*!< USB_Comms_Capable, is the UUT capable of enumerating as a USB host or device? */
  uint32_t USB_Device                                     : 1; /*!< Type_C_Can_Act_As_Device, Indicates whether the UUT can communicate with USB 2.0 or USB 3.1 as a device or as the Upstream Facing Port of a hub. */
  uint32_t USB_Host                                       : 1; /*!<  Type_C_Can_Act_As_Host, Indicates whether the UUT can communicate with USB 2.0 or USB 3.1 as a host or as the Downstream Facing Port of a hub */
  USBPD_CORE_PDO_USBSuspendSupport_TypeDef USB_SuspendSupport: 1; /*!<  USB Suspend support values in PDO definition (Source) */
  uint32_t CAD_tDRP                                       :7;  /*!< The period that DRP shall complete a Source to Sink and back advertisement */
  uint32_t CAD_dcSRC_DRP                                  :7;  /*!< The percent of time that a DRP shall advertise Source during tDRP (in %) */
  uint32_t Reserved2                                      :31;  /*!< reserved bits */
} USBPD_USER_SettingsTypeDef;

typedef struct
{
  uint32_t XID;               /*!< Value provided by the USB-IF assigned to the product   */
  uint16_t VID;               /*!< Vendor ID (assigned by the USB-IF)                     */
  uint16_t PID;               /*!< Product ID (assigned by the manufacturer)              */
} USBPD_IdSettingsTypeDef;

typedef struct
{
  uint32_t VDM_XID_SOP                      :32; /*!< A decimal number assigned by USB-IF before certification */
  uint32_t VDM_USB_VID_SOP                  :16; /*!< A unique 16-bit number, assigned to the Vendor by USB-IF. */
  uint32_t VDM_PID_SOP                      :16; /*!< A unique number assigned by the Vendor ID holder identifying the product. */
  uint32_t VDM_bcdDevice_SOP                :16; /*!< A unique number assigned by the Vendor ID holder containing identity information relevant to the release version of the product. */
  USBPD_ModalOp_TypeDef VDM_ModalOperation  : 1; /*!< Product support Modes based on @ref USBPD_ModalOp_TypeDef */
  USBPD_USBCapa_TypeDef VDM_USBHostSupport  : 1; /*!< Indicates whether the UUT is capable of enumerating USB Host */
  USBPD_USBCapa_TypeDef VDM_USBDeviceSupport: 1; /*!< Indicates whether the UUT is capable of enumerating USB Devices */
  USBPD_ProductType_TypeDef VDM_ProductTypeUFPorCP : 3; /*!< Product type UFP or CablePlug of the UUT based on @ref USBPD_ProductType_TypeDef */
#if defined(USBPD_REV30_SUPPORT)
  USBPD_ProductType_TypeDef VDM_ProductTypeDFP : 3; /*!< Product type DFP of the UUT based on @ref USBPD_ProductType_TypeDef */
  uint32_t Reserved3                        : 7; /*!< Reserved bits */
#else
  uint32_t Reserved3                        :10; /*!< Reserved bits */
#endif /* USBPD_REV30_SUPPORT */
} USBPD_VDM_SettingsTypeDef;

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/* Exported variable ---------------------------------------------------------*/
/** @defgroup USBPD_GUI_API_Exported_Variable USBPD GUI API exported Variable
  * @{
  */
extern GUI_USER_ParamsTypeDef GUI_USER_Params[USBPD_PORT_COUNT];
#if !(defined(_RTOS) || defined(USBPD_THREADX))
extern __IO uint32_t      GUI_Flag;
#endif /* !(_RTOS || USBPD_THREADX) */

/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */
/** @defgroup USBPD_GUI_API_Exported_Functions USBPD GUI API exported functions
  * @{
  */

USBPD_FunctionalState GUI_Init(const uint8_t *(*CB_HWBoardVersion)(void), const uint8_t *(*CB_HWPDType)(void),
                               uint16_t (*CB_GetVoltage)(uint8_t), int16_t (*CB_GetCurrent)(uint8_t));
uint32_t              GUI_InitOS(void *MemoryPtr);
void                  GUI_Reset(void);

void                  GUI_Start(void);
void                  GUI_TimerCounter(void);
uint32_t              GUI_RXProcess(uint32_t Event);
uint32_t              GUI_FormatAndSendNotification(uint32_t PortNum, uint32_t TypeNotification, uint32_t Value);
uint32_t              GUI_GetMessage(uint8_t Character, uint8_t Error);
USBPD_GUI_State       GUI_SendAnswer(uint8_t **pMsgToSend, uint8_t *pSizeMsg);
USBPD_GUI_State       GUI_SendNotification(uint8_t PortNum, uint8_t **pMsgToSend, uint8_t *pSizeMsg,
                                           uint32_t TypeNotifcation, uint32_t Value);
void                  GUI_PostNotificationMessage(uint8_t PortNum, uint16_t EventVal);
void                  GUI_SaveInfo(uint8_t PortNum, uint8_t DataId, uint8_t *Ptr, uint32_t Size);
USBPD_FunctionalState GUI_IsRunning(void);
#if !defined(USE_STM32_UTILITY_OS)
void                  GUI_Execute(void);
#endif /* !USE_STM32_UTILITY_OS */
void                  GUI_RegisterCallback_FreeText(USBPD_StatusTypeDef(*CB_FreeText)(uint8_t, uint8_t *, uint16_t));
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* _GUI_API_H */

