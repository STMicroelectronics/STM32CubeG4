---
pagetitle: Release Notes for Open Bootloader Middleware
lang: en
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

::: {.card .fluid}
::: {.sectione .dark}
<center>
# <small>Release Notes for</small> **Open Bootloader Middleware**
Copyright &copy; 2020 STMicroelectronics\
    
[![ST logo](_htmresc/st_logo.png)](https://www.st.com){.logo}
</center>
:::
:::

# License

Licensed under ST license SLA0044. You may
not use this package except in compliance with the License. You may
obtain a copy of the License at:

[license URL](http://www.st.com/SLA0044)

# Purpose

The **Open Bootloader** package provides an example of an IAP Bootloader that supports all ST Standard Bootloader commands.

The Open Bootloader protocols commands were developed in a manner to be compatible with the Standard Bootloader protocols commands that is programmed by default in the system memory of STM32 devices.

The Open Bootloader is also compatible with STM32CubeProgrammer tool.

In the current release, the Open Bootloader supports USART, FDCAN and I2C protocols.

Here is the list of references to application notes:

- [AN3155](https://www.st.com/resource/en/application_note/CD00264342.pdf) : USART protocol used in the STM32 bootloader
- [AN5405](https://www.st.com/resource/en/application_note/dm00660346.pdf) : FDCAN protocol used in the STM32 bootloader
- [AN4221](https://www.st.com/resource/en/application_note/DM00072315.pdf) : I2C protocol used in the STM32 bootloader

:::

::: {.col-sm-12 .col-lg-8}

# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section2" checked aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">__V2.0.0 / 12-May-2020__</label>
<div>

## Main Changes

### Second release

This is the second official release of Open Bootloader Middleware that adds support for I2C protocol.

The Open Bootloader I2C commands are compatible with the Standard Bootloader I2C commands

- List of implemented commands on I2C protocol:
  - Get commands
  - Get version
  - Get ID
  - Read memory
  - Write memory
  - Non stretch write memory
  - Go command
  - Erase FLASH
  - Non stretch erase FLASH
  - Read protect
  - Non stretch read protect
  - Read unprotect
  - Non stretch read unprotect
  - Write protect
  - Non stretch write protect
  - Write unprotect
  - Non stretch write unprotect

## Known Limitations

- List of missing commands on USART, FDCAN and I2C protocols:
  - Special read command
  - Special write command

## Backward Compatibility

- Compatible with previous release.

## Dependencies

- None.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section1" aria-hidden="true">
<label for="collapse-section1" aria-hidden="true">__V1.0.0 / 01-August-2019__</label>
<div>

## Main Changes

### First release

This is the first official release of Open Bootloader Middleware that supports USART and FDCAN protocols.

The Open Bootloader USART and FDCAN commands are compatible with the Standard Bootloader commands.

- List of implemented commands on USART and FDCAN protocols:
  - Get commands
  - Get version
  - Get ID
  - Read memory
  - Write memory
  - Go command
  - Erase FLASH
  - Read protect
  - Read unprotect
  - Write protect
  - Write unprotect

## Known Limitations

- List of missing commands on USART and FDCAN protocols:
  - Special read command
  - Special write command

## Backward Compatibility

- None.

## Dependencies

- None.

</div>
:::


:::
:::

<footer class="sticky">
For complete documentation on STM32 Microcontrollers, visit: [www.st.com/STM32](http://www.st.com/STM32)

This release note uses up to date web standards and, for this reason, should not
be opened with Internet Explorer but preferably with popular browsers such as
Google Chrome, Mozilla Firefox, Opera or Microsoft Edge.
</footer>
