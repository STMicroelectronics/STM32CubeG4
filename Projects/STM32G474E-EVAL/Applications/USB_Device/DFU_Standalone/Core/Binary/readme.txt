/**
  @page Binary Description of the binary template

  @verbatim
  ******************************************************************************
  * @file    USB_Device/DFU_Standalone/Core/Binary/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the USB DFU application binary file.
  ******************************************************************************
  *
  * Copyright (c) 2019 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                               www.st.com/SLA0044
  *
  ******************************************************************************
  @endverbatim

@par Example Description

This directory contains a binary template (in DFU format) to be loaded into Flash memory using Device
Firmware Upgrade application.

This file was converted to the DFU format using the "DFU File Manager Tool" included in the "DfuSe" PC software install.
For more details on how to convert a .bin file to DFU format please refer to the UM0412 user manual
"Getting started with DfuSe USB device firmware upgrade STMicroelectronics extension" available from the
STMicroelectronics microcontroller website www.st.com.

This binary is a simple LED toggling.
The system Timer (Systick) is used to generate the delay.
The offset address of this binary is 0x0800C000 which matches the definition in DFU application
"USBD_DFU_APP_DEFAULT_ADD".


@par Hardware and Software environment

  - This example runs on stm32g4xx devices.

  - This example has been tested with STM32G474E-EVAL Rev B board and can be
    easily tailored to any other supported device and development board.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
