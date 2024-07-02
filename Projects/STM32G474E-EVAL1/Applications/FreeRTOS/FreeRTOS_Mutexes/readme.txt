/**
  @page FreeRTOS_Mutexes FreeRTOS Mutexes example

  @verbatim
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_Mutexes/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FreeRTOS Mutexes example.
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

How to use mutexes with CMSIS RTOS API.

This application creates three threads, with different priorities, that access the
same mutex, as described below:

MutexHighPriorityThread() has the highest priority so executes
first and grabs the mutex and sleeps for a short period to let the lower
priority threads execute.  When it has completed its demo functionality
it gives the mutex back before suspending itself.
At that point, LED1 toggles.

MutexMediumPriorityThread() attempts to access the mutex by performing
a blocking 'wait'.  This thread blocks when the mutex is already taken
by the high priority thread. It does not unblock until the highest
priority thread  has released the mutex, and it does not actually run until
the highest priority thread has suspended itself.
When it eventually does obtain the mutex all it does is give the mutex back
prior to also suspending itself.
At this point both the high and medium priority threads are suspended and LED2 toggles.

MutexLowPriorityThread() runs at the idle priority.  It spins round
a tight loop attempting to obtain the mutex with a non-blocking call.  As
the lowest priority thread it will not successfully obtain the mutex until
both high and medium priority threads are suspended.  Once it eventually
does obtains the mutex, it first resumes both suspended threads (and LED4 toggles
at that time) prior to giving the mutex back - resulting in the low priority
thread temporarily inheriting the highest thread priority.

In case of error, LED3 toggles.

The following variables can be displayed on the debugger via LiveWatch:
 - HighPriorityThreadCycles
 - MediumPriorityThreadCycles
 - LowPriorityThreadCycles
 These variables must remain equals all the time. If not equal, it means a stall has occurred.

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

RTOS, FreeRTOS, Threading, Mutexes

@par Directory contents
    - FreeRTOS/FreeRTOS_Mutexes/Src/main.c                       Main program
    - FreeRTOS/FreeRTOS_Mutexes/Src/app_FreeRTOS.c               Code for freertos applications
    - FreeRTOS/FreeRTOS_Mutexes/Src/stm32g4xx_hal_timebase_tim.c HAL timebase file
    - FreeRTOS/FreeRTOS_Mutexes/Src/stm32g4xx_it.c               Interrupt handlers
    - FreeRTOS/FreeRTOS_Mutexes/Src/stm32g4xx_hal_msp.c          MSP Initialization file
    - FreeRTOS/FreeRTOS_Mutexes/Src/system_stm32g4xx.c           STM32G4xx system clock configuration file
    - FreeRTOS/FreeRTOS_Mutexes/Inc/main.h                       Main program header file
    - FreeRTOS/FreeRTOS_Mutexes/Inc/stm32g4xx_hal_conf.h         HAL Library Configuration file
    - FreeRTOS/FreeRTOS_Mutexes/Inc/stm32g4xx_it.h               Interrupt handlers header file
    - FreeRTOS/FreeRTOS_Mutexes/Inc/FreeRTOSConfig.h             FreeRTOS Configuration file

@par Hardware and Software environment

  - This application runs on STM32G474QETx devices.

  - This application has been tested with STM32G474E-EVAL1 Rev B board and can be
    easily tailored to any other supported device and development board.


@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
