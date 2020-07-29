---
pagetitle: STM32 USB-C Power Delivery Graphical User Interface (GUI_INTERFACE)
lang: en
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

::: {.card .fluid}
::: {.sectione .dark}
<center>
# <small>Release Notes for</small> STM32 USB-C Power Delivery Graphical User Interface (GUI_INTERFACE)
Copyright &copy; 2018 STMicroelectronics\
    
[![ST logo](_htmresc/st_logo.png)](https://www.st.com){.logo}
</center>
:::
:::

# License

This software component is licensed by ST under BSD 3-Clause license, the "License"; 
You may not use this component except in compliance with the License. You may obtain a copy of the License at:

[http://www.opensource.org/licenses/BSD-3-Clause](http://www.opensource.org/licenses/BSD-3-Clause)

# Purpose

This driver is used in a STM32 application (embedded side) communicating with STM32CubeMonitor-UCPD.

The application can be found in ST website:

[https://www.st.com/en/development-tools/stm32cubemonucpd.html](https://www.st.com/en/development-tools/stm32cubemonucpd.html)

:::

::: {.col-sm-12 .col-lg-8}
# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section14" checked aria-hidden="true">
<label for="collapse-section14" aria-hidden="true">V1.10.2 / 13-May-2020</label>
<div>

## Main Changes

### Maintenance release


## Contents

  Headline
  --------
  Add "usbpd_def.h" include in gui_api.h (usbpd_def.h types used in gui_api.h)

  : Fixed bugs list

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.32.3
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.26
- STM32CubeIDE v1.2.0

## Supported Devices and boards

  All STM32xx devices implementing a USBPD application able to communicate with CubeMonitor_UCPD

## Backward compatibility

  No compatibility break with previous version

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section13" aria-hidden="true">
<label for="collapse-section13" aria-hidden="true">V1.10.1 / 14-Apr.-2020</label>
<div>

## Main Changes

### Maintenance release


## Contents

  Headline
  --------
  User functions for VDM display port should be under switch VDM only

  : Fixed bugs list

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.32.3
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.26
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

  All STM32xx devices implementing a USBPD application able to communicate with CubeMonitor_UCPD

## Backward compatibility

  No compatibility break with previous version

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section12" aria-hidden="true">
<label for="collapse-section12" aria-hidden="true">V1.10.0 / 07-Apr.-2020</label>
<div>

## Main Changes

### Maintenance release


## Contents

  Headline
  --------
  Move TASK ID from Core/inc/usbpd_task.h yo applications/inc/utilities_conf.h
  Add switch USE_STM32_UTILITY_OS to integrate SEQUENCER utility - bis
  Add switch USE_STM32_UTILITY_OS to integrate SEQUENCER utility
  Ticket 81002 - unwanted include file in gui_api.h
  integration of the new utilities SEQ to handle GUI task in non RTOS mode

  : Fixed bugs list

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.32.3
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.26
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

  All STM32xx devices implementing a USBPD application able to communicate with CubeMonitor_UCPD

## Backward compatibility

  No compatibility break with previous version

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section12" aria-hidden="true">
<label for="collapse-section12" aria-hidden="true">V1.9.0 / 15-Nov-2019</label>
<div>

## Main Changes

### Maintenance release


## Contents

  Headline
  --------
  Ticket 74534 - clean code linked to USBPD_CORE_DATATYPE_REQUEST_DO
  Ticket 71492 - PE_Dataswap parameter needs to be duplicated in gui_api
  Ticket 74120 - Compilation issue in case of GUI_INTERFACE use without FreeRTOS
  Ticket 73678 - Initialize CAD_tDRP & CAD_dcSRC_DRP at GUI initialization
  Ticket 73064 - SNK_extended_cap must be answered while SNK or DRP
  Remove warning in NRTOS mode
  Remove dependance with USBPD internal variable

  : Fixed bugs list

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.32.3
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.26
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

  All STM32xx devices implementing a USBPD application able to communicate with CubeMonitor_UCPD

## Backward compatibility

  No compatibility break with previous version

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section11" aria-hidden="true">
<label for="collapse-section11" aria-hidden="true">V1.8.0 / 06-Sept-2019</label>
<div>

## Main Changes

### Maintenance release


## Contents

  Headline
  --------
  [USBPD][GUI] Increase GUI_STACK_SIZE_ADDON_FOR_CMSIS in case of CMSIS OS V2.
  [USBPD][GUI] Ticket 66690 - [FOSS-Audit] SLA0044 headers in Utilities files
  [USBPD][GUI] Ticket 71488 - project settings (and also behavior) differs vs _Gui_Interface switch activation
  [USBPD][GUI] Remove useless include file + use of Nb ports define for USBPD_NbPDO initialisation
  [USBPD][GUI] Update in template of usbpd_gui_memmap_template.h
  [USBPD][GUI] Ticket 70835 - PE_Dataswap parameter needs to be duplicated in gui_api
  [USBPD][GUI] Update in Flash erase function to be able to address F0/G0/G4 and L5 Dual bank flashes
  [USBPD][GUI]rename GUI_NbPDO to USBPD_NbPDO
  [USBPD][GUI] All the DPM_User_Settings should be initialized to be able to pass MQP tests
  [USBPD][GUI] Set DataRoleSwap as TRUE when GUI is enabled
  [USBPD][GUI] Move PE_DataSwap & PE_VconnSwap in the USBPD_USER_SettingsTypeDef for integration in cubeMX
  [USBPD][GUI] Ticket 67078 - Use common defines for XID, VID & PID
  [USBPD][GUI] Ticket 66284 - [CubeMx-UBSPD] Update generated code to make it compatible with CMSIS-RTOS v2
  

  : Fixed bugs list

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.32.3
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.26
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

  All STM32xx devices implementing a USBPD application able to communicate with CubeMonitor_UCPD

## Backward compatibility

  No compatibility break with previous version

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section10" aria-hidden="true">
<label for="collapse-section10" aria-hidden="true">V1.7.0 / 23-May-2019</label>
<div>

## Main Changes

### Maintenance release


## Contents

  Headline
  --------
  Ticket 62471 - Solve Hard Fault called from usage of UCPD voltage calculation using FPU for UCPD
  Ticket 66412 - Allow communication with CubeMonitor-UCPD from CubeMx generated examples (from CubeMx 5.2.1)
  Ticket 66638 - SLA0044 headers in files updated to BSD-3-Clause
  Ticket 67146 - Add memory mapping template in GUI utility (template to be used in user code)

  : Fixed bugs list

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

  All STM32xx devices implementing a USBPD application able to communicate with CubeMonitor_UCPD

## Backward compatibility

  No compatibility break with previous version

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section9" aria-hidden="true">
<label for="collapse-section9" aria-hidden="true">V1.6.0 / 10-April-2019</label>
<div>

## Main Changes

### Maintenance release


## Contents

  Headline
  --------
  Update to wakeup the TX send
  Ticket 62412 - Sink request using GUI seems not coherent with registered SNKPDOs (GUI impact)
  Ticket 62344 - [GUI] Display a warning message in GUI in case of information comes from previous saving done by user
  Remove sModesInfo as no more used
  Re-enable IT when FreeRTOS is used
  Renable SYSTICK after calling FreeRTOS function
  Ticket 61311 - [MB1303] integration of GUI interface
  Add Free text outside _VDM switch
  GUI regression after moving GUI from DPM - use GUIMsgBox instead of DPMMsgBox
  GUIMsgBox used only in case of _RTOS
  Remove dependencies with GUI in DPM USER code
  Manage Free Text
  Ticket 60213 - STM32G071B-DISCO_Demonstrations_USBPD_Analyzer_SW4STM32_Compilation Warnings
  Add include for G4

  : Fixed bugs list

## known limitations

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

## backward compatibility

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section8" aria-hidden="true">
<label for="collapse-section8" aria-hidden="true">V1.5.0 / 28-Jan-2019</label>
<div>

## Main Changes

### Maintenance release

Maintenance release

## Contents

  Headline
  --------
  Ticket 60213 - STM32G071B-DISCO_Demonstrations_USBPD_Analyzer_SW4STM32_Compilation Warnings. Modifications in GUI_INTERFACE module for solving compilation warnings under SW4STM32 (Gcc)

  : Fixed bugs list

## known limitations

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

## backward compatibility

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section7" aria-hidden="true">
<label for="collapse-section7" aria-hidden="true">V1.4.0 / 19-Dec-2018</label>
<div>

## Main Changes

### Maintenance release

Maintenance release

## Contents

  Headline
  --------
  correction link with renaming toogle toggle
  Ticket 57478 - SKEDB not implemented
  Remove extern GUI_Flag variable from application

  : Fixed bugs list

## known limitations

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

## backward compatibility

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section6" aria-hidden="true">
<label for="collapse-section6" aria-hidden="true">V1.3.0 / 10-Dec-2018</label>
<div>

## Main Changes

### Maintenance release

Maintenance release

## Contents

  Headline
  --------
  correction link with renaming toogle toggle
  Ticket 57478 - SKEDB not implemented
  Remove extern GUI_Flag variable from application

  : Fixed bugs list

## known limitations

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

## backward compatibility

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section5" aria-hidden="true">
<label for="collapse-section5" aria-hidden="true">V1.2.0 / 15-Nov-2018</label>
<div>			

## Main Changes

### Maintenance release

Maintenance release

## Contents

  Headline
  --------
  rename DPM_User_Params to GUI_User_Params
  add a link with TRACER_EMB
  solve compilation issue

  
  : Fixed bugs list

## known limitations

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

## backward compatibility

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section4" aria-hidden="true">
<label for="collapse-section4" aria-hidden="true">V1.1.2 / 25-Sept.-2018</label>
<div>			

## Main Changes

### Maintenance release

Maintenance release

## Contents

  Headline
  --------
  Update License information
  Solve compilation issue when using IAR V8.30.1
  Add ping feature 

  
  : Fixed bugs list

## known limitations

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

## backward compatibility

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section3" aria-hidden="true">
<label for="collapse-section3" aria-hidden="true">V1.1.1 / 10-Sept.-2018</label>
<div>			

## Main Changes

### Maintenance release

Maintenance release

## Contents

  Headline
  --------
  Remove compilation warning
  
  : Fixed bugs list

## known limitations

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

## backward compatibility

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section2" aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">V1.1.0 / 06-Sept.-2018</label>
<div>			

## Main Changes

### Maintenance release

Maintenance release

## Contents

  Headline
  --------
  Add cable information managment
  Update around the VDM data (SVID, SVDM modes, ... )

  
  : Fixed bugs list

## known limitations

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

## backward compatibility

## Dependencies

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section2" aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">V1.0.0 / 19-Jun-2018</label>
<div>			

## Main Changes

### Maintenance release

Maintenance release

## Contents

  Headline
  --------
  First official version for STM32 USB-PD GUI Interface utility. This utility enables communication between USB-PD device (controlled on STM32 side) and USB-PD PC tool

  
  : Fixed bugs list

## known limitations

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

## backward compatibility

## Dependencies

</div>
:::

:::
:::

<footer class="sticky">
For complete documentation on STM32,visit: [[www.st.com/stm32](http://www.st.com)]

This release note uses up to date web standards and, for this reason, should not be opened with Internet Explorer
but preferably with popular browsers such as Google Chrome, Mozilla Firefox, Opera or Microsoft Edge.
</footer>
