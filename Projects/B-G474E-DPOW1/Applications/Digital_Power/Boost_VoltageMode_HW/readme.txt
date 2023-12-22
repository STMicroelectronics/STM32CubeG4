/**
  @page Digital_Power Boost_VoltageMode_HW example
 
  @verbatim
  ******************************************************************************
  * @file    Digital_Power/Boost_VoltageMode_HW/readme.txt
  * @author  MCD Vertical Application Team
  * @brief   PSU - Boost converter (Voltage mode) in CCM using HRTIMER+FMAC IPs
  ******************************************************************************
  *
  * Copyright (c) 2019(-2021) STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Application Description

This application runs a Boost converter to output a regulated voltage at 5VDC through two selectable embedded loads.

A Voltage Mode control loop is applied to this Boost plant presuming a 2.5VDC input voltage.
Such input is obtained by halving the 5V provided by USBPD (or alternative user DC supply), through a Buck converter.
Please, note that this Buck converter is used with constant duty cycle (no control loop). 

The PWMs of both the Boost and the Buck converters are generated using the HRTIMER IP. 
The Boost compensator is executed through the FMAC IP usage. 

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in HAL time base ISR.
      This implies that if HAL_Delay() is called from a peripheral ISR process, then
      the HAL time base interrupt must have higher priority (numerically lower) than
      the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the HAL time base is always set to 1 millisecond to have correct HAL operation.


@par Keywords

PSU, BOOST converter, Voltage mode, CCM, STM32 Digital Power, HRTIMER usage, FMAC usage


@par Directory contents
    - Digital_Power/Boost_VoltageMode_SW/Src/app_dpow.c                   Board usage implementation file
    - Digital_Power/Boost_VoltageMode_SW/Src/main.c                       Main program file
    - Digital_Power/Boost_VoltageMode_SW/Src/stm32g4xx_hal_msp.c          MSP Initialization file
    - Digital_Power/Boost_VoltageMode_SW/Src/stm32g4xx_it.c               Interrupt handlers file
    - Digital_Power/Boost_VoltageMode_SW/Src/system_stm32g4xx.c           STM32G4xx system clock configuration file
    - Digital_Power/Boost_VoltageMode_SW/Inc/app_dpow.h                   Board usage implementation header file
    - Digital_Power/Boost_VoltageMode_SW/Inc/main.h                       Main program header file
    - Digital_Power/Boost_VoltageMode_SW/Inc/stm32g4xx_hal_conf.h         HAL Library Configuration file
    - Digital_Power/Boost_VoltageMode_SW/Inc/stm32g4xx_it.h               Interrupt handlers header file


@par Hardware and Software environment

  - Connect the USB Type-A to Micro-B cable on CN3 to control the board through STLink.
  - Remove the jumper from JP1 (if any) and connect a 5Vdc between Vin and GND (JP1) input connector.
  - Limit the current output of your power supply to 400mA. 
  
  Note: if power is supplied via USBPD make sure there is a current limiter on your power source. 
  
  - This application runs on STM32G474RET6 devices.
  - This application has been tested with B-G474E-DPOW1 board.

@par How to use it ?

In order to make the program work, you must do the following:
 - Open the "Boost_VoltageMode_HW.ioc" file using the STM32CubeMX software tools
 - Select your preferred toolchain
 - Generate the example
 - Rebuild all files and load your image into target memory
 - Reset the board to run the example
 - Press Joystick as follow :
      Up    -> Automatic load toggling (50% to 100%)
      Down  -> Manual load selection
      Left  -> Decrease load by 50% (from 0% to 100%)
      Right -> Increase load by 50% (from 100% to 0%)
 - Connect scope on TP1 and make measurements
  
 **/
