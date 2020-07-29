/**
  @page Digital_Power Buck_VoltageMode_HW example
 
  @verbatim
  ******************************************************************************
  * @file    Digital_Power/Buck_VoltageMode_HW/readme.txt
  * @author  MCD Vertical Application Team
  * @brief   Description of the Buck converter in Voltage mode example.
  ******************************************************************************
  *
  * Copyright (c) 2020 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                               www.st.com/SLA0044
  *
  ******************************************************************************
  @endverbatim

@par Application Description

This application runs a Buck converter to output a regulated voltage at 3.3Vdc through two selectable embedded loads.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in HAL time base ISR.
      This implies that if HAL_Delay() is called from a peripheral ISR process, then
      the HAL time base interrupt must have higher priority (numerically lower) than
      the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the HAL time base is always set to 1 millisecond to have correct HAL operation.


@par Keywords

BUCK converter, CCM, Voltage mode, STM32 Digital Power


@par Directory contents
    - Digital_Power/Buck_VoltageMode_HW/Src/main.c                       Main program file
    - Digital_Power/Buck_VoltageMode_HW/Src/stm32g4xx_hal_msp.c          MSP Initialization file
    - Digital_Power/Buck_VoltageMode_HW/Src/stm32g4xx_it.c               Interrupt handlers file
    - Digital_Power/Buck_VoltageMode_HW/Src/system_stm32g4xx.c           STM32G4xx system clock configuration file
    - Digital_Power/Buck_VoltageMode_HW/Src/app_dpow.c                   Board usage implementation file
    - Digital_Power/Buck_VoltageMode_HW/Inc/main.h                       Main program header file
    - Digital_Power/Buck_VoltageMode_HW/Inc/stm32g4xx_hal_conf.h         HAL Library Configuration file
    - Digital_Power/Buck_VoltageMode_HW/Inc/stm32g4xx_it.h               Interrupt handlers header file
    - Digital_Power/Buck_VoltageMode_HW/Inc/app_dpow.h                   Board usage implementation header file
    - Digital_Power/Buck_VoltageMode_HW/Inc/HW_3p3z_controller.h         3p3z controller inline implementation file


@par Hardware and Software environment

  - Connect the USB Type-A to Micro-B cable on CN3 to control the board through STLink.
  - Remove the jumper from JP1 (if any) and connect a 12Vdc between Vin and GND (JP1) input connector.
  
  - This application runs on STM32G474RET6 devices.
  - This application has been tested with STM32G474E-DISCOVERY board.

  - The "Buck_VoltageMode_HW.wds" file contains the board configuration and the outcome computed coefficients.
    It shall be used with the STM32 WDS software tools from Biricha.
    The "Buck_VoltageMode_HW.bode3" file is the measured bode diagram that can be upload into it for comparison with theory.


@par How to use it ?

In order to make the program work, you must do the following:
 - Open the "Buck_VoltageMode_HW.ioc" file using the STM32CubeMX software tools
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
 - Please, refer to the dedicated application note for more details about usage and measurements.
  
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
