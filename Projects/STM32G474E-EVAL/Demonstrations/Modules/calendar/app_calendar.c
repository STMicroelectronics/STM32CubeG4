/**
  ******************************************************************************
  * @file    app_calendar.c
  * @author  MCD Application Team
  * @brief   Calendar application implementation.
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
#ifndef __APP_CALENDAR_C
#define __APP_CALENDAR_C

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32g474e_eval.h"
#include "stm32g474e_eval_lcd.h"
#include "utils.h"
#include "app_calendar.h"
#include "k_config.h"
#include "k_module.h"
#include "k_menu.h"
#include "k_storage.h"

/* Private typedef ----------------------------------------------------------*/
/* Time Structure definition */
struct time_t
{
  uint8_t sec_l;
  uint8_t sec_h;
  uint8_t min_l;
  uint8_t min_h;
  uint8_t hour_l;
  uint8_t hour_h;
};
struct time_t time_struct;

/* Alarm Structure definition */
struct alarm_t
{
  uint8_t sec_l;
  uint8_t sec_h;
  uint8_t min_l;
  uint8_t min_h;
  uint8_t hour_l;
  uint8_t hour_h;
};
struct alarm_t alarm_struct;

/* Date Structure definition */
struct date_t
{
  uint8_t month;
  uint8_t day;
  uint16_t year;
};
struct date_t date_s;

extern tMenuItem psCurrentMenuItem;
extern __IO uint8_t AlarmStatus ;
static __IO uint8_t LedCounter = 0;
static uint32_t DateNb = 0;
static uint32_t wn = 0;
static uint32_t dn = 0;
static uint32_t dc = 0;
static uint16_t daycolumn = 0, dayline = 0;

const char DayNames[7][5] =
  {"Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat.", "Sun."
  };

const uint8_t MonLen[12] =
  {
    32, 29, 32, 31, 32, 31, 32, 32, 31, 32, 31, 32
  };
const char MonthNames[12][10] =
  {"JANUARY  ", "FEBRUARY ", "MARCH    ",
   "APRIL    ", "MAY      ", "JUNE     ",
   "JULY     ", "AUGUST   ", "SEPTEMBER",
   "OCTOBER  ", "NOVEMBER ", "DECEMBER "
  };

RTC_TimeTypeDef Time = {0};
RTC_DateTypeDef Date = {0};

/* Private constants ----------------------------------------------------------*/
#define DATE_LINE_NB                        2
#define TIME_LINE_NB                        3

/* Private function prototypes -----------------------------------------------*/
KMODULE_RETURN _CalendarDemoExec(void);
KMODULE_RETURN _CalendarConfig(void);
KMODULE_RETURN _CalendarExit(void);

void CalendarMenuUserAction(uint8_t sel);
void CalendarUserHeader(void);

static void Calendar_TimeRegulate(RTC_TimeTypeDef *TimeInput, RTC_TimeTypeDef *TimeOutput);
static void Calendar_TimePreAdjust(void);
static void Calendar_TimeSet(void);
static void Calendar_TimeShow(void);
static void Calendar_TimeDisplay(RTC_TimeTypeDef *time);
static void Calendar_DateRegulate(RTC_DateTypeDef *DateInput, RTC_DateTypeDef *DateOutput);
static void Calendar_DatePreAdjust(void);
static void Calendar_DateSet(void);
static void Calendar_DateShow(void);
static void Calendar_DateDisplay(uint8_t Year, uint8_t Month, uint8_t Day);
static void Calendar_AlarmPreAdjust(void);
static void Calendar_AlarmSet(void);
static void Calendar_AlarmDisable(void);
static void Calendar_ClearMenuLines(void);
static void Calendar_WeekDayNum(uint32_t nyear, uint8_t nmonth, uint8_t nday);
static uint8_t Calendar_IsLeapYear(uint16_t nYear);
static void Calendar_RegulateYear(uint8_t *Year, uint8_t *Month, uint8_t *Day);
static void Calendar_RegulateMonth(uint8_t *Year, uint8_t *Month, uint8_t *Day);
static void Calendar_RegulateDay(uint8_t *Year, uint8_t *Month, uint8_t *Day);
static void Calendar_TimePreAdjust(void);


/* Private Variable ----------------------------------------------------------*/
/* Time Menu */
const tMenuItem TimeMenuItems[] =
{
  {"Set"   , 14, 30, SEL_EXEC, MODULE_CALENDAR, Calendar_TimeSet, CalendarMenuUserAction, NULL, NULL, NULL  },
  {"Show"  , 14, 30, SEL_EXEC, MODULE_CALENDAR, Calendar_TimeShow , CalendarMenuUserAction, NULL, NULL, NULL  },
  {"Return", 14, 30, SEL_EXIT, MODULE_CALENDAR, NULL             , NULL                  , NULL, NULL, NULL  },
};
const tMenu TimeMenu = {"Time", TimeMenuItems, countof(TimeMenuItems), TYPE_TEXT, 1, 1 };

/* Date Menu */
const tMenuItem DateMenuItems[] =
{
  {"Set"   , 14, 30, SEL_EXEC, MODULE_CALENDAR, Calendar_DateSet, CalendarMenuUserAction, NULL, NULL, NULL  },
  {"Show"  , 14, 30, SEL_EXEC, MODULE_CALENDAR, Calendar_DateShow, CalendarMenuUserAction, NULL, NULL, NULL  },
  {"Return", 14, 30, SEL_EXIT, MODULE_CALENDAR, NULL             , NULL                  , NULL, NULL, NULL  },
};
const tMenu DateMenu = {"Date", DateMenuItems, countof(DateMenuItems), TYPE_TEXT, 1, 1 };

/* Alarm Menu */
const tMenuItem AlarmMenuItems[] =
{
  {"Set"    , 14, 30, SEL_EXEC, MODULE_CALENDAR, Calendar_AlarmSet    , CalendarMenuUserAction, NULL, NULL, NULL  },
  {"Disable", 14, 30, SEL_EXEC, MODULE_CALENDAR, Calendar_AlarmDisable, CalendarMenuUserAction, NULL, NULL, NULL  },
  {"Return" , 14, 30, SEL_EXIT, MODULE_CALENDAR, NULL                 , NULL                  , NULL, NULL, NULL  },
};
const tMenu AlarmMenu = {"Alarm", AlarmMenuItems, countof(AlarmMenuItems), TYPE_TEXT, 1, 1 };

/* Main Menu */
const tMenuItem CalendarMenuItems[] =
{
  {"Date"  , 14, 30, SEL_SUBMENU, MODULE_CALENDAR, NULL, NULL, (const tMenu*)&DateMenu, NULL, NULL  },
  {"Time"  , 14, 30, SEL_SUBMENU, MODULE_CALENDAR, NULL, NULL, (const tMenu*)&TimeMenu, NULL, NULL  },
  {"Alarm" , 14, 30, SEL_SUBMENU, MODULE_CALENDAR, NULL, NULL, (const tMenu*)&AlarmMenu, NULL, NULL  },
  {"Return", 14, 30, SEL_EXIT   , MODULE_NONE    , NULL, NULL, NULL                   , NULL, NULL  },
};
const tMenu CalendarMenu = {"Calendar", CalendarMenuItems, countof(CalendarMenuItems), TYPE_TEXT, 1, 1 };

/* Character usage */
uint8_t CharWidth;
uint8_t CharHeight;

/* used to exit application */
static __IO uint8_t UserEvent=0;
static __IO uint8_t UserAction=0;

/* Private typedef -----------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
const K_ModuleItem_Typedef ModuleCalendar =
{
  MODULE_CALENDAR,
  _CalendarConfig,
  _CalendarDemoExec,
  _CalendarExit,
  NULL
};

/* RTC Handle variable */
extern RTC_HandleTypeDef RtcHandle;


/**
  * @brief  setup the HW for Calendar application
  * @param  None.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _CalendarConfig(void)
{
  HAL_StatusTypeDef status = HAL_ERROR;

  /* HAL_RTC_Init has been already called for Backup reg access */
  /* check if Date has been already set one time, else configure it to
  1st of january 2018 */
  if(__HAL_RTC_GET_FLAG(&RtcHandle, RTC_FLAG_INITS) == 0x00u)
  {
    /* Set Date to 01.01.2018 */
    Date.Date = 0x01u;
    Date.Month = 0x01u;
    Date.Year = 0x18u;
    Date.WeekDay = RTC_WEEKDAY_MONDAY;

    /* Set time to default */
    Time.Hours = 0x00u;
    Time.Minutes = 0x00u;
    Time.Seconds = 0x00u;
    Time.TimeFormat = RTC_HOURFORMAT12_AM;

    /* Set the RTC current Time */
    status = HAL_RTC_SetTime(&RtcHandle, &Time, RTC_FORMAT_BCD);
    if(status != HAL_OK)
    {
      return KMODULE_ERROR_PRE;
    }

    /* Set the RTC current Date */
    HAL_RTC_SetDate(&RtcHandle, &Date, RTC_FORMAT_BCD);
    if(status != HAL_OK)
    {
      return KMODULE_ERROR_PRE;
    }
  }

  CharHeight = (((sFONT *)UTIL_LCD_GetFont())->Height);
  CharWidth = (((sFONT *)UTIL_LCD_GetFont())->Width);

  return KMODULE_OK;
}

/**
  * @brief  Run the application
  * @param  None.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _CalendarDemoExec(void)
{
  /* App initialization    */
  kMenu_Execute(CalendarMenu);
  return KMODULE_OK;
}


/**
  * @brief  Run the applications
  * @param  None.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _CalendarExit(void)
{
  return KMODULE_OK;
}


/**
  * @brief  Get User action
  * @param  sel   User selection (JOY_SEL,...)
  * @note   This example is the only way to get user information.
  * @retval None
  */
void CalendarMenuUserAction(uint8_t sel)
{
  if (UserAction == 0)
  {
    UserAction = 1;
  }
  /* store user event */
  UserEvent = sel;
}


/**
  * @brief  Adjusts time.
  * @param  None
  * @retval None
  */
static void Calendar_TimeSet(void)
{
  /* Clear menu lines */
  Calendar_ClearMenuLines();

  /* PreAdjust Time */
  Calendar_TimePreAdjust();
}


/**
  * @brief  Displays the current time.
  * @param  time current time
  * @retval None
  */
static void Calendar_TimeDisplay(RTC_TimeTypeDef *time)
{
  char showtime[14] = {0};

  sprintf((char*)showtime,"     %02X:%02X:%02X",time->Hours, time->Minutes, time->Seconds);
  UTIL_LCD_DisplayStringAtLine(TIME_LINE_NB, (uint8_t *) showtime);
}


/**
  * @brief  Displays information on LCD for the time configuration
  * @param  None
  * @retval None
  */
static void Calendar_TimePreAdjust(void)
{
  RTC_TimeTypeDef timecurrent = {0};
  RTC_TimeTypeDef timeset = {0};
  RTC_DateTypeDef dateset = {0};

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &timecurrent, RTC_FORMAT_BCD);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &dateset, RTC_FORMAT_BCD);

  /* Display the current time */
  Calendar_TimeDisplay(&timecurrent);

  /* Call Time selection function */
  Calendar_TimeRegulate(&timecurrent, &timeset);

  /* Set the RTC current Time */
  HAL_RTC_SetTime(&RtcHandle, &timeset, RTC_FORMAT_BCD);
  /* Set the RTC current Date */
  HAL_RTC_SetDate(&RtcHandle, &dateset, RTC_FORMAT_BCD);
}


/**
  * @brief  Shows the current time (HH/MM/SS) on LCD.
  * @param  None
  * @retval None
  */
static void Calendar_TimeShow(void)
{
  uint32_t ypos = (CharHeight * (TIME_LINE_NB + 1));
  uint32_t xpos;
  uint32_t pXSize;
  RTC_TimeTypeDef time = {0};
  RTC_DateTypeDef date = {0};
  uint32_t RCC_InitSysClkSaved;
  RCC_OscInitTypeDef RCC_OscInitStructure, RCC_OscInitStructureSaved; 
  BSP_LCD_GetXSize(LCD_INSTANCE, &pXSize);
  xpos =  (pXSize - 128) >> 1;
  UserAction = 0x00u;

  /* Clear menu lines */
  Calendar_ClearMenuLines();

  kStorage_OpenFileDrawPixel(ypos, xpos, (uint8_t *)"STFILES/WATCH.BMP");

  /* save RCC configuration */
  HAL_RCC_GetOscConfig(&RCC_OscInitStructureSaved);
  RCC_OscInitStructure = RCC_OscInitStructureSaved;
  
  RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStructure.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStructure.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStructure.PLL.PLLN = 23;
  RCC_OscInitStructure.PLL.PLLQ = RCC_PLLQ_DIV8; 
  /* save sysclk source */
  RCC_InitSysClkSaved = (__HAL_RCC_GET_SYSCLK_SOURCE() >> RCC_CFGR_SWS_Pos);
  /* in case PLL is system clock, first set system clock on HSI before
     reconfig PLL */
  if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK)
  {
    __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_HSI);
  }
  /* change clock config and PLL */
  HAL_RCC_OscConfig(&RCC_OscInitStructure);
  __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_PLLCLK);
 
  /* Wait a press on any JoyStick pushbuttons */
  while (UserAction == 0x00u)
  {
    /* Get the RTC current Time */
    HAL_RTC_GetTime(&RtcHandle, &time, RTC_FORMAT_BCD);
    /* Get the RTC current Date */
    HAL_RTC_GetDate(&RtcHandle, &date, RTC_FORMAT_BCD);

    /* Display current time */
    Calendar_TimeDisplay(&time);
  }
  /* restore clock config */
  /* in case PLL is system clock, first set system clock on HSI before
     reconfig PLL */
  if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK)
  {
    __HAL_RCC_SYSCLK_CONFIG(RCC_SYSCLKSOURCE_HSI);
  }
  /* restore initial pll clock config */
  HAL_RCC_OscConfig(&RCC_OscInitStructureSaved);
  /* restore PLL source */
  __HAL_RCC_SYSCLK_CONFIG(RCC_InitSysClkSaved);
}


/**
  * @brief  Adjusts the current date (MM/DD/YYYY).
  * @param  None
  * @retval None
  */
static void Calendar_DateSet(void)
{
  /* Get back font specifications */
  UTIL_LCD_SetFont(&Font20);
  CharHeight = (((sFONT *)UTIL_LCD_GetFont())->Height);
  CharWidth = (((sFONT *)UTIL_LCD_GetFont())->Width);

  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  Calendar_DatePreAdjust();

  UTIL_LCD_SetFont(&Font24);
  CharHeight = (((sFONT *)UTIL_LCD_GetFont())->Height);
  CharWidth = (((sFONT *)UTIL_LCD_GetFont())->Width);

}


/**
  * @brief  Displays the date in graphic mode.
  * @param  Year    Year to display
  * @param  Month   Month to display
  * @param  Date    Date to display
  * @retval None
  */
static void Calendar_DateDisplay(uint8_t Year, uint8_t Month, uint8_t Day)
{
  uint32_t mline = 0, mcolumn = 319, month = 0;
  uint32_t monthlength = 0;
  char linedisplay[26];
  uint32_t pXSize;

  if (Month == 2)
  {
    if (Calendar_IsLeapYear(Year))
    {
      monthlength = 30;
    }
    else
    {
      monthlength = MonLen[Month - 1];
    }
  }
  else
  {
    monthlength = MonLen[Month - 1];
  }

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DrawRect(daycolumn, dayline, CharWidth*2, CharHeight-2, UTIL_LCD_COLOR_WHITE);

  /* Set the Back Color */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
  BSP_LCD_GetXSize(LCD_INSTANCE, &pXSize);
  UTIL_LCD_FillRect(0, 0, pXSize, Font20.Height,UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_FillRect(0, LINE(1), pXSize, Font20.Height, UTIL_LCD_COLOR_ST_PINK);
  /* Set the Text Color */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);

  sprintf((char*)linedisplay,"    %s  20%02d     ", MonthNames[Month - 1], Year);
  UTIL_LCD_DisplayStringAtLine(0, (uint8_t*)linedisplay);

  /* Determines the week number, day of the week of the selected date */
  Calendar_WeekDayNum(Year, Month, Day);
  DateNb = dn;

  sprintf((char*)linedisplay," WEEK: %02ld   DAY: %03ld    ", wn, dc);
  UTIL_LCD_DisplayStringAtLine(1, (uint8_t*)linedisplay);

  /* Set the Back Color */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_GREEN_LIGHT);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_GREEN_LIGHT);
  UTIL_LCD_FillRect(0, LINE(2), pXSize, Font20.Height, UTIL_LCD_COLOR_ST_GREEN_LIGHT);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAtLine(2, (uint8_t*) " Mo Tu We Th Fr Sa Su   ");
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);

  /* Determines the week number, day of the week of the selected date */
  Calendar_WeekDayNum(Year, Month, 1);

  /* Clear first & last calendar lines */
  UTIL_LCD_ClearStringLine(3);
  UTIL_LCD_ClearStringLine(7);
  UTIL_LCD_ClearStringLine(8);

  mline = LINE(3);
  mcolumn = ((CharWidth * 3) * dn);
  for (month = 1; month < monthlength; month++)
  {
    /* display first space */
    UTIL_LCD_DisplayChar(mcolumn, mline, ' ');
    mcolumn += CharWidth;

    if (month == Day)
    {
      daycolumn = mcolumn;
      dayline = mline;
    }

    if (month / 10)
    {
      UTIL_LCD_DisplayChar(mcolumn, mline, ((month / 10) + 0x30));
    }
    else
    {
      UTIL_LCD_DisplayChar(mcolumn, mline, ' ');
    }
    mcolumn += CharWidth;

    UTIL_LCD_DisplayChar(mcolumn, mline, ((month % 10) + 0x30));
    mcolumn += CharWidth;

    if (mcolumn >= ((7 * 3) * CharWidth))
    {
      mcolumn = 0;
      mline += CharHeight;
    }
  }

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_GREEN_LIGHT);
  UTIL_LCD_DrawRect(daycolumn, dayline, CharWidth*2, CharHeight-2, UTIL_LCD_COLOR_ST_GREEN_LIGHT);
}


/**
  * @brief  Displays information on LCD for the time configuration
  * @param  None
  * @retval None
  */
static void Calendar_DatePreAdjust(void)
{
  RTC_DateTypeDef datecurrent = {0};
  RTC_DateTypeDef dateset = {0};
  RTC_TimeTypeDef timeset = {0};

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &timeset, RTC_FORMAT_BCD);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &datecurrent, RTC_FORMAT_BCD);

  /* Call Time selection function */
  Calendar_DateRegulate(&datecurrent, &dateset);

  /* Set the RTC current Time */
  HAL_RTC_SetTime(&RtcHandle, &timeset, RTC_FORMAT_BCD);
  /* Set the RTC current Date */
  HAL_RTC_SetDate(&RtcHandle, &dateset, RTC_FORMAT_BCD);
}


/**
  * @brief  Basically show date (MM/DD/YYYY).
  * @param  None
  * @retval None
  */
static void Calendar_DateShow(void)
{
  RTC_TimeTypeDef time = {0};
  RTC_DateTypeDef date = {0};
  char showdate[20] = {0};

  UserAction = 0x00u;

  /* Clear menu lines */
  Calendar_ClearMenuLines();

  /* Wait a press on any JoyStick pushbuttons */
  while (UserAction == 0x00u)
  {
    /* Get the RTC current Time */
    HAL_RTC_GetTime(&RtcHandle, &time, RTC_FORMAT_BCD);
    /* Get the RTC current Date */
    HAL_RTC_GetDate(&RtcHandle, &date, RTC_FORMAT_BCD);

    sprintf((char*)showdate," %s %02X.%02X.20%02X",DayNames[date.WeekDay - 1], date.Date, date.Month, date.Year);
    UTIL_LCD_DisplayStringAtLine(DATE_LINE_NB, (uint8_t *) showdate);

  }
}


/**
  * @brief  Set the time entered by user, using menu navigation keys.
  * @param  TimeInput     current time when entering function
  * @param  TimeOutput    value selected by user
  * @retval None
  */
static void Calendar_TimeRegulate(RTC_TimeTypeDef *TimeInput, RTC_TimeTypeDef *TimeOutput)
{
  uint16_t charwidth;
  uint8_t timearray[3];
  uint8_t rowarray[3];
  uint8_t index = 0;
  uint8_t digit = 0;
  char display[3];

  charwidth = UTIL_LCD_GetFont()->Width;
  rowarray[0] = 5 * charwidth;
  rowarray[1] = 8 * charwidth;
  rowarray[2] = 11 * charwidth;

  /* Initially display in red selected field */
  /* store time in array */
  timearray[0] = RTC_Bcd2ToByte(TimeInput->Hours);
  timearray[1] = RTC_Bcd2ToByte(TimeInput->Minutes);
  timearray[2] = RTC_Bcd2ToByte(TimeInput->Seconds);

  /* Display current digit in Red */
  digit = timearray[index];
  display[0] = ((digit / 10) + 0x30);
  display[1] = ((digit % 10) + 0x30);
  display[2] = (0x00);

  /* Set the Text Color */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);

  /* Display new value */
  UTIL_LCD_DisplayStringAt(rowarray[index], LINE(TIME_LINE_NB), (uint8_t *)display, LEFT_MODE);

  UserEvent = JOY_NONE;
  /* Endless loop */
  while (UserEvent != JOY_SEL)
  {
    if((UserEvent == JOY_UP) || (UserEvent == JOY_DOWN))
    {
      if(UserEvent == JOY_UP)
      {
        /* Increase current value */
        digit++;

        /* Handle hours , minutes, secondes overlap */
        if(((index == 0) && (digit > 23)) || (digit > 59))
        {
          digit = 0;
        }
      }

      if(UserEvent == JOY_DOWN)
      {
        /* Decrease current value */
        digit--;

        /* Handle hours , minutes, secondes overlap */
        if((index == 0) && (digit > 24))
        {
          digit = 23;
        }
        else if (digit > 60)
        {
          digit = 59;
        }
      }

      /* Display current digit in Red */
      display[0] = ((digit / 10) + 0x30);
      display[1] = ((digit % 10) + 0x30);

      /* Display new value */
      UTIL_LCD_DisplayStringAt(rowarray[index], LINE(TIME_LINE_NB), (uint8_t *)display, LEFT_MODE);

      /* Store digit into local variable */
      timearray[index] = digit;

      UserEvent = JOY_NONE;
    }

    if((UserEvent == JOY_RIGHT) || (UserEvent == JOY_LEFT))
    {
      /* Set the Text Color */
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);

      /* Display new value */
      UTIL_LCD_DisplayStringAt(rowarray[index], LINE(TIME_LINE_NB), (uint8_t *)display, LEFT_MODE);

      /* Handle index following selection */
      if(UserEvent == JOY_RIGHT)
      {
        index++;
        if(index > 2)
        {
          index = 0;
        }
      }
      else
      {
        index--;
        if(index > 2)
        {
          index = 2;
        }
      }
      /* Set the Text Color */
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);

      /* Display current digit in Red */
      digit = timearray[index];
      display[0] = ((digit / 10) + 0x30);
      display[1] = ((digit % 10) + 0x30);

      /* Display new value */
      UTIL_LCD_DisplayStringAt(rowarray[index], LINE(TIME_LINE_NB), (uint8_t *)display, LEFT_MODE);

      UserEvent = JOY_NONE;
    }
  }

  /* store time in array */
  TimeOutput->Hours = RTC_ByteToBcd2(timearray[0]);
  TimeOutput->Minutes = RTC_ByteToBcd2(timearray[1]);
  TimeOutput->Seconds = RTC_ByteToBcd2(timearray[2]);
}



/**
  * @brief  Set date entered by user, using menu navigation keys.
  * @param  DateInput    current date when entering function
  * @param  DateOutput   value selected by user
  * @retval None
  */
static void Calendar_DateRegulate(RTC_DateTypeDef *DateInput, RTC_DateTypeDef *DateOutput)
{
  uint8_t year = RTC_Bcd2ToByte(DateInput->Year);
  uint8_t month = RTC_Bcd2ToByte(DateInput->Month);
  uint8_t day = RTC_Bcd2ToByte(DateInput->Date);

  /* Reset Globale variable */
  UserEvent = JOY_NONE;

  Calendar_DateDisplay(year, month, day);

  /* Regulate year */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_DisplayStringAtLine(9, (uint8_t*) "UP/DOWN: Select Year");
  UTIL_LCD_DisplayStringAtLine(10, (uint8_t*) "SEL: to set  ");
  Calendar_RegulateYear(&year, &month, &day);

  /* Regulate the month */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_ClearStringLine(9);
  UTIL_LCD_ClearStringLine(10);
  UTIL_LCD_DisplayStringAtLine(9, (uint8_t*) "UP/DOWN: Select Month ");
  UTIL_LCD_DisplayStringAtLine(10, (uint8_t*) "SEL: to set  ");
  Calendar_RegulateMonth(&year, &month, &day);

  /* Regulate day */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_ClearStringLine(9);
  UTIL_LCD_ClearStringLine(10);
  UTIL_LCD_DisplayStringAtLine(9, (uint8_t*) "UP/DOWN/LEFT/RIGHT:");
  UTIL_LCD_DisplayStringAtLine(10, (uint8_t*) "Select Day ");
  UTIL_LCD_DisplayStringAtLine(11, (uint8_t*) "SEL: to set  ");
  Calendar_RegulateDay(&year, &month, &day);

  UTIL_LCD_ClearStringLine(9);
  UTIL_LCD_ClearStringLine(10);
  UTIL_LCD_ClearStringLine(11);

  DateOutput->Year = RTC_ByteToBcd2(year);
  DateOutput->Month = RTC_ByteToBcd2(month);
  DateOutput->Date = RTC_ByteToBcd2(day);
  DateOutput->WeekDay= RTC_ByteToBcd2(DateNb+1);
}


/**
  * @brief  Monitor Year selection
  * @param  Year    to be updated
  * @param  Month   current month
  * @param  Day     current day
  * @retval None
  */
static void Calendar_RegulateYear(uint8_t *Year, uint8_t *Month, uint8_t *Day)
{
  uint8_t tmpValue = *Year;

  /* Endless loop */
  while (UserEvent != JOY_SEL)
  {
    if ((UserEvent == JOY_UP) || (UserEvent == JOY_DOWN))
    {
      /* If "UP" pushbutton is pressed */
      if (UserEvent == JOY_UP)
      {
        /* Decrease the value of the digit */
        tmpValue++;

        if (tmpValue > 99)
        {
          tmpValue = 00;
        }
      }
      /* If "DOWN" pushbutton is pressed */
      if (UserEvent == JOY_DOWN)
      {
        /* Decrease the value of the digit */
        tmpValue--;
        if (tmpValue > 99)
        {
          tmpValue = 99;
        }
      }

      /* display updated calendar */
      Calendar_DateDisplay(tmpValue, *Month, *Day);

      /* Reset Globale variable */
      UserEvent = JOY_NONE;
    }
  }

  /* Reset Globale variable */
  UserEvent = JOY_NONE;

  /* return selected value */
  *Year = tmpValue;
}


/**
  * @brief  Monitor Month selection
  * @param  Year   current year
  * @param  Month  to be updated
  * @param  Day    current day
  * @retval None
  */
static void Calendar_RegulateMonth(uint8_t *Year, uint8_t *Month, uint8_t *Day)
{
  uint8_t tmpValue = *Month;

  /* loop on Key press */
  while (UserEvent != JOY_SEL)
  {
    if ((UserEvent == JOY_UP) || (UserEvent == JOY_DOWN))
    {
      /* If "UP" pushbutton is pressed */
      if (UserEvent == JOY_UP)
      {
        /* Decrease the value of the digit */
        tmpValue++;

        if (tmpValue > 12)
        {
          tmpValue = 1;
        }
      }
      /* If "DOWN" pushbutton is pressed */
      if (UserEvent == JOY_DOWN)
      {
        /* Decrease the value of the digit */
        tmpValue--;
        if (tmpValue < 1)
        {
          tmpValue = 12;
        }
      }

      /* display updated calendar */
      Calendar_DateDisplay(*Year, tmpValue, *Day);

      /* Reset Globale variable */
      UserEvent = JOY_NONE;
    }
  }

  /* Reset Globale variable */
  UserEvent = JOY_NONE;

  /* return selected value */
  *Month = tmpValue;
}


/**
  * @brief  Monitor Day selection
  * @param  Year   current year
  * @param  Month  current month
  * @param  Day    to be updated
  * @retval None
  */
static void Calendar_RegulateDay(uint8_t *Year, uint8_t *Month, uint8_t *Day)
{
  uint8_t tmpValue = *Day;
  uint32_t maxvalue= 0;
  uint8_t increase = 0;

  if (*Month == 2)
  {
    if (Calendar_IsLeapYear(*Year))
      { maxvalue= 29; }
    else
      { maxvalue= (MonLen[*Month - 1] - 1); }
  }
  else
  {
    maxvalue= (MonLen[*Month - 1] - 1);
  }

  /* loop on Key press */
  while (UserEvent != JOY_SEL)
  {
    if ((UserEvent != JOY_SEL) && (UserEvent != JOY_NONE))
    {
      if (UserEvent == JOY_UP)
      {
        tmpValue -= 7;
        increase = 0;
      }
      else if (UserEvent == JOY_DOWN)
      {
        tmpValue += 7;
        increase = 1;
      }
      else if (UserEvent == JOY_LEFT)
      {
        tmpValue--;
        increase = 0;
      }
      else if (UserEvent == JOY_RIGHT)
      {
        tmpValue++;
        increase = 1;
      }

      if (tmpValue > maxvalue)
      {
        if(increase != 0)
        {
          tmpValue = 1;
        }
        else
        {
          tmpValue = maxvalue;
        }
      }

      /* display updated calendar */
      Calendar_DateDisplay(*Year, *Month, tmpValue);

      /* Reset Globale variable */
      UserEvent = JOY_NONE;
    }
  }

  /* Reset Globale variable */
  UserEvent = JOY_NONE;

  /* return selected value */
  *Day = tmpValue;
}


/**
  * @brief  Adjusts time.
  * @param  None
  * @retval None
  */
static void Calendar_AlarmSet(void)
{
  uint32_t ypos = (CharHeight * (TIME_LINE_NB + 1));
  uint32_t xpos;
  uint32_t pXSize;

    BSP_LCD_GetXSize(LCD_INSTANCE, &pXSize);
    xpos = (pXSize - 128) >> 1;

  /* Clear menu lines */
  Calendar_ClearMenuLines();

  kStorage_OpenFileDrawPixel(ypos, xpos, (uint8_t *)"STFILES/ALARM.BMP");

  /* PreAdjust Time */
  Calendar_AlarmPreAdjust();
}


/**
  * @brief  Displays information on LCD for the alarm time configuration
  * @param  None
  * @retval None
  */
static void Calendar_AlarmPreAdjust(void)
{
  RTC_TimeTypeDef timecurrent = {0};
  RTC_TimeTypeDef timeset = {0};
  RTC_DateTypeDef dateset = {0};
  RTC_AlarmTypeDef alarm = {0};


  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &timecurrent, RTC_FORMAT_BCD);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &dateset, RTC_FORMAT_BCD);

  /* Display the current time */
  Calendar_TimeDisplay(&timecurrent);

  /* Call Time selection function */
  Calendar_TimeRegulate(&timecurrent, &timeset);

  /* Get back alarm time */
  alarm.Alarm = RTC_ALARM_A;
  alarm.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
  alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  alarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  alarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  alarm.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
  alarm.AlarmTime.Hours = timeset.Hours;
  alarm.AlarmTime.Minutes = timeset.Minutes;
  alarm.AlarmTime.Seconds = timeset.Seconds;

  /* Set the new alarm time */
  HAL_RTC_SetAlarm_IT(&RtcHandle, &alarm, RTC_FORMAT_BCD);
}


/**
  * @brief  Adjusts Alarm.
  * @param  None
  * @retval None
  */
static void Calendar_AlarmDisable(void)
{
  /* Clear menu lines */
  Calendar_ClearMenuLines();

  /* Set the new alarm time */
  HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_A);

  /* Display information */
  UTIL_LCD_DisplayStringAtLine(TIME_LINE_NB, (uint8_t *)"Alarm is disabled");
  HAL_Delay(500);
}


/**
  * @brief  Alarm A callback function of the Irq Handler that makes led toggling
  * @param  None
  * @retval None
  */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  uint32_t time = 0x40;
  uint8_t led = 0;
  __IO uint32_t period;

  while(time > 0)
  {
    BSP_LED_Toggle((Led_TypeDef) led);

    /* manage led to toggle */
    led++;
    if(led > 3)
    {
      led = 0;

      /* waiting period */
      period = 0xFFFFF;
      while(period > 0)
      {
        period--;
      }
    }

    /* waiting period */
    period = 0xFFFFF;
    while(period > 0)
    {
      period--;
    }

    /* decrease time */
    time--;
  }

  /* make sure led s are off when leaving */
  BSP_LED_Off(LED1);
  BSP_LED_Off(LED2);
  BSP_LED_Off(LED3);
  BSP_LED_Off(LED4);
}


/**
  * @brief  Manage menu lines clearing
  * @param  None
  * @retval None
  */
static void Calendar_ClearMenuLines(void)
{
  /* Clear menu lines */
  UTIL_LCD_ClearStringLine(1);
  UTIL_LCD_ClearStringLine(2);
  UTIL_LCD_ClearStringLine(3);
}


/**
  * @brief  Determines the week number, the month number and the week day number.
  * @param  nyear   the week number
  * @param  nmonth  the month number
  * @param  nday    the week day number
  * @retval None
  */
static void Calendar_WeekDayNum(uint32_t nyear, uint8_t nmonth, uint8_t nday)
{
  uint32_t a = 0, b = 0, c = 0, s = 0, e = 0, f = 0, g = 0, d = 0;
  int32_t n = 0;
  if (nmonth < 3)
  {
    a = nyear - 1;
  }
  else
  {
    a = nyear;
  }

  b = (a / 4) - (a / 100) + (a / 400);
  c = ((a - 1) / 4) - ((a - 1) / 100) + ((a - 1) / 400);
  s = b - c;
  if (nmonth < 3)
  {
    e = 0;
    f =  nday - 1 + 31 * (nmonth - 1);
  }
  else
  {
    e = s + 1;
    f = nday + (153 * (nmonth - 3) + 2) / 5 + 58 + s;
  }
  g = (a + b) % 7;
  d = (f + g - e) % 7;
  n = f + 3 - d;
  if (n < 0)
  {
    wn = 53 - ((g - s) / 5);
  }
  else if (n > (364 + s))
  {
    wn = 1;
  }
  else
  {
    wn = (n / 7) + 1;
  }
  dn = d;
  dc = f + 1;
}


/**
  * @brief  Check whether the passed year is Leap or not.
  * @param  nYear  year number to be checked
  * @retval 1: leap year
  *         0: not leap year
  */
static uint8_t Calendar_IsLeapYear(uint16_t nYear)
{
  if (nYear % 4 != 0) return 0;
  if (nYear % 100 != 0) return 1;
  return (uint8_t)(nYear % 400 == 0);
}


#undef __APP_CALENDAR_C
#endif

