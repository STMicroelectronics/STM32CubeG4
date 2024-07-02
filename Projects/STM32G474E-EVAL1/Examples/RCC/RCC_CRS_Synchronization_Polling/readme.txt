/**
  @page RCC_CRS_Synchronization_Polling RCC Clock Recovery Service example

  @verbatim
  ******************************************************************************
  * @file    RCC/RCC_CRS_Synchronization_Polling/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the RCC Clock Recovery Service example.
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

@par Example Description

Configuration of the clock recovery service (CRS) in Polling mode, using the RCC HAL API.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 170 MHz.

Then, HSI48 and LSE oscillators are enabled. In this example CRS will use LSE
oscillator to calibrate HSI48 frequency.
When HSI48 and LSE are started, automatic calibration starts and program call the
polling function to wait for synchronization status.

In this test, there are results which could occur:
 - few loops occurs up to SYNC OK event (if SYNC WARN, HSI48 trimming value will be
 automatically modified)
 - in case of SYNC MISS or SYNC ERROR, there is an issue with synchronization input
 parameters. In this case, user need to apply new parameters after checking synchronization
 information and restart the synchronization.
 - LED3 power on means that a timeout occurred during polling procedure. It may
 due to an issue with LSE synchronization frequency.
 - LED1 will finally power on if SYNC OK is returned by CRS.

Note: HSI48 frequency can be checked with oscilloscope using MCO PIN PA.08

@par Keywords

RCC, System, Clock Configuration, CRS, System clock, HSI48, LSE, Oscillator, PLL, Polling

@par Directory contents

  - RCC/RCC_CRS_Synchronization_Polling/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - RCC/RCC_CRS_Synchonization_Polling/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - RCC/RCC_CRS_Synchonization_Polling/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - RCC/RCC_CRS_Synchonization_Polling/Inc/main.h                  Header for main.c module
  - RCC/RCC_CRS_Synchonization_Polling/Src/stm32g4xx_it.c          Interrupt handlers
  - RCC/RCC_CRS_Synchonization_Polling/Src/main.c                  Main program
  - RCC/RCC_CRS_Synchonization_Polling/Src/system_stm32g4xx.c      STM32G4xx system source file
  - RCC/RCC_CRS_Synchonization_Polling/Src/stm32g4xx_hal_msp.c     HAL MSP module

@par Hardware and Software environment

  - This example runs on STM32G474QETx Devices.

  - This example has been tested with STM32G474E-EVAL1 Rev B board and can be
    easily tailored to any other supported device and development board.


@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
