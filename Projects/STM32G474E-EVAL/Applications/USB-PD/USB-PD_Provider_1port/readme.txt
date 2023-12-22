/**
  @page USB-PD_Provider_1port USBPD provider example

  @verbatim
  ******************************************************************************
  * @file    USB-PD/USB-PD_Provider_1port/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the Provider example.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Application Description

How to create a simple type C provider.

This application initializes the type C port 1 in source mode with only one PDO at 5V.

You need to plug an external 5V power supply on PSU connector.

Connect UCPD cube Monitor on the VCP associated to our board (only available if USB cable is connected)
The UCPD Cube Monitor will be used to trace all the messages exchange between the ports partner.
GUI interface has been enabled in this project. It allows to interact with the device (send and receive
messages).

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in HAL time base ISR.
      This implies that if HAL_Delay() is called from a peripheral ISR process, then
      the HAL time base interrupt must have higher priority (numerically lower) than
      the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority()
      function.

@note The application needs to ensure that the HAL time base is always set to 1 millisecond
      to have correct HAL operation.

@note The FreeRTOS heap size configTOTAL_HEAP_SIZE defined in FreeRTOSConfig.h is set accordingly to the
      OS resources memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.

For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications
on STM32Cube with RTOS".

@par Keywords

USB_PD_Lib, UCPD, Type C, USBPD, FreeRTOS

@par Directory contents
    - USB-PD/USB-PD_Provider_1port/Src/main.c                       Main program file
    - USB-PD/USB-PD_Provider_1port/Src/app_FreeRTOS.c               Code for freertos applications file
    - USB-PD/USB-PD_Provider_1port/Src/stm32g4xx_hal_msp.c          MSP Initialization file
    - USB-PD/USB-PD_Provider_1port/Src/stm32g4xx_it.c               Interrupt handlers file
    - USB-PD/USB-PD_Provider_1port/Src/system_stm32g4xx.c           STM32G4xx system clock configuration file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd.c                      Ucpd init file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_dpm_core.c             Device policy manager core file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_dpm_user.c             Device policy manager users file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_pwr_if.c               Power if management file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_pwr_user.c             Power management file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_vdm_user.c             User vendor define message management file
    - USB-PD/USB-PD_Provider_1port/Inc/FreeRTOSConfig.h             FreeRTOS Configuration file
    - USB-PD/USB-PD_Provider_1port/Inc/main.h                       Main program header file
    - USB-PD/USB-PD_Provider_1port/Inc/stm32_assert.h               assert program header file
    - USB-PD/USB-PD_Provider_1port/Inc/stm32g4xx_hal_conf.h         HAL Library Configuration file
    - USB-PD/USB-PD_Provider_1port/Inc/stm32g4xx_it.h               Interrupt handlers header file
    - USB-PD/USB-PD_Provider_1port/Inc/usbpd_gui_memmap.h           Memory mapping file for connection with UCPD_Monitor
    - USB-PD/USB-PD_Provider_1port/Inc/tracer_emb_conf.h            Configuration file for UCPD tracer
    - USB-PD/USB-PD_Provider_1port/Src/usbpd.h                      header of Ucpd init file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_devices_conf.h         UCPD device configuration file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_dpm_conf.h             UCPD stack configuration file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_dpm_core.h             Device policy manager core header file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_dpm_user.h             Device policy manager users header file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_pwr_if.h               Power if management header file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_pdo_defs.h             PDO definition header file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_pwr_user.h             Power management header file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_vdm_user.h             Vendor define message management header file


@par Hardware and Software environment

  - This application runs on STM32G474QET6 devices.

  - This application has been tested with STM32G474E-EVAL board.


@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Check that you have plugged an external 5V power supply on PSU connector.
 - Run the example
 - Run CubeMx Monitor and open trace tools
 - Connect a type C sink device on the Type-C connector 1

 */
