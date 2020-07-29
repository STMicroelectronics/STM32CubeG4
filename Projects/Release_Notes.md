---
pagetitle: Release Notes for Projects of STM32CubeG4 Firmware Package
lang: en
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

::: {.card .fluid}

::: {.sectione .dark}
<center>
# <small>Release Notes for</small> **Projects of STM32CubeG4 Firmware Package**
Copyright &copy; 2019 STMicroelectronics\

[![ST logo](../_htmresc/st_logo.png)](https://www.st.com){.logo}
</center>

:::

:::

# License

The projects covered by these release notes are delivered under different
licenses. Details of applicable license for each project are provided in the
table of the \"Contents\" section below.

# Purpose

The projects are a set of applicative tasks that aim to explain and provide use
cases of the different products features based on the products hardware (Boards,
STM32 Core(s) features, interconnections and peripherals) and built around the
different firmware components. They can be directly used or adapted by the
STM32CubeG4 Firmware Package users.
They provide basic functionalities, show main features or demonstrate
performance. They are provided with preconfigured projects for the main
supported toolchains.

In the STM32CubeG4 Firmware Package, the projects are organized in a sequential scheme based on the following levels:

- Boards : Hardware platform (evaluation, discovery, nucleo boards)
- Project Category:
  - Applications
  - Demonstrations
  - Examples
  - Examples_LL
  - Examples_MIX
  - Templates
  - Templates_LL

The exhaustive list of projects is provided in this table:
[STM32CubeProjectsList](STM32CubeProjectsList.html).

:::

::: {.col-sm-12 .col-lg-8}
# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section4" checked aria-hidden="true">
<label for="collapse-section4" aria-hidden="true">V1.2.0 / 14-February-2020</label>
<div>

## Main Changes

### Maintenance release

Maintenance release of STM32CubeG4 (STM32Cube for STM32G4 Series) Firmware projects supporting STM32G431xx/31xx/71xx/73xx/74xx devices.

 Headline
 ----------------------------------------------------------
  - General updates to fix known defects and enhancements implementation
  - Add support for CubeIDE Toolchain to replace SW4STM32
  - Add missing MDK-ARM projects
  - Add new OpenBootloader application runnig on STM32G474E-EVAL board
  - USBPD Aplications: general corrections for conformance testing
    - Update of structures definitions and declarations in Inc/usbpd_dpm_user.h, Inc/usbpd_vdm_user.h and Inc/usbpd_dpm_conf.h
    - Update of PDO definition for ability to support connection with UCPD-Monitor in Inc/usbpd_pdo_defs.h
    - Update of services for VBUS management in Inc/usbpd_pwr_if.h, Inc/usbpd_pwr_user.h and Src/usbpd_pwr_if.c
    - Align Src/usbpd_dpm_core.c with latest USBPD Core library
    - New template of default functions added in Inc/usbpd_dpm_user.h and Src/usbpd_dpm_user.c

  : Additional features


## Contents

### Projects
 
  Board                     Project category          Version     License                                                          Path
  ------------------------- -----------------------   ----------- ------------------------------------------------------------     --------------------------------------------------------------------------------------------------------------------
  NUCLEO-G431KB             Demonstrations             v1.2.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G431KB/Demonstrations
  NUCLEO-G431KB             Applications               v1.2.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G431KB/Applications
  NUCLEO-G431KB             HAL Examples               v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431KB/Examples
  NUCLEO-G431KB             HAL Templates              v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431KB/Templates
  NUCLEO-G431KB             LL Templates               v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431KB/Templates_LL
  NUCLEO-G431RB             Applications               v1.2.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G431RB/Applications
  NUCLEO-G431RB             Demonstrations             v1.2.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G431RB/Demonstrations
  NUCLEO-G431RB             HAL Examples               v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431RB/Examples
  NUCLEO-G431RB             LL Examples                v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431RB/Examples_LL
  NUCLEO-G431RB             MIX HAL/LL Examples        v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431RB/Examples_MIX
  NUCLEO-G431RB             HAL Templates              v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431RB/Templates
  NUCLEO-G431RB             LL Templates               v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431RB/Templates_LL
  NUCLEO-G474RE             Applications               v1.2.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G474RE/Applications
  NUCLEO-G474RE             Demonstrations             v1.2.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G474RE/Demonstrations
  NUCLEO-G474RE             HAL Examples               v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Examples
  NUCLEO-G474RE             LL Examples                v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Examples_LL
  NUCLEO-G474RE             MIX HAL/LL Examples        v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Examples_MIX
  NUCLEO-G474RE             HAL Templates              v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Templates
  NUCLEO-G474RE             LL Templates               v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Templates_LL
  STM32G474E-EVAL           Applications               v1.2.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/STM32G474E-EVAL/Applications
  STM32G474E-EVAL           Demonstrations             v1.2.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/STM32G474E-EVAL/Demonstrations
  STM32G474E-EVAL           HAL Examples               v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/STM32G474E-EVAL/Examples
  STM32G474E-EVAL           HAL Templates              v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/STM32G474E-EVAL/Templates
  STM32G474E-EVAL           LL Templates               v1.2.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/STM32G474E-EVAL/Templates_LL
  B-G474E-DPOW1             Applications               v1.1.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/B-G474E-DPOW1/Applications
  B-G474E-DPOW1             Demonstrations             v1.1.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/B-G474E-DPOW1/Demonstrations
  B-G474E-DPOW1             HAL Examples               v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/B-G474E-DPOW1/Examples
  B-G474E-DPOW1             MIX HAL/LL Examples        v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/B-G474E-DPOW1/Examples_MIX
  B-G474E-DPOW1             HAL Templates              v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/B-G474E-DPOW1/Templates
  B-G474E-DPOW1             LL Templates               v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/B-G474E-DPOW1/Templates_LL  
    
  : Projects  :  
  

\


## Known Limitations


## Development Toolchains and Compilers


  Name                                                                 Version                                             
  ------------------------------------------------------------------- -------------------------------------------------    
  IAR Embedded Workbench for ARM (EWARM)toolchain                      V8.32.3                                             
  RealView Microcontroller DevelopmentKit (MDK-ARM) toolchain          V5.27.1                                               
  System Workbench for STM32 (SW4STM32)toolchain                       V2.9                                                

  : Development Toolchains :

## Supported Devices and boards

  Name                                                     
  -------------------------------------------------------- 
  STM32G431xx, STM32G441xx
  STM32G471xx
  STM32G473xx, STM32G483xx
  STM32G474xx, STM32G484xx

  : Supported Devices :


  Name                                                     Reference
  -------------------------------------------------------- -----------------------------------------------------------
  NUCLEO-G431KB Nucleo32 kit                               MB1430 A-01
  NUCLEO-G431RB Nucleo kit                                 MB1367 C-01
  NUCLEO-G474RE Nucleo kit                                 MB1367 C-01
  STM32G474E-EVAL Evaluation Board                         MB1397 B-01
  B-G474E-DPOW1 Discovery Kit                              MB1428 B-01


  : Supported Boards :
  
## Backward Compatibility

None

## Other compatibilities

This software release is compatible with:

   - STM32CubeMX V5.6.0
</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section3" checked aria-hidden="true">
<label for="collapse-section3" aria-hidden="true">V1.1.0 / 28-June-2019</label>
<div>

## Main Changes

### Maintenance release

Maintenance release of STM32CubeG4 (STM32Cube for STM32G4 Series) Firmware projects supporting STM32G431xx/31xx/71xx/73xx/74xx devices
and introducing Discovery Kit B-G474E-DPOW1.

 Headline
 ----------------------------------------------------------
 Update FatFS applications and demonstrations to align with new FatFs middleware version and BSP board driver
 Update UCPD applications and demonstrations to align with new UCPD middleware and new Utilities version 

  : Additional features


## Contents

### Projects
 
  Board                     Project category          Version     License                                                          Path
  ------------------------- -----------------------   ----------- ------------------------------------------------------------     --------------------------------------------------------------------------------------------------------------------
  NUCLEO-G431KB             Demonstrations             v1.1.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G431KB/Demonstrations
  NUCLEO-G431KB             Applications               v1.1.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G431KB/Applications
  NUCLEO-G431KB             HAL Examples               v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431KB/Examples
  NUCLEO-G431KB             HAL Templates              v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431KB/Templates
  NUCLEO-G431KB             LL Templates               v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431KB/Templates_LL
  NUCLEO-G431RB             Applications               v1.1.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G431RB/Applications
  NUCLEO-G431RB             Demonstrations             v1.1.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G431RB/Demonstrations
  NUCLEO-G431RB             HAL Examples               v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431RB/Examples
  NUCLEO-G431RB             LL Examples                v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431RB/Examples_LL
  NUCLEO-G431RB             MIX HAL/LL Examples        v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431RB/Examples_MIX
  NUCLEO-G431RB             HAL Templates              v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431RB/Templates
  NUCLEO-G431RB             LL Templates               v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431RB/Templates_LL
  NUCLEO-G474RE             Applications               v1.1.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G474RE/Applications
  NUCLEO-G474RE             Demonstrations             v1.1.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G474RE/Demonstrations
  NUCLEO-G474RE             HAL Examples               v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Examples
  NUCLEO-G474RE             LL Examples                v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Examples_LL
  NUCLEO-G474RE             MIX HAL/LL Examples        v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Examples_MIX
  NUCLEO-G474RE             HAL Templates              v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Templates
  NUCLEO-G474RE             LL Templates               v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Templates_LL
  STM32G474E-EVAL           Applications               v1.1.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/STM32G474E-EVAL/Applications
  STM32G474E-EVAL           Demonstrations             v1.1.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/STM32G474E-EVAL/Demonstrations
  STM32G474E-EVAL           HAL Examples               v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/STM32G474E-EVAL/Examples
  STM32G474E-EVAL           HAL Templates              v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/STM32G474E-EVAL/Templates
  STM32G474E-EVAL           LL Templates               v1.1.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/STM32G474E-EVAL/Templates_LL
  B-G474E-DPOW1             Applications               v1.0.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/B-G474E-DPOW1/Applications
  B-G474E-DPOW1             Demonstrations             v1.0.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/B-G474E-DPOW1/Demonstrations
  B-G474E-DPOW1             HAL Examples               v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/B-G474E-DPOW1/Examples
  B-G474E-DPOW1             MIX HAL/LL Examples        v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/B-G474E-DPOW1/Examples_MIX
  B-G474E-DPOW1             HAL Templates              v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/B-G474E-DPOW1/Templates
  B-G474E-DPOW1             LL Templates               v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/B-G474E-DPOW1/Templates_LL  
    
  : Projects  :  
  

\


## Known Limitations


## Development Toolchains and Compilers


  Name                                                                 Version                                             
  ------------------------------------------------------------------- -------------------------------------------------    
  IAR Embedded Workbench for ARM (EWARM)toolchain                      V8.20.2                                             
  RealView Microcontroller DevelopmentKit (MDK-ARM) toolchain          V5.25                                               
  System Workbench for STM32 (SW4STM32)toolchain                       V2.4                                                

  : Development Toolchains :

## Supported Devices and boards

  Name                                                     
  -------------------------------------------------------- 
  STM32G431xx, STM32G441xx
  STM32G471xx
  STM32G473xx, STM32G483xx
  STM32G474xx, STM32G484xx

  : Supported Devices :


  Name                                                     Reference
  -------------------------------------------------------- -----------------------------------------------------------
  NUCLEO-G431KB Nucleo32 kit                               MB1430 A-01
  NUCLEO-G431RB Nucleo kit                                 MB1367 C-01
  NUCLEO-G474RE Nucleo kit                                 MB1367 C-01
  STM32G474E-EVAL Evaluation Board                         MB1397 B-01
  B-G474E-DPOW1 Discovery Kit                              MB1428 B-01


  : Supported Boards :
  
## Backward Compatibility

None

## Other compatibilities

This software release is compatible with:

   - STM32CubeMX V5.3.0
</div>
:::


::: {.collapse}
<input type="checkbox" id="collapse-section2" aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">V1.0.1 / 29-May-2019</label>
<div>

## Main Changes

### Patch Release

HRTIM/BasicPWM project released in V1.0.0 is now split in 4 different projects on NUCLEO-G474RE (using HAL and Low-layer interfaces):
   
  - HRTIM_Basic_ArbitraryWaveform

  - HRTIM/HRTIM_Basic_MultiplePWM

  - HRTIM/HRTIM_Basic_PWMMaster

  - HRTIM_Basic_SinglePWM

## Contents

### Projects
 
  Board                     Project category          Version     License                                                          Path
  ------------------------- -----------------------   ----------- ------------------------------------------------------------     --------------------------------------------------------------------------------------------------------------------
  NUCLEO-G474RE             HAL Examples               v1.0.1     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Examples
  NUCLEO-G474RE             LL Examples                v1.0.1     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Examples_LL
  
  : Projects  :  

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section1" aria-hidden="true">
<label for="collapse-section1" aria-hidden="true">V1.0.0 / 12-April-2019</label>
<div>

## Main Changes

### First release

First Official release of STM32CubeG4 (STM32Cube for STM32G4 Series) Firmware projects supporting STM32G431xx/31xx/71xx/73xx/74xx devices
and introducing NUCLEO-G431KB, NUCLEO-G431RB, NUCLEO-G474RE and STM32G474E-EVAL boards.


## Contents

### Projects
 
  Board                     Project category          Version     License                                                          Path
  ------------------------- -----------------------   ----------- ------------------------------------------------------------     --------------------------------------------------------------------------------------------------------------------
  NUCLEO-G431KB             Applications               v1.0.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G431KB/Applications
  NUCLEO-G431KB             Demonstrations             v1.0.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G431KB/Demonstrations
  NUCLEO-G431KB             HAL Examples               v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431KB/Examples
  NUCLEO-G431KB             HAL Templates              v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431KB/Templates
  NUCLEO-G431KB             LL Templates               v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431KB/Templates_LL
  NUCLEO-G431RB             Applications               v1.0.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G431RB/Applications
  NUCLEO-G431RB             Demonstrations             v1.0.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G431RB/Demonstrations
  NUCLEO-G431RB             HAL Examples               v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431RB/Examples
  NUCLEO-G431RB             LL Examples                v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431RB/Examples_LL
  NUCLEO-G431RB             MIX HAL/LL Examples        v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431RB/Examples_MIX
  NUCLEO-G431RB             HAL Templates              v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431RB/Templates
  NUCLEO-G431RB             LL Templates               v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G431RB/Templates_LL
  NUCLEO-G474RE             Applications               v1.0.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G474RE/Applications
  NUCLEO-G474RE             Demonstrations             v1.0.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/NUCLEO-G474RE/Demonstrations
  NUCLEO-G474RE             HAL Examples               v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Examples
  NUCLEO-G474RE             LL Examples                v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Examples_LL
  NUCLEO-G474RE             MIX HAL/LL Examples        v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Examples_MIX
  NUCLEO-G474RE             HAL Templates              v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Templates
  NUCLEO-G474RE             LL Templates               v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/NUCLEO-G474RE/Templates_LL
  STM32G474E-EVAL           Applications               v1.0.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/STM32G474E-EVAL/Applications
  STM32G474E-EVAL           Demonstrations             v1.0.0     [SLA0044](http://www.st.com/SLA0044)                             Projects/STM32G474E-EVAL/Demonstrations
  STM32G474E-EVAL           HAL Examples               v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/STM32G474E-EVAL/Examples
  STM32G474E-EVAL           HAL Templates              v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/STM32G474E-EVAL/Templates
  STM32G474E-EVAL           LL Templates               v1.0.0     [BSD 3-Clause](https://opensource.org/licenses/BSD-3-Clause)     Projects/STM32G474E-EVAL/Templates_LL
  
  : Projects  :  
  

\


## Known Limitations

- STM32G474E-EVAL : Project Template in HAL is not yet ported on Keil and SW4STM32 toolchains 
- USB Device : Warning in compilation of Keil and SW4STM32 Toolchains ("__ALIGN_END" is redefined in the usbd_def.h)

## Development Toolchains and Compilers


  Name                                                                 Version                                             
  ------------------------------------------------------------------- -------------------------------------------------    
  IAR Embedded Workbench for ARM (EWARM)toolchain                      V8.20.2                                             
  RealView Microcontroller DevelopmentKit (MDK-ARM) toolchain          V5.25                                               
  System Workbench for STM32 (SW4STM32)toolchain                       V2.4                                                

  : Development Toolchains :

## Supported Devices and boards

  Name                                                      Reference
  -------------------------------------------------------- -----------------------------------------------------------
  STM32G431xx, STM32G441xx
  STM32G471xx, STM32G473xx, STM32G474xx and STM32G484xx
  NUCLEO-G431KB Nucleo32 kit                                   MB1430 A-01
  NUCLEO-G431RB Nucleo kit                                     MB1367 C-01
  NUCLEO-G474RE Nucleo kit                                     MB1367 C-01
  STM32G474E-EVAL Evaluation Board                             MB1397 B-01
  
## Backward Compatibility

None

## Other compatibilities

This software release is compatible with:

   - STM32CubeMX V5.2.0
</div>
:::

:::

:::

<footer class="sticky">
For complete documentation on STM32G4xx,visit: [[www.st.com/stm32g4](http://www.st.com/stm32g4)]

*This release note uses up to date web standards and, for this reason, should not be opened with Internet Explorer but preferably with popular browsers such as Google Chrome, Mozilla Firefox, Opera or Microsoft Edge.*
</footer>
