---
pagetitle: Release Notes for STM32 USB-C Power Delivery G4 Device Driver
lang: en
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

::: {.card .fluid}

::: {.sectione .dark}
<center>
# <small>Release Notes for</small> STM32 USB-C Power Delivery G4 Device Driver
Copyright &copy; 2019 STMicroelectronics\
    
[![ST logo](_htmresc/st_logo.png)](https://www.st.com){.logo}
</center>
:::
:::

# License

This software component is licensed by ST under Ultimate Liberty license
SLA0044, the \"License\"; You may not use this component except in
compliance with the License. You may obtain a copy of the License at:

[http://www.st.com/SLA0044](http://www.st.com/SLA0044)

# Purpose

The USB-PD device driver provides a set of functions to manage the physical layer (i.e. low level of the type C state machine and low level of message transport). This includes :

- Type C state machine: SRC, SNK or DRP

- Physical layer : message handling SOP, SOP', SOP'', HARDRESET, ...

- Timer server to handle GOODCRC, PRL repetition timing

The USB-PD library is developed following the Universal Serial Bus Power Delivery Specification Revision 3.0, V1.2 (June 06, 2018) and Universal Serial Bus type-C Cable 
and Connector Specification, Revision 1.3 (July 14, 2017).
:::

::: {.col-sm-12 .col-lg-8}
# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section11" checked aria-hidden="true">
<label for="collapse-section11" aria-hidden="true">V4.0.1 / 6-June-2020</label>
<div>

## Main Changes

### Maintenance release

  Headline
  ----------------------------------------------------------
  Ticket 87567 - Source path are missing for MW and Utilities
  Ticket 87584 - Build issue: usbpd_timeserver.h: No such file or directory in NO_PD configuration

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.32.3
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.27
- STM32CubeIDE V1.2.0

## Supported Devices and boards

  All STM32G4xx devices embedding USBPD IP

## Backward compatibility


## Dependencies

 This software release is compatible with USB-C Power Delivery Core Stack Library v3.0.0 

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section10" aria-hidden="true">
<label for="collapse-section10" aria-hidden="true">V4.0.0 / 26-Mar-2020</label>
<div>

## Main Changes

### Maintenance release

  Headline
  ----------------------------------------------------------
  Ticket 81963 - Add inside PRL a mechanism to avoid RX buffer overwrite in case of multiple RX
  Ticket 81964 - Add an optionnal mechansime to manage the tx discard and tx abort by UCPD IP. Interface modifications with USBPD/Core library.
  Correction of CC line not updated in SRC_NO_PD configuration (Ellisys failed TD.PD.C.E1 DFP Attach/Detach Detection)
  Ticket 80142 - Update done to integrate NO PD stack
  Code alignement between all STM32 series USBPD Device modules (NO PD implementation and error recovery)

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.32.3
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.27
- STM32CubeIDE V1.2.0

## Supported Devices and boards

  All STM32G4xx devices embedding USBPD IP

## Backward compatibility

  Implementation of CR Ticket 81963 and Ticket 81964 leads to some interface changes between USBPD Core and USBPD Device driver modules.
  In order to use thise version of USBPD Device driver module, please ensure version of USBPD Core module is v3.0.0 or higher.

## Dependencies

  This software release is compatible with USB-C Power Delivery Core Stack Library v3.0.0 

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section8" aria-hidden="true">
<label for="collapse-section8" aria-hidden="true">v3.1.0 / 04-Feb-2020</label>
<div>

## Main Changes

### Maintenance release

  Headline
  ----------------------------------------------------------
  Ticket 79632 - 4.4.6 SNKAS Connect Audio failed on Ellisys
  Ticket 80142 - Update done to integrate NO PD stack
  Update for the tests: TD.4.4.4 TD.4.4.5
  Ticket 73523 - Implementation of sink with accessory support
  Update Disable Battery LL PWR API to new name : LL_PWR_DisableUCPDDeadBattery


## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.32.3
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.27
- STM32CubeIDE v1.2.0

## Supported Devices and boards

  All STM32G4xx devices embedding USBPD IP

## Backward compatibility

  No compatibility break with previous version

## Dependencies

 This software release is compatible with USB-C Power Delivery Core Stack Library v2.9.0

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section7"  aria-hidden="true">
<label for="collapse-section7" aria-hidden="true">V3.0.0 / 20-Dec-2019</label>
<div>

## Main Changes

### Maintenance release

  Headline
  ----------------------------------------------------------
  Solve randomness reloading of CC1 and CC2 values in local variables in CAD_Check_HW() function according to optimizations options, code sequences.
  Add dynamic voltage disconnect for a SNK
  Fix bug to initialize the default Rp
  Ticket 70982 - Ellisys 4.9.4 Source Vconn Swap (Testing Ufp) failed
  Ticket 76935 - CAD power role overwritten in configuration _SRC & _SNK
  Alignement with G0 (NO PD implementation and error recovery)
  Ticket 74124 - Issue with CC2 (PB4) default mode with G4


## Known Limitations

  Outstanding bugs list : 
  
  Identified issue on LeCroy test 4.10.2 regarding remaining VBUS voltage when externally powered.
  TC 4.10.2 OK requires either a Resistor value change on G4 EVAL board (MB1397) or an update in BSP drivers
  => Ticket 76048 - [EVAL-G4] UCPD VBUS value when 5V external supply connected

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.32.3
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.26
- STM32CubeIDE v1.2.0

## Supported Devices and boards

  All STM32G4xx devices embedding USBPD IP

## Backward compatibility

  Application needs to provide new interface to manage introduction of dynamic voltage (USBPD_PWR_IF_GetVBUSStatus function).

## Dependencies

 This software release is compatible with USB-C Power Delivery Core Stack Library v2.9.0 

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section6" aria-hidden="true">
<label for="collapse-section6" aria-hidden="true">V2.8.0 / 06-May-2019</label>
<div>

## Main Changes

### Maintenance release

  Headline
  ----------------------------------------------------------
  Update of G4 USBPD device following new version of BSP APIs for USBPD Power management (BSP V2)
  Update of Device state state machines : definition of new functions for each Attached and AttachedWait states
  Addition of support of G4 Discovery board B-G474E-DPOW1 (MB1428) in usbpd\\_devices\\_conf\\_template.h file.
  Low Power consumption specific code has been added under compilation switch (Not enabled yet)
  Update of Retry timer value (945 to 905 microsecs)


## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

  All STM32G4xx devices embedding USBPD IP

## Backward compatibility

  No compatibility break with previous version

## Dependencies

 This software release is compatible with USB-C Power Delivery Core Stack Library v2.6.0 

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section5" aria-hidden="true">
<label for="collapse-section5" aria-hidden="true">V2.7.0 / 25-Mar-2019</label>
<div>			

## Main Changes

### Maintenance release

  Headline
  ----------------------------------------------------------
  Removal of usbpd_pwr.c/.h files : BSP PWR functions defined at Device level are now expected to be provided by STM32G474E EVAL BSP component.
  Alignment on APIs, following latest updates done in all STM32xx
  USPBD Devices components
  Update of usbpd_devices_conf_template.h file.

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

  All STM32G4xx devices embedding USBPD IP

## Backward compatibility

  No compatibility break with previous version

## Dependencies

  NA 

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section4" aria-hidden="true">
<label for="collapse-section4" aria-hidden="true">V2.6.0 / 23-Jan-2019</label>
<div>			

## Main Changes

### Maintenance release

  Headline
  ----------------------------------------------------------
  Editorial modifications (Astyle)
  Update in Discharge handling for MB1397 when VBUS is not supplied

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

  All STM32G4xx devices embedding USBPD IP

## Backward compatibility

  No compatibility break with previous version

## Dependencies

  NA 

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section3" aria-hidden="true">
<label for="collapse-section3" aria-hidden="true">V2.5.0 / 10-Dec-2018</label>
<div>			

## Main Changes

### Maintenance release

  Headline
  ----------------------------------------------------------
  Editorial correction (renaming toogle by toggle)

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

  All STM32G4xx devices embedding USBPD IP

## Backward compatibility

  No compatibility break with previous version

## Dependencies

  NA 

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section2" aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">V2.4.0 / 15-Nov-2018</label>
<div>			

## Main Changes

### Initial release

  Headline
  ----------------------------------------------------------
  First official version for STM32G4xx device (source code available)
  Ready for cubemx integration
  Remove BSP reference, now managed trough the header file usbpd_devices_conf.h
  Update for errorrecovery management

## Known Limitations

  Outstanding bugs list : None

  Requirements not met or planned in a forthcoming release : None

## Development Toolchains and Compilers

- IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.2
- RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.25
- System Workbench STM32 (SW4STM32) toolchain V2.7.2

## Supported Devices and boards

  All STM32G4xx devices embedding USBPD IP

## Backward compatibility

  No compatibility break with previous version

## Dependencies

  NA 

</div>
:::

:::
:::

<footer class="sticky">
For complete documentation on **STM32 32-bit Arm Cortex MCUs**,
visit: [http://www.st.com/STM32](http://www.st.com/STM32)

This release note uses up to date web standards and, for this reason, should not
be opened with Internet Explorer but preferably with popular browsers such as
Google Chrome, Mozilla Firefox, Opera or Microsoft Edge.
</footer>
