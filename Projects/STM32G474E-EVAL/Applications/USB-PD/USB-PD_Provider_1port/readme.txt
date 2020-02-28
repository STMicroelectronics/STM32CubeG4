/**
  @page USB-PD_Provider USBPD provider example

  @verbatim
  ******************************************************************************
  * @file    USB-PD/USB-PD_Provider_1port/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the Provider example.
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

@par Application Description

How to create a simple type C provider.

This application initializes the type C port 1 in source mode with only one PDO at 5V.

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

USCPD, Type C, USBPD, FreeRTOS

@par Directory contents
    - USB-PD/USB-PD_Provider_1port/Src/main.c                       Main program file
    - USB-PD/USB-PD_Provider_1port/Src/app_FreeRTOS.c               Code for freertos applications file
    - USB-PD/USB-PD_Provider_1port/Src/stm32g4xx_hal_msp.c          MSP Initialization file
    - USB-PD/USB-PD_Provider_1port/Src/stm32g4xx_it.c               Interrupt handlers file
    - USB-PD/USB-PD_Provider_1port/Src/system_stm32g4xx.c           STM32G4xx system clock configuration file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd.c                      Ucpd init file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_dpm_core.c             device policy manager core file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_dpm_users.c            device policy manager users file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_pwr_if.c               power if managment file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_pwr_user.c             power managment file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_vdm_users.c            user vendor define message managment file
    - USB-PD/USB-PD_Provider_1port/Inc/FreeRTOSConfig.h             FreeRTOS Configuration file
    - USB-PD/USB-PD_Provider_1port/Inc/main.h                       Main program header file
    - USB-PD/USB-PD_Provider_1port/Inc/stm32_assert.h               assert program header file
    - USB-PD/USB-PD_Provider_1port/Inc/stm32g4xx_hal_conf.h         HAL Library Configuration file
    - USB-PD/USB-PD_Provider_1port/Inc/stm32g4xx_it.h               Interrupt handlers header file
    - USB-PD/USB-PD_Provider_1port/Inc/main.h                       Main program header file
    - USB-PD/USB-PD_Provider_1port/Inc/main.h                       Main program header file
    - USB-PD/USB-PD_Provider_1port/Inc/tracer_emb_conf.h            setting file for UCPD tracer
    - USB-PD/USB-PD_Provider_1port/Src/usbpd.h                      header of Ucpd init file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_devices_conf.h         UCPD device configuration file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_dpm_conf.h             UCPD stack configuration file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_dpm_core.h             device policy manger core header file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_dpm_users.h            device policy manager users header file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_pwr_if.h               power if managment header file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_pdo_defs.h             pdo definition header file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_pwr_user.h             power managment header file
    - USB-PD/USB-PD_Provider_1port/Src/usbpd_vdm_users.h            vendor define message managment header file


@par Hardware and Software environment

  - This application runs on STM32G474QET6 devices.

  - This application has been tested with STM32G474E-EVAL board.


@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 - Run CubeMx Monitor and open trace tools
 - Connect a type C sink device on the Type-C connector 1

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
