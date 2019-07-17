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
<input type="checkbox" id="collapse-section6" checked aria-hidden="true">
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
  Removal of usbpd\_pwr.c/.h files : BSP PWR functions defined at Device level are now expected to be provided by STM32G474E EVAL BSP component.
  Alignment on APIs, following latest updates done in all STM32xx
    USPBD Devices components
  Update of usbpd\_devices\_conf\_template.h file.

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
  Remove BSP reference, now managed trough the header file usbpd\_devices\_conf.h
  Update for errorrecovery management

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
