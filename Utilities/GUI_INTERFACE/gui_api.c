/**
  ******************************************************************************
  * @file    gui_api.c
  * @author  MCD Application Team
  * @brief   GUI_API provides functions associated with answers the MCU can send
             to the computer via UART.
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

#if defined(_GUI_INTERFACE)

#define GUI_API_C

#include <stdlib.h>
#include "usbpd_core.h"
#include "gui_api.h"
#include "bsp_gui.h"
#include "data_struct_tlv.h"
#include "usbpd_dpm_core.h"
#include "usbpd_trace.h"
#include "usbpd_dpm_user.h"
#include "usbpd_dpm_conf.h"
#include "tracer_emb.h"
#ifdef _VDM
#include "usbpd_vdm_user.h"
#endif /* _VDM */
#include "usbpd_pdo_defs.h"
#ifndef USBPD_TCPM_MODULE_ENABLED
#else
#include "usbpd_tcpci.h"
#endif /* USBPD_TCPM_MODULE_ENABLED */
#if defined(_RTOS)
#include "cmsis_os.h"
#else
#if defined(USE_STM32_UTILITY_OS)
#include "utilities_conf.h"
#endif /*USE_STM32_UTILITY_OS */
#endif /*_RTOS*/

/* generic hal function valid for all stm32 */
extern uint32_t HAL_GetTick(void);
extern void HAL_NVIC_SystemReset(void);

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_GUI_API
  * @{
  */

/* Private enums -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup USBPD_GUI_API_Private_Defines USBPD GUI API Private Defines
  * @{
  */

#if !defined(USBPD_FW_VERSION)
#define USBPD_FW_VERSION          0xFFFFFFFFU
#endif /* !USBPD_FW_VERSION */

#if !defined(USBPD_START_PORT_NUMBER)
#define USBPD_START_PORT_NUMBER   0U
#endif /* !USBPD_START_PORT_NUMBER */

#define GUI_PORT_BIT_POSITION     5U /* Bit position of port number in TAG id */

#define GUI_BOX_MESSAGES_MAX      30U

#define GUI_TIMER_ENABLE_MSK      ((uint16_t)0x8000U)       /*!< Enable Timer Mask */
#define GUI_TIMER_READ_MSK        ((uint16_t)0x7FFFU)       /*!< Read Timer Mask   */

#if defined(USBPDCORE_SNK_CAPA_EXT)
#define GUI_VDM_CABLE_INFO      USBPD_CORE_SNK_EXTENDED_CAPA + 1U
#else
#define GUI_VDM_CABLE_INFO      USBPD_CORE_UNSTRUCTURED_VDM + 1U
#endif /* USBPDCORE_SNK_CAPA_EXT */

#if defined(_RTOS)
#if (osCMSIS < 0x20000U)
#define GUI_STACK_SIZE_ADDON_FOR_CMSIS              1
#else
#define GUI_STACK_SIZE_ADDON_FOR_CMSIS              5
#endif /* osCMSIS < 0x20000U */
#define FREERTOS_GUI_PRIORITY                   osPriorityLow
#define FREERTOS_GUI_STACK_SIZE                 (240 * GUI_STACK_SIZE_ADDON_FOR_CMSIS)
#endif /*_RTOS*/

/**
  * @brief  USB PD GUI REJECT REASON
  */
typedef enum
{
  GUI_REJ_DPM_REJECT                         = 0x00, /*<! Use in DPM_CONFIG_REJ */
  GUI_REJ_DPM_BUSY                           = 0x01, /*<! Use in DPM_CONFIG_REJ */
  GUI_REJ_DPM_TIMEOUT                        = 0x02, /*<! Use in DPM_CONFIG_REJ */
  GUI_REJ_DPM_INVALID_MESSAGE                = 0x03, /*<! Use in DPM_CONFIG_REJ (incorrect param in DPM_CONFIG_GET) */
  GUI_REJ_DPM_OUT_OF_RANGE                   = 0x04, /*<! Use in DPM_CONFIG_REJ */
  GUI_REJ_DPM_INVALID_PORT_NUMBER            = 0x05, /*<! Use in DPM_CONFIG_REJ */
  GUI_REJ_DPM_NOT_READY                      = 0x06, /*<! Use in DPM_MESSAGE_REJ */
} USBPD_GUI_Reject_Reason;

/**
  * @brief  USB PD GUI REGISTER TCPC
  */
#define GUI_REG_NUMBER_OF_REGISTERS          39U
typedef enum
{
  GUI_REG_VENDOR_ID                          = 0x00, /*<! VENDOR_ID */
  GUI_REG_PRODUCT_ID                         = 0x02, /*<! PRODUCT_ID */
  GUI_REG_DEVICE_ID                          = 0x04, /*<! DEVICE_ID */
  GUI_REG_USBTYPEC_REV                       = 0x06, /*<! USBTYPEC_REV */
  GUI_REG_USBPD_REV_VER                      = 0x08, /*<! USBPD_REV_VER */
  GUI_REG_USBPD_INTERFACE_REV                = 0x0A, /*<! USBPD_INTERFACE_REV */
  GUI_REG_ALERT                              = 0x10, /*<! ALERT */
  GUI_REG_ALERT_MASK                         = 0x12, /*<! ALERT_MASK */
  GUI_REG_POWER_STATUS_MASK                  = 0x14, /*<! POWER_STATUS_MASK */
  GUI_REG_FAULT_STATUS_MASK                  = 0x15, /*<! FAULT_STATUS_MASK */
  GUI_REG_CONFIG_STANDARD_OUTPUT             = 0x18, /*<! CONFIG_STANDARD_OUTPUT */
  GUI_REG_TCPC_CONTROL                       = 0x19, /*<! TCPC_CONTROL */
  GUI_REG_ROLE_CONTROL                       = 0x1A, /*<! ROLE_CONTROL */
  GUI_REG_FAULT_CONTROL                      = 0x1B, /*<! FAULT_CONTROL */
  GUI_REG_POWER_CONTROL                      = 0x1C, /*<! POWER_CONTROL */
  GUI_REG_CC_STATUS                          = 0x1D, /*<! CC_STATUS */
  GUI_REG_POWER_STATUS                       = 0x1E, /*<! POWER_STATUS */
  GUI_REG_FAULT_STATUS                       = 0x1F, /*<! FAULT_STATUS */
  GUI_REG_COMMAND                            = 0x23, /*<! COMMAND */
  GUI_REG_DECIVE_CAP1                        = 0x24, /*<! DECIVE_CAP1 */
  GUI_REG_DECIVE_CAP2                        = 0x26, /*<! DECIVE_CAP2 */
  GUI_REG_STD_INPUT_CAP                      = 0x28, /*<! STD_INPUT_CAP */
  GUI_REG_STD_OUTPUT_CAP                     = 0x29, /*<! STD_OUTPUT_CAP */
  GUI_REG_MSG_HEADER_INFO                    = 0x2E, /*<! MSG_HEADER_INFO */
  GUI_REG_RX_DETECT                          = 0x2F, /*<! RX_DETECT */
  GUI_REG_RX_BYTE_COUNT                      = 0x30, /*<! RX_BYTE_COUNT */
  GUI_REG_RX_BUFFER_FRAME_TYPE               = 0x31, /*<! RX_BUFFER_FRAME_TYPE */
  GUI_REG_RX_HEADER                          = 0x32, /*<! RX_HEADER */
  GUI_REG_RX_DATA                            = 0x34, /*<! RX_DATA */
  GUI_REG_TRANSMIT                           = 0x50, /*<! TRANSMIT */
  GUI_REG_TX_BYTE_COUNT                      = 0x51, /*<! TX_BYTE_COUNT */
  GUI_REG_TX_HEADER                          = 0x52, /*<! TX_HEADER */
  GUI_REG_TX_DATA                            = 0x54, /*<! TX_DATA */
  GUI_REG_VBUS_VOLTAGE                       = 0x70, /*<! VBUS_VOLTAGE */
  GUI_REG_VBUS_SINK_DISCONNECT_THRESHOLD     = 0x72, /*<! VBUS_SINK_DISCONNECT_THRESHOLD */
  GUI_REG_VBUS_STOP_DISCHARGE_THRESHOLD      = 0x74, /*<! VBUS_STOP_DISCHARGE_THRESHOLD */
  GUI_REG_VBUS_VOLTAGE_ALARM_HI_CFG          = 0x76, /*<! VBUS_VOLTAGE_ALARM_HI_CFG */
  GUI_REG_VBUS_VOLTAGE_ALARM_LO_CFG          = 0x78, /*<! VBUS_VOLTAGE_ALARM_LO_CFG */
  GUI_REG_VENDOR_DATA                        = 0x80, /*<! VENDOR_DATA */
} USBPD_GUI_RegisterTCPC;

/**
  * @brief  USB PD GUI TAG PARAM
  */
typedef enum
{
  GUI_PARAM_SOP                              = 0x00, /*<! SOP support */
  GUI_PARAM_SOP1                             = 0x00, /*<! SOP prime support */
  GUI_PARAM_SOP2                             = 0x00, /*<! SOP second support */
  GUI_PARAM_FASTROLESWAP                     = 0x02, /*<! It enables the FRS */
  GUI_PARAM_DATAROLESWAP_TO_UFP              = 0x03, /*<! Support of Data role swap to UFP */
  GUI_PARAM_DEFAULTPOWERROLE                 = 0x04, /*<! DRP on PE side but no toggle on CAD
                                                          (dependency with CADRoleToggle) */
  GUI_PARAM_DRP_SUPPORT                      = 0x05, /*<! DRP support (Power Role swap support) */
  GUI_PARAM_CADROLETOGGLE                    = 0x06, /*<! CAD Role Toggle */
  GUI_PARAM_PE_SCAP_HR                       = 0x07, /*<! Hard reset after N_SOURCE_CAPS message sending tentatives */
  GUI_PARAM_VCONNSWAP                        = 0x08, /*<! It enables or disables the VCONN swap capability */
  GUI_PARAM_VDM_SUPPORT                      = 0x09, /*<! Indicate if VDM messages support is enabled or not */
  GUI_PARAM_PING_SUPPORT                     = 0x0A, /*<! Indicate if Ping message support is enabled or not */
  GUI_PARAM_PPS_SUPPORT                      = 0x0B, /*<! PPS message supported by PE  */
  GUI_PARAM_SRCCAPAEXTSUPPORT                = 0x0B, /*<! Source_Capabilities_Extended message supported by PE  */
  GUI_PARAM_ALERTSUPPORT                     = 0x0B, /*<! Alert message supported by PE  */
  GUI_PARAM_STATUSSUPPORT                    = 0x0B, /*<! Status message supported by PE
                                                          (Is_Alert_Supported should be enabled) */
  GUI_PARAM_BATTERYSTATUSSUPPORT             = 0x0B, /*<! Battery_Status message supported by PE  */
  GUI_PARAM_BATTERYCAPASUPPORT               = 0x0B, /*<! Battery_Capabilities message supported by PE  */
  GUI_PARAM_MANUINFOSUPPORT                  = 0x0B, /*<! Manufacturer_Info message supported by PE  */
  GUI_PARAM_COUNTRYCODESSUPPORT              = 0x0B, /*<! Country_Codes message supported by PE  */
  GUI_PARAM_COUNTRYINFOSUPPORT               = 0x0B, /*<! Country_Info message supported by PE  */
  GUI_PARAM_SECURESPONSESUPPORT              = 0x0B, /*<! Security_Response message supported by PE  */
  GUI_PARAM_SNK_PDO                          = 0x0C, /*<! maximum 7 * 32bit */
  GUI_PARAM_SRC_PDO                          = 0x0D, /*<! maximum 7 * 32bit */
  GUI_PARAM_TDRP                             = 0x0E, /*<! The period a DRP shall complete a Source to Sink
                                                          and back advertisement */
  GUI_PARAM_DCSRC_DRP                        = 0x0F, /*<! The percent of time that a DRP shall advertise
                                                          Source during tDRP (in %) */
  GUI_PARAM_RESPONDS_TO_DISCOV_SOP           = 0x10, /*<! UUT can respond successfully to a Discover Identity
                                                          command from its port partner using SOP. */
  GUI_PARAM_ATTEMPTS_DISCOV_SOP              = 0x11, /*<! UUT can send a Discover Identity command to
                                                          its port partner using SOP */
  GUI_PARAM_XID_SOP                          = 0x12, /*<! A decimal number assigned by USB-IF before certification */
  GUI_PARAM_DATA_CAPABLE_AS_USB_HOST_SOP     = 0x13, /*<! Indicates whether the UUT is capable of
                                                          enumerating USB Host */
  GUI_PARAM_DATA_CAPABLE_AS_USB_DEVICE_SOP   = 0x14, /*<! Indicates whether the UUT is capable of
                                                          enumerating as a USB Device */
  GUI_PARAM_PRODUCT_TYPE_SOP                 = 0x15, /*<! What is the product type of the UUT?  */
  GUI_PARAM_MODAL_OPERATION_SUPPORTED_SOP    = 0x16, /*<! Does the product support Modes?  */
  GUI_PARAM_USB_VID_SOP                      = 0x17, /*<! A unique 16-bit number, assigned to the Vendor by USB-IF. */
  GUI_PARAM_PID_SOP                          = 0x18, /*<! A unique number assigned by the Vendor ID holder
                                                          identifying the product */
  GUI_PARAM_BCDDEVICE_SOP                    = 0x19, /*<! A unique number assigned by the Vendor ID holder containing
                                                          identity information relevant to the release version
                                                          of the product. */
  GUI_PARAM_MEASUREREPORTING                 = 0x1A, /*<! 0x00: Disable Measure reporting
                                                          (1 << 8) | (tr between 0 & 127): Enable Measure reporting
                                                             every tr x 40 ms
                                                             (if xx = 0 means only 1 indication to report to GUI)
                                                          */
  GUI_PARAM_MANUINFOPORT_VID                 = 0x1B, /*<! Vendor ID (assigned by the USB-IF))
                                                          used by Manufacturer info  set for port */
  GUI_PARAM_MANUINFOPORT_PID                 = 0x1B, /*<! Product ID (assigned by the manufacturer)
                                                          used by Manufacturer info  set for port */
  GUI_PARAM_MANUINFOPORT_STRING              = 0x1B, /*<! Vendor string used by Manufacturer info  set for port */
  GUI_PARAM_DATAROLESWAP_TO_DFP              = 0x1C, /*<! Support of Data role swap to DFP */
  GUI_PARAM_NB_TAG                           = (GUI_PARAM_DATAROLESWAP_TO_DFP + 1), /*<! Number max of param */
  GUI_PARAM_ALL                              = 0xFFu,
} USBPD_GUI_Tag_Param;

/**
  * @brief  USB PD GUI TAG INDICATION
  */
typedef enum
{
  GUI_IND_NUMBEROFRCVSNKPDO                  = 0x00, /*<! The number of received Sink Power Data Objects from
                                                          port Partner (when Port partner is a Sink or a DRP port) */
  GUI_IND_RDOPOSITION                        = 0x01, /*<! RDO Position of requested DO in Source list of capabilities */
  GUI_IND_LISTOFRCVSRCPDO                    = 0x02, /*<! The list of received Source Power Data Objects from
                                                          Port partner (when Port partner is a Source or a DRP port) */
  GUI_IND_NUMBEROFRCVSRCPDO                  = 0x03, /*<! The number of received Source Power Data Objects from
                                                          Port Partner (when Port partner is a Source or a DRP port) */
  GUI_IND_LISTOFRCVSNKPDO                    = 0x04, /*<! The list of received Sink Power Data Objects from
                                                          Port partner (when Port partner is a Sink or a DRP port) */
  GUI_IND_ISCONNECTED                        = 0x05, /*<! USB PD connection state */
  GUI_IND_CC                                 = 0x06, /*<! CC side */
  GUI_IND_DATAROLE                           = 0x07, /*<! It defines the initial data role. */
  GUI_IND_POWERROLE                          = 0x08, /*<! It defines the power role. */
  GUI_IND_CCDEFAULTCURRENTADVERTISED         = 0x09, /*<! advertising the current capability */
  GUI_IND_VCONNON                            = 0x0A, /*<! Vconn Status */
  GUI_IND_VBUS_LEVEL                         = 0x0C, /*<! VBUS level (in mV) */
  GUI_IND_IBUS_LEVEL                         = 0x0C, /*<! IBUS level (in mA) */
  GUI_IND_PD_SPECREVISION                    = 0x0D, /*<! Selected Specification revision */
  GUI_IND_PD_MESSAGENOTIF                    = 0x0E, /*<! Send notifications of PD messages */
  GUI_IND_NBBATTERIES                        = 0x0F, /*<! Number of batteries supported by the devices. */
  GUI_IND_COUNTRYCODES                       = 0x10, /*<! List of the country codes received
                                                          in the COUNTRY_CODES message */
  GUI_IND_SVDM_SVIDS                         = 0x11, /*<! List of the SVDM SVID received
                                                          in the SVDM Discovery SVID message */
  GUI_IND_SVDM_MODES                         = 0x12, /*<! List of the country codes received
                                                          in the SVDM Discovery SVID message */
  GUI_IND_TIMESTAMP                          = 0x13, /*<! Timestamp used for VBUS and IBUS values */
  GUI_IND_PPS                                = 0x14, /*<! PPS value based on @ref USBPD_PPSSDB_TypeDef */
  GUI_IND_STATUS                             = 0x15, /*<! Status value based on @ref USBPD_SDB_TypeDef */
  GUI_IND_VDM_IDENTITY                       = 0x16, /*<! VDM Identity based on @ref USBPD_IDHeaderVDO_TypeDef */
  GUI_IND_CABLE_VDO                          = 0x19, /*<! VDM Cable Object based on @ref USBPD_CableVdo_TypeDef */
  GUI_IND_ALL                                = (GUI_IND_CABLE_VDO + 1), /*<! Number max of indication */
} USBPD_GUI_Tag_Indication;

/**
  * @brief  USB PD GUI TAG PARAM_MSG
  */
typedef enum
{
  GUI_PARAM_MSG_SOPTYPE                      = 0x00, /*<! SOP type used for DPM_MESSAGE_REQ */
  GUI_PARAM_MSG_RDOPOSITION                  = 0x01, /*<! RDO position of Received SRC PDO (used for RESET_REQ) */
  GUI_PARAM_MSG_REQUESTEDVOLTAGE             = 0x02, /*<! Requested voltage in REQUEST message (in mV) */
  GUI_PARAM_MSG_ALERTMSG                     = 0x04, /*<! Alert message used in ALERT message */
  GUI_PARAM_MSG_COUNTRYCODE                  = 0x05, /*<! Coutry code used in GET_COUNTRY_INFO message */
  GUI_PARAM_MSG_SVDM_SVID                    = 0x06, /*<! SVID used in SVDM disco mode, enter mode & exit mode */
  GUI_PARAM_MSG_SVDM_MODEINDEX               = 0x07, /*<! Mode index used in SVDM enter and exit mode messages */
  GUI_PARAM_MSG_UVDM_DATA                    = 0x08, /*<! Data used in UVDM message */
  GUI_PARAM_MSG_DP_STATUS                    = 0x09, /*<! Data used in DP status message */
  GUI_PARAM_MSG_DP_CONFIGURE                 = 0x0A, /*<! Data used in DP config message */
  GUI_PARAM_MSG_DP_ATTENTION                 = 0x0B, /*<! Data used in DP attention message */
  GUI_PARAM_MSG_BATTERYREF                   = 0x0C, /*<! Battery reference used in Get battery capa & status message */
  GUI_PARAM_MSG_MANUINFODATA                 = 0x0D, /*<! Manufacturer_Info data used
                                                          in Get Manufacturer info message */
  GUI_PARAM_MSG_FREE_TEXT                    = 0x0E, /*<! Free text */
  GUI_PARAM_MSG_ALL                          = (GUI_PARAM_MSG_FREE_TEXT + 1), /*<! Number max of msg param */
} USBPD_GUI_Tag_ParamMsg;

/**
  * @brief  USB PD GUI MESSAGES
  */
typedef enum
{
  GUI_MSG_GOTOMIN                            = 0x00, /*<! Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_PING                               = 0x01, /*<! Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_GET_SRC_CAPA                       = 0x03, /*<! Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_GET_SNK_CAPA                       = 0x04, /*<! Message sent only if port partner is DRP.
                                                          Answer to this message is a SNK_CAPA
                                                          Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_DR_SWAP                            = 0x05, /*<! Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_PR_SWAP                            = 0x06, /*<! Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_VCONN_SWAP                         = 0x07, /*<! Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_SOFT_RESET                         = 0x08, /*<! Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_GET_SOURCE_CAPA_EXTENDED           = 0x09, /*<! Specific to PD3.0
                                                          Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_GET_STATUS                         = 0x0A, /*<! Specific to PD3.0. Return a STATUS message
                                                          Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_FR_SWAP                            = 0x0B, /*<! Specific to PD3.0
                                                          Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_GET_PPS_STATUS                     = 0x0C, /*<! Specific to PD3.0. Wait for PPS_STATUS message
                                                          Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_GET_COUNTRY_CODES                  = 0x0D, /*<! Specific to PD3.0. Return a COUNTRY_CODES message
                                                          Note: Refer to Control message in USB_PD specv3.0 */
  GUI_MSG_SOURCE_CAPA                        = 0x0E, /*<! DPM_CONFIG_SET_REQ to be sent before sending this message
                                                          Note: Refer to data message in USB_PD specv3.0 */
  GUI_MSG_REQUEST                            = 0x0F, /*<! Select one of the capability available.
                                                          Note: Refer to data message in USB_PD specv3.0  */
  GUI_MSG_ALERT                              = 0x11, /*<! Specific to PD3.0
                                                          Note: Refer to data message in USB_PD specv3.0 */
  GUI_MSG_GET_COUNTRY_INFO                   = 0x12, /*<! Return COUNTRY_INFO message.
                                                          Note: Refer to data message in USB_PD specv3.0 */
  GUI_MSG_VDM_DISCO_IDENT                    = 0x13, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_VDM_DISCO_SVID                     = 0x14, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_VDM_DISCO_MODE                     = 0x15, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_VDM_ENTER_MODE                     = 0x16, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_VDM_EXIT_MODE                      = 0x17, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_VDM_ATTENTION                      = 0x18, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_VDM_UNSTRUCTURED                   = 0x19, /*<! 1..7 x 32bit
                                                          Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_FREE_TEXT                          = 0x1A, /*<! Free text */
  GUI_MSG_DISPLAY_PORT_STATUS                = 0x20, /*<! 1..7 x 32bit
                                                          Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_DISPLAY_PORT_CONFIG                = 0x21, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_DISPLAY_PORT_ATTENTION             = 0x22, /*<! Note: Refer to VDM message in USB_PD specv3.0 */
  GUI_MSG_HARD_RESET                         = 0x24, /*<! Note: refer to Reset messages in USB_PD specv3.0 */
  GUI_MSG_CABLE_RESET                        = 0x25, /*<! Note: refer to Reset messages in USB_PD specv3.0 */
  GUI_MSG_GET_BAT_CAPA                       = 0x26, /*<! Return BAT_CAPA message
                                                          Note: Refer to extended message in USB_PD specv3.0 */
  GUI_MSG_GET_BAT_STATUS                     = 0x27, /*<! Return BAT_STATUS message
                                                          Note: Refer to extended message in USB_PD specv3.0 */
  GUI_MSG_GET_MANU_INFO                      = 0x28, /*<! Return MANU_INFO message
                                                          Note: Refer to extended message in USB_PD specv3.0 */
  GUI_MSG_SECU_REQUEST                       = 0x29, /*<! Note: Refer to extended message in USB_PD specv3.0 */
  GUI_MSG_FIRM_UPDATE_REQUEST                = 0x2A, /*<! Note: Refer to extended message in USB_PD specv3.0 */
  GUI_MSG_GET_SINK_CAPA_EXTENDED             = 0x2B, /*<! Specific to PD3.0
                                                            Note: Refer to Control message in USB_PD specv3.0 */
} USBPD_GUI_Messages;

/**
  * @brief  USB PD GUI INIT
  */
typedef enum
{
  GUI_INIT_HWBOARDVERSION                    = 0x00, /*<! ASCII stream to indicate STM32 version like STM32F032xB */
  GUI_INIT_HWPDTYPE                          = 0x01, /*<! ASCII stream to indicate PD type used by the devices like:
                                                          - AFE solution (MB1257B)
                                                          - 1602 solution (MB1303 rev A)
                                                          - TCPM solution (TCPC FUSB305)
                                                          - () */
  GUI_INIT_NBPORTMAX                         = 0x02, /*<! Indicate maximum number of ports which can be configured
                                                          in the device (value 1, 2 , 3) */
  GUI_INIT_FWVERSION                         = 0x03, /*<! 4 bytes for FW version + 4 bytes for Stack version */
  GUI_INIT_TYPECSPECREVISION                 = 0x04, /*<! Type-C revision supported by the port */
  GUI_INIT_PDSPECREVISION                    = 0x04, /*<! PD spec revision supported by the port */
  GUI_INIT_EXTENDEDMESSAGESUNCKUNKED         = 0x06, /*<! Supports extended messages */
  GUI_INIT_ACCESSORYSUPP                     = 0x07, /*<! Accessory supported or not */
  GUI_INIT_POWERACCESSORYDETECTION           = 0x08, /*<! It enables or disables powered accessory detection */
  GUI_INIT_POWERACCESSORYTRANSITION          = 0x09, /*<! It enables or disables transition from
                                                          Powered.accessory to Try.SNK */
  GUI_INIT_ISCABLE                           = 0x0B, /*<! Cable (asserts Ra on VConn, communicates using SOP'
                                                          and SOP''), when this mode is selected the stack will run
                                                          in cable mode so all parameters like PowerRole are not used */
  GUI_INIT_TRYFEATURE                        = 0x10, /*<! Try Feature  */
  GUI_INIT_RPRESISTORVALUE                   = 0x12, /*<! RP resistor value */
  GUI_INIT_USBSUPPORT                        = 0x13, /*<! USB_Comms_Capable, is the UUT capable of enumerating
                                                          as a USB host or device? */
  GUI_INIT_USBDEVICE                         = 0x14, /*<! Type_C_Can_Act_As_Device, Indicates whether the UUT can
                                                          communicate with USB 2.0 or USB 3.1 as a device or as
                                                          the Upstream Facing Port of a hub.  */
  GUI_INIT_USBHOST                           = 0x15, /*<! Type_C_Can_Act_As_Host, Indicates whether the UUT can
                                                          communicate with USB 2.0 or USB 3.1 as a host or
                                                          as the Downstream Facing Port of a hub */
  GUI_INIT_UNCONSTRAINED_POWERED             = 0x16, /*<! UUT has an external power source available that is sufficient
                                                          to adequately power the system while charging external
                                                          devices or the UUTs primary function is to charge
                                                          external devices. */
  GUI_INIT_USBSUSPENDSUPPORT                 = 0x17, /*<! USB Suspend support */
  GUI_INIT_VCONNDISCHARGE                    = 0x18, /*<! It enables or disables the VCONN discharge on CC pin */
  GUI_INIT_VCONNILIM                         = 0x19, /*<! enables or not VconnLim */
  GUI_INIT_VCONNILIMVALUE                    = 0x1A, /*<! It allows changing the default current limit supplying
                                                          VCONN on the CC pins */
  GUI_INIT_VCONNMONITORING                   = 0x1B, /*<! It enables or disables UVLO threshold detection
                                                          on VCONN pin */
  GUI_INIT_VCONNTHRESHOLDUVLO                = 0x1C, /*<! High UVLO threshold of 4.65 V; 1b: Low UVLO threshold
                                                          of 2.65 V (case of VCONN-powered accessories
                                                          operating down to 2.7 V) */
  GUI_INIT_VCONNSUPPLY                       = 0x1D, /*<! It enables or disables the VCONN supply
                                                          capability on CC pin  */
  GUI_INIT_NB_PORT_START                     = 0x1E, /*<! Start Port number */
  GUI_INIT_ORIGINAL_SETTINGS                 = 0x1F, /*<! This flag allows to know if the user settings read by
                                                          the FW are from the original FW or a previous configuration
                                                          saved by the user. */
} USBPD_GUI_Init;

typedef enum
{
  GUI_USER_EVENT_TIMER,         /* TIMER EVENT */
  GUI_USER_EVENT_GUI,           /* GUI EVENT */
  GUI_USER_EVENT_NONE,          /* NO EVENT */
} GUI_USER_EVENT;

/**
  * @brief  USBPD DPM handle Structure definition
  * @{
  */
typedef struct
{
  uint32_t              ListOfRcvSRCPDO[USBPD_MAX_NB_PDO];   /*!< The list of received Source Power Data Objects
                                                                  from Port partner (when Port partner is a Source
                                                                  or a DRP port). */
  uint32_t              NumberOfRcvSRCPDO;                   /*!< The number of received Source Power Data Objects
                                                                  from port Partner (when Port partner is a Source
                                                                  or a DRP port). This parameter must be set
                                                                  to a value lower than USBPD_MAX_NB_PDO */
  uint32_t              ListOfRcvSNKPDO[USBPD_MAX_NB_PDO];   /*!< The list of received Sink Power Data Objects
                                                                  from Port partner (when Port partner is a Sink
                                                                  or a DRP port). */
  uint32_t              NumberOfRcvSNKPDO;                   /*!< The number of received Sink Power Data Objects
                                                                  from port Partner(when Port partner is a Sink
                                                                  or a DRP port). This parameter must be set to
                                                                  a value lower than USBPD_MAX_NB_PDO */
  uint32_t              RDOPosition;                         /*!< RDO Position of requested DO in Source list
                                                                  of capabilities */
  uint32_t              RequestedVoltage;                    /*!< Value of requested voltage */
  uint32_t              RequestedCurrent;                    /*!< Value of requested current */
  uint32_t              RDOPositionPrevious;                 /*!< RDO Position of previous requested DO
                                                                  in Source list of capabilities */
  uint32_t              RcvRequestDOMsg;                     /*!< Received request Power Data Object message
                                                                  from the port Partner */
#if defined(USBPD_REV30_SUPPORT)
#if _STATUS
  USBPD_SDB_TypeDef     RcvStatus;                           /*!< Status received by port partner */
#endif /* _STATUS */
#if _PPS
  USBPD_PPSSDB_TypeDef  RcvPPSStatus;                        /*!< PPS Status received by port partner */
#endif /* _PPS */
#if _SRC_CAPA_EXT
  USBPD_SCEDB_TypeDef   RcvSRCExtendedCapa;                  /*!< SRC Extended Capability received by port partner */
#endif /* _SRC_CAPA_EXT */
#if defined(USBPDCORE_SNK_CAPA_EXT)
  USBPD_SKEDB_TypeDef   RcvSNKExtendedCapa;                  /*!< SNK Extended Capability received by port partner */
#endif /* USBPDCORE_SNK_CAPA_EXT */
#if _MANU_INFO
  USBPD_GMIDB_TypeDef   GetManufacturerInfo;                 /*!< Get Manufacturer Info */
#endif /* _MANU_INFO */
#if _BATTERY
  USBPD_GBSDB_TypeDef   GetBatteryStatus;                    /*!< Get Battery status */
  USBPD_GBCDB_TypeDef   GetBatteryCapability;                /*!< Get Battery Capability */
  USBPD_BSDO_TypeDef    BatteryStatus;                       /*!< Battery status */
#endif /* _BATTERY */
  USBPD_ADO_TypeDef     RcvAlert;                            /*!< Save the Alert received by port partner */
#endif /* USBPD_REV30_SUPPORT */
#ifdef _VCONN_SUPPORT
  USBPD_DiscoveryIdentity_TypeDef VDM_DiscoCableIdentify;                /*!< VDM Cable Discovery Identify */
#endif /* _VCONN_SUPPORT */
#ifdef _VDM
  USBPD_DiscoveryIdentity_TypeDef   VDM_DiscoIdentify;                   /*!< VDM Discovery Identify */
  USBPD_SVIDPortPartnerInfo_TypeDef VDM_SVIDPortPartner;                 /*!< VDM SVID list */
  USBPD_ModeInfo_TypeDef            VDM_ModesPortPartner;                /*!< VDM Modes list */
#endif /* _VDM */
} GUI_HandleTypeDef;

/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/** @defgroup USBPD_GUI_API_Private_Macros USBPD GUI API Private Macros
  * @{
  */
#define __GUI_SET_TAG_ID(_PORT_, _TAG_)  (((_PORT_) << GUI_PORT_BIT_POSITION) | (_TAG_))

#if defined(_RTOS)
#if (osCMSIS < 0x20000U)
#define GUI_START_TIMER(_PORT_,_TIMER_,_TIMEOUT_)   do{                                                             \
                                                        _TIMER_[_PORT_] = (_TIMEOUT_) |  GUI_TIMER_ENABLE_MSK;      \
                                                        osMessagePut(GUIMsgBox,GUI_USER_EVENT_TIMER, 0);            \
                                                      }while(0);
#else
#define GUI_START_TIMER(_PORT_,_TIMER_,_TIMEOUT_)   do{                                                             \
                                                        uint32_t eventtim = GUI_USER_EVENT_TIMER;                   \
                                                        _TIMER_[_PORT_] = (_TIMEOUT_) |  GUI_TIMER_ENABLE_MSK;      \
                                                        (void)osMessageQueuePut(GUIMsgBox, &eventtim, 0U, 0U);      \
                                                      }while(0);
#endif /* osCMSIS < 0x20000U */
#else
#define GUI_START_TIMER(_PORT_, _TIMER_,_TIMEOUT_)  _TIMER_[_PORT_] = (_TIMEOUT_) |  GUI_TIMER_ENABLE_MSK;
#endif /* _RTOS */

#define IS_GUI_TIMER_RUNNING(_PORT_, _TIMER_)       ((uint16_t)((_TIMER_)[(_PORT_)] & GUI_TIMER_READ_MSK) > 0)
#define IS_GUI_TIMER_EXPIRED(_PORT_, _TIMER_)       (GUI_TIMER_ENABLE_MSK == (_TIMER_)[(_PORT_)])

#if defined(_SNK) || defined(_DRP)
#define GUI_UPDATE_VOLTAGE_MIN(_PDO_VOLT_, _SNK_VOLT_) \
  if ((_PDO_VOLT_) < (_SNK_VOLT_)) \
  { \
    /* Update min voltage */ \
    (_SNK_VOLT_) = (_PDO_VOLT_); \
  }
#define GUI_UPDATE_VOLTAGE_MAX(_PDO_VOLT_, _SNK_VOLT_) \
  if ((_PDO_VOLT_) > (_SNK_VOLT_)) \
  { \
    /* Update min voltage */ \
    (_SNK_VOLT_) = (_PDO_VOLT_); \
  }
#define GUI_UPDATE_CURRENT_MAX(_PDO_CURRENT_, _SNK_CURRENT_) \
  if ((_PDO_CURRENT_) > (_SNK_CURRENT_)) \
  { \
    /* Update min current */ \
    (_SNK_CURRENT_) = (_PDO_CURRENT_); \
  }
#define GUI_UPDATE_POWER_MAX(_PDO_POWER_, _SNK_POWER_) \
  if ((_PDO_POWER_) > (_SNK_POWER_)) \
  { \
    /* Update min POWER */ \
    (_SNK_POWER_) = (_PDO_POWER_); \
  }
#define GUI_CHECK_VOLTAGE_MIN(_PDO_VOLT_, _SNK_VOLT_) \
  /* Update min voltage */ \
  (_SNK_VOLT_) = (_PDO_VOLT_);
#define GUI_CHECK_VOLTAGE_MAX(_PDO_VOLT_, _SNK_VOLT_) \
  /* Update min voltage */ \
  (_SNK_VOLT_) = (_PDO_VOLT_);
#define GUI_CHECK_CURRENT_MAX(_PDO_CURRENT_, _SNK_CURRENT_) \
  /* Update min current */ \
  (_SNK_CURRENT_) = (_PDO_CURRENT_);
#define GUI_CHECK_POWER_MAX(_PDO_POWER_, _SNK_POWER_) \
  /* Update min POWER */ \
  (_SNK_POWER_) = (_PDO_POWER_);

#define GUI_DECODE_50MV(_Value_)           ((uint16_t)(((_Value_) * 50U)))     /* From 50mV multiples to mV     */
#define GUI_DECODE_100MV(_Value_)          ((uint16_t)(((_Value_) * 100U)))    /* From 100mV multiples to mV    */
#define GUI_DECODE_10MA(_Value_)           ((uint16_t)(((_Value_) * 10U)))     /* From 10mA multiples to mA     */
#define GUI_DECODE_50MA(_Value_)           ((uint16_t)(((_Value_) * 50U)))     /* From 50mA multiples to mA     */
#define GUI_DECODE_MW(_Value_)             ((uint16_t)(((_Value_) * 250U)))    /* From 250mW multiples to mW    */
#endif /* _SNK) || _DRP */

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup USBPD_GUI_API_Private_Variables USBPD GUI API Private Variables
  * @{
  */
/*!< Timer used to send measurement report*/
__IO uint16_t GUI_TimerMeasReport[USBPD_PORT_COUNT];

extern USBPD_SettingsTypeDef DPM_Settings[USBPD_PORT_COUNT];
#ifdef _VDM
extern USBPD_VDM_SettingsTypeDef  DPM_VDM_Settings[USBPD_PORT_COUNT];
#endif /* _VDM */
GUI_USER_ParamsTypeDef GUI_USER_Params[USBPD_PORT_COUNT];

uint8_t PtrDataRx[TLV_SIZE_MAX];
uint8_t Processed[TLV_SIZE_MAX];

USBPD_GUI_State GUI_State = GUI_STATE_INIT;
/*!< Flag to indicate that user settings is from original FW (not saved into the FLASH) */
uint8_t GUI_OriginalSettings;

#if defined(_RTOS)
osMessageQId  GUIMsgBox;
#if (osCMSIS >= 0x20000U)
osThreadAttr_t GUI_Thread_Atrr =
{
  .name       = "GUI",
  .priority   = FREERTOS_GUI_PRIORITY, /*osPriorityLow,*/
  .stack_size = FREERTOS_GUI_STACK_SIZE
};
#endif /* osCMSIS >= 0x20000U */
#else
__IO uint32_t GUI_Flag = GUI_USER_EVENT_NONE;
#endif /* _RTOS */

const uint8_t *(*pCB_HWBoardVersion)(void)  = NULL;
const uint8_t *(*pCB_HWPDType)(void)        = NULL;
uint16_t (*pCB_GetVoltage)(uint8_t)         = NULL;
int16_t (*pCB_GetCurrent)(uint8_t)         = NULL;
USBPD_StatusTypeDef(*pCB_FreeText)(uint8_t, uint8_t *, uint16_t) = NULL;

GUI_HandleTypeDef GUI_SaveInformation[USBPD_PORT_COUNT];

/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/
/** @defgroup USBPD_GUI_API_Private_Functions GUI API Private Functions
  * @{
  */
#ifdef _RTOS
#if (osCMSIS < 0x20000U)
static void      TaskGUI(void const *argument);
#else
static void      TaskGUI(void *argument);
#endif /* osCMSIS < 0x20000U */
static uint32_t  CheckGUITimers(void);
#endif /* _RTOS */
static void      GUI_CALLBACK_RX(uint8_t Character, uint8_t Error);
static void      Send_DpmInitCnf(uint8_t PortNum, uint8_t *string);
static void      Request_MessageReq(uint8_t PortNum, uint8_t *instruction, uint8_t *pEncodedMsg);
static void      Send_DpmConfigSetCnf(uint8_t PortNum, uint8_t *instruction, uint8_t *pEncodedMsg);
static void      Send_DpmConfigGetCnf(uint8_t PortNum, uint8_t *instruction, uint8_t *pEncodedMsg);
static void      Send_DpmConfigGetRej(uint8_t PortNum, uint8_t *pEncodedMsg, USBPD_GUI_Reject_Reason RejecReason);
static void      Send_DpmMessageRej(uint8_t PortNum, uint8_t *pEncodedMsg, USBPD_GUI_Reject_Reason RejecReason);
static void      Send_DpmRegisterReadCnf(uint8_t PortNum, uint8_t *string, uint8_t param);
static void      Send_DpmRegisterWriteCnf(uint8_t PortNum, uint8_t *string, uint8_t *toProcess);
static USBPD_StatusTypeDef Manage_FreeText(uint8_t PortNum, uint8_t *pPayload, uint16_t Size);
#if defined(_SNK) || defined(_DRP)
static void      UpdateSNKPowerPort0(void);
#if USBPD_PORT_COUNT==2
static void      UpdateSNKPowerPort1(void);
#endif /* USBPD_PORT_COUNT==2 */
#endif /* _SNK) || _DRP */
#if defined(USE_STM32_UTILITY_OS)
void                  GUI_Execute(void);
#endif /* USE_STM32_UTILITY_OS */

/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */
/** @addtogroup USBPD_GUI_API_Exported_Functions
  * @{
  */

/**
  * @brief  GUI initialization function
  * @param  CB_HWBoardVersion BSP callback to retrieve HW Board version
  * @param  CB_HWPDType       BSP callback to retrieve HW PD Type
  * @param  CB_GetVoltage     BSP callback to retrieve VBUS voltage
  * @param  CB_GetCurrent     BSP callback to retrieve IBUS current
  * @retval Functional state
  */
USBPD_FunctionalState GUI_Init(const uint8_t *(*CB_HWBoardVersion)(void), const uint8_t *(*CB_HWPDType)(void),
                               uint16_t (*CB_GetVoltage)(uint8_t), int16_t (*CB_GetCurrent)(uint8_t))
{
  USBPD_FunctionalState _status = USBPD_ENABLE;
  /* Retrieve data from FLASH if needed */
  GUI_OriginalSettings = ((GUI_OK == BSP_GUI_LoadDataFromFlash()) ? USBPD_FALSE : USBPD_TRUE);

  /* Need to update CAD_tDRP & CAD_dcSRC_DRP if CAD_SNKToggleTime not initialized */
  if ((0U == DPM_Settings[USBPD_PORT_0].CAD_SNKToggleTime) || (0U == DPM_Settings[USBPD_PORT_0].CAD_SRCToggleTime))
  {
    DPM_USER_Settings[USBPD_PORT_0].CAD_tDRP      = 80U;
    DPM_USER_Settings[USBPD_PORT_0].CAD_dcSRC_DRP = 50U;
  }
  else
  {
    DPM_USER_Settings[USBPD_PORT_0].CAD_dcSRC_DRP = (DPM_Settings[USBPD_PORT_0].CAD_SRCToggleTime * 100U) /
                                                    (DPM_Settings[USBPD_PORT_0].CAD_SRCToggleTime +
                                                     DPM_Settings[USBPD_PORT_0].CAD_SNKToggleTime);
    DPM_USER_Settings[USBPD_PORT_0].CAD_tDRP = (DPM_Settings[USBPD_PORT_0].CAD_SRCToggleTime * 100U) /
                                               DPM_USER_Settings[USBPD_PORT_0].CAD_dcSRC_DRP;
  }
#if USBPD_PORT_COUNT==2
  if ((0U == DPM_Settings[USBPD_PORT_1].CAD_SNKToggleTime) || (0U == DPM_Settings[USBPD_PORT_1].CAD_SRCToggleTime))
  {
    DPM_USER_Settings[USBPD_PORT_1].CAD_tDRP      = 80U;
    DPM_USER_Settings[USBPD_PORT_1].CAD_dcSRC_DRP = 50U;
  }
  else
  {
    DPM_USER_Settings[USBPD_PORT_1].CAD_dcSRC_DRP = (DPM_Settings[USBPD_PORT_1].CAD_SRCToggleTime * 100U) /
                                                    (DPM_Settings[USBPD_PORT_1].CAD_SRCToggleTime +
                                                     DPM_Settings[USBPD_PORT_1].CAD_SNKToggleTime);
    DPM_USER_Settings[USBPD_PORT_1].CAD_tDRP = (DPM_Settings[USBPD_PORT_1].CAD_SRCToggleTime * 100U) /
                                               DPM_USER_Settings[USBPD_PORT_1].CAD_dcSRC_DRP;
  }
#endif /* USBPD_PORT_COUNT==2 */


  pCB_HWBoardVersion  = CB_HWBoardVersion;
  pCB_HWPDType        = CB_HWPDType;
  pCB_GetVoltage      = CB_GetVoltage;
  pCB_GetCurrent      = CB_GetCurrent;

  /* Register 2 callbacks for notification in DPM */
  USBPD_DPM_SetNotification_GUI(GUI_FormatAndSendNotification, GUI_PostNotificationMessage, GUI_SaveInfo);

#if defined(_RTOS)
#if (osCMSIS < 0x20000U)
  osMessageQDef(MsgBox, GUI_BOX_MESSAGES_MAX, uint32_t);
  osThreadDef(GUI, TaskGUI, FREERTOS_GUI_PRIORITY, 0, FREERTOS_GUI_STACK_SIZE);
  GUIMsgBox = osMessageCreate(osMessageQ(MsgBox), NULL);
  if (NULL == osThreadCreate(osThread(GUI), &GUIMsgBox))
#else
  GUIMsgBox = osMessageQueueNew(GUI_BOX_MESSAGES_MAX, sizeof(uint32_t), NULL);
  if (NULL == osThreadNew(TaskGUI, &GUIMsgBox, &GUI_Thread_Atrr))
#endif /* osCMSIS < 0x20000U */
  {
    _status = USBPD_DISABLE;
  }
  /* Enable IRQ which has been disabled by FreeRTOS services */
  __enable_irq();
#else /* RTOS */
  GUI_Start();
#if defined(USE_STM32_UTILITY_OS)
  UTIL_SEQ_RegTask(TASK_GUI, 0, GUI_Execute);
  UTIL_SEQ_SetTask(TASK_GUI, 0);
#endif /*USE_STM32_UTILITY_OS */
#endif /* _RTOS */

  return _status;
}

void GUI_Start(void)
{
  /* register the GUI callback to manage UART reception */
  TRACER_EMB_StartRX(GUI_CALLBACK_RX);
}

/**
  * @brief  GUI Task
  * @param  pEvent  GUI User event
  * @retval None
  */
#ifdef _RTOS
#if (osCMSIS < 0x20000U)
static void TaskGUI(void const *pEvent)
#else
static void TaskGUI(void *pEvent)
#endif /* osCMSIS < 0x20000U */
#else
void GUI_Execute(void)
#endif /* _RTOS */
{
#ifdef _RTOS
  uint32_t _timing = osWaitForever;
  osMessageQId  queue = *(osMessageQId *)pEvent;

  GUI_Start();
  do
  {
#if (osCMSIS < 0x20000U)
    osEvent event = osMessageGet(queue, _timing);
    switch (((GUI_USER_EVENT)event.value.v & 0xFU))
#else
    uint32_t event;
    (void)osMessageQueueGet(queue, &event, NULL, _timing);
    switch ((GUI_USER_EVENT)(event & 0xFU))
#endif /* osCMSIS < 0x20000U */
    {
      case GUI_USER_EVENT_GUI:
      {
#if (osCMSIS < 0x20000U)
        (void)GUI_RXProcess((uint32_t)event.value.v);
#else
        (void)GUI_RXProcess((uint32_t)event);
#endif /* osCMSIS < 0x20000U */
        /* Sent an event to check if measurement report has been requested */
#if (osCMSIS < 0x20000U)
        (void)osMessagePut(GUIMsgBox, GUI_USER_EVENT_TIMER, 0U);
#else
        uint32_t eventtim = (uint32_t)GUI_USER_EVENT_TIMER;
        (void)osMessageQueuePut(GUIMsgBox, &eventtim, 0U, 0U);
#endif /* osCMSIS < 0x20000U */
        break;
      }
      case GUI_USER_EVENT_TIMER:
      {
#endif /* _RTOS */
#ifndef _RTOS
        switch (GUI_Flag & 0xF)
        {
          case GUI_USER_EVENT_NONE:
            break;
          case GUI_USER_EVENT_GUI:
            GUI_RXProcess(GUI_Flag);
          case GUI_USER_EVENT_TIMER:
            break;
          default:
            break;
        }
        GUI_Flag = GUI_USER_EVENT_NONE;
#endif /* !_RTOS */
        for (uint8_t _instance = 0; _instance < (uint8_t)USBPD_PORT_COUNT; _instance++)
        {
          /* -------------------------------------------------  */
          /* Check if timeout related to Measurement reporting  */
          /* -------------------------------------------------  */
          /* - Send a GUI Event only if PE is connected
             and Measurement report has been enabled          */
          if ((USBPD_TRUE == DPM_Params[_instance].PE_IsConnected)
              && (1U == GUI_USER_Params[_instance].u.d.MeasReportActivation)
              && (0U != GUI_USER_Params[_instance].u.d.MeasReportValue))
          {
            /* Check if timer has expired */
            if (IS_GUI_TIMER_EXPIRED(_instance, GUI_TimerMeasReport))
            {
              uint32_t event_mr = GUI_USER_EVENT_GUI
                                  | (_instance << GUI_PE_PORT_NUM_Pos)
                                  | (GUI_NOTIF_MEASUREMENT << GUI_PE_NOTIF_Pos);
              (void)GUI_RXProcess(event_mr);
            }
            /* Start or Restart Measurement report timer */
            if (!(IS_GUI_TIMER_RUNNING(_instance, GUI_TimerMeasReport)))
            {
              GUI_START_TIMER(_instance, GUI_TimerMeasReport,
                              (uint16_t)(GUI_USER_Params[_instance].u.d.MeasReportValue) * GUI_NOTIF_MEASUREMENT_STEP);
            }
          }
          else
          {
            /* Stop measurement report timer */
            GUI_TimerMeasReport[_instance] = 0;
          }
        }
#ifdef _RTOS
        break;
      }

      default:
        break;
    }
    _timing = CheckGUITimers();
  } while (1 == 1);
#endif /* !_RTOS */
}

#ifdef _RTOS
static uint32_t CheckGUITimers(void)
{
  uint32_t _timing = osWaitForever;
  uint32_t _current_timing;

  /* Calculate the minimum timers to wake-up GUI task */
  for (uint8_t instance = 0; instance < (uint8_t)USBPD_PORT_COUNT; instance++)
  {
    /* Check if Measurement reporting has been enabled by the GUI */
    _current_timing = (uint32_t)(GUI_TimerMeasReport[instance]) & GUI_TIMER_READ_MSK;
    if (_current_timing > 0U)
    {
      if (_current_timing < _timing)
      {
        _timing = _current_timing;
      }
    }
  }
  return _timing;
}
#endif /* !_RTOS */

void GUI_TimerCounter(void)
{
  /* Increment GUI timer only if Measurement report has been activated */
  if (1U == GUI_USER_Params[USBPD_PORT_0].u.d.MeasReportActivation)
  {
    if ((GUI_TimerMeasReport[USBPD_PORT_0] & GUI_TIMER_READ_MSK) > 0U)
    {
      GUI_TimerMeasReport[USBPD_PORT_0]--;
    }
#if !defined(_RTOS)&&defined(USE_STM32_UTILITY_OS)
    else
    {
      UTIL_SEQ_SetTask(TASK_GUI, 0);
    }
#endif /* !_RTOS && USE_STM32_UTILITY_OS */
  }
#if USBPD_PORT_COUNT==2
  if (1 == GUI_USER_Params[USBPD_PORT_1].u.d.MeasReportActivation)
  {
    if ((GUI_TimerMeasReport[USBPD_PORT_1] & GUI_TIMER_READ_MSK) > 0U)
    {
      GUI_TimerMeasReport[USBPD_PORT_1]--;
    }
#if !defined(_RTOS)&&defined(USE_STM32_UTILITY_OS)
    else
    {
      UTIL_SEQ_SetTask(TASK_GUI, 0);
    }
#endif /* !_RTOS && USE_STM32_UTILITY_OS */
  }
#endif /* USBPD_PORT_COUNT == 2 */
}

/**
  * @brief  callback called to end a transfer.
  * @param  Character Byte received by the device
  * @param  Error     Error detected in the reception
  * @retval 1 if message to send to DPM application
  */
void GUI_CALLBACK_RX(uint8_t Character, uint8_t Error)
{
  uint32_t event;
  __disable_irq();
  event = GUI_GetMessage(Character, Error);
  __enable_irq();
  if (event == 1U)
  {
#if defined(_RTOS)
#if (osCMSIS < 0x20000U)
    (void)osMessagePut(GUIMsgBox, GUI_USER_EVENT_GUI, 1);
#else
    uint32_t eventgui = (uint32_t)GUI_USER_EVENT_GUI;
    (void)osMessageQueuePut(GUIMsgBox, &eventgui, 0U, 0U);
#endif /* osCMSIS < 0x20000U */
#else
    GUI_Flag = GUI_USER_EVENT_GUI;
#if defined(USE_STM32_UTILITY_OS)
    GUI_RXProcess(GUI_Flag);
    GUI_Flag = GUI_USER_EVENT_NONE;
#endif /* USE_STM32_UTILITY_OS */
#endif /* _RTOS */
  }
}

/**
  * @brief  Main Trace RX process to push data on the media.
  * @param  Event     GUI trace event
  * @retval Timing
  */
uint32_t GUI_RXProcess(uint32_t Event)
{
  uint8_t *msg;
  USBPD_GUI_State state;
  uint8_t size;

  if (0U == (Event & GUI_PE_NOTIF_Msk))
  {
    /* Message have been received by GUI */
    state = GUI_SendAnswer(&msg, &size);

    switch (state)
    {
      case GUI_STATE_RESET:
        /* DPM_RESET_REQ received by the device */
        /* Save parameters in FLASH */
        (void)BSP_GUI_SaveDataInFlash();
        HAL_NVIC_SystemReset();
        break;
      case GUI_STATE_INIT:
        /* DPM_INIT_REQ received by the device */
        break;
      default:
        /* Another GUI messages received by the device */
        TRACER_EMB_Add(msg, size);
        break;
    }
  }
  else
  {
    uint16_t type_event = ((uint16_t)Event & GUI_PE_NOTIF_Msk) >> GUI_PE_NOTIF_Pos;
    if (GUI_NOTIF_MEASUREMENT == type_event)
    {
      /* Notification related to timeout for measure reporting */
      /* Send a notification to associated port */
      (void)GUI_FormatAndSendNotification(((Event & GUI_PE_PORT_NUM_Msk) >> GUI_PE_PORT_NUM_Pos),
                                          GUI_NOTIF_MEASUREREPORTING,
                                          HAL_GetTick());
    }
    else
    {
      /* Message have been received by PE */
      /* Send a notification to associated port */
      (void)GUI_FormatAndSendNotification(((Event & GUI_PE_PORT_NUM_Msk) >> GUI_PE_PORT_NUM_Pos),
                                          GUI_NOTIF_PE_EVENT,
                                          type_event);
    }
  }
  return 0;
}

/**
  * @brief  TRACE function to send a notification
  * @param  PortNum           Index of current used port
  * @param  TypeNotification  Type of requested notification is a combination
  *                           of @ref USBPD_TRACE_TRACE_TYPE_NOTIFICATION
  * @param  Value             Value depending of TypeNotification
  * @note   If TypeNotification == GUI_NOTIF_ISCONNECTED, Value should be equal to 0 (Not connected) or 1 (connected)
  * @retval USBPD Status
  */
uint32_t GUI_FormatAndSendNotification(uint32_t PortNum, uint32_t TypeNotification, uint32_t Value)
{
  uint8_t *msg;
  uint8_t size;
  /* Only send notification if GUI is connected */
  if (GUI_STATE_INIT != GUI_SendNotification((uint8_t)PortNum, &msg, &size, TypeNotification, Value))
  {
    TRACER_EMB_Add(msg, size);
  }
  return 0;
}


/**
  * @brief  Function called through UART IT RX to fill the GUI RX buffer
  * @param  Character Received byte
  * @param  Error     Error if issue during reception
  * @retval 1 indicates that a complete GUI message have been received
  */
uint32_t GUI_GetMessage(uint8_t Character, uint8_t Error)
{
  uint32_t status = 0;
  static uint16_t counter = 0U;   /* Allows for example to count the number of SOF or EOF we received consecutively */
  static uint16_t cursor = 0U;    /* Our position in the buffer */
  static uint16_t currentSize;    /* How many bytes we should expect */
  static enum receptionStateMachine
  {
    Rstart,
    RonGoing,
    Rsize,
    Rend,
    Rerror
  } Rstate = Rstart;

  if (cursor < TLV_SIZE_MAX)
  {
    PtrDataRx[cursor] = Character;
  }
  else /* buffer too small, overflow */
  {
    cursor = 0U;
    counter = 0U;
    Rstate = Rstart;
    return 0;
  }

  if (Error != 0U)
  {
    Rstate = Rerror;
  }

  switch (Rstate)
  {
    case Rstart :
      if (PtrDataRx[cursor] == TLV_SOF)
      {
        counter++;
      }
      else
      {
        counter = 0U;
        /* cursor is unsigned, but it will inevitably increments at the end of function,
           and we need a 0 at the next pass here */
        cursor = 0xFFFFU;
      }

      if (counter == 4U) /* The whole SOF is received */
      {
        counter = 0U;
        Rstate = Rsize;
      }
      break;
    case Rsize :
      counter++;

      if (counter == 3U) /* We have received the size */
      {
        currentSize = (uint16_t)((PtrDataRx[cursor - 1U]) << 8) + (uint16_t)(PtrDataRx[cursor]);
        if (0U == currentSize)
        {
          Rstate = Rend;
        }
        else
        {
          Rstate = RonGoing;
        }
        counter = 0U;
      }
      break;
    case RonGoing :
      counter++;

      if (counter == currentSize)
      {
        /* When there is no value, the first EOF is handled by Rongoing, before Rend takes control */
        counter = 0U;
        Rstate = Rend;
      }
      break;
    case Rend :
      counter++;
      if ((PtrDataRx[cursor] == TLV_EOF)
          && (PtrDataRx[cursor - 1U] == TLV_EOF)
          && (PtrDataRx[cursor - 2U] == TLV_EOF)
          && (PtrDataRx[cursor - 3U] == TLV_EOF))
      {
        /* The semaphore must be given only in this case, because otherwise
           it means we didn't receive the correct size of bytes */
        if (counter == 4U)
        {
          status = 1U;
        }
        counter = 0U;
        cursor = 0xFFFFU;
        Rstate = Rstart;
      }

      if (cursor == (currentSize + 11U))
      {
        /* No complete EOF arrived. We reset the buffer for safety even if the instruction might be complete. */
        counter = 0U;
        cursor = 0xFFFFU;
        Rstate = Rstart;
      }
      break;
    case Rerror :
      counter = 0U;
      cursor = 0xFFFFU;
      Rstate = Rstart;
      break;
    default:
      break;
  }

  cursor++;
  return status;
}

/**
  * @brief  Answer to a message received by the GUI
  * @param  pMsgToSend Pointer on the message to send
  * @param  pSizeMsg   Pointer on the size of the message to send
  * @retval GUI state
  */
USBPD_GUI_State GUI_SendAnswer(uint8_t **pMsgToSend, uint8_t *pSizeMsg)
{
  /* Extract the port from the tag*/
  uint8_t port = PtrDataRx[TLV_TAG_POSITION] >> GUI_PORT_BIT_POSITION;

  /* Do the appropriate treatment in response to what we have received */
  switch (PtrDataRx[TLV_TAG_POSITION] & 0x1FU)
  {
    case DPM_RESET_REQ:
      /* Reset*/
      return GUI_STATE_RESET;

    case DPM_INIT_REQ:
    {
      Send_DpmInitCnf(port, Processed);
      *pMsgToSend = Processed;
      *pSizeMsg = (uint8_t)(TLV_get_string_length(Processed) + 8U);
      TRACER_EMB_Add(*pMsgToSend, *pSizeMsg);
      GUI_State = GUI_STATE_RUNNING;
      if (0U == port)
      {
        /* Send a notification all the port */
        (void)GUI_FormatAndSendNotification(USBPD_PORT_0,
                                            GUI_NOTIF_ISCONNECTED | GUI_NOTIF_PE_EVENT | GUI_NOTIF_TIMESTAMP,
                                            USBPD_NOTIFY_ALL);
#if USBPD_PORT_COUNT==2
        (void)GUI_FormatAndSendNotification(USBPD_PORT_1,
                                            GUI_NOTIF_ISCONNECTED | GUI_NOTIF_PE_EVENT | GUI_NOTIF_TIMESTAMP,
                                            USBPD_NOTIFY_ALL);
#endif /* USBPD_PORT_COUNT == 2 */
      }
      return GUI_STATE_INIT;
    }

    case DPM_CONFIG_GET_REQ:
      if (0U != port)
      {
        Send_DpmConfigGetCnf((port - 1U), PtrDataRx, Processed);
      }
      else
      {
        Send_DpmConfigGetRej(port, Processed, GUI_REJ_DPM_INVALID_PORT_NUMBER);
      }
      break;
    case DPM_CONFIG_SET_REQ:
      if (0U != port)
      {
        Send_DpmConfigSetCnf((port - 1U), PtrDataRx, Processed);
      }
      else
      {
        Send_DpmConfigGetRej(port, Processed, GUI_REJ_DPM_INVALID_PORT_NUMBER);
      }
      break;
    case DPM_MESSAGE_REQ:
      if (0U != port)
      {
        Request_MessageReq((port - 1U), PtrDataRx, Processed);
      }
      else
      {
        Send_DpmMessageRej(port, Processed, GUI_REJ_DPM_INVALID_PORT_NUMBER);
      }
      break;
    case DPM_REGISTER_READ_REQ:
      if (0U != port)
      {
        /* If size is 0*/
        if ((PtrDataRx[TLV_LENGTH_HIGH_POSITION] == 0U) && (PtrDataRx[TLV_LENGTH_LOW_POSITION] == 0U))
        {
          Send_DpmRegisterReadCnf((port - 1U), Processed, 0xFF);
        }
        else
        {
          /* PtrDataRx[TLV_VALUE_POSITION] is the tag in parameter,
             when only one parameter is needed (cf USBPD GUI specification)*/
          Send_DpmRegisterReadCnf((port - 1U), Processed, PtrDataRx[TLV_VALUE_POSITION]);
        }
      }
      else
      {
        Send_DpmConfigGetRej(port, Processed, GUI_REJ_DPM_INVALID_PORT_NUMBER);
      }
      break;
    case DPM_REGISTER_WRITE_REQ:
      if (0U != port)
      {
        Send_DpmRegisterWriteCnf((port - 1U), Processed, PtrDataRx);
      }
      else
      {
        Send_DpmConfigGetRej(port, Processed, GUI_REJ_DPM_INVALID_PORT_NUMBER);
      }
      break;
    default :
      break;
  }

  *pMsgToSend = Processed;
  *pSizeMsg = (uint8_t)(TLV_get_string_length(Processed) + 8U);
  return GUI_State;
}

/**
  * @brief  Answer to a message received by the GUI
  * @param  PortNum           Port number
  * @param  pMsgToSend        Pointer on the message to send
  * @param  pSizeMsg          Pointer on the size of the message to send
  * @param  TypeNotification  Type of the notification
  * @param  Value             Value of the notification
  * @retval GUI state
  */
USBPD_GUI_State GUI_SendNotification(uint8_t PortNum, uint8_t **pMsgToSend, uint8_t *pSizeMsg,
                                     uint32_t TypeNotification, uint32_t Value)
{
  TLV_ToSend_Data_t send_tlv;
  USBPD_GUI_State gui_state = GUI_STATE_INIT;

  /* Send a notitification only if GUI is running */
  if (GUI_State == GUI_STATE_RUNNING)
  {
    (void)TLV_init_encode(&send_tlv, __GUI_SET_TAG_ID((PortNum + 1U), DPM_MESSAGE_IND), TLV_SIZE_MAX, Processed);

    /* Check PD connection */
    if ((TypeNotification & GUI_NOTIF_ISCONNECTED) == GUI_NOTIF_ISCONNECTED)
    {
      /* Is Connected*/
      (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_ISCONNECTED, 1,
                    (uint8_t[]){ DPM_Params[PortNum].PE_Power });
      if (USBPD_TRUE == DPM_Params[PortNum].PE_IsConnected)
      {
        uint8_t rp_value;
        /* CC line */
        (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_CC, 1,
                      (uint8_t[]) { DPM_Params[PortNum].ActiveCCIs });
        /* Power Role*/
        (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_POWERROLE, 1,
                      (uint8_t[]) { DPM_Params[PortNum].PE_PowerRole });
        /* CC Default Current Advertised */
        rp_value = 3; /* (uint8_t)CAD_GetRPValue(PortNum); */
        (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_CCDEFAULTCURRENTADVERTISED, 1, &rp_value);
        if (USBPD_POWER_EXPLICITCONTRACT == DPM_Params[PortNum].PE_Power)
        {
          /* Data Role*/
          (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_DATAROLE, 1,
                        (uint8_t[]) { DPM_Params[PortNum].PE_DataRole });
          /* Vconn ON*/
          (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_VCONNON, 1,
                        (uint8_t[]) { DPM_Params[PortNum].VconnStatus });
          /* PD_Spec Revision */
          (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_PD_SPECREVISION, 1,
                        (uint8_t[]) { DPM_Params[PortNum].PE_SpecRevision });
        }
      }
    }

    /* Check PD message event */
    if ((TypeNotification & GUI_NOTIF_PE_EVENT) == GUI_NOTIF_PE_EVENT)
    {
      if ((uint32_t)USBPD_NOTIFY_ALL != Value)
      {
        /* PD_MessageNotif */
        (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_PD_MESSAGENOTIF, 1, (uint8_t *)&Value);

        switch (Value)
        {
          case USBPD_NOTIFY_POWER_STATE_CHANGE :
            /* Is Connected*/
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_ISCONNECTED, 1,
                          (uint8_t[]) { DPM_Params[PortNum].PE_Power });
            if (USBPD_POWER_NO == DPM_Params[PortNum].PE_Power)
            {
              /* CC line */
              (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_CC, 1,
                            (uint8_t[]) { DPM_Params[PortNum].ActiveCCIs });
              /* PowerRole*/
              (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_POWERROLE, 1,
                            (uint8_t[]) { DPM_Params[PortNum].PE_PowerRole });
            }
            break;
          case USBPD_NOTIFY_GETSNKCAP_ACCEPTED :
            /* NumberOfRcvSNKPDO */
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_NUMBEROFRCVSNKPDO, 1,
                          (uint8_t[]) { GUI_SaveInformation[PortNum].NumberOfRcvSNKPDO
            });
            /* ListOfRcvSNKPDO*/
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_LISTOFRCVSNKPDO,
                          (uint16_t)(GUI_SaveInformation[PortNum].NumberOfRcvSNKPDO * 4U),
                          (uint8_t *) GUI_SaveInformation[PortNum].ListOfRcvSNKPDO);
            break;
          case USBPD_NOTIFY_GETSRCCAP_ACCEPTED :
            /* NumberOfRcvSRCPDO*/
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_NUMBEROFRCVSRCPDO, 1,
                          (uint8_t[]) { GUI_SaveInformation[PortNum].NumberOfRcvSRCPDO });
            /* ListOfRcvSRCPDO*/
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_LISTOFRCVSRCPDO,
                          (uint16_t)(GUI_SaveInformation[PortNum].NumberOfRcvSRCPDO * 4U),
                          (uint8_t *) GUI_SaveInformation[PortNum].ListOfRcvSRCPDO);
            break;
          case USBPD_NOTIFY_POWER_EXPLICIT_CONTRACT :
            if (USBPD_PORTPOWERROLE_SNK == DPM_Params[PortNum].PE_PowerRole)
            {
              /* NumberOfRcvSRCPDO*/
              (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_NUMBEROFRCVSRCPDO, 1,
                            (uint8_t[]) { GUI_SaveInformation[PortNum].NumberOfRcvSRCPDO });
              /* ListOfRcvSRCPDO*/
              (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_LISTOFRCVSRCPDO,
                            (uint16_t)(GUI_SaveInformation[PortNum].NumberOfRcvSRCPDO * 4U),
                            (uint8_t *) GUI_SaveInformation[PortNum].ListOfRcvSRCPDO);
            }
            /* RDOPosition */
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_RDOPOSITION, 1,
                          (uint8_t[]) { GUI_SaveInformation[PortNum].RDOPosition });
            /* DataRole*/
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_DATAROLE, 1,
                          (uint8_t[]) { DPM_Params[PortNum].PE_DataRole });
            /* PowerRole*/
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_POWERROLE, 1,
                          (uint8_t[]) { DPM_Params[PortNum].PE_PowerRole });
            /* VconnON*/
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_VCONNON, 1,
                          (uint8_t[]) { DPM_Params[PortNum].VconnStatus });
            /* PD_SpecRevision */
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_PD_SPECREVISION, 1,
                          (uint8_t[]) { DPM_Params[PortNum].PE_SpecRevision });
            break;
          case USBPD_NOTIFY_POWER_SWAP_TO_SNK_DONE :
          case USBPD_NOTIFY_POWER_SWAP_TO_SRC_DONE :
            /* PowerRole*/
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_POWERROLE, 1,
                          (uint8_t[]) { DPM_Params[PortNum].PE_PowerRole });
            break;
          case USBPD_NOTIFY_DATAROLESWAP_UFP :
          case USBPD_NOTIFY_DATAROLESWAP_DFP :
            /* DataRole*/
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_DATAROLE, 1,
                          (uint8_t[]) { DPM_Params[PortNum].PE_DataRole });
            break;
          case USBPD_NOTIFY_PD_SPECIFICATION_CHANGE :
            /* PD_SpecRevision */
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_PD_SPECREVISION, 1,
                          (uint8_t[]) { DPM_Params[PortNum].PE_SpecRevision });
            break;
          case USBPD_NOTIFY_VCONN_SWAP_COMPLETE :
            /* VconnON */
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_VCONNON, 1,
                          (uint8_t[]) { DPM_Params[PortNum].VconnStatus });
            break;
#if _PPS
          case USBPD_NOTIFY_PPS_STATUS_RECEIVED :
            /* PPS value */
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_PPS, 4,
                          (uint8_t *)&GUI_SaveInformation[PortNum].RcvPPSStatus.d32);
            break;
#endif /* _PPS */
#if _STATUS
          case USBPD_NOTIFY_STATUS_RECEIVED :
          {
            uint8_t tab[5] =
            {
              GUI_SaveInformation[PortNum].RcvStatus.InternalTemp,
              GUI_SaveInformation[PortNum].RcvStatus.PresentInput,
              GUI_SaveInformation[PortNum].RcvStatus.PresentBatteryInput,
              GUI_SaveInformation[PortNum].RcvStatus.EventFlags,
              GUI_SaveInformation[PortNum].RcvStatus.TemperatureStatus
            };
            /* Status value */
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_STATUS, 5, tab);
            break;
          }
#endif /* _STATUS */
#ifdef _VDM
          case USBPD_NOTIFY_VDM_IDENTIFY_RECEIVED :
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_VDM_IDENTITY, 4,
                          (uint8_t *)&GUI_SaveInformation[PortNum].VDM_DiscoIdentify.IDHeader.d32);
            break;
          case USBPD_NOTIFY_VDM_SVID_RECEIVED :
          {
            uint32_t index;
            uint16_t size;
            /* List of received SVDM SVID */
            if (0 != GUI_SaveInformation[PortNum].VDM_SVIDPortPartner.NumSVIDs)
            {
              (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_SVDM_SVIDS, 2,
                            (uint8_t *)&GUI_SaveInformation[PortNum].VDM_SVIDPortPartner.SVIDs[0]);
              size = 2;
              for (index = 1; index < GUI_SaveInformation[PortNum].VDM_SVIDPortPartner.NumSVIDs; index++)
              {
                (void)TLV_addValue(&send_tlv,
                                   (uint8_t *)&GUI_SaveInformation[PortNum].VDM_SVIDPortPartner.SVIDs[index], 2);
                size += 2;
              }
              TLV_UpdateSizeTag(&send_tlv, size);
            }
          }
          break;
          case USBPD_NOTIFY_VDM_MODE_RECEIVED :
          {
            uint32_t index;
            uint16_t size;
            /* List of received SVDM MODES */
            if (0 != GUI_SaveInformation[PortNum].VDM_ModesPortPartner.NumModes)
            {
              (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_SVDM_MODES, 4,
                            (uint8_t *)&GUI_SaveInformation[PortNum].VDM_ModesPortPartner.Modes[0]);
              size = 4;
              for (index = 1; index < GUI_SaveInformation[PortNum].VDM_ModesPortPartner.NumModes; index++)
              {
                (void)TLV_addValue(&send_tlv,
                                   (uint8_t *)&GUI_SaveInformation[PortNum].VDM_ModesPortPartner.Modes[index], 4);
                size += 4;
              }
              TLV_UpdateSizeTag(&send_tlv, size);
            }
          }
          break;
#endif /* _VDM */
#if defined(_VCONN_SUPPORT)
          case USBPD_NOTIFY_VDM_CABLE_IDENT_RECEIVED :
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_CABLE_VDO, 4,
                          (uint8_t *)&GUI_SaveInformation[PortNum].VDM_DiscoCableIdentify.CableVDO.d32);
            break;
#endif /* _VCONN_SUPPORT */
          default:
            break;
        }
      }
      else
      {
        /* Send all the indication parameters if connected */
        if (USBPD_TRUE == DPM_Params[PortNum].PE_IsConnected)
        {
#if defined(_VDM)
          uint32_t index;
#endif /* _VDM */

          if (0U != GUI_SaveInformation[PortNum].NumberOfRcvSNKPDO)
          {
            /* NumberOfRcvSNKPDO */
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_NUMBEROFRCVSNKPDO, 1,
                          (uint8_t[]) { GUI_SaveInformation[PortNum].NumberOfRcvSNKPDO });
            /* ListOfRcvSNKPDO*/
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_LISTOFRCVSNKPDO,
                          (uint16_t)(GUI_SaveInformation[PortNum].NumberOfRcvSNKPDO * 4U),
                          (uint8_t *)GUI_SaveInformation[PortNum].ListOfRcvSNKPDO);
          }
          /* RDOPosition */
          (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_RDOPOSITION, 1,
                        (uint8_t[]) { GUI_SaveInformation[PortNum].RDOPosition });
          if (USBPD_PORTPOWERROLE_SNK == DPM_Params[PortNum].PE_PowerRole)
          {
            /* NumberOfRcvSRCPDO*/
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_NUMBEROFRCVSRCPDO, 1,
                          (uint8_t[]) { GUI_SaveInformation[PortNum].NumberOfRcvSRCPDO });
            /* ListOfRcvSRCPDO*/
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_LISTOFRCVSRCPDO,
                          (uint16_t)GUI_SaveInformation[PortNum].NumberOfRcvSRCPDO * 4U,
                          (uint8_t *)GUI_SaveInformation[PortNum].ListOfRcvSRCPDO);
          }
          /* PD_SpecRevision */
          (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_PD_SPECREVISION, 1,
                        (uint8_t[]) { DPM_Params[PortNum].PE_SpecRevision });

#if defined(USBPD_REV30_SUPPORT)
#if _SRC_CAPA_EXT
          /* Number of batteries supported by the port partner */
          (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_NBBATTERIES, 1,
                        (uint8_t[]) { GUI_SaveInformation[PortNum].RcvSRCExtendedCapa.NbBatteries });
#endif /* _SRC_CAPA_EXT */
#endif /* USBPD_REV30_SUPPORT */

#if defined(_VDM)
          if (0U != GUI_SaveInformation[PortNum].VDM_DiscoIdentify.IDHeader.d32)
          {
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_VDM_IDENTITY, 4,
                          (uint8_t *)&GUI_SaveInformation[PortNum].VDM_DiscoIdentify.IDHeader.d32);
          }
#if defined(_VCONN_SUPPORT)
          if (1U == GUI_SaveInformation[PortNum].VDM_DiscoCableIdentify.CableVDO_Presence)
          {
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_CABLE_VDO, 4,
                          (uint8_t *)&GUI_SaveInformation[PortNum].VDM_DiscoCableIdentify.CableVDO.d32);
          }
#endif /* _VCONN_SUPPORT */
          /* List of received SVDM SVID */
          if (0 != GUI_SaveInformation[PortNum].VDM_SVIDPortPartner.NumSVIDs)
          {
            uint16_t size;
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_SVDM_SVIDS, 2,
                          (uint8_t *)&GUI_SaveInformation[PortNum].VDM_SVIDPortPartner.SVIDs[0]);
            size = 2;
            for (index = 1; index < GUI_SaveInformation[PortNum].VDM_SVIDPortPartner.NumSVIDs; index++)
            {
              (void)TLV_addValue(&send_tlv,
                                 (uint8_t *)&GUI_SaveInformation[PortNum].VDM_SVIDPortPartner.SVIDs[index], 2);
              size += 2;
            }
            TLV_UpdateSizeTag(&send_tlv, size);
          }

          /* List of received SVDM MODES */
          if (0 != GUI_SaveInformation[PortNum].VDM_ModesPortPartner.NumModes)
          {
            uint16_t size;
            (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_SVDM_MODES, 4,
                          (uint8_t *)&GUI_SaveInformation[PortNum].VDM_ModesPortPartner.Modes[0]);
            size = 4;
            for (index = 1; index < GUI_SaveInformation[PortNum].VDM_ModesPortPartner.NumModes; index++)
            {
              (void)TLV_addValue(&send_tlv,
                                 (uint8_t *)&GUI_SaveInformation[PortNum].VDM_ModesPortPartner.Modes[index], 4);
              size += 4;
            }
            TLV_UpdateSizeTag(&send_tlv, size);
          }
#endif /* _VDM */
        }
      }
    }

    /* Check if notification linked to a measure report */
    if ((TypeNotification & GUI_NOTIF_MEASUREREPORTING) == GUI_NOTIF_MEASUREREPORTING)
    {
      uint32_t vsense = 0;
      int32_t isense = 0;

      (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_TIMESTAMP, 4, (uint8_t *)&Value);

      if (NULL != pCB_GetVoltage)
      {
        vsense = pCB_GetVoltage(PortNum);
      }
      if (NULL != pCB_GetVoltage)
      {
        isense = pCB_GetCurrent(PortNum);
        if (isense < 0)
        {
          isense = -isense;
        }
      }

      /* VBUS & IBUS level */
      vsense = ((uint32_t)(isense) << 16) | vsense;
      (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_VBUS_LEVEL, 4, (uint8_t *)&vsense);
    }

    /* Check if timestamp */
    if ((TypeNotification & GUI_NOTIF_TIMESTAMP) == GUI_NOTIF_TIMESTAMP)
    {
      (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_TIMESTAMP, 4, (uint8_t *)&Value);
    }

    /* Check if power change */
    if ((TypeNotification & GUI_NOTIF_POWER_EVENT) == GUI_NOTIF_POWER_EVENT)
    {
      /* Is Connected*/
      (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_ISCONNECTED, 1, (uint8_t[]) { DPM_Params[PortNum].PE_Power });
      if (USBPD_POWER_NO == DPM_Params[PortNum].PE_Power)
      {
        /* CC line */
        (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_CC, 1, (uint8_t[]) { DPM_Params[PortNum].ActiveCCIs });
        /* PowerRole*/
        (void)TLV_add(&send_tlv, (uint8_t)GUI_IND_POWERROLE, 1, (uint8_t[]) { DPM_Params[PortNum].PE_PowerRole });
      }
    }

    TLV_deinit_encode(&send_tlv);

    if (0U != TLV_get_string_length(Processed))
    {
      *pMsgToSend = Processed;
      *pSizeMsg = (uint8_t)TLV_get_string_length(Processed) + 8U;
      gui_state = GUI_STATE_RUNNING;

    }

  }

  return gui_state;
}

/**
  * @brief  Callback function called by PE to inform GUI about PE event.
  * @param  PortNum   Port number
  * @param  EventVal Event vale @ref USBPD_NotifyEventValue_TypeDef
  * @retval None
  */
void GUI_PostNotificationMessage(uint8_t PortNum, uint16_t EventVal)
{
#if defined(_SNK) || defined(_DRP)
  if ((uint16_t)USBPD_NOTIFY_REQUEST_ACCEPTED == EventVal)
  {
    DPM_USER_Settings[PortNum].DPM_SNKRequestedPower.OperatingVoltageInmVunits
      = GUI_SaveInformation[PortNum].RequestedVoltage;
    DPM_USER_Settings[PortNum].DPM_SNKRequestedPower.OperatingPowerInmWunits
      = (GUI_SaveInformation[PortNum].RequestedVoltage * GUI_SaveInformation[PortNum].RequestedCurrent) / 1000U;
  }
#endif /* _SNK || _DRP */

  /* Send a notitification only if GUI is running */
  if (GUI_State == GUI_STATE_RUNNING)
  {
    uint32_t event = (uint32_t)GUI_USER_EVENT_GUI
                     | ((uint32_t)PortNum << GUI_PE_PORT_NUM_Pos)
                     | ((uint32_t)EventVal << GUI_PE_NOTIF_Pos);
#if defined(_RTOS)
#if (osCMSIS < 0x20000U)
    (void)osMessagePut(GUIMsgBox, event, 0);
#else
    (void)osMessageQueuePut(GUIMsgBox, &event, 0U, 0U);
#endif /* osCMSIS < 0x20000U */
#else
    GUI_Flag = event;
#if defined(USE_STM32_UTILITY_OS)
    GUI_RXProcess(GUI_Flag);
    GUI_Flag = GUI_USER_EVENT_NONE;
#endif /* USE_STM32_UTILITY_OS */
#endif /* _RTOS */
  }
}

/**
  * @brief  Check is GUI is connected to the board or not
  * @retval USBPD state
  */
USBPD_FunctionalState GUI_IsRunning(void)
{
  return ((GUI_State == GUI_STATE_RUNNING) ? USBPD_ENABLE : USBPD_DISABLE);
}

/**
  * @brief  GUI callback to save information coming from PortPartner
  * @param  PortNum Port number
  * @param  DataId  Type of data to be updated in DPM based on @ref USBPD_CORE_DataInfoType_TypeDef
  * @param  Ptr     Pointer on the data
  * @param  Size    Nb of bytes to be updated in GUI
  * @retval None
  */
void GUI_SaveInfo(uint8_t PortNum, uint8_t DataId, uint8_t *Ptr, uint32_t Size)
{
  uint32_t index;

  /* Check type of information targeted by request */
  switch (DataId)
  {
    /* Case requested DO position Data information :
    */
    case USBPD_CORE_DATATYPE_RDO_POSITION :
      if (Size == 4U)
      {
        uint8_t *temp;
        temp = (uint8_t *)&GUI_SaveInformation[PortNum].RDOPosition;
        (void)memcpy(temp, Ptr, Size);
        GUI_SaveInformation[PortNum].RDOPositionPrevious = *Ptr;
        temp = (uint8_t *)&GUI_SaveInformation[PortNum].RDOPositionPrevious;
        (void)memcpy(temp, Ptr, Size);
      }
      break;

    /* Case Received Source PDO values Data information :
    */
    case USBPD_CORE_DATATYPE_RCV_SRC_PDO :
      if (Size <= (USBPD_MAX_NB_PDO * 4U))
      {
        uint8_t *rdo;
        GUI_SaveInformation[PortNum].NumberOfRcvSRCPDO = (Size / 4U);
        /* Copy PDO data in DPM Handle field */
        for (index = 0U; index < (Size / 4U); index++)
        {
          rdo = (uint8_t *)&GUI_SaveInformation[PortNum].ListOfRcvSRCPDO[index];
          (void)memcpy(rdo, (Ptr + (index * 4U)), (4U * sizeof(uint8_t)));
        }
      }
      break;

    /* Case Received Sink PDO values Data information :
    */
    case USBPD_CORE_DATATYPE_RCV_SNK_PDO :
      if (Size <= (USBPD_MAX_NB_PDO * 4U))
      {
        uint8_t *rdo;
        GUI_SaveInformation[PortNum].NumberOfRcvSNKPDO = (Size / 4U);
        /* Copy PDO data in DPM Handle field */
        for (index = 0U; index < (Size / 4U); index++)
        {
          rdo = (uint8_t *)&GUI_SaveInformation[PortNum].ListOfRcvSNKPDO[index];
          (void)memcpy(rdo, (Ptr + (index * 4U)), (4U * sizeof(uint8_t)));
        }
      }
      break;

    /* Case Received Request PDO Data information :
    */
    case USBPD_CORE_DATATYPE_RCV_REQ_PDO :
      if (Size == 4U)
      {
        uint8_t *rdo;
        rdo = (uint8_t *)&GUI_SaveInformation[PortNum].RcvRequestDOMsg;
        (void)memcpy(rdo, Ptr, Size);
      }
      break;

#if defined(USBPD_REV30_SUPPORT)
#if _STATUS
    case USBPD_CORE_INFO_STATUS :
    {
      uint8_t *info_status;
      info_status = (uint8_t *)&GUI_SaveInformation[PortNum].RcvStatus;
      (void)memcpy(info_status, Ptr, Size);
      break;
    }
#endif /* _STATUS */
#if _PPS
    case USBPD_CORE_PPS_STATUS :
    {
      uint8_t  *ext_capa;
      ext_capa = (uint8_t *)&GUI_SaveInformation[PortNum].RcvPPSStatus;
      (void)memcpy(ext_capa, Ptr, Size);
      break;
    }
#endif /* _PPS */
#if _SRC_CAPA_EXT
    case USBPD_CORE_EXTENDED_CAPA :
    {
      uint8_t  *ext_capa;
      ext_capa = (uint8_t *)&GUI_SaveInformation[PortNum].RcvSRCExtendedCapa;
      (void)memcpy(ext_capa, Ptr, Size);
      break;
    }
#endif /* _SRC_CAPA_EXT */
#if defined(USBPDCORE_SNK_CAPA_EXT)
    case USBPD_CORE_SNK_EXTENDED_CAPA :
    {
      uint8_t  *_snk_ext_capa;
      _snk_ext_capa = (uint8_t *)&GUI_SaveInformation[PortNum].RcvSNKExtendedCapa;
      (void)memcpy(_snk_ext_capa, Ptr, Size);
      break;
    }
#endif /* USBPDCORE_SNK_CAPA_EXT */
#if _MANU_INFO
    case USBPD_CORE_GET_MANUFACTURER_INFO:
    {
      uint8_t *temp = (uint8_t *)Ptr;
      GUI_SaveInformation[PortNum].GetManufacturerInfo.ManufacturerInfoTarget = *temp;
      temp++;
      GUI_SaveInformation[PortNum].GetManufacturerInfo.ManufacturerInfoRef    = *temp;
      break;
    }
#endif /* _MANU_INFO */
#if _BATTERY
    case USBPD_CORE_BATTERY_STATUS:
      GUI_SaveInformation[PortNum].BatteryStatus.d32 = *Ptr;
      break;

    case USBPD_CORE_GET_BATTERY_STATUS:
      GUI_SaveInformation[PortNum].GetBatteryStatus.BatteryStatusRef = *(uint8_t *)Ptr;
      break;

    case USBPD_CORE_GET_BATTERY_CAPABILITY:
    {
      uint8_t *temp = (uint8_t *)Ptr;
      GUI_SaveInformation[PortNum].GetBatteryCapability.BatteryCapRef = *temp;
      break;
    }
#endif /* _BATTERY */
#if _ALERT
    case USBPD_CORE_ALERT:
    {
      uint8_t  *alert;
      alert = (uint8_t *)&GUI_SaveInformation[PortNum].RcvAlert.d32;
      (void)memcpy(alert, Ptr, Size);
      break;
    }
#endif /* _ALERT */
#endif /* USBPD_REV30_SUPPORT */
#if defined(_VCONN_SUPPORT)
    case GUI_VDM_CABLE_INFO:
    {
      uint8_t  *disco_ident;
      disco_ident = (uint8_t *)&GUI_SaveInformation[PortNum].VDM_DiscoCableIdentify;
      (void)memcpy(disco_ident, Ptr, Size);
      break;
    }
#endif /* _VCONN_SUPPORT */

    /* In case of unexpected data type (Set request could not be fulfilled) :
     */
    default :
      break;
  }
}

/**
  * @brief  Register callback function to be used with Free Text feature
  * @param  CB_FreeText Callback function to register (port number, payload and size)
  * @retval None
  */
void GUI_RegisterCallback_FreeText(USBPD_StatusTypeDef(*CB_FreeText)(uint8_t, uint8_t *, uint16_t))
{
  pCB_FreeText = CB_FreeText;
}

/**
  * @}
  */

/** @defgroup USBPD_GUI_API_Private_Functions GUI API Private Functions
  * @{
  */
/**
  * @brief  Send DPM_INIT_CNF message to GUI
  * @param  PortNum   Port number
  * @param  pEncodedMsg Pointer on the message to be encoded
  * @retval None
  */
static void Send_DpmInitCnf(uint8_t PortNum, uint8_t *pEncodedMsg)
{
  TLV_ToSend_Data_t ToSendTLV;
  (void)TLV_init_encode(&ToSendTLV, __GUI_SET_TAG_ID(PortNum, DPM_INIT_CNF), TLV_SIZE_MAX, pEncodedMsg);

  /* Information by board */
  if (0U == PortNum)
  {
    /* HWBoardVersion */
    const uint8_t *hwversion;
    if (NULL != pCB_HWBoardVersion)
    {
      hwversion = pCB_HWBoardVersion();
    }
    else
    {
      hwversion = (uint8_t *) "UNKNOWN";
    }
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_HWBOARDVERSION, (uint16_t)(strlen((const char *)hwversion)), hwversion);

    /* HWPDType */
    const uint8_t *hwpdtype;
    if (NULL != pCB_HWBoardVersion)
    {
      hwpdtype  = pCB_HWPDType();
    }
    else
    {
      hwpdtype = (uint8_t *) "UNKNOWN";
    }
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_HWPDTYPE, (uint16_t)(strlen((const char *)hwpdtype)), hwpdtype);

    /* NbPortMax */
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_NBPORTMAX, 1, (uint8_t[]) { USBPD_PORT_COUNT });
    /* FW Version */
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_FWVERSION, 8, (uint8_t[])
    {
      (USBPD_FW_VERSION & 0x000000FFU),
      (USBPD_FW_VERSION & 0x0000FF00U) >> 8,
      (USBPD_FW_VERSION & 0x00FF0000U) >> 16,
      (USBPD_FW_VERSION & 0xFF000000U) >> 24,
      (_LIB_ID & 0x000000FFU),
      (_LIB_ID & 0x0000FF00U) >> 8,
      (_LIB_ID & 0x00FF0000U) >> 16,
      (_LIB_ID & 0xFF000000U) >> 24
    }
                 );
    /* Start Port number */
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_NB_PORT_START, 1, (uint8_t[]) { USBPD_START_PORT_NUMBER });
    /* Start Port number */
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_NB_PORT_START, 1, (uint8_t[]) { USBPD_START_PORT_NUMBER });

    /* Original settings */
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_ORIGINAL_SETTINGS, 1, (uint8_t *)&GUI_OriginalSettings);

    TLV_deinit_encode(&ToSendTLV);
  }
  else
  {
    uint16_t settings;
    uint8_t port = PortNum - 1U;
#if defined(MB1303)
    uint16_t fake = 0xADDE;
#endif /*MB1303*/
    /* TypeCSpecVersion & PDSpecRevision */
    settings = (uint16_t)DPM_Settings[port].PE_SpecRevision;
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_TYPECSPECREVISION, 2, (uint8_t[]) { 0x12, settings });
#if defined(USBPD_REV30_SUPPORT)
    /* Extended Messages UnCkunked */
    settings = (uint16_t)DPM_Settings[port].PE_PD3_Support.d.PE_UnchunkSupport;
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_EXTENDEDMESSAGESUNCKUNKED, 1, (uint8_t *)&settings);
#endif /* USBPD_REV30_SUPPORT */
    /* AccessorySupp */
    settings = (uint16_t)DPM_Settings[port].CAD_AccesorySupport;
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_ACCESSORYSUPP, 1, (uint8_t *)&settings);
    /*PowerAccessoryDetection*/
    settings = (uint16_t)DPM_USER_Settings[port].PWR_AccessoryDetection;
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_POWERACCESSORYDETECTION, 1, (uint8_t *)&settings);
    /*PowerAccessoryTransition*/
    settings = (uint16_t)DPM_USER_Settings[port].PWR_AccessoryTransition;
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_POWERACCESSORYTRANSITION, 1, (uint8_t *)&settings);
    /*IsCable*/
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_ISCABLE, 1, (uint8_t[]) { 0x00 });
    /*TryFeature*/
    settings = (uint16_t)DPM_Settings[port].CAD_TryFeature;
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_TRYFEATURE, 1, (uint8_t *)&settings);
    /*RpResistorValue*/
    settings = (uint16_t)DPM_USER_Settings[port].PWR_RpResistorValue;
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_RPRESISTORVALUE, 1, (uint8_t *)&settings);
    /*USBSupport*/
    settings = (uint16_t)DPM_USER_Settings[port].USB_Support;
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_USBSUPPORT, 1, (uint8_t *)&settings);
    /*USBDevice*/
    settings = (uint16_t)DPM_USER_Settings[port].USB_Device;
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_USBDEVICE, 1, (uint8_t *)&settings);
    /*USBHost*/
    settings = (uint16_t)DPM_USER_Settings[port].USB_Host;
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_USBHOST, 1, (uint8_t *)&settings);
    /*Externally_Powered*/
    settings = (uint16_t)DPM_USER_Settings[port].PWR_UnconstrainedPower;
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_UNCONSTRAINED_POWERED, 1, (uint8_t *)&settings);
    /*USBSuspendSupport*/
    settings = (uint16_t)DPM_USER_Settings[port].USB_SuspendSupport;
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_USBSUSPENDSUPPORT, 1, (uint8_t *)&settings);
#if defined(MB1303)
    /*VConnDischarge*/
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_VCONNDISCHARGE, 2, (uint8_t *)&fake);
    /*VConnLim*/
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_VCONNILIM, 2, (uint8_t *)&fake);
    /*VConnLimValue*/
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_VCONNILIMVALUE, 2, (uint8_t *)&fake);
    /*VConnMonitoring*/
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_VCONNMONITORING, 2, (uint8_t *)&fake);
    /*VConnThresholdUVLO*/
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_VCONNTHRESHOLDUVLO, 2, (uint8_t *)&fake);
    /*VConnSupply*/
    (void)TLV_add(&ToSendTLV, (uint8_t)GUI_INIT_VCONNSUPPLY, 2, (uint8_t *)&fake);
#endif /*MB1303*/

    TLV_deinit_encode(&ToSendTLV);
  }
}

/**
  * @brief      Execution of the message received
  * @note       Generation of a TLV instruction to send in the context of DPM_MESSAGE
  */
static void Request_MessageReq(uint8_t PortNum, uint8_t *instruction, uint8_t *pEncodedMsg)
{
  USBPD_StatusTypeDef status = USBPD_ERROR;
  TLV_Received_Data_t process_tlv;
  TLV_ToSend_Data_t send_tlv;

  (void)TLV_init_decode(&process_tlv, instruction);
  (void)TLV_init_encode(&send_tlv, __GUI_SET_TAG_ID((PortNum + 1), DPM_MESSAGE_CNF), TLV_SIZE_MAX, pEncodedMsg);

  /*
    In case there is an error, we use this variable to know when to send a DPM_MESSAGE_REJ
    instead of a DPM_MESSAGE_CNF. This variable contains the cause if there is one.
  */
  USBPD_GUI_Reject_Reason error;

  uint8_t tag;
  uint16_t length;
  uint8_t *value;
  (void)TLV_get(&process_tlv, &tag, &length, &value);
  switch (tag)
    /*
      Each case should:
      - Apply his specific request in the stack. If parameters are needed, they can be found in the value pointer
      - If there is an error, set the error variable according to it (cf USBPD_GUI_Specification)
      - Otherwise in case of success, do nothing else
    */
  {
    case GUI_MSG_GOTOMIN :
      status = USBPD_DPM_RequestGotoMin(PortNum);
      break;
    case GUI_MSG_PING :
      status = USBPD_DPM_RequestPing(PortNum);
      break;
    case GUI_MSG_GET_SRC_CAPA :
      status = USBPD_DPM_RequestGetSourceCapability(PortNum);
      break;
    case GUI_MSG_GET_SNK_CAPA :
      status = USBPD_DPM_RequestGetSinkCapability(PortNum);
      break;
    case GUI_MSG_DR_SWAP :
      status = USBPD_DPM_RequestDataRoleSwap(PortNum);
      break;
    case GUI_MSG_PR_SWAP :
      status = USBPD_DPM_RequestPowerRoleSwap(PortNum);
      break;
    case GUI_MSG_VCONN_SWAP :
      status = USBPD_DPM_RequestVconnSwap(PortNum);
      break;
    case GUI_MSG_SOURCE_CAPA :
      status = USBPD_DPM_RequestSourceCapability(PortNum);
      break;
    case GUI_MSG_REQUEST :
    {
      uint16_t voltage = 0;
      uint8_t nb_expected_tag = 0U;
      uint8_t index_pdo = 0;
      if (length > TLV_SIZE_MAX)
      {
        break;
      }
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      while ((0U != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 2U))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_RDOPOSITION == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          index_pdo = value[0];
        }
        else if (GUI_PARAM_MSG_REQUESTEDVOLTAGE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          voltage = USBPD_LE16(&value[0]);
        }
        else
        {
          /* Nothing to do */
        }

        (void)TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (2U == nb_expected_tag)
      {
        status = USBPD_DPM_RequestMessageRequest(PortNum, index_pdo, voltage);
      }
      break;
    }
    case GUI_MSG_SOFT_RESET :
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
      {
        status = USBPD_DPM_RequestSoftReset(PortNum, (USBPD_SOPType_TypeDef)value[0]);
      }
      break;
    case GUI_MSG_HARD_RESET :
      status = USBPD_DPM_RequestHardReset(PortNum);
      break;
    case GUI_MSG_CABLE_RESET :
      status = USBPD_DPM_RequestCableReset(PortNum);
      break;
#if defined(_VDM) || defined(_VCONN_SUPPORT)
    case GUI_MSG_VDM_DISCO_IDENT :
      if (length > TLV_SIZE_MAX)
      {
        break;
      }
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
      {
        status = USBPD_DPM_RequestVDM_DiscoveryIdentify(PortNum, (USBPD_SOPType_TypeDef)value[0]);
      }
      break;
    case GUI_MSG_VDM_DISCO_SVID :
      if (length > TLV_SIZE_MAX)
      {
        break;
      }
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
      {
        status = USBPD_DPM_RequestVDM_DiscoverySVID(PortNum, (USBPD_SOPType_TypeDef)value[0]);
      }
      break;
    case GUI_MSG_VDM_DISCO_MODE :
    {
      uint16_t svid = 0;
      uint8_t nb_expected_tag = 0U;
      uint8_t sop = USBPD_SOPTYPE_SOP;
      if (length > TLV_SIZE_MAX)
      {
        break;
      }
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      while ((0U != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 2U))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          sop = value[0];
        }
        else if (GUI_PARAM_MSG_SVDM_SVID == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          svid = USBPD_LE16(&value[0]);
        }
        (void)TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (2U == nb_expected_tag)
      {
        status = USBPD_DPM_RequestVDM_DiscoveryMode(PortNum, (USBPD_SOPType_TypeDef)sop, svid);
      }
    }
    break;
    case GUI_MSG_VDM_ENTER_MODE :
    {
      uint16_t mode = 0;
      uint16_t svid = 0;
      uint8_t nb_expected_tag = 0U;
      uint8_t sop = USBPD_SOPTYPE_SOP;
      if (length > TLV_SIZE_MAX)
      {
        break;
      }
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      while ((0U != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 3U))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          sop = value[0];
        }
        else if (GUI_PARAM_MSG_SVDM_SVID == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          svid = USBPD_LE16(&value[0]);
        }
        else if (GUI_PARAM_MSG_SVDM_MODEINDEX == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          mode = USBPD_LE16(&value[0]);
        }
        (void)TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (3U == nb_expected_tag)
      {
        status = USBPD_DPM_RequestVDM_EnterMode(PortNum, (USBPD_SOPType_TypeDef)sop, svid, mode);
      }
    }
    break;
    case GUI_MSG_VDM_EXIT_MODE :
    {
      uint16_t svid = 0;
      uint16_t mode = 0;
      uint8_t nb_expected_tag = 0U;
      uint8_t sop = USBPD_SOPTYPE_SOP;
      if (length > TLV_SIZE_MAX)
      {
        break;
      }
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      while ((0U != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 3U))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          sop = value[0];
        }
        else if (GUI_PARAM_MSG_SVDM_SVID == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          svid = USBPD_LE16(&value[0]);
        }
        else if (GUI_PARAM_MSG_SVDM_MODEINDEX == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          mode = value[0];
        }
        (void)TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (3U == nb_expected_tag)
      {
        status = USBPD_DPM_RequestVDM_ExitMode(PortNum, (USBPD_SOPType_TypeDef)sop, svid, mode);
      }
    }
    break;
    case GUI_MSG_VDM_UNSTRUCTURED :
      break;
#endif /* _VDM || _VCONN_SUPPORT */
#if defined(_VDM)
    case GUI_MSG_DISPLAY_PORT_STATUS :
    {
      uint32_t dp_status;
      uint16_t svid = 0;
      uint8_t nb_expected_tag = 0U;
      uint8_t sop = USBPD_SOPTYPE_SOP;
      if (length > TLV_SIZE_MAX)
      {
        break;
      }
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      while ((0U != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 3U))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          sop = value[0];
        }
        else if (GUI_PARAM_MSG_SVDM_SVID == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          svid = USBPD_LE16(&value[0]);
        }
        else if (GUI_PARAM_MSG_DP_STATUS == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          dp_status = USBPD_LE32(&value[0]);
        }
        (void)TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (3U == nb_expected_tag)
      {
        status = USBPD_DPM_RequestDisplayPortStatus(PortNum, (USBPD_SOPType_TypeDef)sop, svid, &dp_status);
      }
    }
    break;
    case GUI_MSG_DISPLAY_PORT_CONFIG :
    {
      uint32_t dp_config;
      uint16_t svid = 0;
      uint8_t nb_expected_tag = 0U;
      uint8_t sop = USBPD_SOPTYPE_SOP;
      if (length > TLV_SIZE_MAX)
      {
        break;
      }
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      while ((0U != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 3U))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          sop = value[0];
        }
        else if (GUI_PARAM_MSG_SVDM_SVID == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          svid = USBPD_LE16(&value[0]);
        }
        else if (GUI_PARAM_MSG_DP_CONFIGURE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          dp_config = USBPD_LE32(&value[0]);
        }
        (void)TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (3U == nb_expected_tag)
      {
        status = USBPD_DPM_RequestDisplayPortConfig(PortNum, (USBPD_SOPType_TypeDef)sop, svid, &dp_config);
      }
    }
    break;
    case GUI_MSG_DISPLAY_PORT_ATTENTION :
      break;
    case GUI_MSG_VDM_ATTENTION :
    {
      uint16_t svid = 0;
      uint8_t nb_expected_tag = 0U;
      uint8_t sop = USBPD_SOPTYPE_SOP;
      if (length > TLV_SIZE_MAX)
      {
        break;
      }
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      while ((0U != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 2U))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          sop = value[0];
        }
        else if (GUI_PARAM_MSG_SVDM_SVID == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          svid = USBPD_LE16(&value[0]);
        }
        (void)TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (2U == nb_expected_tag)
      {
        status = USBPD_DPM_RequestAttention(PortNum, (USBPD_SOPType_TypeDef)sop, svid);
      }
    }
    break;
#endif /* _VDM */
    case GUI_MSG_FREE_TEXT :
    {
      uint8_t nb_expected_tag = 0U;
      if (length > TLV_SIZE_MAX)
      {
        break;
      }
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      while ((0U != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 1U))
      {
        nb_expected_tag++;
      }
      if (1U == nb_expected_tag)
      {
        status = Manage_FreeText(PortNum, value, length);
      }
      break;
    }
#ifdef USBPD_REV30_SUPPORT
    case GUI_MSG_FR_SWAP :
      status = USBPD_DPM_RequestFastRoleSwap(PortNum);
      break;
    case GUI_MSG_GET_PPS_STATUS :
      status = USBPD_DPM_RequestGetPPS_Status(PortNum);
      break;
    case GUI_MSG_GET_COUNTRY_CODES :
      status = USBPD_DPM_RequestGetCountryCodes(PortNum);
      break;
    case GUI_MSG_GET_STATUS :
      status = USBPD_DPM_RequestGetStatus(PortNum);
      break;
    case GUI_MSG_GET_SOURCE_CAPA_EXTENDED :
      status = USBPD_DPM_RequestGetSourceCapabilityExt(PortNum);
      break;
    case GUI_MSG_GET_SINK_CAPA_EXTENDED :
      status = USBPD_DPM_RequestGetSinkCapabilityExt(PortNum);
      break;
    case GUI_MSG_ALERT :
      if (length > TLV_SIZE_MAX)
      {
        break;
      }
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      if (GUI_PARAM_MSG_ALERTMSG == (USBPD_GUI_Tag_ParamMsg)tag)
      {
        USBPD_ADO_TypeDef alert;
        alert.d32 = USBPD_LE32(&value[0]);
        status = USBPD_DPM_RequestAlert(PortNum, alert);
      }
      break;
    case GUI_MSG_GET_COUNTRY_INFO :
      if (length > TLV_SIZE_MAX)
      {
        break;
      }
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      if (GUI_PARAM_MSG_COUNTRYCODE == (USBPD_GUI_Tag_ParamMsg)tag)
      {
        uint16_t country_code;
        country_code = USBPD_LE16(&value[0]);
        status = USBPD_DPM_RequestGetCountryInfo(PortNum, country_code);
      }
      break;
    case GUI_MSG_GET_BAT_CAPA :
      if (length > TLV_SIZE_MAX)
      {
        break;
      }
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      if (GUI_PARAM_MSG_BATTERYREF == (USBPD_GUI_Tag_ParamMsg)tag)
      {
        status = USBPD_DPM_RequestGetBatteryCapability(PortNum, (uint8_t *)&value[0]);
      }
      break;
    case GUI_MSG_GET_BAT_STATUS :
      if (length > TLV_SIZE_MAX)
      {
        break;
      }
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      if (GUI_PARAM_MSG_BATTERYREF == (USBPD_GUI_Tag_ParamMsg)tag)
      {
        status = USBPD_DPM_RequestGetBatteryStatus(PortNum, (uint8_t *)&value[0]);
      }
      break;
    case GUI_MSG_GET_MANU_INFO :
    {
      uint16_t manu_info;
      uint8_t nb_expected_tag = 0U;
      uint8_t sop = (uint8_t)USBPD_SOPTYPE_SOP;
      if (length > TLV_SIZE_MAX)
      {
        break;
      }
      (void)TLV_get(&process_tlv, &tag, &length, &value);
      while ((0U != length) && (TLV_SIZE_MAX > length) && (nb_expected_tag < 2U))
      {
        nb_expected_tag++;
        if (GUI_PARAM_MSG_SOPTYPE == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          sop = value[0];
        }
        else if (GUI_PARAM_MSG_MANUINFODATA == (USBPD_GUI_Tag_ParamMsg)tag)
        {
          manu_info = USBPD_LE16(&value[0]);
        }
        else
        {
          /* Nothing to do */
        }

        (void)TLV_get(&process_tlv, &tag, &length, &value);
      }
      if (2U == nb_expected_tag)
      {
        status = USBPD_DPM_RequestGetManufacturerInfo(PortNum, (USBPD_SOPType_TypeDef)sop, (uint8_t *)&manu_info);
      }
      break;
    }
    case GUI_MSG_SECU_REQUEST :
      status = USBPD_DPM_RequestSecurityRequest(PortNum);
      break;
    case GUI_MSG_FIRM_UPDATE_REQUEST :
      break;
#endif /* USBPD_REV30_SUPPORT */
    default :
      break;
  }

  /* Only applies if an error was specified (= if the case couldn't success)*/
  if (USBPD_OK != status)
  {
    switch (status)
    {
      case USBPD_BUSY:
        error = GUI_REJ_DPM_NOT_READY;
        break;
      case USBPD_TIMEOUT:
        error = GUI_REJ_DPM_TIMEOUT;
        break;
      case USBPD_NOTSUPPORTED:
      case USBPD_ERROR:
      default:
        error = GUI_REJ_DPM_REJECT;
        break;
    }
    TLV_deinit_encode(&send_tlv);
    (void)TLV_init_encode(&send_tlv, __GUI_SET_TAG_ID((PortNum + 1), DPM_MESSAGE_REJ), TLV_SIZE_MAX,
                          pEncodedMsg); /* Turn the tag 0x0A into 0x0B to signal an error*/
    (void)TLV_addValue(&send_tlv, (uint8_t *)&error, 1);
  }

  TLV_deinit_encode(&send_tlv);
  TLV_deinit_decode(&process_tlv);
}

/**
  * @brief      Application of the received configuration
  * @note       Generation of a TLV instruction to send in the context of DPM_CONFIG_SET
  */
static void Send_DpmConfigSetCnf(uint8_t PortNum, uint8_t *instruction, uint8_t *pEncodedMsg)
{
  TLV_ToSend_Data_t send_tlv;
  TLV_Received_Data_t process_tlv;
  /*
    In case there is an error, we use this variable to know when to send a DPM_CONFIG_SET instead
    of a DPM_MESSAGE_CNF. This variable contains the cause if there is one.
  */
  uint8_t *value;
  uint16_t size;
  uint16_t dcdrp = 0;
  uint8_t error = 0xFF;
  uint8_t param_not_applicated[GUI_PARAM_ALL]; /* List of parameters who could not be applied. */
  uint8_t counter_param_not_applicated = 0U;  /* Counter of not applicated parameters */
  uint8_t tag;
  uint8_t flag_drp = 0;
  uint8_t tdrp = 0;

  (void)TLV_init_decode(&process_tlv, instruction);
  /* pEncodedMsg can be sent as it is at this point, if there isn't any error during application of settings*/
  (void)TLV_init_encode(&send_tlv, __GUI_SET_TAG_ID((PortNum + 1), DPM_CONFIG_SET_CNF), TLV_SIZE_MAX, pEncodedMsg);

  /* Do while loop, used to parse and apply each parameter */
  while ((uint8_t)(0xFF) != TLV_get(&process_tlv, &tag, &size, &value))
    /*
      As long as we haven't read all the parameters. If error is set, we continue to loop
      to apply the other parameters if it's because of a DPM_REJECT
      While loop, used to parse and apply each parameter
    */
  {
    switch ((USBPD_GUI_Tag_Param)tag)
      /*
        Each case should:
        - Apply his specific parameter. If data is needed, it can be found in the value pointer
        - In case of success, do nothing else
        - Otherwise if there is an error, set the error variable according to it (cf USBPD_GUI_Specification)
        - If the error is DPM_REJECT (0x00), add the parameter tag to param_not_applicated, using the
          counter counter_param_not_applicated. It allows to list the rejected parameters in the DPM_CONFIG_REJ
      */
    {
      case GUI_PARAM_SOP :
#if !defined(USBPDCORE_LIB_NO_PD)
        /* SOP & SOP1 & SOP2 */
        /* SOP1_Debug & SOP2_Debug not implemented */
        DPM_Settings[PortNum].PE_SupportedSOP = value[0];
#endif /* !USBPDCORE_LIB_NO_PD */
        break;
#if defined(USBPD_REV30_SUPPORT)
      case GUI_PARAM_FASTROLESWAP :
        /*FastRoleSwap*/
        DPM_Settings[PortNum].PE_PD3_Support.d.PE_FastRoleSwapSupport = value[0];
        break;
#endif /*USBPD_REV30_SUPPORT*/
      case GUI_PARAM_DATAROLESWAP_TO_UFP :
        /*DataRoleSwap to UFP */
        DPM_USER_Settings[PortNum].PE_DR_Swap_To_UFP = value[0];
        break;
      case GUI_PARAM_DATAROLESWAP_TO_DFP :
        /*DataRoleSwap to DFP */
        DPM_USER_Settings[PortNum].PE_DR_Swap_To_DFP = value[0];
        break;
      case GUI_PARAM_DEFAULTPOWERROLE :
        /*DefaultPowerRole*/
        DPM_Settings[PortNum].PE_DefaultRole = (USBPD_PortPowerRole_TypeDef)value[0];
        break;
#if !defined(USBPDCORE_LIB_NO_PD)
      case GUI_PARAM_DRP_SUPPORT :
        /*DRP_Support*/
        DPM_Settings[PortNum].PE_RoleSwap = value[0];
        break;
      case GUI_PARAM_CADROLETOGGLE :
        /*CADRoleToggle*/
        DPM_Settings[PortNum].CAD_RoleToggle = value[0];
        break;
      case GUI_PARAM_PE_SCAP_HR :
        /*PE_SCAP_HR*/
        DPM_Settings[PortNum].PE_CapscounterSupport = value[0];
        break;
      case GUI_PARAM_VCONNSWAP :
        /*VConnSwap*/
        DPM_USER_Settings[PortNum].PE_VconnSwap = value[0];
        break;
#if defined(_VDM)
      case GUI_PARAM_VDM_SUPPORT :
        /* DRP Support */
        DPM_Settings[PortNum].PE_VDMSupport = value[0];
        break;
#endif /*_VDM*/
      case GUI_PARAM_PING_SUPPORT :
        /* Ping Support */
        DPM_Settings[PortNum].PE_PingSupport = value[0];
        break;
#if defined(USBPD_REV30_SUPPORT)
      case GUI_PARAM_PPS_SUPPORT :
        DPM_Settings[PortNum].PE_PD3_Support.PD3_Support = USBPD_LE16(&value[0]);
        break;
#endif /*USBPD_REV30_SUPPORT*/
#endif /* !USBPDCORE_LIB_NO_PD */
#if defined(_SNK) || defined(_DRP)
      case GUI_PARAM_SNK_PDO :
      {
        uint32_t index_pdo;
        uint32_t index_gui;
        uint32_t pdo;
#if USBPD_PORT_COUNT==2
        if (USBPD_PORT_0 == PortNum)
#endif /* USBPD_PORT_COUNT==2 */
        {
          (void)memset(PORT0_PDO_ListSNK, 0, sizeof(PORT0_PDO_ListSNK));
          index_pdo = 0;
          for (index_gui = 0; index_gui < size; index_gui = index_gui + 4U)
          {
            pdo = USBPD_LE32(&value[index_gui]);
            PORT0_PDO_ListSNK[index_pdo] = pdo;
            index_pdo++;
          }
          USBPD_NbPDO[0] = (uint8_t)(size / 4U);
          UpdateSNKPowerPort0();
        }
#if USBPD_PORT_COUNT==2
        else
        {
          (void)memset(PORT1_PDO_ListSNK, 0, sizeof(PORT1_PDO_ListSNK));
          index_pdo = 0;
          for (index_gui = 0; index_gui < size; index_gui = index_gui + 4U)
          {
            pdo = USBPD_LE32(&value[index_gui]);
            PORT1_PDO_ListSNK[index_pdo] = pdo;
            index_pdo++;
          }
          USBPD_NbPDO[2] = (uint8_t)(size / 4U);
          UpdateSNKPowerPort1();
        }
#endif /* USBPD_PORT_COUNT==2 */
        break;
      }
#endif /* (_SNK) || (_DRP) */
#if defined(_SRC) || defined(_DRP)
      case GUI_PARAM_SRC_PDO :
      {
        uint32_t index_pdo;
        uint32_t index_gui;
        uint32_t pdo;
#if USBPD_PORT_COUNT==2
        if (USBPD_PORT_0 == PortNum)
#endif /* USBPD_PORT_COUNT==2 */
        {
          (void)memset(PORT0_PDO_ListSRC, 0, sizeof(PORT0_PDO_ListSRC));
          index_pdo = 0;
          for (index_gui = 0; index_gui < size; index_gui = index_gui + 4U)
          {
            pdo = USBPD_LE32(&value[index_gui]);
            PORT0_PDO_ListSRC[index_pdo] = pdo;
            index_pdo++;
          }
          USBPD_NbPDO[1] = (uint8_t)(size / 4U);
        }
#if USBPD_PORT_COUNT==2
        else
        {
          (void)memset(PORT1_PDO_ListSRC, 0, sizeof(PORT1_PDO_ListSRC));
          index_pdo = 0;
          for (index_gui = 0; index_gui < size; index_gui = index_gui + 4U)
          {
            pdo = USBPD_LE32(&value[index_gui]);
            PORT1_PDO_ListSRC[index_pdo] = pdo;
            index_pdo++;
          }
          USBPD_NbPDO[3] = (uint8_t)(size / 4U);
        }
#endif /* USBPD_PORT_COUNT==2 */
        break;
      }
#endif /* (_SRC) || (_DRP) */
      case GUI_PARAM_TDRP :
        tdrp = value[0];
        DPM_USER_Settings[PortNum].CAD_tDRP = tdrp;
        flag_drp = 1;
        break;
      case GUI_PARAM_DCSRC_DRP :
        dcdrp = value[0];
        DPM_USER_Settings[PortNum].CAD_dcSRC_DRP = dcdrp;
        flag_drp = 1;
        break;
      case GUI_PARAM_RESPONDS_TO_DISCOV_SOP :
        DPM_Settings[PortNum].PE_RespondsToDiscovSOP = value[0];
        break;
      case GUI_PARAM_ATTEMPTS_DISCOV_SOP :
        DPM_Settings[PortNum].PE_AttemptsDiscovSOP = value[0];
        break;
      case GUI_PARAM_XID_SOP :
        DPM_ID_Settings[PortNum].XID       = USBPD_LE32(&value[0]);
        break;
      case GUI_PARAM_USB_VID_SOP :
        DPM_ID_Settings[PortNum].VID       = USBPD_LE16(&value[0]);
        break;
      case GUI_PARAM_PID_SOP :
        DPM_ID_Settings[PortNum].PID       = USBPD_LE16(&value[0]);
        break;
#if defined(_VDM)
      case GUI_PARAM_DATA_CAPABLE_AS_USB_HOST_SOP :
        DPM_VDM_Settings[PortNum].VDM_USBHostSupport    = (USBPD_USBCapa_TypeDef)value[0];
        break;
      case GUI_PARAM_DATA_CAPABLE_AS_USB_DEVICE_SOP :
        DPM_VDM_Settings[PortNum].VDM_USBDeviceSupport  = (USBPD_USBCapa_TypeDef)value[0];
        break;
      case GUI_PARAM_PRODUCT_TYPE_SOP :
        DPM_VDM_Settings[PortNum].VDM_ProductTypeUFPorCP = (USBPD_ProductType_TypeDef)value[0];
        break;
      case GUI_PARAM_MODAL_OPERATION_SUPPORTED_SOP :
        DPM_VDM_Settings[PortNum].VDM_ModalOperation    = (USBPD_ModalOp_TypeDef)value[0];
        break;
      case GUI_PARAM_BCDDEVICE_SOP :
        DPM_VDM_Settings[PortNum].VDM_bcdDevice_SOP     = USBPD_LE16(&value[0]);
        break;
#endif /* _VDM */
      case GUI_PARAM_MEASUREREPORTING :
        /* MeasurementReporting */
        GUI_USER_Params[PortNum].u.MeasurementReporting = value[0];
        break;
#if defined(USBPD_REV30_SUPPORT)
#if _MANU_INFO
      case GUI_PARAM_MANUINFOPORT_VID :
        DPM_USER_Settings[PortNum].DPM_ManuInfoPort.VID = USBPD_LE16(&value[0]);
        DPM_USER_Settings[PortNum].DPM_ManuInfoPort.PID = USBPD_LE16(&value[2]);
        size -= 4U;
        (void)memcpy(DPM_USER_Settings[PortNum].DPM_ManuInfoPort.ManuString, &value[4], size);
        break;
#endif /* _MANU_INFO */
#endif /*USBPD_REV30_SUPPORT*/
      default :
        error = (uint8_t)GUI_REJ_DPM_REJECT;
        param_not_applicated[counter_param_not_applicated] = tag;
        counter_param_not_applicated++;
        break;
    }
  }

#if !defined(USBPDCORE_LIB_NO_PD)
  /* Updtate CAD_SNKToggleTime & CAD_SRCToggleTime only if TDRP and/or DC_SRC_DRP have been received */
  if (1U == flag_drp)
  {
    uint16_t calcul;
    if (0U == tdrp)
    {
      tdrp = (uint8_t)(DPM_Settings[PortNum].CAD_SNKToggleTime + DPM_Settings[PortNum].CAD_SRCToggleTime);
    }
    if (0U == dcdrp)
    {
      dcdrp = (uint16_t)
              ((DPM_Settings[PortNum].CAD_SRCToggleTime * 100U)
               / (DPM_Settings[PortNum].CAD_SNKToggleTime + DPM_Settings[PortNum].CAD_SRCToggleTime));
    }
    calcul = (tdrp * dcdrp) / 100U;
    DPM_Settings[PortNum].CAD_SRCToggleTime = (uint8_t)calcul;
    calcul = tdrp * (100U - dcdrp) / 100U;
    DPM_Settings[PortNum].CAD_SNKToggleTime = (uint8_t)calcul;
  }
#endif /* !USBPDCORE_LIB_NO_PD */

  /* Only applies if an error was specified. Will send a DPM_CONFIG_REJ instead of DPM_CONFIG_SET_CNF*/
  if (error != 0xFFU)
  {
    TLV_deinit_encode(&send_tlv);
    (void)TLV_init_encode(&send_tlv, __GUI_SET_TAG_ID((PortNum + 1), DPM_CONFIG_REJ), TLV_SIZE_MAX, pEncodedMsg);
    (void)TLV_addValue(&send_tlv, &error, 1);

    /* If the cause of error is DPM_REJECT, we have to list the tag of parameters which didn't pass */
    if (error != 0U)
    {
      uint32_t index;
      for (index = 0; index < counter_param_not_applicated; index++)
      {
        (void)TLV_addValue(&send_tlv, &param_not_applicated[index], 1);
      }
    }
    TLV_deinit_encode(&send_tlv);
  }
}

static void Send_DpmConfigGetCnf(uint8_t PortNum, uint8_t *instruction, uint8_t *pEncodedMsg)
{
  TLV_ToSend_Data_t send_tlv;
  (void)TLV_init_encode(&send_tlv, __GUI_SET_TAG_ID((PortNum + 1), DPM_CONFIG_GET_CNF), TLV_SIZE_MAX, pEncodedMsg);
  uint16_t length = TLV_get_string_length(instruction) - TLV_HEADER_SIZE;
  uint8_t index = 0;
  uint8_t param;

  /* This is a state machine. */
  do
  {
    /* If there is no parameters, we go through each case of the state machine in one pass. (conditionnal breaks) */
    if (0U == length)
    {
      param = (uint8_t)GUI_PARAM_ALL;
    }
    else
    {
      /* If there are, we loop the state machine and go through one case each time. */
      param = instruction[TLV_VALUE_POSITION + index];
    }

    /*
    Each case should simply use the TLV_add function to insert his corresponding data,
    as suggested in comment. Manual cast to (uint8_t *) may be required.
    */
    switch ((USBPD_GUI_Tag_Param)param)
    {
      case GUI_PARAM_ALL :
#if !defined(USBPDCORE_LIB_NO_PD)
      case GUI_PARAM_SOP :
      {
        /* SOP & SOP1 & SOP2 */
        /* SOP1_Debug & SOP2_Debug not implemented */
        uint8_t settings = (uint8_t)DPM_Settings[PortNum].PE_SupportedSOP;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_SOP, 1, &settings);
        if (0U != length)
        {
          break;
        }
      }
#if defined(USBPD_REV30_SUPPORT)
      case GUI_PARAM_FASTROLESWAP :
      {
        /*FastRoleSwap*/
        uint8_t settings = (uint8_t)DPM_Settings[PortNum].PE_PD3_Support.d.PE_FastRoleSwapSupport;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_FASTROLESWAP, 1, &settings);
        if (0U != length)
        {
          break;
        }
      }
#endif /*USBPD_REV30_SUPPORT*/
#endif /* !USBPDCORE_LIB_NO_PD */
      case GUI_PARAM_DATAROLESWAP_TO_UFP :
      {
        /* DataRoleSwap to UFP */
        uint8_t settings = (uint8_t)DPM_USER_Settings[PortNum].PE_DR_Swap_To_UFP;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_DATAROLESWAP_TO_UFP, 1, &settings);
        if (0U != length)
        {
          break;
        }
      }
      case GUI_PARAM_DATAROLESWAP_TO_DFP :
      {
        /* DataRoleSwap to DFP */
        uint8_t settings = (uint8_t)DPM_USER_Settings[PortNum].PE_DR_Swap_To_DFP;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_DATAROLESWAP_TO_DFP, 1, &settings);
        if (0U != length)
        {
          break;
        }
      }
      case GUI_PARAM_DEFAULTPOWERROLE :
      {
        /*DefaultPowerRole*/
        uint8_t settings = (uint8_t)DPM_Settings[PortNum].PE_DefaultRole;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_DEFAULTPOWERROLE, 1, &settings);
        if (0U != length)
        {
          break;
        }
      }
#if !defined(USBPDCORE_LIB_NO_PD)
      case GUI_PARAM_DRP_SUPPORT :
      {
        /*DRP_Support*/
        uint8_t settings = (uint8_t)DPM_Settings[PortNum].PE_RoleSwap;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_DRP_SUPPORT, 1, &settings);
        if (0U != length)
        {
          break;
        }
      }
      case GUI_PARAM_CADROLETOGGLE :
      {
        /*CADRoleToggle*/
        uint8_t settings = (uint8_t)DPM_Settings[PortNum].CAD_RoleToggle;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_CADROLETOGGLE, 1, &settings);
        if (0U != length)
        {
          break;
        }
      }
      case GUI_PARAM_PE_SCAP_HR :
      {
        /*PE_SCAP_HR*/
        uint8_t settings = (uint8_t)DPM_Settings[PortNum].PE_CapscounterSupport;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_PE_SCAP_HR,  1, &settings);
        if (0U != length)
        {
          break;
        }
      }
#if defined(_VDM)
      case GUI_PARAM_VDM_SUPPORT :
      {
        /* DRP Support */
        uint8_t settings = (uint8_t)DPM_Settings[PortNum].PE_VDMSupport;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_VDM_SUPPORT, 1, &settings);
        if (0U != length)
        {
          break;
        }
      }
#endif /*_VDM*/
      case GUI_PARAM_PING_SUPPORT :
      {
        /* Ping Support */
        uint8_t settings = (uint8_t)DPM_Settings[PortNum].PE_PingSupport;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_PING_SUPPORT, 1, &settings);
        if (0U != length)
        {
          break;
        }
      }
#if defined(USBPD_REV30_SUPPORT)
      case GUI_PARAM_PPS_SUPPORT :
      {
        /* PD3 Support */
        uint16_t settings = (uint16_t)DPM_Settings[PortNum].PE_PD3_Support.PD3_Support;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_PPS_SUPPORT, 2, (uint8_t *)&settings);
        if (0U != length)
        {
          break;
        }
      }
#endif /*USBPD_REV30_SUPPORT*/
#endif /* !USBPDCORE_LIB_NO_PD */
      case GUI_PARAM_VCONNSWAP :
      {
        /*VConnSwap*/
        uint8_t settings = (uint8_t)DPM_USER_Settings[PortNum].PE_VconnSwap;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_VCONNSWAP, 1, &settings);
        if (0U != length)
        {
          break;
        }
      }
#if defined(_SNK) || defined(_DRP)
      case GUI_PARAM_SNK_PDO :
      {
        uint8_t *list_pdo;
        uint8_t nb_pdo;
#if USBPD_PORT_COUNT==2
        if (USBPD_PORT_0 == PortNum)
#endif /* USBPD_PORT_COUNT==2 */
        {
          nb_pdo    =  USBPD_NbPDO[0];
          list_pdo  = (uint8_t *)PORT0_PDO_ListSNK;
        }
#if USBPD_PORT_COUNT==2
        else
        {
          nb_pdo    =  USBPD_NbPDO[2];
          list_pdo  = (uint8_t *)PORT1_PDO_ListSNK;
        }
#endif /* USBPD_PORT_COUNT==2 */
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_SNK_PDO, ((uint16_t)(nb_pdo) * 4U), list_pdo);
      }
      if (0U != length)
      {
        break;
      }
#endif /* _SNK || _DRP */
#if defined(_SRC) || defined(_DRP)
      case GUI_PARAM_SRC_PDO :
      {
        uint8_t *list_pdo;
        uint8_t nb_pdo;
#if USBPD_PORT_COUNT==2
        if (USBPD_PORT_0 == PortNum)
#endif /* USBPD_PORT_COUNT==2 */
        {
          nb_pdo    =  USBPD_NbPDO[1];
          list_pdo  = (uint8_t *)PORT0_PDO_ListSRC;
        }
#if USBPD_PORT_COUNT==2
        else
        {
          nb_pdo    =  USBPD_NbPDO[3];
          list_pdo  = (uint8_t *)PORT1_PDO_ListSRC;
        }
#endif /* USBPD_PORT_COUNT==2 */
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_SRC_PDO, ((uint16_t)(nb_pdo) * 4U), list_pdo);
      }
      if (0U != length)
      {
        break;
      }
#endif /* _SRC || _DRP */
      case GUI_PARAM_TDRP :
      {
        /* Calculate the current value of tDRP (value between 50ms and 100ms) */
        uint8_t value = (uint8_t)DPM_USER_Settings[PortNum].CAD_tDRP;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_TDRP, 1, &value);
      }
      if (0U != length)
      {
        break;
      }
      case GUI_PARAM_DCSRC_DRP :
      {
        /* Calculate the current value of tDRP (value between 50ms and 100ms) */
        uint8_t value = (uint8_t)DPM_USER_Settings[PortNum].CAD_dcSRC_DRP;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_DCSRC_DRP, 1, &value);
      }
      if (0U != length)
      {
        break;
      }
#if !defined(USBPDCORE_LIB_NO_PD)
      case GUI_PARAM_RESPONDS_TO_DISCOV_SOP :
      {
        uint8_t settings = (uint8_t)DPM_Settings[PortNum].PE_RespondsToDiscovSOP;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_RESPONDS_TO_DISCOV_SOP, 1, &settings);
      }
      if (0U != length)
      {
        break;
      }
      case GUI_PARAM_ATTEMPTS_DISCOV_SOP :
      {
        uint8_t settings = (uint8_t)DPM_Settings[PortNum].PE_AttemptsDiscovSOP;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_ATTEMPTS_DISCOV_SOP, 1, &settings);
      }
      if (0U != length)
      {
        break;
      }
#endif /* !USBPDCORE_LIB_NO_PD */
      case GUI_PARAM_XID_SOP :
      {
        uint32_t value = DPM_ID_Settings[PortNum].XID;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_XID_SOP, 4, (uint8_t *)&value);
      }
      if (0U != length)
      {
        break;
      }
      case GUI_PARAM_USB_VID_SOP :
      {
        uint16_t value = (uint16_t)DPM_ID_Settings[PortNum].VID;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_USB_VID_SOP, 2, (uint8_t *)&value);
      }
      if (0U != length)
      {
        break;
      }
      case GUI_PARAM_PID_SOP :
      {
        uint16_t value = (uint16_t)DPM_ID_Settings[PortNum].PID;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_PID_SOP, 2, (uint8_t *)&value);
      }
      if (0U != length)
      {
        break;
      }
#if defined(_VDM)
      case GUI_PARAM_DATA_CAPABLE_AS_USB_HOST_SOP :
      {
        uint8_t settings = (uint8_t)DPM_VDM_Settings[PortNum].VDM_USBHostSupport;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_DATA_CAPABLE_AS_USB_HOST_SOP, 1, &settings);
      }
      if (0U != length)
      {
        break;
      }
      case GUI_PARAM_DATA_CAPABLE_AS_USB_DEVICE_SOP :
      {
        uint8_t settings = (uint8_t)DPM_VDM_Settings[PortNum].VDM_USBDeviceSupport;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_DATA_CAPABLE_AS_USB_DEVICE_SOP, 1, &settings);
      }
      if (0U != length)
      {
        break;
      }
      case GUI_PARAM_PRODUCT_TYPE_SOP :
      {
        uint8_t settings = (uint8_t)DPM_VDM_Settings[PortNum].VDM_ProductTypeUFPorCP;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_PRODUCT_TYPE_SOP, 1, &settings);
      }
      if (0U != length)
      {
        break;
      }
      case GUI_PARAM_MODAL_OPERATION_SUPPORTED_SOP :
      {
        uint8_t settings = (uint8_t)DPM_VDM_Settings[PortNum].VDM_ModalOperation;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_MODAL_OPERATION_SUPPORTED_SOP, 1, &settings);
      }
      if (0U != length)
      {
        break;
      }
      case GUI_PARAM_BCDDEVICE_SOP :
      {
        uint16_t value = (uint16_t)DPM_VDM_Settings[PortNum].VDM_bcdDevice_SOP;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_BCDDEVICE_SOP, 2, (uint8_t *)&value);
      }
      if (0U != length)
      {
        break;
      }
#endif /* _VDM */
      case GUI_PARAM_MEASUREREPORTING :
      {
        uint8_t settings = (uint8_t)GUI_USER_Params[PortNum].u.MeasurementReporting;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_MEASUREREPORTING, 1, &settings);
      }
      if (0U != length)
      {
        break;
      }
#if defined(USBPD_REV30_SUPPORT)
#if _MANU_INFO
      case GUI_PARAM_MANUINFOPORT_VID :
      {
        uint8_t size = (uint8_t)sizeof(DPM_USER_Settings[PortNum].DPM_ManuInfoPort.ManuString) + 4U;
        (void)TLV_add(&send_tlv, (uint8_t)GUI_PARAM_MANUINFOPORT_VID, size,
                      (uint8_t *)&DPM_USER_Settings[PortNum].DPM_ManuInfoPort);
      }
      if (0U != length)
      {
        break;
      }
#endif /* _MANU_INFO */
#endif /*USBPD_REV30_SUPPORT*/
      default:
        if (0U != length)
        {
          /* At least 1 parameter does not exist */
          TLV_deinit_encode(&send_tlv);
          Send_DpmConfigGetRej(PortNum, pEncodedMsg, GUI_REJ_DPM_INVALID_MESSAGE);
          goto __end;
        }
        break;
    }
    index++;
    /*
    Loop until we haven't processed each specified parameter.
    If there weren't any (length = 0), we exit the loop after one execution.
    */
  } while (index < length);

__end:
  TLV_deinit_encode(&send_tlv);
}

static void Send_DpmConfigGetRej(uint8_t PortNum, uint8_t *pEncodedMsg, USBPD_GUI_Reject_Reason RejectReason)
{
  UNUSED(PortNum);
  TLV_ToSend_Data_t send_tlv;
  (void)TLV_init_encode(&send_tlv, (uint8_t)DPM_CONFIG_REJ, TLV_SIZE_MAX, pEncodedMsg);
  (void)TLV_add(&send_tlv, (uint8_t)RejectReason, 0, NULL);
  TLV_deinit_encode(&send_tlv);
}

static void Send_DpmMessageRej(uint8_t PortNum, uint8_t *pEncodedMsg, USBPD_GUI_Reject_Reason RejectReason)
{
  UNUSED(PortNum);
  TLV_ToSend_Data_t send_tlv;
  (void)TLV_init_encode(&send_tlv, (uint8_t)DPM_MESSAGE_REJ, TLV_SIZE_MAX, pEncodedMsg);
  (void)TLV_add(&send_tlv, (uint8_t)RejectReason, 0, NULL);
  TLV_deinit_encode(&send_tlv);
}

static void Send_DpmRegisterReadCnf(uint8_t PortNum, uint8_t *pEncodedMsg, uint8_t param)
{
  TLV_ToSend_Data_t ToSendTLV;
  (void)TLV_init_encode(&ToSendTLV, __GUI_SET_TAG_ID((PortNum + 1), DPM_REGISTER_READ_CNF), TLV_SIZE_MAX, pEncodedMsg);

  uint8_t stateMachineArray[GUI_REG_NUMBER_OF_REGISTERS] =
  {
    (uint8_t)GUI_REG_VENDOR_ID, (uint8_t)GUI_REG_PRODUCT_ID, (uint8_t)GUI_REG_DEVICE_ID,
    (uint8_t)GUI_REG_USBTYPEC_REV, (uint8_t)GUI_REG_USBPD_REV_VER, (uint8_t)GUI_REG_USBPD_INTERFACE_REV,
    (uint8_t)GUI_REG_ALERT, (uint8_t)GUI_REG_ALERT_MASK, (uint8_t)GUI_REG_POWER_STATUS_MASK,
    (uint8_t)GUI_REG_FAULT_STATUS_MASK, (uint8_t)GUI_REG_CONFIG_STANDARD_OUTPUT, (uint8_t)GUI_REG_TCPC_CONTROL,
    (uint8_t)GUI_REG_ROLE_CONTROL, (uint8_t)GUI_REG_FAULT_CONTROL, (uint8_t)GUI_REG_POWER_CONTROL,
    (uint8_t)GUI_REG_CC_STATUS, (uint8_t)GUI_REG_POWER_STATUS, (uint8_t)GUI_REG_FAULT_STATUS,
    (uint8_t)GUI_REG_COMMAND, (uint8_t)GUI_REG_DECIVE_CAP1, (uint8_t)GUI_REG_DECIVE_CAP2,
    (uint8_t)GUI_REG_STD_INPUT_CAP, (uint8_t)GUI_REG_STD_OUTPUT_CAP, (uint8_t)GUI_REG_MSG_HEADER_INFO,
    (uint8_t)GUI_REG_RX_DETECT, (uint8_t)GUI_REG_RX_BYTE_COUNT, (uint8_t)GUI_REG_RX_BUFFER_FRAME_TYPE,
    (uint8_t)GUI_REG_RX_HEADER, (uint8_t)GUI_REG_RX_DATA, (uint8_t)GUI_REG_TRANSMIT, (uint8_t)GUI_REG_TX_BYTE_COUNT,
    (uint8_t)GUI_REG_TX_HEADER, (uint8_t)GUI_REG_TX_DATA, (uint8_t)GUI_REG_VBUS_VOLTAGE,
    (uint8_t)GUI_REG_VBUS_SINK_DISCONNECT_THRESHOLD, (uint8_t)GUI_REG_VBUS_STOP_DISCHARGE_THRESHOLD,
    (uint8_t)GUI_REG_VBUS_VOLTAGE_ALARM_HI_CFG, (uint8_t)GUI_REG_VBUS_VOLTAGE_ALARM_LO_CFG,
    (uint8_t)GUI_REG_VENDOR_DATA
  };
  uint8_t counter = 0;

  /*  If param is not at 0xFF, then it means we only want one parameter.
      We then have to get out of the state machine after the first state */
  if (param != 0xFFU)
  {
    *stateMachineArray = param;
  }

  do
  {
    switch (stateMachineArray[counter])
      /*
        In each case, the register needs to be read. Inclusion in the TLV which
        is going to be sent has to be done with TLV_add().
      */
    {
      case GUI_REG_VENDOR_ID :

        break;
      case GUI_REG_PRODUCT_ID :

        break;
      case GUI_REG_DEVICE_ID :

        break;
      case GUI_REG_USBTYPEC_REV :

        break;
      case GUI_REG_USBPD_REV_VER :

        break;
      case GUI_REG_USBPD_INTERFACE_REV :

        break;
      case GUI_REG_ALERT :

        break;
      case GUI_REG_ALERT_MASK :

        break;
      case GUI_REG_POWER_STATUS_MASK :

        break;
      case GUI_REG_FAULT_STATUS_MASK :

        break;
      case GUI_REG_CONFIG_STANDARD_OUTPUT :

        break;
      case GUI_REG_TCPC_CONTROL :

        break;
      case GUI_REG_ROLE_CONTROL :

        break;
      case GUI_REG_FAULT_CONTROL :

        break;
      case GUI_REG_POWER_CONTROL :

        break;
      case GUI_REG_CC_STATUS :

        break;
      case GUI_REG_POWER_STATUS :

        break;
      case GUI_REG_FAULT_STATUS :

        break;
      case GUI_REG_COMMAND :

        break;
      case GUI_REG_DECIVE_CAP1 :

        break;
      case GUI_REG_DECIVE_CAP2 :

        break;
      case GUI_REG_STD_INPUT_CAP :

        break;
      case GUI_REG_STD_OUTPUT_CAP :

        break;
      case GUI_REG_MSG_HEADER_INFO :

        break;
      case GUI_REG_RX_DETECT :

        break;
      case GUI_REG_RX_BYTE_COUNT :

        break;
      case GUI_REG_RX_BUFFER_FRAME_TYPE :

        break;
      case GUI_REG_RX_HEADER :

        break;
      case GUI_REG_RX_DATA :

        break;
      case GUI_REG_TRANSMIT :

        break;
      case GUI_REG_TX_BYTE_COUNT :

        break;
      case GUI_REG_TX_HEADER :

        break;
      case GUI_REG_TX_DATA :

        break;
      case GUI_REG_VBUS_VOLTAGE :

        break;
      case GUI_REG_VBUS_SINK_DISCONNECT_THRESHOLD :

        break;
      case GUI_REG_VBUS_STOP_DISCHARGE_THRESHOLD :

        break;
      case GUI_REG_VBUS_VOLTAGE_ALARM_HI_CFG :

        break;
      case GUI_REG_VBUS_VOLTAGE_ALARM_LO_CFG :

        break;
      case GUI_REG_VENDOR_DATA :

        break;
      default :
        break;
    }
    counter++;
  } while ((counter < GUI_REG_NUMBER_OF_REGISTERS) && (param != 0xFFU));

  TLV_deinit_encode(&ToSendTLV);
}

static void Send_DpmRegisterWriteCnf(uint8_t PortNum, uint8_t *pEncodedMsg, uint8_t *toProcess)
{
  TLV_Received_Data_t ToProcessTLV;
  (void)TLV_init_decode(&ToProcessTLV, toProcess);

  uint8_t registerName;
  uint16_t registerSize;
  uint8_t *registerValue;

  (void)TLV_get(&ToProcessTLV, &registerName, &registerSize, &registerValue);

  switch (registerName)
    /*
      Each case should write the data pointed by registerValue in the appropriate register,
      while taking into account the size of the data with registerSize.
    */
  {
    case GUI_REG_VENDOR_ID :

      break;
    case GUI_REG_PRODUCT_ID :

      break;
    case GUI_REG_DEVICE_ID :

      break;
    case GUI_REG_USBTYPEC_REV :

      break;
    case GUI_REG_USBPD_REV_VER :

      break;
    case GUI_REG_USBPD_INTERFACE_REV :

      break;
    case GUI_REG_ALERT :

      break;
    case GUI_REG_ALERT_MASK :

      break;
    case GUI_REG_POWER_STATUS_MASK :

      break;
    case GUI_REG_FAULT_STATUS_MASK :

      break;
    case GUI_REG_CONFIG_STANDARD_OUTPUT :

      break;
    case GUI_REG_TCPC_CONTROL :

      break;
    case GUI_REG_ROLE_CONTROL :

      break;
    case GUI_REG_FAULT_CONTROL :

      break;
    case GUI_REG_POWER_CONTROL :

      break;
    case GUI_REG_CC_STATUS :

      break;
    case GUI_REG_POWER_STATUS :

      break;
    case GUI_REG_FAULT_STATUS :

      break;
    case GUI_REG_COMMAND :

      break;
    case GUI_REG_DECIVE_CAP1 :

      break;
    case GUI_REG_DECIVE_CAP2 :

      break;
    case GUI_REG_STD_INPUT_CAP :

      break;
    case GUI_REG_STD_OUTPUT_CAP :

      break;
    case GUI_REG_MSG_HEADER_INFO :

      break;
    case GUI_REG_RX_DETECT :

      break;
    case GUI_REG_RX_BYTE_COUNT :

      break;
    case GUI_REG_RX_BUFFER_FRAME_TYPE :

      break;
    case GUI_REG_RX_HEADER :

      break;
    case GUI_REG_RX_DATA :

      break;
    case GUI_REG_TRANSMIT :

      break;
    case GUI_REG_TX_BYTE_COUNT :

      break;
    case GUI_REG_TX_HEADER :

      break;
    case GUI_REG_TX_DATA :

      break;
    case GUI_REG_VBUS_VOLTAGE :

      break;
    case GUI_REG_VBUS_SINK_DISCONNECT_THRESHOLD :

      break;
    case GUI_REG_VBUS_STOP_DISCHARGE_THRESHOLD :

      break;
    case GUI_REG_VBUS_VOLTAGE_ALARM_HI_CFG :

      break;
    case GUI_REG_VBUS_VOLTAGE_ALARM_LO_CFG :

      break;
    case GUI_REG_VENDOR_DATA :

      break;
    default :
      break;
  }
  TLV_deinit_decode(&ToProcessTLV);

  TLV_ToSend_Data_t ToSendTLV;
  /* Generation of the empty response, as it should be done*/
  (void)TLV_init_encode(&ToSendTLV, __GUI_SET_TAG_ID((PortNum + 1), DPM_REGISTER_WRITE_CNF), 12, pEncodedMsg);
  TLV_deinit_encode(&ToSendTLV);
}

/**
  * @brief  Manage Free text sent by the GUI
  * @param  PortNum     The current port number
  * @param  pPayload    Pointer of the text to manage
  * @param  Size        Size of the text
  * @retval USBPD Status
  */
static USBPD_StatusTypeDef Manage_FreeText(uint8_t PortNum, uint8_t *pPayload, uint16_t Size)
{
  USBPD_StatusTypeDef _status = USBPD_FAIL;

  if (NULL != pCB_FreeText)
  {
    pCB_FreeText(PortNum, pPayload, Size);
    _status = USBPD_OK;
  }

  return _status;
}

#if defined(_SNK) || defined(_DRP)
/**
  * @brief  Function to update SNK PDO and power user settings on Port0
  * @retval None
  */
static void UpdateSNKPowerPort0(void)
{
  USBPD_PDO_TypeDef pdo;
  uint32_t _max_power = 0;
  uint16_t _voltage;
  uint16_t _current;
  uint16_t _power;
  uint16_t _min_voltage = 0xFFFF;
  uint16_t _max_voltage = 0;
  uint16_t _max_current = 0;

  for (uint32_t _index = 0; _index < USBPD_NbPDO[0]; _index++)
  {
    pdo.d32 = PORT0_PDO_ListSNK[_index];
    switch (pdo.GenericPDO.PowerObject)
    {
      case USBPD_CORE_PDO_TYPE_FIXED:    /*!< Fixed Supply PDO */
        _voltage = GUI_DECODE_50MV(pdo.SNKFixedPDO.VoltageIn50mVunits);
        GUI_UPDATE_VOLTAGE_MIN(_voltage, _min_voltage);
        GUI_UPDATE_VOLTAGE_MAX(_voltage, _max_voltage);
        _current = GUI_DECODE_10MA(pdo.SNKFixedPDO.OperationalCurrentIn10mAunits);
        GUI_UPDATE_CURRENT_MAX(_current, _max_current);
        break;
      case USBPD_CORE_PDO_TYPE_BATTERY:  /*!< Battery Supply PDO */
        _voltage = GUI_DECODE_50MV(pdo.SNKBatteryPDO.MinVoltageIn50mVunits);
        GUI_UPDATE_VOLTAGE_MIN(_voltage, _min_voltage);
        _voltage = GUI_DECODE_50MV(pdo.SNKBatteryPDO.MaxVoltageIn50mVunits);
        GUI_UPDATE_VOLTAGE_MAX(_voltage, _max_voltage);
        _power = GUI_DECODE_MW(pdo.SNKBatteryPDO.OperationalPowerIn250mWunits);
        GUI_UPDATE_POWER_MAX(_power, _max_power);
        break;
      case USBPD_CORE_PDO_TYPE_VARIABLE: /*!< Variable Supply (non-battery) PDO */
        _voltage = GUI_DECODE_50MV(pdo.SNKVariablePDO.MinVoltageIn50mVunits);
        GUI_UPDATE_VOLTAGE_MIN(_voltage, _min_voltage);
        _voltage = GUI_DECODE_50MV(pdo.SNKVariablePDO.MaxVoltageIn50mVunits);
        GUI_UPDATE_VOLTAGE_MAX(_voltage, _max_voltage);
        _current = GUI_DECODE_10MA(pdo.SNKVariablePDO.OperationalCurrentIn10mAunits);
        GUI_UPDATE_CURRENT_MAX(_current, _max_current);
        break;
#if _PPS
      case USBPD_CORE_PDO_TYPE_APDO:     /*!< Augmented Power Data Object (APDO) */
        _voltage = GUI_DECODE_100MV(pdo.SRCSNKAPDO.MinVoltageIn100mV);
        GUI_UPDATE_VOLTAGE_MIN(_voltage, _min_voltage);
        _voltage = GUI_DECODE_100MV(pdo.SRCSNKAPDO.MaxVoltageIn100mV);
        GUI_UPDATE_VOLTAGE_MAX(_voltage, _max_voltage);
        _current = GUI_DECODE_50MA(pdo.SRCSNKAPDO.MaxCurrentIn50mAunits);
        GUI_UPDATE_CURRENT_MAX(_current, _max_current);
        break;
#endif /*_USBPD_REV30_SUPPORT && PPS*/
      default:
        break;
    }
  }

  GUI_CHECK_VOLTAGE_MIN(_min_voltage,
                        DPM_USER_Settings[USBPD_PORT_0].DPM_SNKRequestedPower.MinOperatingVoltageInmVunits);
  GUI_CHECK_VOLTAGE_MAX(_max_voltage,
                        DPM_USER_Settings[USBPD_PORT_0].DPM_SNKRequestedPower.MaxOperatingVoltageInmVunits);
  GUI_CHECK_CURRENT_MAX(_max_current,
                        DPM_USER_Settings[USBPD_PORT_0].DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits);
  _max_power = ((uint32_t)(_max_voltage) * (uint32_t)(_max_current)) / 1000U;
  GUI_CHECK_POWER_MAX(_max_power, DPM_USER_Settings[USBPD_PORT_0].DPM_SNKRequestedPower.MaxOperatingPowerInmWunits);
}

#if USBPD_PORT_COUNT==2
/**
  * @brief  Function to update SNK PDO and power user settings on Port1
  * @retval None
  */
static void UpdateSNKPowerPort1(void)
{
  USBPD_PDO_TypeDef pdo;
  uint32_t _max_power = 0;
  uint16_t _voltage = 0;
  uint16_t _current = 0;
  uint16_t _power = 0;
  uint16_t _min_voltage = 0xFFFF;
  uint16_t _max_voltage = 0;
  uint16_t _max_current = 0;

  for (uint32_t _index = 0; _index < USBPD_NbPDO[2]; _index++)
  {
    pdo.d32 = PORT1_PDO_ListSNK[_index];
    switch (pdo.GenericPDO.PowerObject)
    {
      case USBPD_CORE_PDO_TYPE_FIXED:    /*!< Fixed Supply PDO */
        _voltage = GUI_DECODE_50MV(pdo.SNKFixedPDO.VoltageIn50mVunits);
        GUI_UPDATE_VOLTAGE_MIN(_voltage, _min_voltage);
        GUI_UPDATE_VOLTAGE_MAX(_voltage, _max_voltage);
        _current = GUI_DECODE_10MA(pdo.SNKFixedPDO.OperationalCurrentIn10mAunits);
        GUI_UPDATE_CURRENT_MAX(_current, _max_current);
        break;
      case USBPD_CORE_PDO_TYPE_BATTERY:  /*!< Battery Supply PDO */
        _voltage = GUI_DECODE_50MV(pdo.SNKBatteryPDO.MinVoltageIn50mVunits);
        GUI_UPDATE_VOLTAGE_MIN(_voltage, _min_voltage);
        _voltage = GUI_DECODE_50MV(pdo.SNKBatteryPDO.MaxVoltageIn50mVunits);
        GUI_UPDATE_VOLTAGE_MAX(_voltage, _max_voltage);
        _power = GUI_DECODE_MW(pdo.SNKBatteryPDO.OperationalPowerIn250mWunits);
        GUI_UPDATE_POWER_MAX(_power, _max_power);
        break;
      case USBPD_CORE_PDO_TYPE_VARIABLE: /*!< Variable Supply (non-battery) PDO */
        _voltage = GUI_DECODE_50MV(pdo.SNKVariablePDO.MinVoltageIn50mVunits);
        GUI_UPDATE_VOLTAGE_MIN(_voltage, _min_voltage);
        _voltage = GUI_DECODE_50MV(pdo.SNKVariablePDO.MaxVoltageIn50mVunits);
        GUI_UPDATE_VOLTAGE_MAX(_voltage, _max_voltage);
        _current = GUI_DECODE_10MA(pdo.SNKVariablePDO.OperationalCurrentIn10mAunits);
        GUI_UPDATE_CURRENT_MAX(_current, _max_current);
        break;
#if _PPS
      case USBPD_CORE_PDO_TYPE_APDO:     /*!< Augmented Power Data Object (APDO) */
        _voltage = GUI_DECODE_100MV(pdo.SRCSNKAPDO.MinVoltageIn100mV);
        GUI_UPDATE_VOLTAGE_MIN(_voltage, _min_voltage);
        _voltage = GUI_DECODE_100MV(pdo.SRCSNKAPDO.MaxVoltageIn100mV);
        GUI_UPDATE_VOLTAGE_MAX(_voltage, _max_voltage);
        _current = GUI_DECODE_50MA(pdo.SRCSNKAPDO.MaxCurrentIn50mAunits);
        GUI_UPDATE_CURRENT_MAX(_current, _max_current);
        break;
#endif /*_USBPD_REV30_SUPPORT && PPS*/
      default:
        break;
    }
  }

  GUI_CHECK_VOLTAGE_MIN(_min_voltage,
                        DPM_USER_Settings[USBPD_PORT_1].DPM_SNKRequestedPower.MinOperatingVoltageInmVunits);
  GUI_CHECK_VOLTAGE_MAX(_max_voltage,
                        DPM_USER_Settings[USBPD_PORT_1].DPM_SNKRequestedPower.MaxOperatingVoltageInmVunits);
  GUI_CHECK_CURRENT_MAX(_max_current,
                        DPM_USER_Settings[USBPD_PORT_1].DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits);
  _max_power = (_max_voltage * _max_current) / 1000;
  GUI_CHECK_POWER_MAX(_max_power, DPM_USER_Settings[USBPD_PORT_1].DPM_SNKRequestedPower.MaxOperatingPowerInmWunits);
}
#endif /* USBPD_PORT_COUNT==2 */
#endif /* _SNK) || _DRP */
/**
  * @}
  */

/**
  * @}
  */
#endif /* _GUI_INTERFACE */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
