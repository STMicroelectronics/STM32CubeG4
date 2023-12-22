/**
  ******************************************************************************
  * @file    app_main.c
  * @author  MCD Application Team
  * @brief   Main application (root menu) implementation
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
#define __APP_MAIN_C

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
#include "stm32g474e_eval.h"
#include "stm32g474e_eval_lcd.h"
#include "utils.h"
#include "k_config.h"
#include "k_module.h"
#include "k_menu.h"
#include "k_storage.h"

/* Private constants ---------------------------------------------------------*/
#define ICON_WIDTH  64
#define ICON_HEIGHT 64

/* Display columns positions */
#define X1_POS    32
#define X2_POS    X1_POS + ICON_WIDTH + 3
#define X3_POS    X2_POS + ICON_WIDTH + 3
#define X4_POS    X3_POS + ICON_WIDTH + 3

/* Display ranks positions */
#define Y1_POS    32
#define Y2_POS    Y1_POS + ICON_HEIGHT + 3
#define Y3_POS    Y2_POS + ICON_HEIGHT + 3

/* Private function prototypes -----------------------------------------------*/
static KMODULE_RETURN AppMainExec(void);
static KMODULE_RETURN AppMainExecCheckRessource(void);
static void AppMain_About(void);
static void AppMain_UserEvent(uint8_t sel);

/* Private Variable ----------------------------------------------------------*/
/* Variable used to exit an execution context */
__IO uint8_t exit_func;

tMenuItem MainMenuItems[] =
{
    {"White LED"         , X1_POS, Y1_POS, SEL_MODULE, MODULE_WHITELED    , NULL         , NULL             , NULL, "STFILES/Icon_Led.bmp",           "STFILES/Icon_Led_pink.bmp" },
    {"Math"              , X2_POS, Y1_POS, SEL_MODULE, MODULE_MATH        , NULL         , NULL             , NULL, "STFILES/Icon_Math.bmp",          "STFILES/Icon_Math_pink.bmp" },
    {"UCPD"              , X3_POS, Y1_POS, SEL_MODULE, MODULE_UCPD        , NULL         , NULL             , NULL, "STFILES/Icon_Ucpd.bmp",          "STFILES/Icon_Ucpd_pink.bmp" },
    {"Calendar"          , X4_POS, Y1_POS, SEL_MODULE, MODULE_CALENDAR    , NULL         , NULL             , NULL, "STFILES/Icon_Calendar.bmp",      "STFILES/Icon_Calendar_pink.bmp"  },
    {"Images Viewer"     , X1_POS, Y2_POS, SEL_MODULE, MODULE_VIEWBMP     , NULL         , NULL             , NULL, "STFILES/Icon_Bmp.bmp",           "STFILES/Icon_Bmp_pink.bmp"  },
    {"Audio"             , X2_POS, Y2_POS, SEL_MODULE, MODULE_AUDIO       , NULL         , NULL             , NULL, "STFILES/Icon_Audio.bmp",         "STFILES/Icon_Audio_pink.bmp"  },
    {"Thermometer"       , X3_POS, Y2_POS, SEL_MODULE, MODULE_TSENSOR     , NULL         , NULL             , NULL, "STFILES/Icon_Sensor.bmp",        "STFILES/Icon_Sensor_pink.bmp"  },
    {"Low Power Mode"    , X4_POS, Y2_POS, SEL_MODULE, MODULE_LOWPOWER    , NULL         , NULL             , NULL, "STFILES/Icon_LowPower.bmp",      "STFILES/Icon_LowPower_pink.bmp"  },
    {"Files Browser"     , X1_POS, Y3_POS, SEL_MODULE, MODULE_FILESBRO    , NULL         , NULL             , NULL, "STFILES/Icon_FileBrowser.bmp",   "STFILES/Icon_FileBrowser_pink.bmp" },
    {"About"             , X4_POS, Y3_POS, SEL_EXEC,   0                  , AppMain_About, AppMain_UserEvent, NULL, "STFILES/Icon_About.bmp",         "STFILES/Icon_About_pink.bmp" },
};

const tMenu MainMenu = {
  "Main menu", MainMenuItems, countof(MainMenuItems), TYPE_ICON, 3, 4
  };

/* Private typedef -----------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
const K_ModuleItem_Typedef ModuleAppMain =
{
  MODULE_MAIN_APP,
  NULL,
  AppMainExec,
  NULL,
  AppMainExecCheckRessource
};

/* Private function ----------------------------------------------------------*/
/**
  * @brief  Run the main application
  * @param  None.
  * @note   run and display main menu.
  * @retval KMODULE_RETURN status.
  */
static KMODULE_RETURN AppMainExec(void)
{
  uint8_t index;
  uint8_t indexcol;
  uint8_t indexlin;
  uint8_t iconnb = countof(MainMenuItems);
  uint32_t x, y;
  uint8_t column_nb;
  uint8_t line_nb;
  uint8_t inter_x;
  uint8_t inter_y;

  BSP_LCD_GetXSize(LCD_INSTANCE, &x);
  BSP_LCD_GetYSize(LCD_INSTANCE, &y);

#define MIN_INTER_X 2 // Min 2 pixel between each icon
  /* compute nb of icon on a line */
  column_nb = ( x - MIN_INTER_X) / (ICON_WIDTH + MIN_INTER_X) ;

  /* compute nb of icon lines */
  line_nb = (iconnb / column_nb);

  if((iconnb % column_nb) != 0)
  {
    line_nb++;
  }

  /* compute space between incons on x axis */
  inter_x = ((x - (column_nb * ICON_WIDTH)) / (column_nb + 1)) ;

  /* compute space between incons on x axis */
  inter_y = ((y - UTIL_LCD_GetFont()->Height) - (line_nb * ICON_HEIGHT)) / (line_nb + 1);

  /* fill menu item icon position */
  indexcol = 0;
  indexlin = 0;
  for(index = 0; index < iconnb; index++)
  {
    MainMenuItems[index].x = (inter_x + (indexcol * (inter_x + ICON_WIDTH)));
    MainMenuItems[index].y = (inter_y + (UTIL_LCD_GetFont()->Height) + (indexlin * (inter_y + ICON_HEIGHT)));

    /* increment column index */
    indexcol++;
    if(indexcol >= column_nb)
    {
      indexcol = 0;
      indexlin++;
    }
  }

  kMenu_Execute(MainMenu);
  return KMODULE_OK;
}

/**
  * @brief  check the main application resources
  * @param  None.
  * @note   None.
  * @retval KMODULE_RETURN status.
  */
static KMODULE_RETURN AppMainExecCheckRessource(void)
{
 uint8_t index;

  /* check icon menu */
  for(index = 0; index < countof(MainMenuItems); index++)
  {
    if(kStorage_FileExist((uint8_t *)MainMenuItems[index].pIconPath) != KSTORAGE_NOERROR)
    {
      return KMODULE_ERROR_ICON;
    }
  }

  return KMODULE_OK;
}

/**
  * @brief  Run the About information
  * @param  None.
  * @note   display About information.
  * @retval None.
  */
static void AppMain_About(void)
{
  exit_func = 0;
  UTIL_LCD_SetFont(&Font20);
  UTIL_LCD_Clear(UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);

  UTIL_LCD_DisplayStringAt(0,2*Font20.Height, (uint8_t *)EVAL_BOARD, CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0,3*Font20.Height, (uint8_t *)"Demonstration", CENTER_MODE);

  UTIL_LCD_DisplayStringAt(0,5*Font20.Height, (uint8_t *)DEMO_VERSION, CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0,6*Font20.Height, (uint8_t *)DEMO_DATE, CENTER_MODE);

  UTIL_LCD_DisplayStringAt(0,8*Font20.Height, (uint8_t *)DEMO_INFO1, CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0,9*Font20.Height, (uint8_t *)DEMO_INFO2, CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0,10*Font20.Height, (uint8_t *)DEMO_INFO3, CENTER_MODE);
  while(exit_func==0);
}

/**
  * @brief  Callback used to get user feedback
  * @param  None.
  * @note   None.
  * @retval None.
  */
static void AppMain_UserEvent(uint8_t sel)
{
  exit_func = 1;
}



