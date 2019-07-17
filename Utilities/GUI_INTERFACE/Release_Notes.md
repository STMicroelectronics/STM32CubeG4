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
:::

::: {.col-sm-12 .col-lg-8}
# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section10" checked aria-hidden="true">
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
