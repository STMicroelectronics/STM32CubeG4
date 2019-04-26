/**
  ******************************************************************************
  * @file    usbpd_vdm_user.c
  * @author  MCD Application Team
  * @brief   USBPD provider demo file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics International N.V.
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
#if defined(_VDM) || defined(_UVDM) || defined(_VCONN_SUPPORT)
#include "usbpd_core.h"
#include "usbpd_dpm_conf.h"
#include "usbpd_vdm_user.h"
#include "usbpd_dpm_user.h"
#include "demo_application.h"
#if defined(_GUI_INTERFACE)
#include "gui_api.h"
#include "usbpd_gui_memmap.h"
#endif /* _GUI_INTERFACE */
#include "string.h"

/* Private define ------------------------------------------------------------*/
#if defined(_VDM)
#define SVDM_DP_STATUS SVDM_SPECIFIC_1
#define SVDM_DP_CONFIG SVDM_SPECIFIC_2

#define MAX_SVID_USER   1

/*
 * DP Pin assignement
 */
#define  DP_PIN_ASSIGNMENT_NONE 0x00            /*!< De-select pin assignment.  */
#define  DP_PIN_ASSIGNMENT_A    0x01            /*!< Select Pin Assignment A    */
#define  DP_PIN_ASSIGNMENT_B    0x02            /*!< Select Pin Assignment B    */
#define  DP_PIN_ASSIGNMENT_C    0x04            /*!< Select Pin Assignment C    */
#define  DP_PIN_ASSIGNMENT_D    0x08            /*!< Select Pin Assignment D    */
#define  DP_PIN_ASSIGNMENT_E    0x10            /*!< Select Pin Assignment E    */
#define  DP_PIN_ASSIGNMENT_F    0x20            /*!< Select Pin Assignment F    */

/* Pin configs B/D/F support multi-function */
#define MODE_DP_PIN_MF_MASK 0x2a
/* Pin configs A/B support BR2 signaling levels */
#define MODE_DP_PIN_BR2_MASK 0x3
/* Pin configs C/D/E/F support DP signaling levels */
#define MODE_DP_PIN_DP_MASK 0x3c

#define MODE_DP_V13  0x1
#define MODE_DP_GEN2 0x2

#define MODE_DP_MODE_SNK  0x1
#define MODE_DP_MODE_SRC  0x2
#define MODE_DP_MODE_BOTH 0x3

#define MODE_DP_STATUS_CONNECT_NO     0x0    /*!< no (DFP|UFP)_D is connected or disabled */
#define MODE_DP_STATUS_CONNECT_DFP_D  0x1    /*!< DFP_D connected                         */
#define MODE_DP_STATUS_CONNECT_UFP_D  0x2    /*!< UFP_D connected                         */
#define MODE_DP_STATUS_CONNECT_BOTH   0x3    /*!< DFP_D & UFP_D connected                 */

#define DISPLAY_PORT_SVID       0xFF01U          /*!< SVID For Display Port              */
#define PRODUCT_VDO             0xAAAAAAAAU      /*!< Device version + USB Product ID    */

/**
  * @brief Type-C to Plug/Receptacle
  * @{
  */
typedef enum {
  CABLE_TO_PLUG        = 0, /*0b0*/
  CABLE_TO_RECEPTACLE  = 1, /*0b1*/
} USBPD_CableToPR;

/**
  * @}
  */
#endif /* _VDM */

/* Private typedef -----------------------------------------------------------*/
/* GNU Compiler */
#if defined   (__GNUC__)
/* ARM Compiler */
#elif defined   (__CC_ARM)
#pragma anon_unions
/* IAR Compiler */
#elif defined (__ICCARM__)
#endif
#if defined(_VDM)
/*
 * DisplayPort modes capabilities
 */

typedef union
{
  uint32_t d32;
  struct
  {
    uint32_t   SignalDirection  : 2;    /*!< signal direction ( 00b=rsv, 01b=sink, 10b=src 11b=both ) */
    uint32_t   Supports         : 4;    /*!< xxx1: Supports DPv1.3, xx1x Supports USB Gen 2 signaling
                                             Other bits are reserved.                                 */
    uint32_t   PlugOrRecept     : 1;    /*!< Plug | Receptacle (0b == plug, 1b == receptacle)         */
    uint32_t   USB20            : 1;    /*!< USB 2.0 signaling (0b=yes, 1b=no)                        */
    uint32_t   DFP_D_Pin        : 8;    /*!< DFP_D pin assignment supported                           */
    uint32_t   UFP_D_Pin        : 8;    /*!< UFP_D pin assignment supported                           */
    uint32_t   Reserved         : 8;    /*!< Reserved                                                 */
  }d;
}USBPD_DPModeTypeDef;

/*
 * Structure to SVID supported by the devices
 */
typedef struct {
  uint32_t  NumSVIDs;
#ifdef _SRC
  uint16_t  SVIDs[MAX_SVID_USER*2%4];
#else
  uint16_t  SVIDs[MAX_SVID_USER];
#endif /* _SRC */
}USBPD_SVIDUSerInfo_TypeDef;
#endif /* _VDM */

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* List of callbacks for VDM layer */

#if defined(_VDM)
static USBPD_StatusTypeDef USBPD_VDM_DiscoverIdentity(uint8_t PortNum, USBPD_DiscoveryIdentity_TypeDef *pIdentity);
static USBPD_StatusTypeDef USBPD_VDM_DiscoverSVIDs(uint8_t PortNum, uint16_t **p_SVID_Info, uint8_t *nb);
static USBPD_StatusTypeDef USBPD_VDM_DiscoverModes(uint8_t PortNum, uint16_t SVID, uint32_t **p_ModeInfo, uint8_t *nbMode);
static USBPD_StatusTypeDef USBPD_VDM_ModeEnter(uint8_t PortNum, uint16_t SVID, uint32_t ModeIndex);
static USBPD_StatusTypeDef USBPD_VDM_ModeExit(uint8_t PortNum, uint16_t SVID, uint32_t ModeIndex);
static void                USBPD_VDM_InformSVID(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, USBPD_SVIDInfo_TypeDef *pListSVID);
static void                USBPD_VDM_InformMode(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, USBPD_ModeInfo_TypeDef *pModesInfo);
static void                USBPD_VDM_InformModeEnter(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, uint16_t SVID, uint32_t ModeIndex);
static void                USBPD_VDM_SendAttention(uint8_t PortNum, uint8_t *NbData, uint32_t *VDO);
static void                USBPD_VDM_ReceiveAttention(uint8_t PortNum, uint8_t NbData, uint32_t VDO);
static void                USBPD_VDM_SendSpecific(uint8_t PortNum, USBPD_VDM_Command_Typedef VDMCommand, uint8_t *NbData, uint32_t *VDO);
static USBPD_StatusTypeDef USBPD_VDM_ReceiveSpecific(uint8_t PortNum, USBPD_VDM_Command_Typedef VDMCommand, uint8_t *NbData, uint32_t *VDO);
static void                USBPD_VDM_InformSpecific(uint8_t PortNum, USBPD_VDM_Command_Typedef VDMCommand, uint8_t *NbData, uint32_t *VDO);
#endif /* _VDM */
#if defined(_VDM) || defined(_VCONN_SUPPORT)
static void                USBPD_VDM_InformIdentity(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef status, USBPD_DiscoveryIdentity_TypeDef *pIdentity);
#endif /* _VDM || _VCONN_SUPPORT */
#if defined(_UVDM)
static void USBPD_VDM_SendUVDM(uint8_t PortNum, USBPD_UVDMHeader_TypeDef *pUVDM_Header, uint8_t *pNbData, uint32_t *pVDO);
static USBPD_StatusTypeDef USBPD_VDM_ReceiveUVDM(uint8_t PortNum, USBPD_UVDMHeader_TypeDef UVDM_Header, uint8_t *pNbData, uint32_t *pVDO);
#endif /* _UVDM */


/* Private variables ---------------------------------------------------------*/
#ifdef _VDM
#if defined(_GUI_INTERFACE)
USBPD_VDM_SettingsTypeDef       DPM_VDM_Settings[USBPD_PORT_COUNT];
#pragma location=GUI_FLASH_ADDR_DPM_VDM_SETTINGS
const USBPD_VDM_SettingsTypeDef DPM_VDM_FLASH_Settings[USBPD_PORT_COUNT] =
#else
const USBPD_VDM_SettingsTypeDef DPM_VDM_Settings[USBPD_PORT_COUNT] =
#endif /* _GUI_INTERFACE */
{
  {
    .VDM_XID_SOP                = 0x0000AAAAu,    /*!< A decimal number assigned by USB-IF before certification */
    .VDM_USB_VID_SOP            = USBPD_VID,      /*!< A decimal number assigned by USB-IF before certification */
    .VDM_PID_SOP                = 0xAAAAu,        /*!< A unique number assigned by the Vendor ID holder identifying the product. */
    .VDM_ModalOperation         = MODAL_OPERATION_NONSUPP, /*!< Product support Modes based on @ref USBPD_ModalOp_TypeDef */
    .VDM_bcdDevice_SOP          = 0xAAAAu,        /*!< A unique number assigned by the Vendor ID holder containing identity information relevant to the release version of the product. */
    .VDM_USBHostSupport         = USB_NOTCAPABLE, /*!< Indicates whether the UUT is capable of enumerating USB Host */
#if defined(_SNK)
    .VDM_USBDeviceSupport       = USB_CAPABLE, /*!< Indicates whether the UUT is capable of enumerating USB Devices */
#else
    .VDM_USBDeviceSupport       = USB_NOTCAPABLE, /*!< Indicates whether the UUT is capable of enumerating USB Devices */
#endif
#if defined(_SNK_DP)
    .VDM_ProductTypeUFPorCP     = PRODUCT_TYPE_AMA, /*!< Product type UFP or CablePlug of the UUT based on @ref USBPD_ProductType_TypeDef */
#else
    .VDM_ProductTypeUFPorCP     = PRODUCT_TYPE_UNDEFINED, /*!< Product type UFP or CablePlug of the UUT based on @ref USBPD_ProductType_TypeDef */
#endif /* _SNK_DP */
#if defined(USBPD_REV30_SUPPORT)
#if defined(_SRC_DP)
    .VDM_ProductTypeDFP         = PRODUCT_TYPE_AMC, /*!< Product type DFP of the UUT based on @ref USBPD_ProductType_TypeDef */
#else
    .VDM_ProductTypeDFP         = PRODUCT_TYPE_UNDEFINED, /*!< Product type DFP of the UUT based on @ref USBPD_ProductType_TypeDef */
#endif /* _SRC_DP */
#endif /* USBPD_REV30_SUPPORT */
  }
};
#endif /* _VDM */

const USBPD_VDM_Callbacks vdmCallbacks =
{
#if defined(_VDM)
  USBPD_VDM_DiscoverIdentity,
  USBPD_VDM_DiscoverSVIDs,
  USBPD_VDM_DiscoverModes,
  USBPD_VDM_ModeEnter,
  USBPD_VDM_ModeExit,
#else
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
#endif /* _VDM */
  USBPD_VDM_InformIdentity,
#if defined(_VDM)
  USBPD_VDM_InformSVID,
  USBPD_VDM_InformMode,
  USBPD_VDM_InformModeEnter,
  NULL,
  USBPD_VDM_SendAttention,
  USBPD_VDM_ReceiveAttention,
  USBPD_VDM_SendSpecific,
  USBPD_VDM_ReceiveSpecific,
  USBPD_VDM_InformSpecific,
#else
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
#endif /* _VDM */
#if defined(_UVDM)
  USBPD_VDM_SendUVDM,
  USBPD_VDM_ReceiveUVDM,
#endif /* _UVDM */
};

extern USBPD_ParamsTypeDef DPM_Params[USBPD_PORT_COUNT];

#if defined(_VDM)
uint8_t VDM_Mode_On[USBPD_PORT_COUNT];

USBPD_IDHeaderVDO_TypeDef IDHeaderVDO[USBPD_PORT_COUNT];

USBPD_DiscoveryIdentity_TypeDef sIdentity[USBPD_PORT_COUNT];

USBPD_IDHeaderVDO_TypeDef       Remote_IDHeaderVDO[USBPD_PORT_COUNT];
USBPD_CertStatVdo_TypeDef       Remote_CertStatVDO[USBPD_PORT_COUNT];
USBPD_ProductVdo_TypeDef         Remote_ProductVDO[USBPD_PORT_COUNT];
uint16_t Remote_CurrentSVID[USBPD_PORT_COUNT];
uint16_t Remote_SVID_Mode[USBPD_PORT_COUNT];

USBPD_SVIDUSerInfo_TypeDef SVIDInfo[USBPD_PORT_COUNT];
USBPD_SVIDPortPartnerInfo_TypeDef SVIDPortPartner[USBPD_PORT_COUNT];
USBPD_ModeInfo_TypeDef sModesInfo[USBPD_PORT_COUNT];

const USBPD_DPModeTypeDef vdo_dp_modes[USBPD_PORT_COUNT][2] =
{
  {
    {
      .d.UFP_D_Pin        = DP_PIN_ASSIGNMENT_NONE,              /* UFP pin cfg supported : none         */
      .d.DFP_D_Pin        = DP_PIN_ASSIGNMENT_C | DP_PIN_ASSIGNMENT_E, /* DFP pin cfg supported */
      .d.USB20            = 1,              /* USB2.0 signalling even in AMode      */
      .d.PlugOrRecept     = CABLE_TO_PLUG,  /* its a plug                           */
      .d.Supports         = MODE_DP_V13,    /* DPv1.3 Support, no Gen2              */
      .d.SignalDirection  = MODE_DP_MODE_SNK  /* Its a sink only                      */
    },
    {
      .d.UFP_D_Pin        = DP_PIN_ASSIGNMENT_NONE,                  /* UFP pin cfg supported : none     */
      .d.DFP_D_Pin        = DP_PIN_ASSIGNMENT_C | DP_PIN_ASSIGNMENT_D, /* DFP pin cfg supported */
      .d.USB20            = 0,                  /* USB2.0 signalling even in AMode  */
      .d.PlugOrRecept     = CABLE_TO_RECEPTACLE,/* its a receptacle                 */
      .d.Supports         = MODE_DP_V13,        /* DPv1.3 Support, no Gen2          */
      .d.SignalDirection  = MODE_DP_MODE_BOTH   /* Its a sink / src                 */
    }
  },
};

USBPD_DPStatus_TypeDef sDP_Status[USBPD_PORT_COUNT] =
{
  {
    .d.Reserved       = 0,
    .d.IRQ_HPD        = 0,
    .d.HPDState       = 0,
    .d.ExitDPMode     = 0,
    .d.USBConfig      = 0,
    .d.MultiFunction  = 1,
    .d.Enable         = 1,
    .d.PowerLow       = 0,
    .d.ConnectStatus  = MODE_DP_STATUS_CONNECT_DFP_D,
  },
};

#if defined(_GUI_INTERFACE)
USBPD_DPConfig_TypeDef sDP_Config[USBPD_PORT_COUNT] =
{
  {
    .d.SelectConfiguration  = MODE_DP_CONFIG_SELECT_UFP_U_AS_DFP_D,
    .d.Signaling            = MODE_DP_V13, /* Support of DPv1.3 */
    .d.Reserved1            = 0,
    .d.UFP_U_Pin            = DP_PIN_ASSIGNMENT_C,
    .d.Reserved2            = 0,
  },
};
#endif /* _GUI_INTERFACE */
#endif /* _VDM */

/* Private functions ---------------------------------------------------------*/

#if defined(_VDM)
/**
  * @brief  VDM Discovery identity callback (answer to Discover Identity request message)
  * @param  PortNum   current port number
  * @param  pIdentity Pointer on USBPD_DiscoveryIdentity_TypeDef structure
  * @retval USBPD status
  */
static USBPD_StatusTypeDef USBPD_VDM_DiscoverIdentity(uint8_t PortNum, USBPD_DiscoveryIdentity_TypeDef *pIdentity)
{
  IDHeaderVDO[PortNum].d32                      = 0;
#if defined(USBPD_REV30_SUPPORT)
  if ((DPM_Params[PortNum].PE_SpecRevision) > USBPD_SPECIFICATION_REV2)
  {
    IDHeaderVDO[PortNum].b30.VID                  = DPM_VDM_Settings[PortNum].VDM_USB_VID_SOP;
    IDHeaderVDO[PortNum].b30.ModalOperation       = DPM_VDM_Settings[PortNum].VDM_ModalOperation;
    IDHeaderVDO[PortNum].b30.USBHostCapability    = DPM_VDM_Settings[PortNum].VDM_USBHostSupport;
    IDHeaderVDO[PortNum].b30.USBDevCapability     = DPM_VDM_Settings[PortNum].VDM_USBDeviceSupport;
    IDHeaderVDO[PortNum].b30.ProductTypeUFPorCP   = DPM_VDM_Settings[PortNum].VDM_ProductTypeUFPorCP;
    IDHeaderVDO[PortNum].b30.ProductTypeDFP       = DPM_VDM_Settings[PortNum].VDM_ProductTypeDFP;
  }
  else
  {
#endif /* USBPD_REV30_SUPPORT */
    IDHeaderVDO[PortNum].b20.VID                    = DPM_VDM_Settings[PortNum].VDM_USB_VID_SOP;
    IDHeaderVDO[PortNum].b20.ModalOperation         = DPM_VDM_Settings[PortNum].VDM_ModalOperation;
    IDHeaderVDO[PortNum].b20.USBHostCapability      = DPM_VDM_Settings[PortNum].VDM_USBHostSupport;
    IDHeaderVDO[PortNum].b20.USBDevCapability       = DPM_VDM_Settings[PortNum].VDM_USBDeviceSupport;
    IDHeaderVDO[PortNum].b20.ProductTypeUFPorCP     = DPM_VDM_Settings[PortNum].VDM_ProductTypeUFPorCP;
#if defined(USBPD_REV30_SUPPORT)
  }
#endif /* USBPD_REV30_SUPPORT */
  sIdentity[PortNum].IDHeader.d32               = IDHeaderVDO[PortNum].d32;
  sIdentity[PortNum].CertStatVDO.b.XID          = DPM_VDM_Settings[PortNum].VDM_XID_SOP;
  sIdentity[PortNum].ProductVDO.b.USBProductId  = DPM_VDM_Settings[PortNum].VDM_PID_SOP;
  sIdentity[PortNum].ProductVDO.b.bcdDevice     = DPM_VDM_Settings[PortNum].VDM_bcdDevice_SOP;

  sIdentity[PortNum].CableVDO_Presence    = 0;
  sIdentity[PortNum].AMA_VDO_Presence     = 0;
  if (PRODUCT_TYPE_AMA == DPM_VDM_Settings[PortNum].VDM_ProductTypeUFPorCP)
  {
    USBPD_AMAVdo_TypeDef      ama_vdo =
    {
#if defined (USBPD_BILLBOARD)
      .b.AMA_USB_SS_Support   = AMA_USB2P0_BILLBOARD,
#else
      .b.AMA_USB_SS_Support   = AMA_USB2P0_ONLY,
#endif /* USBPD_BILLBOARD */
      .b.VBUSRequirement      = VBUS_REQUIRED,
      .b.VCONNRequirement     = VCONN_NOT_REQUIRED,
      .b.VCONNPower           = VCONN_1W,
      .b.Reserved             = 0x0000,
      .b.AMAFWVersion         = 0x1,
      .b.AMAHWVersion         = 0x1,
    };

    sIdentity[PortNum].AMA_VDO_Presence     = 1;
    sIdentity[PortNum].AMA_VDO.d32          = ama_vdo.d32;
  }

#if defined(USBPD_REV30_SUPPORT)
  if (PRODUCT_TYPE_AMC == DPM_VDM_Settings[PortNum].VDM_ProductTypeDFP)
  {
    USBPD_AMAVdo_TypeDef      ama_vdo =
    {
#if defined (USBPD_BILLBOARD)
      .b.AMA_USB_SS_Support   = AMA_USB2P0_BILLBOARD,
#else
      .b.AMA_USB_SS_Support   = AMA_USB2P0_ONLY,
#endif /* USBPD_BILLBOARD */
      .b.VBUSRequirement      = VBUS_REQUIRED,
      .b.VCONNRequirement     = VCONN_NOT_REQUIRED,
      .b.VCONNPower           = VCONN_1W,
      .b.Reserved             = 0x0000,
      .b.AMAFWVersion         = 0x1,
      .b.AMAHWVersion         = 0x1,
    };

    sIdentity[PortNum].AMA_VDO_Presence     = 1;
    sIdentity[PortNum].AMA_VDO.d32          = ama_vdo.d32;
  }
#endif /* USBPD_REV30_SUPPORT */

  *pIdentity = sIdentity[PortNum];

  return USBPD_ACK;
}

/**
  * @brief  VDM Discover SVID callback (retrieve SVID supported by device for answer to Discovery mode)
  * @param  PortNum        current port number
  * @param  p_SVID_Info    Pointer on USBPD_SVIDInfo_TypeDef structure
  * @param  nb             Pointer on SVIDInfo
  * @retval USBPD status USBPD_ACK, USBPD_NAK, USBPD_BUSY
  */
static USBPD_StatusTypeDef USBPD_VDM_DiscoverSVIDs(uint8_t PortNum, uint16_t **p_SVID_Info, uint8_t *nb)
{
  USBPD_StatusTypeDef _status = USBPD_NAK;
  if (MODAL_OPERATION_SUPPORTED == DPM_VDM_Settings[PortNum].VDM_ModalOperation )
  {
    *nb = SVIDInfo[PortNum].NumSVIDs;
    *p_SVID_Info = SVIDInfo[PortNum].SVIDs;
    _status = USBPD_ACK;
  }
  return _status;
}

/**
  * @brief  VDM Discover Mode callback (report all the modes supported by SVID)
  * @param  PortNum    current port number
  * @param  SVID       SVID ID
  * @param  p_ModeInfo Pointer on USBPD_ModeInfo_TypeDef structure
  * @param  nbMode     pointer to the Discover Mode
  * @retval USBPD status
  */
static USBPD_StatusTypeDef USBPD_VDM_DiscoverModes(uint8_t PortNum, uint16_t SVID, uint32_t **p_ModeInfo, uint8_t *nbMode)
{
  USBPD_StatusTypeDef _status = USBPD_NAK;

  if (MODAL_OPERATION_SUPPORTED == DPM_VDM_Settings[PortNum].VDM_ModalOperation )
  {
    switch(SVID)
    {
    case DISPLAY_PORT_SVID :
      *nbMode  = 2;
      *p_ModeInfo = (uint32_t *)vdo_dp_modes;
      _status = USBPD_ACK;
      break;
    default :
      *nbMode = 0;
      *p_ModeInfo = NULL;
       break;
    }
  }
  return _status;
}
/**
  * @brief  VDM Mode enter callback
  * @param  PortNum current port number
  * @param  SVID      SVID ID
  * @param  ModeIndex Index of the mode to be entered
  * @retval USBPD status
  */
static USBPD_StatusTypeDef USBPD_VDM_ModeEnter(uint8_t PortNum, uint16_t SVID, uint32_t ModeIndex)
{
  USBPD_StatusTypeDef status = USBPD_NAK;

  if(VDM_Mode_On[PortNum] != 0) return status;

  if ((DISPLAY_PORT_SVID == SVID) && (ModeIndex == 1))
  {
    status = USBPD_ACK;
    VDM_Mode_On[PortNum] = 1;
  }

  if ((DISPLAY_PORT_SVID == SVID) && (ModeIndex == 2))
  {
    status = USBPD_ACK;
    VDM_Mode_On[PortNum] = 2;
  }

  return status;
}

/**
  * @brief  VDM Mode exit callback
  * @param  PortNum current port number
  * @param  SVID      SVID ID
  * @param  ModeIndex Index of the mode to be exited
  * @retval USBPD status
  */
static USBPD_StatusTypeDef USBPD_VDM_ModeExit(uint8_t PortNum, uint16_t SVID, uint32_t ModeIndex)
{
  USBPD_StatusTypeDef status = USBPD_NAK;

  if(VDM_Mode_On[PortNum] == 0) return status;

  if ((DISPLAY_PORT_SVID == SVID) && (ModeIndex == 1) && (VDM_Mode_On[PortNum] == 1))
  {
    status = USBPD_ACK;
    VDM_Mode_On[PortNum] = 0;
  }

  if ((DISPLAY_PORT_SVID == SVID) && (ModeIndex == 2) && (VDM_Mode_On[PortNum] == 2))
  {
    status = USBPD_ACK;
    VDM_Mode_On[PortNum] = 0;
  }

  return status;
}

/**
  * @brief  VDM Send Specific message callback
  * @param  PortNum    current port number
  * @param  NbData     Pointer of number of VDO to send
  * @param  VDO        Pointer of VDO to send
  * @retval None
  */
static void USBPD_VDM_SendAttention(uint8_t PortNum, uint8_t *NbData, uint32_t *VDO)
{
  if (USBPD_PORTDATAROLE_DFP == DPM_Params[PortNum].PE_DataRole)
  {
    USBPD_DPStatus_TypeDef dp_status = {0};
    dp_status.d.ConnectStatus = MODE_DP_STATUS_CONNECT_DFP_D;

    *VDO = dp_status.d32;
    *NbData = 1;
  }
  else
  {
    sDP_Status[PortNum].d.ConnectStatus = MODE_DP_STATUS_CONNECT_UFP_D;

    *VDO = sDP_Status[PortNum].d32;
    *NbData = 1;
  }
}

/**
  * @brief  VDM Attention callback to forward information from PE stack
  * @param  PortNum   current port number
  * @param  NbData    Number of received VDO
  * @param  VDO       Received VDO
  * @retval None
  */
static void USBPD_VDM_ReceiveAttention(uint8_t PortNum, uint8_t NbData, uint32_t VDO)
{
  if (USBPD_PORTDATAROLE_DFP == DPM_Params[PortNum].PE_DataRole)
  {
    if (NbData == 1)
    {
      USBPD_DPStatus_TypeDef vdo;
      vdo.d32 = VDO;

      /* Check state of HPD pin in attention message */
      if (0 == vdo.d.HPDState)
      {
        /* Screen has been disconnected... Disconnect laptop */
      }
      else
      {
        /* Screen has been connected... Sent a config message to screen for connection*/
        USBPD_PE_SVDM_RequestSpecific(PortNum, USBPD_SOPTYPE_SOP, SVDM_DP_CONFIG, DISPLAY_PORT_SVID);
      }
    }
  }
  else
  {
    if (NbData == 1)
    {
      /* Check if DP Status has been received in the attention message*/
      USBPD_DPStatus_TypeDef dp_status;
      dp_status.d32 = VDO;

      if (dp_status.d.ConnectStatus == MODE_DP_STATUS_CONNECT_UFP_D)
      {
        USBPD_PE_SVDM_RequestSpecific(PortNum, USBPD_SOPTYPE_SOP, SVDM_DP_CONFIG, DISPLAY_PORT_SVID);
      }
    }
  }
}
#endif /* _VDM */

#if defined(_VDM) || defined(_VCONN_SUPPORT)
/**
  * @brief  VDM Discovery identity callback (answer after sending a Discover Identity req message)
  * @param  PortNum       current port number
  * @param  SOPType       SOP type
  * @param  CommandStatus Command status based on @ref USBPD_VDM_CommandType_Typedef
  * @param  pIdentity     Pointer on the discovery identity information based on @ref USBPD_DiscoveryIdentity_TypeDef
  * @retval None
*/
static void USBPD_VDM_InformIdentity(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, USBPD_DiscoveryIdentity_TypeDef *pIdentity)
{
  switch(CommandStatus)
  {
  case SVDM_RESPONDER_ACK :
#if defined(_VCONN_SUPPORT)
    if (USBPD_SOPTYPE_SOP1 == SOPType)
    {
      uint8_t*  disco_ident;
      disco_ident = (uint8_t*)&DPM_Ports[PortNum].VDM_DiscoCableIdentify;
      memcpy(disco_ident, (uint8_t*)pIdentity, sizeof(USBPD_DiscoveryIdentity_TypeDef));

      DEMO_SetCableInfo(PortNum, pIdentity);
    }
    else
#endif /* _VCONN_SUPPORT */
#if defined(_VDM)
    {
      uint8_t*  disco_ident;
      disco_ident = (uint8_t*)&DPM_Ports[PortNum].VDM_DiscoIdentify;
      memcpy(disco_ident, (uint8_t*)pIdentity, sizeof(USBPD_DiscoveryIdentity_TypeDef));

#if defined(_GUI_INTERFACE)
      if (USBPD_ENABLE != GUI_IsRunning())
#endif /* _GUI_INTERFACE */
      {
        /* Alternate mode presence */
        if (pIdentity->AMA_VDO_Presence == 1)
        {
          /* Request to get SVID */
          USBPD_PE_SVDM_RequestSVID(PortNum, USBPD_SOPTYPE_SOP);
        }
      }
      memset(&DPM_Ports[PortNum].VDM_SVIDPortPartner, 0, sizeof(USBPD_SVIDPortPartnerInfo_TypeDef));
      Remote_CurrentSVID[PortNum]                   = 0;
    }
#endif /* _VDM */
    break;
  case SVDM_RESPONDER_NAK :
    /* Nothing to do */
    break;
  case SVDM_RESPONDER_BUSY :
    /* retry in 50ms */
    break;
  default :
    break;
  }
}
#endif /* _VDM || _VCONN_SUPPORT */

#if defined(_VDM)
/**
  * @brief  Inform SVID callback
  * @param  PortNum       current port number
  * @param  SOPType       SOP type
  * @param  CommandStatus Command status based on @ref USBPD_VDM_CommandType_Typedef
  * @param  pListSVID     Pointer of list of SVID based on @ref USBPD_SVIDInfo_TypeDef
  * @retval USBPD status
  */
static void USBPD_VDM_InformSVID(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, USBPD_SVIDInfo_TypeDef *pListSVID)
{
  uint32_t index = 0;

  switch(CommandStatus)
  {
  case SVDM_RESPONDER_ACK :
    if (1 == DPM_Ports[PortNum].VDM_SVIDPortPartner.FlagAllSVIDReceived)
    {
      /* New list of SVIDs is being received. */
      memset(&DPM_Ports[PortNum].VDM_SVIDPortPartner, 0, sizeof(USBPD_SVIDPortPartnerInfo_TypeDef));
    }
    /* Save the received SVIDs */
    for (index = 0; index < pListSVID->NumSVIDs; index++)
    {
      DPM_Ports[PortNum].VDM_SVIDPortPartner.SVIDs[DPM_Ports[PortNum].VDM_SVIDPortPartner.NumSVIDs++] = pListSVID->SVIDs[index];
    }

    /* Check if all the SVIDs have been received */
    if ((pListSVID->AllSVID_Received == 0) && (0 == (DPM_Ports[PortNum].VDM_SVIDPortPartner.NumSVIDs %12)))
    {
      /* Request a new SVID message */
      USBPD_PE_SVDM_RequestSVID(PortNum, USBPD_SOPTYPE_SOP);
    }
    else
    {
      /* All the SVIDs have been received, request discovery mode on 1st SVID available
      in the list */
      DPM_Ports[PortNum].VDM_SVIDPortPartner.FlagAllSVIDReceived = 1;
      /* Request a discovery mode */
      Remote_SVID_Mode[PortNum] = 0;
#if defined(_GUI_INTERFACE)
      if (USBPD_ENABLE != GUI_IsRunning())
#endif /* _GUI_INTERFACE */
      {
        USBPD_PE_SVDM_RequestMode(PortNum, USBPD_SOPTYPE_SOP, DPM_Ports[PortNum].VDM_SVIDPortPartner.SVIDs[0]);
      }
    }
    break;
  case SVDM_RESPONDER_NAK :
    /* Nothing to do */
    break;
  case SVDM_RESPONDER_BUSY :
    // retry in 50ms
    break;
  default :
	  break;
  }
}

/**
  * @brief  Inform Mode callback
  * @param  PortNum         current port number
  * @param  SOPType         SOP type
  * @param  CommandStatus   Command status based on @ref USBPD_VDM_CommandType_Typedef
  * @param  pModesInfo      Pointer of Modes info based on @ref USBPD_ModeInfo_TypeDef
  * @retval None
  */
static void USBPD_VDM_InformMode(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, USBPD_ModeInfo_TypeDef *pModesInfo)
{
  switch(CommandStatus)
  {
  case SVDM_RESPONDER_ACK :
    {
      USBPD_DPModeTypeDef dp_mode;
      DPM_Ports[PortNum].VDM_ModesPortPartner.NumModes = pModesInfo->NumModes;
      for (uint32_t index = 0; index < DPM_Ports[PortNum].VDM_ModesPortPartner.NumModes; index++)
      {
        DPM_Ports[PortNum].VDM_ModesPortPartner.Modes[index] = pModesInfo->Modes[index];
      }

      DPM_Ports[PortNum].VDM_ModesPortPartner.SVID = pModesInfo->SVID;

      dp_mode.d32 = (DPM_Ports[PortNum].VDM_ModesPortPartner.Modes[0]);

#if defined(_GUI_INTERFACE)
      if (USBPD_ENABLE != GUI_IsRunning())
#endif /* _GUI_INTERFACE */
      {
        /* Enter in the mode only if DFP_D and UFP_D are supported */
        if ((dp_mode.d.SignalDirection == MODE_DP_MODE_BOTH) || (MODE_DP_MODE_SNK == dp_mode.d.SignalDirection))
        {
          /* Request to enter in the 1st mode */
          USBPD_PE_SVDM_RequestModeEnter(PortNum, SOPType, pModesInfo->SVID, 1);
        }
      }
    }
    break;
  case SVDM_RESPONDER_NAK :
#if defined(_GUI_INTERFACE)
    if (USBPD_ENABLE != GUI_IsRunning())
#endif /* _GUI_INTERFACE */
    {
      /* All the SVIDs have been received, request discovery mode on next SVID available
      in the list */
      Remote_SVID_Mode[PortNum]++;
      /* Request a discovery mode */
      USBPD_PE_SVDM_RequestMode(PortNum, USBPD_SOPTYPE_SOP, DPM_Ports[PortNum].VDM_SVIDPortPartner.SVIDs[Remote_SVID_Mode[PortNum]]);
    }
    break;
  case SVDM_RESPONDER_BUSY :
    // retry in 50ms
    break;
  default :
    break;
  }
}

/**
  * @brief  Inform Mode enter callback
  * @param  PortNum   current port number
  * @param  SOPType   SOP type
  * @param  SVID      SVID ID
  * @param  ModeIndex Index of the mode to be entered
  * @retval None
  */
static void USBPD_VDM_InformModeEnter(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, USBPD_VDM_CommandType_Typedef CommandStatus, uint16_t SVID, uint32_t ModeIndex)
{
  switch(CommandStatus)
  {
  case SVDM_RESPONDER_ACK :
#if defined(_GUI_INTERFACE)
    if (USBPD_ENABLE != GUI_IsRunning())
#endif /* _GUI_INTERFACE */
    {
      if (DPM_Ports[PortNum].VDM_SVIDPortPartner.SVIDs[(ModeIndex - 1)] == SVID)
      {
        VDM_Mode_On[PortNum] = 1;

        USBPD_PE_SVDM_RequestSpecific(PortNum, USBPD_SOPTYPE_SOP, SVDM_DP_STATUS, DISPLAY_PORT_SVID);
      }
    }
    break;
  case SVDM_RESPONDER_NAK :
    /* Nothing to do */
    break;
  case SVDM_RESPONDER_BUSY :
    /* retry in 50ms */
    break;
  default :
    break;
  }
}

/**
  * @brief  VDM Fill DP Status
  * @param  PortNum    current port number
  * @param  pDP_Status Pointer on @ref USBPD_DPStatus_TypeDef
  * @retval None
  */
void USBPD_VDM_FillDPStatus(uint8_t PortNum, USBPD_DPStatus_TypeDef *pDP_Status)
{
#if defined(_GUI_INTERFACE)
  memcpy(&sDP_Status[PortNum], pDP_Status, sizeof(USBPD_DPStatus_TypeDef));
#endif /* _GUI_INTERFACE */
}

/**
  * @brief  VDM Fill DP Config
  * @param  PortNum    current port number
  * @param  pDP_Config Pointer on @ref USBPD_DPConfig_TypeDef
  * @retval None
  */
void USBPD_VDM_FillDPConfig(uint8_t PortNum, USBPD_DPConfig_TypeDef *pDP_Config)
{
#if defined(_GUI_INTERFACE)
  memcpy(&sDP_Config[PortNum], pDP_Config, sizeof(USBPD_DPConfig_TypeDef));
#endif /* _GUI_INTERFACE */
}

/**
  * @brief  VDM Send Specific message callback
  * @param  PortNum    current port number
  * @param  VDMCommand VDM command based on @ref USBPD_VDM_Command_Typedef
  * @param  NbData     Pointer of number of VDO to send
  * @param  VDO        Pointer of VDO to send
  * @retval None
  */
static void USBPD_VDM_SendSpecific(uint8_t PortNum, USBPD_VDM_Command_Typedef VDMCommand, uint8_t *NbData, uint32_t *VDO)
{
  if (USBPD_PORTDATAROLE_DFP == DPM_Params[PortNum].PE_DataRole)
  {
    switch(VDMCommand)
    {
    case SVDM_DP_STATUS :
      {
#if defined(_GUI_INTERFACE)
        *VDO = sDP_Status[PortNum].d32;
#else
        USBPD_DPStatus_TypeDef dp_status = {0};
        dp_status.d.ConnectStatus = MODE_DP_STATUS_CONNECT_DFP_D;

        *VDO = dp_status.d32;
#endif /* _GUI_INTERFACE */
        *NbData = 1;
      }
      break;
    case SVDM_DP_CONFIG :
      {
        USBPD_DPModeTypeDef dp_mode;
        dp_mode.d32 = (DPM_Ports[PortNum].VDM_ModesPortPartner.Modes[0]);

        if (DP_PIN_ASSIGNMENT_C == (dp_mode.d.DFP_D_Pin & DP_PIN_ASSIGNMENT_C))
        {
#if defined(_GUI_INTERFACE)
          *VDO = sDP_Config[PortNum].d32;
#else
          USBPD_DPConfig_TypeDef vdo_config = {0};
          vdo_config.d.SelectConfiguration = MODE_DP_CONFIG_SELECT_UFP_U_AS_DFP_D;
          /* Support of DPv1.3 */
          vdo_config.d.Signaling = 0x1;
          vdo_config.d.UFP_U_Pin = DP_PIN_ASSIGNMENT_C;
          *VDO = vdo_config.d32;
#endif /* _GUI_INTERFACE */
          *NbData = 1;
        }
        else
        {
          /* Config not supported */
        }
      }
      break;
    default :
      break;
    }
  }
  else
  {
    switch(VDMCommand)
    {
    case SVDM_DP_STATUS :
      {
        sDP_Status[PortNum].d.ConnectStatus = MODE_DP_STATUS_CONNECT_UFP_D;

        *VDO = sDP_Status[PortNum].d32;
        *NbData = 1;
      }
      break;
    case SVDM_DP_CONFIG :
      *VDO = 0;
      *NbData = 0;
      break;

    default :
      break;
    }
  }
}

/**
  * @brief  VDM Receive Specific message callback
  * @param  PortNum     Current port number
  * @param  VDMCommand  VDM command based on @ref USBPD_VDM_Command_Typedef
  * @param  pNbData     Pointer of number of received VDO and used for the answer
  * @param  pVDO        Pointer of received VDO and use for the answer
  * @retval USBPD status
  */
static USBPD_StatusTypeDef USBPD_VDM_ReceiveSpecific(uint8_t PortNum, USBPD_VDM_Command_Typedef VDMCommand, uint8_t *pNbData, uint32_t *pVDO)
{
  USBPD_StatusTypeDef status = USBPD_ACK;
  if (USBPD_PORTDATAROLE_DFP == DPM_Params[PortNum].PE_DataRole)
  {
    /* Data role is DFP */
    switch(VDMCommand)
    {
    case SVDM_DP_STATUS :
      {
        /* Port0 is configured as SRC */
        sDP_Status[PortNum].d.ConnectStatus = MODE_DP_STATUS_CONNECT_DFP_D;

        *pVDO = sDP_Status[PortNum].d32;
        *pNbData = 1;
      }
      break;
    case SVDM_DP_CONFIG :
      break;

    default :
      status = USBPD_NAK;
      break;
    }
  }
  else
  {
    /* Data role is UFP */
    switch(VDMCommand)
    {
    case SVDM_DP_STATUS :
      {
        USBPD_DPStatus_TypeDef vdo;
        if (*pNbData == 1)
        {
          vdo.d32 = pVDO[0];
          if ((MODE_DP_STATUS_CONNECT_DFP_D == vdo.d.ConnectStatus) || (MODE_DP_STATUS_CONNECT_BOTH == vdo.d.ConnectStatus))
          {
            sDP_Status[PortNum].d.ConnectStatus = MODE_DP_STATUS_CONNECT_UFP_D;

            *pVDO = sDP_Status[PortNum].d32;
            *pNbData = 1;
          }
          else
          {
            status = USBPD_NAK;
          }
        }
        else
        {
          status = USBPD_NAK;
        }
      }
      break;
    case SVDM_DP_CONFIG :
      {
        /* Handle DP configuration according to Plug orientation : to be completed */
        *pVDO = 0;
        *pNbData = 0;
      }
      break;

    default :
      status = USBPD_NAK;
      break;
    }
  }
  return status;
}

/**
  * @brief  VDM Inform Specific callback
  * @param  PortNum    current port number
  * @param  VDMCommand VDM command based on @ref USBPD_VDM_Command_Typedef
  * @param  NbData     Pointer of number of received VDO
  * @param  VDO        Pointer of received VDO
  * @retval status
  */
static void USBPD_VDM_InformSpecific(uint8_t PortNum, USBPD_VDM_Command_Typedef VDMCommand, uint8_t *NbData, uint32_t *VDO)
{
  if (USBPD_PORTDATAROLE_DFP == DPM_Params[PortNum].PE_DataRole)
  {
    switch(VDMCommand)
    {
    case SVDM_DP_STATUS :
      {
        USBPD_DPStatus_TypeDef vdo_received;
        vdo_received.d32 = VDO[0];
        /* SEND SVDM_DP_CONFIG only if Port is connected */
        if (MODE_DP_STATUS_CONNECT_NO != vdo_received.d.ConnectStatus)
        {
          USBPD_PE_SVDM_RequestSpecific(PortNum, USBPD_SOPTYPE_SOP, SVDM_DP_CONFIG, DISPLAY_PORT_SVID);
        }
      }
      break;
    case SVDM_DP_CONFIG :
      /* Handle DP configuration information sending : to be completed */
      break;

    default :
      return;
    }
  }
  else
  {
    switch(VDMCommand)
    {
    case SVDM_DP_STATUS :
      {
        USBPD_DPStatus_TypeDef vdo_received;

        vdo_received.d32 = VDO[0];

        if (vdo_received.d.ConnectStatus != MODE_DP_STATUS_CONNECT_NO)
        {
          sDP_Status[PortNum].d.ConnectStatus = MODE_DP_STATUS_CONNECT_UFP_D;
        }
        else
        {
          /* DP not connected */
          sDP_Status[PortNum].d.ConnectStatus = MODE_DP_STATUS_CONNECT_NO;
        }
      }
      break;
    case SVDM_DP_CONFIG :
      break;

    default :
      break;
    }
  }
}
#endif /* _VDM */

#if defined(_UVDM)
static void USBPD_VDM_SendUVDM(uint8_t PortNum, USBPD_UVDMHeader_TypeDef *pUVDM_Header, uint8_t *pNbData, uint32_t *pVDO)
{
  pUVDM_Header->b.VID = USBPD_VID;
  *pNbData            = 0;
}

static USBPD_StatusTypeDef USBPD_VDM_ReceiveUVDM(uint8_t PortNum, USBPD_UVDMHeader_TypeDef UVDM_Header, uint8_t *pNbData, uint32_t *pVDO)
{
  USBPD_StatusTypeDef _status = USBPD_OK;

  if (USBPD_VID != UVDM_Header.b.VID)
  {
    _status = USBPD_ERROR;
  }
#if defined(_FWUPD)
  else
  {
    /* Request devices to switch to CC loader */
  }
#endif /* _FWUPD */
  return _status;
}
#endif /* _UVDM */

#ifdef _SNK_DP
/**
  * @brief  Initialize HPD Alert pin 
  * @param  PortNum    current port number
  * @retval USBPD status
  */
static USBPD_StatusTypeDef HPD_Init(uint8_t PortNum)
{
  MUX_HPDStateTypeDef hpd_state;
  if (MUX_OK != BSP_MUX_Init(TYPE_C_MUX_2))
  {
    return USBPD_ERROR;
  }

#if defined(_SRC_DP)
  if (MUX_OK != BSP_MUX_Init(TYPE_C_MUX_1))
  {
    return USBPD_ERROR;
  }
//  if (MUX_OK != BSP_MUX_SetHPDState(TYPE_C_MUX_1, HPD_STATE_HIGH))
//  {
//    return USBPD_ERROR;
//  }
//  BSP_MUX_SetDPPinAssignment(TYPE_C_MUX_1, PLUG_ORIENTATION_FLIPPED, DFP_PIN_ASSIGNMMENT_C);
#endif /*_SRC_DP*/

  /* Register HPD callback function */
  if (BSP_MUX_RegisterHPDCallbackFunc(TYPE_C_MUX_2, USBPD_VDM_UserCallback) != MUX_OK)
  {
    return USBPD_ERROR;
  }

  if (MUX_OK != BSP_MUX_GetHPDState(TYPE_C_MUX_2, &hpd_state))
  {
    return USBPD_ERROR;
  }

#if defined(_SRC_DP_NA)
  /* If DP is plugged, set the HPD in pin connected to laptop through DP port */
  if (MUX_OK != BSP_MUX_SetHPDState(TYPE_C_MUX_1, hpd_state))
  {
    return USBPD_ERROR;
  }
#endif /*_SRC_DP*/

  if (HPD_STATE_HIGH == hpd_state)
  {
    sDP_Status[PortNum].d.HPDState = 1;
    sDP_Status[PortNum].d.Enable = 1;
  }
  else
  {
    sDP_Status[PortNum].d.HPDState = 0;
    sDP_Status[PortNum].d.Enable = 0;
  }

  return USBPD_OK;
}

static void USBPD_VDM_UserCallback(MUX_TypeCMuxIdTypeDef TypeCMuxId, 
                            MUX_HPDStateTypeDef   HPDState)
{
  if (HPD_STATE_HIGH == HPDState)
  {
    DEMO_PostMMIMessage(DEMO_MMI_ACTION_HPD_DETECT_HIGH_PORT1);
    sDP_Status[USBPD_PORT_1].d.HPDState = 1;
    sDP_Status[USBPD_PORT_1].d.Enable = 1;
  }
  else
  {
    DEMO_PostMMIMessage(DEMO_MMI_ACTION_HPD_DETECT_LOW_PORT1);
    sDP_Status[USBPD_PORT_1].d.HPDState = 0;
    sDP_Status[USBPD_PORT_1].d.Enable = 1;
  }

  /* Send attention message only if USBPD_PORT_1 is connected*/
  if (USBPD_POWER_EXPLICITCONTRACT == DPM_Params[USBPD_PORT_1].PE_Power)
  {
    USBPD_PE_SVDM_RequestAttention(USBPD_PORT_1, USBPD_SOPTYPE_SOP, DISPLAY_PORT_SVID);
  }
}
#endif /* _SNK_DP */

/* Exported functions ---------------------------------------------------------*/
/**
  * @brief  VDM Initialization function
  * @param  PortNum     Index of current used port
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_VDM_UserInit(uint8_t PortNum)
{
#if defined(_VDM)
  uint32_t index = 0;

   /* Initialize SVID supported by consumer */
  SVIDInfo[PortNum].NumSVIDs = MAX_SVID_USER;

  for (index = 0; index < MAX_SVID_USER; index++)
  {
    SVIDInfo[PortNum].SVIDs[index] = DISPLAY_PORT_SVID + index;
  }
#endif /* _VDM */

  USBPD_PE_InitVDM_Callback(PortNum, (USBPD_VDM_Callbacks *)&vdmCallbacks);

  return USBPD_OK;
}

#if defined(_VDM)
/**
  * @brief  VDM Reset function
  * @retval status
  */
void USBPD_VDM_UserReset(uint8_t PortNum)
{
  /* Reset Port Partner variables*/
  Remote_CurrentSVID[PortNum]       = 0;
  sModesInfo[PortNum].NumModes      = 0;
  SVIDPortPartner[PortNum].NumSVIDs = 0;
  SVIDPortPartner[PortNum].FlagAllSVIDReceived  = 0;
  VDM_Mode_On[PortNum] = 0;
}
#endif /* _VDM */

#endif /* _VDM || _VCONN_SUPPORT */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

