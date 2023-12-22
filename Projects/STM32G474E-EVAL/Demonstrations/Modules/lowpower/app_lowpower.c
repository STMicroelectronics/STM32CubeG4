/**
  ******************************************************************************
  * @file    app_lowpower.c
  * @author  MCD Application Team
  * @brief   Low power application implementation.
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
  */
#ifndef __APP_LOWPOWER_C
#define __APP_LOWPOWER_C

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "app_lowpower.h"
#include "main.h"
#include "stm32g4xx_hal.h"
#include "stm32g474e_eval.h"
#include "stm32g474e_eval_lcd.h"
#include "utils.h"
#include "k_config.h"
#include "k_module.h"
#include "k_menu.h"
#include "k_window.h"

/* Private typedef ----------------------------------------------------------*/
/* Private constants ----------------------------------------------------------*/
/* Wake up source */
#define WAKEUP_GPIO 0UL
#define WAKEUP_RTC  1UL
/* Private function prototypes -----------------------------------------------*/
KMODULE_RETURN _LowPowerDemoExec(void);
KMODULE_RETURN _LowPowerDemoConfig(void);
KMODULE_RETURN _LowPowerDemoUnConfig(void);

void LowPowerDemo(void);
void LowPowerUserAction(uint8_t sel);
void LowPowerStandbyWakeupPin(void);
void LowPowerStandbyRTCAlarm(void);
void LowPowerStopEXTI(void);
void LowPowerStopRTCAlarm(void);
void LowPowerSleepEXTI(void);
void LowPowerSleepRTCAlarm(void);

static void LowPowerHandleAlarm(void);

void SystemPeripheralClockDisable(uint32_t WakeupSource);
void SystemPeripheralClockEnable(void);
void GPIOoptimStandby(uint32_t WakeupSource);
/* Private Variable ----------------------------------------------------------*/
/* standby mode menu */
const tMenuItem StandbyModeMenuItems[] =
{
    {"Wakeup pin"    , 14, 30,    SEL_EXEC, MODULE_LOWPOWER, LowPowerStandbyWakeupPin, LowPowerUserAction, NULL, NULL, NULL  },
    {"RTC Alarm"     , 14, 30,    SEL_EXEC, MODULE_LOWPOWER, LowPowerStandbyRTCAlarm,  LowPowerUserAction, NULL, NULL, NULL  },
    {"Return"        ,  0,  0,    SEL_EXIT, MODULE_LOWPOWER, NULL, NULL, NULL, NULL, NULL  }
};

const tMenu StandbyModeMenu = {
  "Standby mode", StandbyModeMenuItems, countof(StandbyModeMenuItems), TYPE_TEXT, 1, 1
};

/* sleep mode menu */
const tMenuItem SleepModeMenuItems[] =
{
    {"EXTI pin"     , 14, 30,    SEL_EXEC, MODULE_LOWPOWER, LowPowerSleepEXTI,     LowPowerUserAction, NULL, NULL, NULL },
    {"RTC Alarm"    , 14, 30,    SEL_EXEC, MODULE_LOWPOWER, LowPowerSleepRTCAlarm, LowPowerUserAction, NULL, NULL, NULL  },
    {"Return"       ,  0,  0,    SEL_EXIT, MODULE_LOWPOWER, NULL, NULL, NULL, NULL }
};

const tMenu SleepModeMenu = {
  "Sleep mode", SleepModeMenuItems, countof(SleepModeMenuItems), TYPE_TEXT, 1, 1
};



/* stop mode menu */
const tMenuItem StopModeMenuItems[] =
{
    {"EXTI pin"     , 14, 30,    SEL_EXEC, MODULE_LOWPOWER, LowPowerStopEXTI,     LowPowerUserAction, NULL, NULL, NULL },
    {"RTC Alarm"    , 14, 30,    SEL_EXEC, MODULE_LOWPOWER, LowPowerStopRTCAlarm, LowPowerUserAction, NULL, NULL, NULL  },
    {"Return"       ,  0,  0,    SEL_EXIT, MODULE_LOWPOWER, NULL, NULL, NULL, NULL }
};

const tMenu StopModeMenu = {
  "Stop mode", StopModeMenuItems, countof(StopModeMenuItems), TYPE_TEXT, 1, 1
};


/* Main menu */
const tMenuItem LowPowerMenuItems[] =
{
    {"STOP mode"   , 14, 30, SEL_SUBMENU, MODULE_LOWPOWER, NULL, NULL, (const tMenu*)&StopModeMenu, NULL, NULL  },
    {"Sleep mode"  , 14, 30, SEL_SUBMENU, MODULE_LOWPOWER, NULL, NULL, (const tMenu*)&SleepModeMenu, NULL, NULL  },
    {"STANDBY mode", 14, 30, SEL_SUBMENU, MODULE_LOWPOWER, NULL, NULL, (const tMenu*)&StandbyModeMenu, NULL, NULL  },
    {"Return"      ,  0,  0, SEL_EXIT, MODULE_NONE, NULL, NULL, NULL, NULL }
};

const tMenu LowpowerMenu = {
  "Low power", LowPowerMenuItems, countof(LowPowerMenuItems), TYPE_TEXT, 1, 1};

/* used to exit application */
static __IO uint8_t user_event=0;
static __IO uint8_t user_action=0;

/* Private typedef -----------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
const K_ModuleItem_Typedef ModuleLowPower =
{
  MODULE_LOWPOWER,
  _LowPowerDemoConfig,
  _LowPowerDemoExec,
  _LowPowerDemoUnConfig,
  NULL
};

/* RTC Handle variable */
extern RTC_HandleTypeDef RtcHandle;

/**
  * @brief  Configure the Lowpower application
  * @param  None.
  * @note   run and display information about the lowpower feature.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _LowPowerDemoConfig(void)
{
  /*#### Disable all used wakeup sources ####*/
  HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_B);
  /* Disable all previous wake up interrupt */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);

  /*#### Clear all related wakeup flags ####*/
  /* Clear the Alarm interrupt pending bit */
  __HAL_RTC_ALARM_CLEAR_FLAG(&RtcHandle,RTC_FLAG_ALRBF);
  /* Clear PWR wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2 | PWR_FLAG_SB);

    return KMODULE_OK;
}

/**
  * @brief  un-Configure the Lowpower application
  * @param  None.
  * @note   run and display information about the lowpower feature.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _LowPowerDemoUnConfig(void)
{
  return KMODULE_OK;
}

/**
  * @brief  Run the Lowpower application
  * @param  None.
  * @note   run and display information about the lowpower feature.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _LowPowerDemoExec(void)
{
  /* Prepare Execute the main MMI of lowpower application */
  kMenu_Execute(LowpowerMenu);
  return KMODULE_OK;
}

/**
  * @brief  Get User action
  * @param  sel   User selection (JOY_SEL,...)
  * @note   This example is the only way to get user information.
  * @retval None
  */
void LowPowerUserAction(uint8_t sel)
{
  if (user_action == 0)
  {
    user_action = 1;
    user_event = sel;
  }
}

/**
  * @brief  Run the Lowpower Standby mode Wakeup pin
  * @param  None.
  * @note   run and display information about the lowpower feature.
  * @retval None.
  */
void LowPowerStandbyWakeupPin(void)
{
  kWindow_Popup("STANDBY WAKEUP", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "JOY sel will\nstart\nstandby mode\n",                  \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE);

  /* Wait for key press */
  HAL_Delay(100);
  user_event = JOY_UP;
  while(user_event != JOY_SEL)
  {
    user_action = 0;
  };

  kWindow_Popup("STANDBY WAKEUP", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\n\npress USER button\nto exit\nand reset\n",                  \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE);

  /* USER push-button will be used to wakeup the system from Standby mode */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2_LOW);
  HAL_Delay(10);

  /* Optimize GPIO consumption (analog) for standby mode */
  GPIOoptimStandby(WAKEUP_GPIO);

  /* Clear PWR wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

  /* Standby mode */
  HAL_PWR_EnterSTANDBYMode();

  /* Following code should never be reach as MCU reset when exiting from standby */
  while(1);
}

/**
  * @brief  Run the Lowpower Standby mode RTC Alarm
  * @param  None.
  * @note   run and display information about the lowpower feature.
  * @retval None.
  */
void LowPowerStandbyRTCAlarm(void)
{
  kWindow_Popup("STANDBY Alarm", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\nset delay time\n",                            \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* Set the alarm */
  LowPowerHandleAlarm();

  kWindow_Popup("STANDBY Alarm", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\nstandby mode\nstarted\nwait alarm\nto exit\nand reset\n",                            \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* Optimize GPIO consumption (analog) for standby mode */
  GPIOoptimStandby(WAKEUP_RTC);

  /* Clear PWR wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

  /* Enter StandBy mode */
  HAL_PWR_EnterSTANDBYMode();

  /* Following code should never be reach as MCU reset when exiting from standby */
  while(1);
}

/**
  * @brief  Enter in sleep mode and exit by pressing the USER button
  * @param  None
  * @note   This example enter in sleep mode.
  * @retval None
  */
void LowPowerSleepEXTI(void)
{
  kWindow_Popup("SLEEP EXTI", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\n\nPress JOY sel to\nstart\nsleep mode\n",   \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* Wait for key press */
  HAL_Delay(100);
  user_event = JOY_UP;
  while(user_event != JOY_SEL)
  {
    user_action = 0;
  };

  kWindow_Popup("SLEEP EXTI", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,     \
                "\n\nsleep mode\nstarted\npress USER button\nto exit\n",\
                 UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* USER push-button will be used to wakeup the system from SLEEP mode */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* avoid wakeup from JOY event */
  BSP_JOY_DeInit(JOY1, JOY_ALL);
  BSP_IO_DeInit(JOY1);

  /* Clear PWR wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

  /*************************************************************/
  /*  Disable all peripheral clock during SLEEP mode           */
  /*  and put GPIO in analog mode to optimize power consumption */
  /*************************************************************/
  SystemPeripheralClockDisable(WAKEUP_GPIO);

  /* Suspend HAL tick irq */
  HAL_SuspendTick();

  /* Enable Flash power down mode during Sleep mode      */
  /* (comment this line if power consumption figures     */
  /*  must be measured with Flash still on in Sleep mode */
  __HAL_FLASH_SLEEP_POWERDOWN_ENABLE();

  /*********************/
  /* Enter Sleep Mode   */
  /*********************/
  HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);

  /* Resume HAL tick irq */
  HAL_ResumeTick();

  /********************************************************/
  /*  Restore peripheral clock and Gpio configuration     */
  /*  which have been disabled during SLEEP mode           */
  /********************************************************/
  SystemPeripheralClockEnable();

  /* Restore Joystick in interrupt mode */
  BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL);

  /* Display info */
  LowPowerExitDisplay(SLEEP);
}

/**
  * @brief  Enter in sleep mode and exit by an alarm
  * @param  None
  * @note   This example enter in sleep mode.
  * @retval None
  */
void LowPowerSleepRTCAlarm(void)
{
  kWindow_Popup("SLEEP Alarm", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\nset delay time\n",                          \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* set the alarm */
  LowPowerHandleAlarm();

  kWindow_Popup("SLEEP Alarm", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,  \
                "\n\nsleep mode\nstarted\nwait alarm\nto exit\n",\
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* De Init Joystick */
  BSP_JOY_DeInit(JOY1, JOY_ALL);
  BSP_IO_DeInit(JOY1);

  /*************************************************************/
  /*  Disable all peripheral clock during SLEEP mode           */
  /*  and put GPIO in analog mode to optimize power consumption */
  /*************************************************************/
  SystemPeripheralClockDisable(WAKEUP_RTC);

  /* Suspend HAL tick irq */
  HAL_SuspendTick();

  /* Enable Flash power down mode during Sleep mode       */
  /* (comment this line if power consumption figures      */
  /*  must be measured with Flash still on in Sleep mode) */
  __HAL_FLASH_SLEEP_POWERDOWN_ENABLE();

  /*******************/
  /* Enter Sleep Mode */
  /*******************/
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

  /* Resume HAL tick irq */
  HAL_ResumeTick();

  /********************************************************/
  /*  Restore peripheral clock and Gpio configuration     */
  /*  which have been disabled during SLEEP mode          */
  /********************************************************/
  SystemPeripheralClockEnable();

  /* Display info */
  LowPowerExitDisplay(SLEEP);

  /* Deactivate RTC alaram */
  HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_B);

  /* Clear the Alarm interrupt pending bit */
  __HAL_RTC_ALARM_CLEAR_FLAG(&RtcHandle,RTC_FLAG_ALRBF);

  /* Restore Joystic in interrupt mode */
  BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL);
}

/**
  * @brief  Enter in stop mode and exit by pressing the USER button
  * @param  None
  * @note   This example enter in stop mode.
  * @retval None
  */
void LowPowerStopEXTI(void)
{
  kWindow_Popup("STOP EXTI", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\n\nPress JOY sel to\nstart\nstop mode\n",   \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* Wait for keypress */
  HAL_Delay(100);
  user_event = JOY_UP;
  while(user_event != JOY_SEL)
  {
    user_action = 0;
  };

  kWindow_Popup("STOP EXTI", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,     \
                "\n\nstop mode\nstarted\npress USER button\nto exit\n",\
                 UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* USER push-button will be used to wakeup the system from STOP mode */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* avoid wakeup from JOY event */
  BSP_JOY_DeInit(JOY1, JOY_ALL);
  BSP_IO_DeInit(JOY1);

  /* Clear PWR wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

  /*************************************************************/
  /*  Disable all peripheral clock during STOP mode            */
  /*  and put GPIO in analog mode to optimize power consumption */
  /*************************************************************/
  SystemPeripheralClockDisable(WAKEUP_GPIO);

  /*********************/
  /* Enter Stop Mode   */
  /*********************/
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

  /* Restore the clock configuration */
  SystemClock_Config();

  /********************************************************/
  /*  Restore peripheral clock and Gpio configuration     */
  /*  which have been disabled during STOP mode           */
  /********************************************************/
  SystemPeripheralClockEnable();

  /* Restore Joystick in interrupt mode */
  BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL);

  /* Display info */
  LowPowerExitDisplay(STOP);
}

/**
  * @brief  Enter in stop mode and exit by an alarm
  * @param  None
  * @note   This example enter in stop mode.
  * @retval None
  */
void LowPowerStopRTCAlarm(void)
{
  kWindow_Popup("STOP Alarm", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "\nset delay time\n",                          \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* set the alarm */
  LowPowerHandleAlarm();

  kWindow_Popup("STOP Alarm", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,  \
                "\n\nstop mode\nstarted\nwait alarm\nto exit\n",\
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* De Init Joystick */
  BSP_JOY_DeInit(JOY1, JOY_ALL);
  BSP_IO_DeInit(JOY1);

  /*************************************************************/
  /*  Disable all peripheral clock during STOP mode            */
  /*  and put GPIO in analog mode to optimize power consumption */
  /*************************************************************/
  SystemPeripheralClockDisable(WAKEUP_RTC);

  /*******************/
  /* Enter Stop Mode */
  /*******************/
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

  /* Restore the clock configuration */
  SystemClock_Config();

  /********************************************************/
  /*  Restore peripheral clock and Gpio configuration     */
  /*  which have been disabled during STOP mode           */
  /********************************************************/
  SystemPeripheralClockEnable();

  /* Display info */
  LowPowerExitDisplay(STOP);

  HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_B);

  /* Clear the Alarm interrupt pending bit */
  __HAL_RTC_ALARM_CLEAR_FLAG(&RtcHandle,RTC_FLAG_ALRBF);

  BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL);
}

/**
  * @brief  Get user info to setup an alarm
  * @note   This function wait user info to setup the alarm.
  * @retval None
  */
static void LowPowerHandleAlarm(void)
{
  enum {
    HOURS,
    MINUTES,
    SECONDS,
    END
  };

  RTC_DateTypeDef currentdate = {0};
  RTC_TimeTypeDef time = {0};
  RTC_TimeTypeDef currenttime = {0};
  RTC_AlarmTypeDef Alarm = {0};
  sFONT *font;
  uint8_t temp[16];
  uint8_t exit = 0;
  uint8_t index = SECONDS;
  uint8_t position;
  uint8_t statpos;
  uint32_t pXSize;

  /* get current font */
  font = UTIL_LCD_GetFont();
  BSP_LCD_GetXSize(LCD_INSTANCE, &pXSize);
  statpos = (pXSize >> 1) - (4 * font->Width);

  /* Get the alarm time from user */
  do
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
    sprintf((char *)temp, "%.2d:%.2d:%.2d", time.Hours, time.Minutes, time.Seconds);
    UTIL_LCD_DisplayStringAt(statpos, 5 * font->Height, temp, LEFT_MODE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);

    switch(index)
    {
    case HOURS :
      position = statpos;
      sprintf((char *)temp, "%.2d", time.Hours);
      break;
    case MINUTES:
      position = statpos + (3 * font->Width);
      sprintf((char *)temp, "%.2d", time.Minutes);
      break;
    case SECONDS :
      position = statpos + (6 * font->Width);
      sprintf((char *)temp, "%.2d", time.Seconds);
      break;
    }
    UTIL_LCD_DisplayStringAt(position, 5 * font->Height, temp, LEFT_MODE);

    user_event = JOY_NONE;
    user_action = 0;
    while(user_event == JOY_NONE);
    switch(user_event)
    {
    case JOY_UP :
      if(index == HOURS)
      {
        if( time.Hours == 23 ) time.Hours = 0;
        else
          time.Hours++;
      }

      if(index == MINUTES)
      {
        if(time.Minutes == 59 ) time.Minutes= 0;
        else
          time.Minutes++;
      }
      if(index == SECONDS)
      {
        if(time.Seconds == 59 ) time.Seconds =0;
        else
          time.Seconds++;
      }
      break;
    case JOY_DOWN :
      if(index == HOURS)
      {
        if (time.Hours == 0 ) time.Hours = 23;
        else time.Hours--;
      }
      if(index == MINUTES)
      {
        if(time.Minutes == 0) time.Minutes=59;
        else
          time.Minutes--;
      }
      if(index == SECONDS)
      {
        if(time.Seconds == 0) time.Seconds = 59;
        else
          time.Seconds--;
      }
      break;
    case JOY_RIGHT :
      if(index != SECONDS ) index++;
      break;
    case JOY_LEFT :
      if(index != HOURS ) index--;
      break;
    case JOY_SEL :
      exit = 1;
      break;
    }
  } while(exit == 0);


  HAL_RTC_GetDate(&RtcHandle, &currentdate, RTC_FORMAT_BIN);
  HAL_Delay(100);
  HAL_RTC_GetTime(&RtcHandle, &currenttime,  RTC_FORMAT_BIN);
  time.SubSeconds = currenttime.SubSeconds;
  if((time.Seconds + currenttime.Seconds) > 60 )  time.Minutes++;
  Alarm.AlarmTime.Seconds = ((time.Seconds + currenttime.Seconds) % 60);

  if((time.Minutes + currenttime.Minutes) > 60 )  time.Hours++;
  Alarm.AlarmTime.Minutes = ((time.Minutes + currenttime.Minutes) % 60);

  Alarm.AlarmTime.Hours = ((time.Hours + currenttime.Hours) % 24);

  /* Set the alarm */
  Alarm.Alarm = RTC_ALARM_B;
  Alarm.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
  Alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  Alarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  Alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  Alarm.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
  if(HAL_RTC_SetAlarm_IT(&RtcHandle, &Alarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    while(1);
  }
}

/**
  * @brief  Display message when exiting from Stop/Standby mode
  * @param  Mode   Mode which MCU exit from. Could be STANDBY or STOP
  * @retval None
  */
void LowPowerExitDisplay(uint32_t Mode)
{
  char msg[22] = "\n\nexit from\n";

  if(Mode == STANDBY)
  {
    strcat(msg, "STANDBY\n");
  }
  else if(Mode == SLEEP)
  {
    strcat(msg, "SLEEP\n");
  }
  else
  {
    strcat(msg, "STOP\n");
  }

  kWindow_Popup("Low Power", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                msg, UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  HAL_Delay(500);
}

/**
  * @brief  Deactivate RTC alarm
  * @param  hrtc RTChandle
  * @retval None
  */
void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc)
{
  HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_B);
}


/**
  * @brief  Disable System peripheral clocks, and optimse GPIO consumption
  * @param  WakeupSource source for wakeup. Could be WAKEUP_RTC or WAKEUP_GPIO
  * @retval None
  */
void SystemPeripheralClockDisable(uint32_t WakeupSource)
{
  GPIO_InitTypeDef GPIO_InitStructure;

   /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /* Set all GPIO in analog state to reduce power consumption */
  /* But keep configuration of few GPIO to keep Demo working */
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;

//#define DEBUG_SWD
#if defined(DEBUG_SWD)
  GPIO_InitStructure.Pin = (GPIO_PIN_All & ~( GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15));  // SWD
#else
  GPIO_InitStructure.Pin = GPIO_PIN_All;
#endif
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = (GPIO_PIN_All & ~(GPIO_PIN_14 | GPIO_PIN_15));  // LCD
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

  if (WakeupSource == WAKEUP_GPIO)
  {
    GPIO_InitStructure.Pin = (GPIO_PIN_All & ~(GPIO_PIN_9 | GPIO_PIN_13)); // LCD + USER button
  }
  else
  { /* RTC wakeup */
    GPIO_InitStructure.Pin = (GPIO_PIN_All & ~GPIO_PIN_9); // LCD
  }
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = GPIO_PIN_All;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* Disable remaining clocks */
  __HAL_RCC_FLASH_CLK_DISABLE();
  __HAL_RCC_SYSCFG_CLK_DISABLE();
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();

  if (WakeupSource != WAKEUP_GPIO)
  {
    __HAL_RCC_GPIOC_CLK_DISABLE(); // GPIOC required for USER button
  }
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();

  __HAL_RCC_I2C3_CLK_DISABLE();
  __HAL_RCC_SPI2_CLK_DISABLE();
  __HAL_RCC_DMA1_CLK_DISABLE();
  __HAL_RCC_DMA2_CLK_DISABLE();
  __HAL_RCC_FMC_CLK_DISABLE();
  __HAL_RCC_SAI1_CLK_DISABLE();
  __HAL_RCC_QSPI_CLK_DISABLE();
  if (WakeupSource != WAKEUP_RTC)
  {
    __HAL_RCC_RTC_DISABLE();
    __HAL_RCC_RTCAPB_CLK_DISABLE();
  }

  /* keep perihral clock disabled during sleep mode */
  RCC->AHB1SMENR  = 0x0;
  RCC->AHB2SMENR  = 0x0;
  RCC->AHB3SMENR  = 0x0;
  RCC->APB1SMENR1 = 0x0;
  RCC->APB1SMENR2 = 0x0;
  RCC->APB2SMENR  = 0x0;
}

/**
  * @brief  Restore System peripheral clocks and GPIO configuration
  * @param  wakeupsource source for wakeup. Could be WAKEUP_RTC or WAKEUP_GPIO
  * @retval None
  */
void SystemPeripheralClockEnable(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable minimum System peripheral clocks */
  /* others will be enabled via BSP layers   */
  __HAL_RCC_FLASH_CLK_ENABLE();
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  __HAL_RCC_I2C3_CLK_ENABLE();
  __HAL_RCC_SPI2_CLK_ENABLE();
  __HAL_RCC_RTC_ENABLE();
  __HAL_RCC_RTCAPB_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* PA0 used for MFX interrupt */
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Pin = GPIO_PIN_0 ;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Pin = GPIO_PIN_8 ;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Pin = GPIO_PIN_9 ;
  GPIO_InitStructure.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  GPIO_InitStructure.Pin = GPIO_PIN_11 ;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);
}

/**
  * @brief  Disable System peripheral clocks
  * @param  WakeupSource source for wakeup. Could be WAKEUP_RTC or WAKEUP_GPIO
  * @retval None
  */
void GPIOoptimStandby(uint32_t WakeupSource)
{
  GPIO_InitTypeDef GPIO_InitStructure;

   /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /* Set all GPIO in analog state to reduce power consumption */
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;

  GPIO_InitStructure.Pin = GPIO_PIN_All;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

  if (WakeupSource == WAKEUP_GPIO)
  {
    /* Keep pull up on PC13 during standby for user button wakeup  */
    HAL_PWREx_EnablePullUpPullDownConfig();
    HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_C, PWR_GPIO_BIT_13);
  }
  /* Disable remaining clocks */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
}

#undef __APP_LOWPOWER_C
#endif

