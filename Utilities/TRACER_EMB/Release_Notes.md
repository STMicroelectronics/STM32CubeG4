---
pagetitle: STM32 USB-C Power Delivery embedded Tracer
lang: en
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

::: {.card .fluid}
::: {.sectione .dark}
<center>
# <small>Release Notes for</small> STM32 USB-C Power Delivery embedded Tracer
Copyright &copy; 2018 STMicroelectronics\
    
[![ST logo](_htmresc/st_logo.png)](https://www.st.com){.logo}
</center>
:::
:::

# License

This software component is licensed by ST under Ultimate Liberty license SLA0044, the "License"; 
You may not use this component except in compliance with the License. You may obtain a copy of the License at:

[http://www.st.com/SLA0044](http://www.st.com/SLA0044)

# Purpose

This driver is used for the embedded USB-PD tracer used to debug USB-PD application.
:::

::: {.col-sm-12 .col-lg-8}
# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section2" checked aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">V1.1.0 / 10-April-2019</label>
<div>

## Main Changes

### Maintenance release


## Contents

  Headline
  --------
  Add new defines in tracer_emb_conf_template.h (as done in projects tracer_emb_conf.h files)
  Update tracer file
  remove dependence with USBPD Application
  Update for low power management
  call DPM_TraceWakeUp after added message
  Move macro definition about USART/LPUART inside tracer_em_hw.c
  Change emb_conf to be able to have trace independent of USART or LPUART
  Add LPUART files
  Add management of LPUART1

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
<input type="checkbox" id="collapse-section1" aria-hidden="true">
<label for="collapse-section1" aria-hidden="true">V1.0.0 / 18-Dec.-2018</label>
<div>			

## Main Changes

### Maintenance release

Maintenance release

## Contents

  Headline
  --------
  tracer embedded creation
  
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
