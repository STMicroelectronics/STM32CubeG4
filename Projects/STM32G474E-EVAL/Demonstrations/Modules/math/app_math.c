/**
  ******************************************************************************
  * @file    app_math.c
  * @author  MCD Application Team
  * @brief   Math application implementation.
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
#ifndef __APP_MATH_C
#define __APP_MATH_C

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx.h"

#include <string.h>
#include <stdio.h>
#include <arm_math.h>
#include "stm32g474e_eval.h"
#include "stm32g474e_eval_lcd.h"
#include "utils.h"
#include "app_math.h"
#include "k_config.h"
#include "k_module.h"
#include "k_menu.h"
#include "stm32g4xx_ll_cordic.h"

/* Private typedef ----------------------------------------------------------*/
extern tMenuItem psCurrentMenuItem;

/* Private constants ----------------------------------------------------------*/
enum {
  CORDIC_INIT,
  CORDIC_SELECT_FUNCTION,
  CORDIC_WAIT_EVENT,
  CORDIC_EXIT,
  CORDIC_END
};

enum {
  FMAC_INIT,
  FMAC_SELECT_FUNCTION,
  FMAC_WAIT_EVENT,
  FMAC_EXIT,
  FMAC_END
};

/* Function list table dimensions */
#define CORDIC_MAX_FUNCTIONS          3U
#define FMAC_MAX_FUNCTIONS            2U
#define CORDIC_MAX_FUNCTION_NAME_LENGHT     18U
#define FMAC_MAX_FUNCTION_NAME_LENGHT       12U

/* User messages */
#define MSG_TITLE_SELECTION "UP/DOWN: Select - RIGHT: Quit"

/* CORDIC */
#define CORDIC_BLOCKSIZE            100U

/* FMAC */
#define FMAC_BLOCKSIZE              220U
#define FMAC_NUM_TAPS_FIR            52U /* FIR 51: Num Taps FIR must be even for arm_math.h */
#define FMAC_NUM_STAGES_BIQUAD        4U /* IIR 8: Number of 2nd order stages, overall order is 2*numStages */
#define FMAC_STATE_SIZE_FIR     (FMAC_BLOCKSIZE + FMAC_NUM_TAPS_FIR)
#define FMAC_STATE_SIZE_BIQUAD  (4 * FMAC_NUM_STAGES_BIQUAD)
#define FMAC_STATE_SIZE         ((FMAC_STATE_SIZE_FIR>FMAC_STATE_SIZE_BIQUAD)?FMAC_STATE_SIZE_FIR:FMAC_STATE_SIZE_BIQUAD)
#define FMAC_FREQ_SAMPLING       500000U /* 500 KHz */
#define FMAC_FREQ_SAMPLING_KHZ  (FMAC_FREQ_SAMPLING / 1000)

/* common */
#define MATH_DISPLAY_CYCLES           1U
#define MATH_DISPLAY_CPULOAD          2U

/* Private function prototypes -----------------------------------------------*/
KMODULE_RETURN _MathDemoExec(void);
KMODULE_RETURN _MathConfig(void);
KMODULE_RETURN _MathExit(void);

void MathMenuUserAction(uint8_t sel);
void MathUserHeader(void);

static void Math_CORDIC(void);
static void Math_FMAC(void);
static void Math_DisplayTitle(char * Title);
static void Math_DisplayBottomMessage(char * msg);
static void Math_DisplayWindow(void);
static void Math_ClearWindow(void);
static void Cordic_DisplayList(uint8_t Select);
static void Fmac_DisplayList(uint8_t Select);
static void Cordic_DisplayFunction(uint8_t Select);
static void Fmac_DisplayFunction(uint8_t Select);
static void Math_DisplaySignal32b(int32_t *Buffer, uint32_t BufferSize, uint32_t Color);
static void Math_DisplaySignal16b(int16_t *Buffer, uint32_t BufferSize, uint32_t Color);
static void Cordic_DisplayInfo(char * SWLibName, uint32_t PeriphTime, uint32_t SWLibTime, uint32_t DisplayStyle);
static void Fmac_DisplayInfo(char * SWLibName, uint32_t SWLibCpuLoad);

extern void Error_Handler(void);


/* Private Variable ----------------------------------------------------------*/

/* Main Menu */
const tMenuItem MathMenuItems[] =
{
  {"CORDIC (Trigo)"     , 14, 30, SEL_EXEC, MODULE_MATH, Math_CORDIC, MathMenuUserAction, NULL, NULL, NULL  },
  {"FMAC (Filtering)"   , 14, 30, SEL_EXEC, MODULE_MATH, Math_FMAC  , MathMenuUserAction, NULL, NULL, NULL  },
  {"Return"             , 14, 30, SEL_EXIT, MODULE_MATH, NULL       , NULL              , NULL, NULL, NULL  },
};


const tMenu MathMenu = {"Math", MathMenuItems, countof(MathMenuItems), TYPE_TEXT, 1, 1 };

const char CordicFunctionList[CORDIC_MAX_FUNCTIONS][CORDIC_MAX_FUNCTION_NAME_LENGHT] =
                              {"SIN+COS (Polling)", "SIN+COS (DMA)", "SQRT (Polling)"};
const char FmacFunctionList[FMAC_MAX_FUNCTIONS][FMAC_MAX_FUNCTION_NAME_LENGHT] =
                              {"FIR 51", "IIR 8"};

/* Used to exit application */
static __IO uint8_t UserEvent = 0;
static __IO uint8_t UserAction = 0;
static __IO uint16_t Keypressed = FALSE;

/* Variable for FMAC notification */
__IO uint32_t FmacOutputDataReady;

/* Coefficients to run FIR 51:
   For FMAC, 51 coefficients are needed
   For arm_math.h lib, a 52th coefficient is required (0) to reach even number
   of coefficients */
const q15_t fir51_Coeffs[FMAC_NUM_TAPS_FIR] = {
  -30,-89,-81,120,447,569,251,-215,-252,200,455,11,-554,
  -292,572,680,-403,-1122,-34,1566,897,-1946,-2697,2202,10159,14092,10159,2202,
  -2697,-1946,897,1566,-34,-1122,-403,680,572,-292,-554,11,455,200,-252,-215,251,
  569,447,120,-81,-89,-30,0};

/* Coefficients to run IIR 8:
   For FMAC, 9+8 coefficients are needed for IIR 8
   For arm_math.h lib, biquad cascade direct form 1 is used as no IIR available,
   and 6*FMAC_NUM_STAGES_BIQUAD are required */
const q15_t iir8_Coeffs[17] = {
  46,367,1286,2571,3214,2571,1286,367,46,
  16851,-22722,18748,-10971,4325,-1143,179,-13};

const q15_t biquad_Coeffs[6*FMAC_NUM_STAGES_BIQUAD] = {
  16384, 0, 32767,  16384,   8596,   -22180,
  16384, 0, 32767,  16384,   6620,   -9548,
  16384, 0, 32767,  16384,   5629,   -3216,
  16384, 0, 32767,  16384,   5208,   -521};

/* FMAC Input signal: Sine 1KHz + Sine 21 KHz */
const int16_t SIN_1_21KHZ[FMAC_BLOCKSIZE]={
  0,7988,-6977,20337,-7782,23856,0,18307,13479,8426,26040,1053,31130,1050,26042,8420,13481,
  18299,0,23851,-7786,20334,-6985,7988,-10,-7988,6969,-20339,7777,-23862,0,-18314,-13476,
  -8432,-26037,-1055,-31130,-1048,-26044,-8413,-13484,-18292,0,-23845,7790,-20331,6993,
  -7987,20,7989,-6960,20342,-7773,23868,0,18322,13474,8439,26035,1058,31130,1045,26046,
  8407,13486,18284,0,23839,-7794,20329,-7001,7987,-29,-7989,6952,-20345,7769,-23874,0,
  -18329,-13471,-8445,-26033,-1060,-31130,-1043,-26048,-8400,-13488,-18277,0,-23833,7799,
  -20326,7010,-7987,39,7989,-6944,20347,-7765,23879,0,18337,13469,8452,26031,1063,31130,
  1040,26050,8394,13491,18269,0,23828,-7803,20323,-7018,7986,-49,-7990,6936,-20350,7761,
  -23885,0,-18344,-13466,-8458,-26029,-1065,-31130,-1038,-26053,-8388,-13493,-18262,0,
  -23822,7807,-20321,7026,-7986,59,7990,-6928,20353,-7756,23891,0,18352,13464,8464,26026,
  1068,31130,1035,26055,8381,13496,18254,0,23816,-7811,20318,-7034,7986,-68,-7990,6920,
  -20355,7752,-23897,0,-18359,-13462,-8471,-26024,-1070,-31130,-1033,-26057,-8375,-13498,
  -18247,0,-23810,7816,-20315,7042,-7985,78,7991,-6911,20358,-7748,23902,0,18367,13459,
  8477,26022,1073,31129,1030,26059,8368,13501,18239,0,23805,-7820,20313,-7050,7985,-88,
  -7991,6903,-20360};


/* Private typedef -----------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
const K_ModuleItem_Typedef ModuleMath =
{
  MODULE_MATH,
  _MathConfig,
  _MathDemoExec,
  _MathExit,
  NULL
};

/**
  * @brief  setup the HW for Math application
  * @param  None.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _MathConfig(void)
{
  /* CORDIC Configuration */
  CordicHandle.Instance = CORDIC;
  if (HAL_CORDIC_Init(&CordicHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /* FMAC Configuration */
  FmacHandle.Instance = FMAC;
  if (HAL_FMAC_Init(&FmacHandle) != HAL_OK)
  {
    Error_Handler();
  }

  return KMODULE_OK;
}

/**
  * @brief  Run the applications
  * @param  None.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _MathDemoExec(void)
{
  /* App initialization    */
  kMenu_Execute(MathMenu);
  return KMODULE_OK;
}


/**
  * @brief  Exit from the application
  * @param  None.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _MathExit(void)
{
  /* CORDIC DeInitialisation */
  HAL_CORDIC_DeInit(&CordicHandle);

  /* FMAC DeInitialisation */
  HAL_FMAC_DeInit(&FmacHandle);

  return KMODULE_OK;
}


/**
  * @brief  Get User action
  * @param  sel User selection (JOY_SEL,...)
  * @note   This is the only way to get user information.
  * @retval None
  */
void MathMenuUserAction(uint8_t sel)
{
  if (UserAction == 0)
  {
    UserAction = 1;
    UserEvent = sel;
  }
}


/**
  * @brief  Manage the CORDIC function selection
  * @param  None
  * @retval None
  */
static void Math_CORDIC(void)
{
  uint8_t sel = 1;
  uint8_t application_state = CORDIC_INIT;

  do
  {
    switch(application_state)
    {
    case CORDIC_INIT :
      Math_DisplayTitle("CORDIC");
      Math_DisplayBottomMessage(MSG_TITLE_SELECTION);
      Math_DisplayWindow();

    case CORDIC_SELECT_FUNCTION :
      Cordic_DisplayList(sel);
      Cordic_DisplayFunction(sel);
      application_state = CORDIC_WAIT_EVENT;

    case CORDIC_WAIT_EVENT :
      /* User action init */
      UserEvent = JOY_NONE;
      UserAction = 0;
      while(UserEvent == JOY_NONE);

      /* treatment of the user request */
      switch(UserEvent)
      {
      case JOY_DOWN:
        if (sel == CORDIC_MAX_FUNCTIONS)
        {
          sel = 1;
        }
        else
        {
          sel ++;
        }
        application_state = CORDIC_SELECT_FUNCTION;
        break;

      case JOY_UP:
        if (sel == 1)
        {
          sel = CORDIC_MAX_FUNCTIONS;
        }
        else
        {
          sel --;
        }
        application_state = CORDIC_SELECT_FUNCTION;
        break;

      case JOY_SEL:
      case JOY_LEFT:
        /* don't care */
        break;

      case JOY_RIGHT:
        application_state = CORDIC_EXIT;
        break;
      }

      /* Reset the UserEvent */
      UserEvent = JOY_NONE;
      UserAction = 0;
      break;

    case CORDIC_EXIT :
      application_state = CORDIC_END;
      break;
    }
  } while(application_state != CORDIC_END);
}

/**
  * @brief  Manage the FMAC function selection
  * @param  None
  * @retval None
  */
static void Math_FMAC(void)
{
  uint8_t sel = 1;
  uint8_t application_state = FMAC_INIT;

  do
  {
    switch(application_state)
    {
    case FMAC_INIT :
      Math_DisplayTitle("FMAC");
      Math_DisplayBottomMessage(MSG_TITLE_SELECTION);
      Math_DisplayWindow();

    case FMAC_SELECT_FUNCTION :
      Fmac_DisplayList(sel);
      Fmac_DisplayFunction(sel);
      application_state = FMAC_WAIT_EVENT;

    case FMAC_WAIT_EVENT :
      /* User action init */
      UserEvent = JOY_NONE;
      UserAction = 0;
      while(UserEvent == JOY_NONE);

      /* treatment of the user request */
      switch(UserEvent)
      {
      case JOY_DOWN:
        if (sel == FMAC_MAX_FUNCTIONS)
        {
          sel = 1;
        }
        else
        {
          sel ++;
        }
        application_state = FMAC_SELECT_FUNCTION;
        break;

      case JOY_UP:
        if (sel == 1)
        {
          sel = FMAC_MAX_FUNCTIONS;
        }
        else
        {
          sel --;
        }
        application_state = FMAC_SELECT_FUNCTION;
        break;

      case JOY_SEL:
      case JOY_LEFT:
        /* don't care */
        break;

      case JOY_RIGHT:
        application_state = FMAC_EXIT;
        break;
      }

      /* Reset the UserEvent */
      UserEvent = JOY_NONE;
      UserAction = 0;
      break;

    case FMAC_EXIT :
      application_state = FMAC_END;
      break;
    }
  } while(application_state != FMAC_END);
}

/**
  * @brief  Displays the title
  * @param  Title Characters string to display
  * @retval None
  */
static void Math_DisplayTitle(char * Title)
{
  uint32_t XSize;

  /* Clear screen */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Display Title */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
  BSP_LCD_GetXSize(LCD_INSTANCE, &XSize);
  UTIL_LCD_FillRect(0, LINE(0), XSize, Font24.Height, UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAt(0, LINE(0), (uint8_t *)Title, CENTER_MODE);
}

/**
  * @brief  Displays the window for signal representation
  * @param  None
  * @retval None
  */
static void Math_DisplayWindow(void)
{
  uint32_t x, y;
  uint32_t XSize, YSize;
  uint32_t window_XSize, window_YSize;

  /* Get LCD size */
  BSP_LCD_GetXSize(0, (uint32_t *) &XSize);
  BSP_LCD_GetYSize(0, (uint32_t *) &YSize);

  /* Set graph window size */
  window_XSize = (XSize / 2) + 6;
  window_YSize = (YSize / 2) + 10;

  /* Set box initial coordinate */
  x = 0;
  y = (YSize / 4) - 5;

  /* Display Window border */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_FillRect(x, y, window_XSize, window_YSize, UTIL_LCD_COLOR_ST_BLUE);

  /* Clear box */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_FillRect(x + 3, y + 3, window_XSize - 6, window_YSize - 6, UTIL_LCD_COLOR_WHITE);
}

/**
  * @brief  Clear the window for signal representation
  * @param  None
  * @retval None
  */
static void Math_ClearWindow(void)
{
  uint32_t x, y;
  uint32_t XSize, YSize;
  uint32_t window_XSize, window_YSize;

  /* Get LCD size */
  BSP_LCD_GetXSize(0, (uint32_t *) &XSize);
  BSP_LCD_GetYSize(0, (uint32_t *) &YSize);

  /* Set graph window size */
  window_XSize = (XSize / 2) + 6;
  window_YSize = (YSize / 2) + 10;

  /* Set box initial coordinate */
  x = 0;
  y = (YSize / 4) - 5;

  /* Clear box */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_FillRect(x + 3, y + 3, window_XSize - 6, window_YSize - 6, UTIL_LCD_COLOR_WHITE);
}

/**
  * @brief  Displays the message at the bottom of the screen
  * @param  msg characters string to display
  * @retval None
  */
static void Math_DisplayBottomMessage(char * msg)
{
  uint32_t XSize;

  BSP_LCD_GetXSize(LCD_INSTANCE, &XSize);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_FillRect(0, LINE(19), XSize, Font20.Height, UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAt(0, LINE(19), (uint8_t *)msg, CENTER_MODE);
  UTIL_LCD_SetFont(&Font24);
}

/**
  * @brief  Displays the Cordic function list
  * @param  Select actual selection
  * @retval None
  */
static void Cordic_DisplayList(uint8_t Select)
{
  uint8_t i;

  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DrawRect(170 - 3, 40 - 3 , 6 + CORDIC_MAX_FUNCTION_NAME_LENGHT*Font12.Width, 6 + CORDIC_MAX_FUNCTIONS*Font12.Height, UTIL_LCD_COLOR_ST_GREEN_DARK);

  for (i = 0; i < CORDIC_MAX_FUNCTIONS; i++)
  {
    if (i == (Select - 1))
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    }
    else
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
    }

    UTIL_LCD_DisplayStringAt(170, 40 + Font12.Height*i, (uint8_t *)CordicFunctionList[i], LEFT_MODE);
  }

  UTIL_LCD_SetFont(&Font24);
}

/**
  * @brief  Displays the Fmac function list
  * @param  Select actual selection
  * @retval None
  */
static void Fmac_DisplayList(uint8_t Select)
{
  uint8_t i;

  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_DrawRect(170 - 3, 40 -3 , 6 + FMAC_MAX_FUNCTION_NAME_LENGHT*Font16.Width, 6 + FMAC_MAX_FUNCTIONS*Font16.Height, UTIL_LCD_COLOR_ST_GREEN_DARK);

  for (i = 0; i < FMAC_MAX_FUNCTIONS; i++)
  {
    if (i == (Select - 1))
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    }
    else
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
    }

    UTIL_LCD_DisplayStringAt(170, 40 + Font16.Height*i, (uint8_t *)FmacFunctionList[i], LEFT_MODE);
  }

  UTIL_LCD_SetFont(&Font24);
}

/**
  * @brief  Display the math function
  * @param  Select actual selection
  * @retval None
  */
static void Cordic_DisplayFunction(uint8_t Select)
{
  CORDIC_ConfigTypeDef sConfig;

  uint64_t TimeStart, TimeEnd;
  uint32_t TimePeriph, TimeSWLib;
  uint32_t index;

  int32_t InputBuffer[CORDIC_BLOCKSIZE];
  int32_t OutputBuffer[CORDIC_BLOCKSIZE*2] = {0};
  int32_t OutputBuffer2[CORDIC_BLOCKSIZE] = {0};

  int32_t *ptrIn = InputBuffer;
  int32_t *ptrOut = OutputBuffer;

  switch (Select)
  {
  case 1 : /* SIN + COS (Polling) */
  case 2 : /* SIN + COS (DMA) */
    /* Prepare input buffer for CORDIC, range [0..2pi] */
    for (uint32_t index = 0; index < CORDIC_BLOCKSIZE; index++)
    {
      InputBuffer[index] = (2 * index * (0x80000000U / CORDIC_BLOCKSIZE));
    }

    /* Prepare CORDIC configuration */
    sConfig.Function         = CORDIC_FUNCTION_SINE;
    sConfig.Precision        = CORDIC_PRECISION_6CYCLES;
    sConfig.Scale            = CORDIC_SCALE_0;
    sConfig.NbWrite          = CORDIC_NBWRITE_1;
    sConfig.NbRead           = CORDIC_NBREAD_2;
    sConfig.InSize           = CORDIC_INSIZE_32BITS;
    sConfig.OutSize          = CORDIC_OUTSIZE_32BITS;

    break;

  case 3 : /* SQRT (Polling) */
    /* Prepare input buffer for CORDIC, range [0,027..0,75] */
    for (uint32_t index = 0; index < CORDIC_BLOCKSIZE; index++)
    {
      InputBuffer[index] = 0x0374BC6AU + (index*((0x5FFFFFFFU - 0x0374BC6AU) / CORDIC_BLOCKSIZE));
    }

    /* Prepare CORDIC configuration */
    sConfig.Function         = CORDIC_FUNCTION_SQUAREROOT;
    sConfig.Precision        = CORDIC_PRECISION_3CYCLES;
    sConfig.Scale            = CORDIC_SCALE_0;
    sConfig.NbWrite          = CORDIC_NBWRITE_1;
    sConfig.NbRead           = CORDIC_NBREAD_1;
    sConfig.InSize           = CORDIC_INSIZE_32BITS;
    sConfig.OutSize          = CORDIC_OUTSIZE_32BITS;

    break;
  }

  /* Configure the CORDIC peripheral */
  if (HAL_CORDIC_Configure(&CordicHandle, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* Get TimeStart */
  TimeStart = (((uint64_t)SystemCoreClock / 1000U) * (HAL_GetTick() + 1)) - SysTick->VAL;

  switch (Select)
  {
  case 1 : /* SIN + COS (Polling) */
    /* Start CORDIC calculation in Polling Mode */
    LL_CORDIC_WriteData(CORDIC, *ptrIn++);
    for (index = (CORDIC_BLOCKSIZE-1); index > 0; index--)
    {
      LL_CORDIC_WriteData(CORDIC, *ptrIn++);
      *ptrOut++ = LL_CORDIC_ReadData(CORDIC); /* SIN */
      *ptrOut++ = LL_CORDIC_ReadData(CORDIC); /* COS */
    }
    *ptrOut++ = LL_CORDIC_ReadData(CORDIC);   /* SIN */
    *ptrOut++ = LL_CORDIC_ReadData(CORDIC);   /* COS */

    break;

  case 2 : /* SIN + COS (DMA) */
    /* Start CORDIC calculation in DMA Mode */
    if (HAL_CORDIC_Calculate_DMA(&CordicHandle, InputBuffer, OutputBuffer,
                                 CORDIC_BLOCKSIZE, CORDIC_DMA_DIR_IN_OUT) != HAL_OK)
    {
      Error_Handler();
    }

    /* Wait CORDIC calculation completion */
    while (HAL_CORDIC_GetState(&CordicHandle) != HAL_CORDIC_STATE_READY) {}

    break;

  case 3 : /* SQRT (Polling) */
    /* Start CORDIC calculation in Polling Mode */
    LL_CORDIC_WriteData(CORDIC, *ptrIn++);
    for (index = (CORDIC_BLOCKSIZE-1); index > 0; index--)
    {
      LL_CORDIC_WriteData(CORDIC, *ptrIn++);
      *ptrOut++ = LL_CORDIC_ReadData(CORDIC); /* SQRT */
    }
    *ptrOut++ = LL_CORDIC_ReadData(CORDIC);   /* SQRT */

    break;
  }

  /* Measure computation time */
  TimeEnd = (((uint64_t)SystemCoreClock / 1000U)*(HAL_GetTick() + 1)) - SysTick->VAL;
  TimePeriph = TimeEnd - TimeStart;

  switch (Select)
  {
  case 1 : /* SIN + COS (Polling) */
  case 2 : /* SIN + COS (DMA) */
    /* Reorder output buffers : SIN in OutputBuffer, COS in OutputBuffer2 */
    for (uint32_t index = 0; index < CORDIC_BLOCKSIZE; index++)
    {
      OutputBuffer[index] = OutputBuffer[index*2];
      OutputBuffer2[index] = OutputBuffer[(index*2) + 1];
    }
    break;
  case 3 : /* SQRT (Polling) */
    break;
  }

  /* Clear Window */
  Math_ClearWindow();

  /* Display computed Signal */
  Math_DisplaySignal32b(OutputBuffer, CORDIC_BLOCKSIZE, UTIL_LCD_COLOR_ST_PINK);

  /* Display second computed Signal if any */
  switch (Select)
  {
  case 1 : /* SIN + COS (Polling) */
  case 2 : /* SIN + COS (DMA) */
    Math_DisplaySignal32b(OutputBuffer2, CORDIC_BLOCKSIZE, UTIL_LCD_COLOR_ST_PURPLE);
  break;
  case 3 : /* SQRT (Polling) */
    break;
  }

  switch (Select)
  {
  case 1 : /* SIN + COS (Polling) */
  case 2 : /* SIN + COS (DMA) */
    /* Prepare input buffer for arm fast math lib, range [0..2pi] */
    for (index = 0; index < CORDIC_BLOCKSIZE; index++)
    {
      InputBuffer[index] = (index*(0x80000000U / CORDIC_BLOCKSIZE));
    }
    break;

  case 3 : /* SQRT (Polling) */
    /* Prepare input buffer for arm fast math lib, range [0,027..0,75] */
    for (uint32_t index = 0; index < CORDIC_BLOCKSIZE; index++)
    {
      InputBuffer[index] = 0x0374BC6AU + (index*((0x5FFFFFFFU - 0x0374BC6AU) / CORDIC_BLOCKSIZE));
    }
    break;
  }

  /* Get TimeStart */
  TimeStart = (((uint64_t)SystemCoreClock / 1000U)*(HAL_GetTick() + 1)) - SysTick->VAL;

  switch (Select)
  {
  case 1 : /* SIN + COS (Polling) */
  case 2 : /* SIN + COS (DMA) */
    /* Perform arm fast math lib calculation */
    for (index = 0; index < CORDIC_BLOCKSIZE; index++)
    {
       OutputBuffer[index] = arm_sin_q31(InputBuffer[index]);
       OutputBuffer2[index] = arm_cos_q31(InputBuffer[index]);
    }
    break;

  case 3 : /* SQRT (Polling) */
    /* Perform arm fast math lib calculation */
    for (index = 0; index < CORDIC_BLOCKSIZE; index++)
    {
       arm_sqrt_q31(InputBuffer[index], &OutputBuffer[index]);
    }
    break;
  }

  /* Measure computation time */
  TimeEnd = (((uint64_t)SystemCoreClock / 1000U)*(HAL_GetTick() + 1)) - SysTick->VAL;
  TimeSWLib = TimeEnd - TimeStart;

  /* Display infos */
  switch (Select)
  {
  case 1 : /* SIN + COS (Polling) */
    Cordic_DisplayInfo("arm_math.h", TimePeriph, TimeSWLib, MATH_DISPLAY_CYCLES);
    break;

  case 2 : /* SIN + COS (DMA) */
    Cordic_DisplayInfo("arm_math.h", TimePeriph, TimeSWLib, (MATH_DISPLAY_CYCLES | MATH_DISPLAY_CPULOAD));
    break;

  case 3 : /* SQRT (Polling) */
    Cordic_DisplayInfo("arm_math.h", TimePeriph, TimeSWLib, MATH_DISPLAY_CYCLES);
    break;
  }
}

/**
  * @brief  Display the math function
  * @param  Select actual selection
  * @retval None
  */
static void Fmac_DisplayFunction(uint8_t Select)
{
  FMAC_FilterConfigTypeDef sConfig;

  uint64_t TimeStart, TimeEnd;
  uint32_t TimeSWLib;
  uint64_t CpuLoadSWLib;
  uint32_t index;

  int16_t  InputBuffer[FMAC_BLOCKSIZE];
  int16_t  OutputBuffer[FMAC_BLOCKSIZE] = {0};

  uint16_t FmacOutputDataSize;
  uint16_t FmacInputDataSize;

  arm_fir_instance_q15 fir_S;
  arm_biquad_casd_df1_inst_q15 iir_S;

  q15_t State[FMAC_STATE_SIZE] = {0};

  /* Null vector to preload FMAC internal output buffer (OutputBufferSize)*/
  int16_t null_vector[16] = {0};

  /* Reset Fmac notification */
  FmacOutputDataReady = 0;

  switch (Select)
  {
  case 1 : /* FIR 51 */
    /* Prepare Input Buffer: samples must not be scaled before injecting
       in FMAC, as gain (R) is 0 */
    for (index = 0; index < FMAC_BLOCKSIZE; index++)
    {
      InputBuffer[index] = SIN_1_21KHZ[index];
    }

    /* Prepare FMAC configuration */
    sConfig.InputBaseAddress    = 0;
    sConfig.InputBufferSize     = 0x40;
    sConfig.InputThreshold      = FMAC_THRESHOLD_1;
    sConfig.InputAccess         = FMAC_BUFFER_ACCESS_DMA;
    sConfig.CoeffBaseAddress    = 0x40;
    sConfig.CoeffBufferSize     = 51;
    sConfig.CoeffBSize          = 51;
    sConfig.CoeffASize          = 0;
    sConfig.Filter              = FMAC_FUNC_CONVO_FIR;
    sConfig.OutputAccess        = FMAC_BUFFER_ACCESS_DMA;
    sConfig.OutputBaseAddress   = 0x80;
    sConfig.OutputBufferSize    = 16;
    sConfig.OutputThreshold     = 1;
    sConfig.P                   = 51;
    sConfig.Q                   = 0;
    sConfig.R                   = 0;
    sConfig.Clip                = FMAC_CLIP_ENABLED;
    sConfig.pCoeffB             = (int16_t*)fir51_Coeffs;
    sConfig.pCoeffA             = NULL;
    break;

  case 2 : /* IIR 8 */
    /* Prepare Input Buffer: samples must be right shifted by 2 before injecting
       in FMAC, as gain (R) is 2 (to avoid saturation) */
    for (index = 0; index < FMAC_BLOCKSIZE; index++)
    {
      InputBuffer[index] = SIN_1_21KHZ[index] >> 2;
    }

    /* Prepare FMAC configuration */
    sConfig.InputBaseAddress    = 0;
    sConfig.InputBufferSize     = 0x40;
    sConfig.InputThreshold      = FMAC_THRESHOLD_1;
    sConfig.InputAccess         = FMAC_BUFFER_ACCESS_DMA;
    sConfig.CoeffBaseAddress    = 0x40;
    sConfig.CoeffBufferSize     = 17;
    sConfig.CoeffBSize          = 9;
    sConfig.CoeffASize          = 8;
    sConfig.Filter              = FMAC_FUNC_IIR_DIRECT_FORM_1;
    sConfig.OutputAccess        = FMAC_BUFFER_ACCESS_DMA;
    sConfig.OutputBaseAddress   = 0x80;
    sConfig.OutputBufferSize    = 16;
    sConfig.OutputThreshold     = 1;
    sConfig.P                   = 9;
    sConfig.Q                   = 8;
    sConfig.R                   = 2;
    sConfig.Clip                = FMAC_CLIP_ENABLED;
    sConfig.pCoeffB             = (int16_t*)iir8_Coeffs;
    sConfig.pCoeffA             = (int16_t*)iir8_Coeffs + 9;
    break;
  }

  /* Configure FMAC peripheral */
  if (HAL_FMAC_FilterConfig(&FmacHandle,&sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* Perform FMAC calculation */
  if (HAL_FMAC_FilterPreload(&FmacHandle, NULL, 0, (int16_t *)null_vector, sConfig.OutputBufferSize) != HAL_OK)
  {
    Error_Handler();
  }

  /* Start FMAC */
  FmacInputDataSize = FMAC_BLOCKSIZE;
  FmacOutputDataSize = FmacInputDataSize - sConfig.CoeffBSize + 1;
  if (HAL_FMAC_FilterStart(&FmacHandle, OutputBuffer, &FmacOutputDataSize) != HAL_OK)
  {
    Error_Handler();
  }

  /* Provide input data to FMAC */
  if (HAL_FMAC_AppendFilterData(&FmacHandle, InputBuffer, &FmacInputDataSize) != HAL_OK)
  {
    Error_Handler();
  }

  /* Wait FMAC calculation completion */
  while(FmacOutputDataReady == 0);

  /* Stop FMAC */
  if (HAL_FMAC_FilterStop(&FmacHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /* Clear Window */
  Math_ClearWindow();

  /* Display graphic, discarding first 40 samples (due to filter settling) */
  Math_DisplaySignal16b((int16_t*)SIN_1_21KHZ + 40, 100, UTIL_LCD_COLOR_ST_GRAY_LIGHT);
  Math_DisplaySignal16b(OutputBuffer + 40, 100, UTIL_LCD_COLOR_ST_PINK);

  switch (Select)
  {
  case 1 : /* FIR 51 */
    /* Prepare Input Buffer for FIR: samples are not scaled */
    for (index = 0; index < FMAC_BLOCKSIZE; index++)
    {
      InputBuffer[index] = SIN_1_21KHZ[index];
    }
    break;

  case 2 : /* IIR 8 */
    /* Prepare Input Buffer for biquad filter: samples must be right shifted by 3 to avoid saturation */
    for (index = 0; index < FMAC_BLOCKSIZE; index++)
    {
      InputBuffer[index] = SIN_1_21KHZ[index] >> 3;
    }
    break;
  }

  /* Get TimeStart */
  TimeStart = (((uint64_t)SystemCoreClock/1000U)*(HAL_GetTick() + 1)) - SysTick->VAL;

  switch (Select)
  {
  case 1 : /* FIR 51 */
    /* Initialize filter with arm math library */
    arm_fir_init_q15(&fir_S, FMAC_NUM_TAPS_FIR, (q15_t*)fir51_Coeffs, State, FMAC_BLOCKSIZE);

    /* Perform calculation with arm math library */
    arm_fir_q15(&fir_S, InputBuffer, OutputBuffer, FMAC_BLOCKSIZE);

    break;

  case 2 : /* IIR 8 */
    /* Initialize filter with arm math library */
    arm_biquad_cascade_df1_init_q15(&iir_S, FMAC_NUM_STAGES_BIQUAD, (q15_t*)biquad_Coeffs, State, 0);

    /* Perform calculation with arm math library */
    arm_biquad_cascade_df1_q15(&iir_S, InputBuffer, OutputBuffer, FMAC_BLOCKSIZE);

    break;
  }

  /* Measure computation time */
  TimeEnd = (((uint64_t)SystemCoreClock / 1000U)*(HAL_GetTick() + 1)) - SysTick->VAL;
  TimeSWLib = TimeEnd - TimeStart;

  /* Compute CPU load */
  CpuLoadSWLib = (uint64_t)100 * (uint64_t)(TimeSWLib / FMAC_BLOCKSIZE) * (uint64_t)FMAC_FREQ_SAMPLING / (uint64_t)SystemCoreClock;

  /* Display time */
  Fmac_DisplayInfo("arm_math.h", (uint32_t) CpuLoadSWLib);
}

/**
  * @brief  Displays the signal graphic
  * @param  Buffer     data array to display
  * @param  BufferSize number of data
  * @param  Color      color of the graphic plots
  * @retval None
  */
static void Math_DisplaySignal32b(int32_t *Buffer, uint32_t BufferSize, uint32_t Color)
{
  uint64_t x1, y1, x2, y2;
  uint32_t XSize, YSize;
  uint32_t window_XSize, window_YSize;
  uint32_t index;

  /* Get LCD size */
  BSP_LCD_GetXSize(0, &XSize);
  BSP_LCD_GetYSize(0, &YSize);

  /* Set graph window size */
  window_XSize = XSize / 2;
  window_YSize = YSize / 2;

  x1 = 3;
  y1 = (YSize / 2) - (((int64_t)Buffer[0] * window_YSize) / 0xFFFFFFFFU);

  for (index = 1; index < BufferSize; index++)
  {
    x2 = 3 + ((index * window_XSize/BufferSize));
    y2 = (YSize / 2) - (((int64_t)Buffer[index] * window_YSize) / 0xFFFFFFFFU); /* '-' because inverse coordinates */

    /* Draw line until (x2,y2) */
    UTIL_LCD_DrawLine(x1, y1, x2, y2, Color);
    x1 = x2;
    y1 = y2;
  }
}

/**
  * @brief  Displays the signal graphic
  * @param  Buffer     data array to display
  * @param  BufferSize number of data
  * @param  Color      color of the graphic plots
  * @retval None
  */
static void Math_DisplaySignal16b(int16_t *Buffer, uint32_t BufferSize, uint32_t Color)
{
  int32_t x1, y1, x2, y2;
  int32_t XSize, YSize;
  int32_t window_XSize, window_YSize;
  int32_t index;

  /* Get LCD size */
  BSP_LCD_GetXSize(0, (uint32_t *) &XSize);
  BSP_LCD_GetYSize(0, (uint32_t *) &YSize);

  /* Set graph window size */
  window_XSize = XSize / 2;
  window_YSize = YSize / 2;

  x1 = 3;
  y1 = (YSize / 2) - (Buffer[0] * window_YSize / 0xFFFF);

  for (index = 1; index < BufferSize; index++)
  {
    x2 = 3 + ((index * window_XSize/BufferSize));
    y2 = (YSize / 2) - (Buffer[index] * window_YSize / 0xFFFF); /* '-' because inverse coordinates */

    /* Draw line until (x2,y2) */
    UTIL_LCD_DrawLine(x1, y1, x2, y2, Color);
    x1 = x2;
    y1 = y2;
  }
}

/**
  * @brief  Displays the time performance of computation with Peripheral and SW Lib
  * @param  SWLibName    pointer to characters string of Sw lib name
  * @param  PeriphTime   time value in cycles, with peripheral
  * @param  SWLibTime    time value in cycles, with SW lib
  * @param  DisplayStyle Style of info display (cycles, cpu load, or both)
  * @retval None
  */
static void Cordic_DisplayInfo(char * SWLibName, uint32_t PeriphTime, uint32_t SWLibTime, uint32_t DisplayStyle)
{
  uint8_t StringToDisplay[100];
  uint8_t line = 6;

  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_DisplayStringAt(170, LINE(line++), (uint8_t*)"Time to compl.", LEFT_MODE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
  sprintf((char*)StringToDisplay,"%ld cycles ", PeriphTime);
  UTIL_LCD_DisplayStringAt(170, LINE(line++), (uint8_t *)StringToDisplay, LEFT_MODE);

  if ((DisplayStyle & MATH_DISPLAY_CPULOAD) != 0)
  {
    UTIL_LCD_DisplayStringAt(170, LINE(line++), (uint8_t *)"0% cpu load", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(170, LINE(line++), (uint8_t *)"             ", LEFT_MODE);
  }

  line++;
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_DisplayStringAt(170, LINE(line++), (uint8_t *)SWLibName, LEFT_MODE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);

  sprintf((char*)StringToDisplay,"%ld cycles ", SWLibTime);
  UTIL_LCD_DisplayStringAt(170, LINE(line++), (uint8_t *)StringToDisplay, LEFT_MODE);

  if ((DisplayStyle & MATH_DISPLAY_CPULOAD) != 0)
  {
    UTIL_LCD_DisplayStringAt(170, LINE(line++), (uint8_t *)"100% cpu load", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(170, LINE(line++), (uint8_t *)"             ", LEFT_MODE);
  }

  line = 12;
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
  sprintf((char*)StringToDisplay,"%d samples ", CORDIC_BLOCKSIZE);
  UTIL_LCD_DisplayStringAt(20, LINE(line++), (uint8_t *)StringToDisplay, LEFT_MODE);
}

/**
  * @brief  Displays the time performance of computation with Peripheral and SW Lib
  * @param  SWLibName     pointer to characters string of Sw lib name
  * @param  SWLibCpuLoad  cpu load, with SW lib
  * @retval None
  */
static void Fmac_DisplayInfo(char * SWLibName, uint32_t SWLibCpuLoad)
{
  uint8_t StringToDisplay[100];
  uint8_t line = 6;

  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_DisplayStringAt(170, LINE(line++), (uint8_t *)"FMAC (DMA)", LEFT_MODE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_DisplayStringAt(170, LINE(line++), (uint8_t *)"0% cpu load ", LEFT_MODE);

  line++;
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
  UTIL_LCD_DisplayStringAt(170, LINE(line++), (uint8_t *)SWLibName, LEFT_MODE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
  sprintf((char*)StringToDisplay,"%ld%% cpu load   ", SWLibCpuLoad);
  UTIL_LCD_DisplayStringAt(170, LINE(line++), (uint8_t *)StringToDisplay, LEFT_MODE);

  line = 12;
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
  sprintf((char*)StringToDisplay,"%ldKHz sampling", (uint32_t)FMAC_FREQ_SAMPLING_KHZ);
  UTIL_LCD_DisplayStringAt(2, LINE(line++), (uint8_t *)StringToDisplay, LEFT_MODE);
}

/**
  * @brief  FMAC Output Data process complete callback.
  * @param  FmacHandle DMA handle.
  * @retval None
  */
void HAL_FMAC_OutputDataReadyCallback(FMAC_HandleTypeDef *FmacHandle)
{
  FmacOutputDataReady ++;
}

/**
* @brief  CORDIC MSP Initialization
* @param  CordicHandle pointer to the CORDIC handle
* @retval None
*/
void HAL_CORDIC_MspInit(CORDIC_HandleTypeDef* CordicHandle)
{
  /* CORDIC Peripheral clock enable */
  __HAL_RCC_CORDIC_CLK_ENABLE();

  /* DMA1 and DMAMUX1 Peripherals clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA WRITE channel Init */
  DmaCordicWriteHandle.Instance                 = DMA1_Channel3;
  DmaCordicWriteHandle.Init.Request             = DMA_REQUEST_CORDIC_WRITE;
  DmaCordicWriteHandle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  DmaCordicWriteHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  DmaCordicWriteHandle.Init.MemInc              = DMA_MINC_ENABLE;
  DmaCordicWriteHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  DmaCordicWriteHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  DmaCordicWriteHandle.Init.Mode                = DMA_NORMAL;
  DmaCordicWriteHandle.Init.Priority            = DMA_PRIORITY_LOW;
  HAL_DMA_Init(&DmaCordicWriteHandle);
  /* Link DMA handle to CORDIC */
  __HAL_LINKDMA(CordicHandle,hdmaIn,DmaCordicWriteHandle);

  /* DMA READ channel Init */
  DmaCordicReadHandle.Instance                  = DMA1_Channel4;
  DmaCordicReadHandle.Init.Request              = DMA_REQUEST_CORDIC_READ;
  DmaCordicReadHandle.Init.Direction            = DMA_PERIPH_TO_MEMORY;
  DmaCordicReadHandle.Init.PeriphInc            = DMA_PINC_DISABLE;
  DmaCordicReadHandle.Init.MemInc               = DMA_MINC_ENABLE;
  DmaCordicReadHandle.Init.PeriphDataAlignment  = DMA_PDATAALIGN_WORD;
  DmaCordicReadHandle.Init.MemDataAlignment     = DMA_MDATAALIGN_WORD;
  DmaCordicReadHandle.Init.Mode                 = DMA_NORMAL;
  DmaCordicReadHandle.Init.Priority             = DMA_PRIORITY_LOW;
  HAL_DMA_Init(&DmaCordicReadHandle);
  /* Link DMA handle to CORDIC */
  __HAL_LINKDMA(CordicHandle,hdmaOut,DmaCordicReadHandle);

  /* DMA interrupts Init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}

/**
* @brief  CORDIC MSP De-Initialization
* @param  CordicHandle pointer to the CORDIC handle
* @retval None
*/
void HAL_CORDIC_MspDeInit(CORDIC_HandleTypeDef* CordicHandle)
{
  /* CORDIC Peripheral clock disable */
  __HAL_RCC_CORDIC_CLK_DISABLE();

  /* Reset CORDIC peripheral */
  __HAL_RCC_CORDIC_FORCE_RESET();
  __HAL_RCC_CORDIC_RELEASE_RESET();

  /* CORDIC DMA DeInit */
  HAL_DMA_DeInit(CordicHandle->hdmaIn);
  HAL_DMA_DeInit(CordicHandle->hdmaOut);

  /* DMA interrupts DeInit */
  HAL_NVIC_DisableIRQ(DMA1_Channel3_IRQn);
  HAL_NVIC_DisableIRQ(DMA1_Channel4_IRQn);
}

/**
  * @brief  Initializes the FMAC BSP.
  * @param  FmacHandle pointer to the HAL FMAC handle
  * @retval None
  */
void HAL_FMAC_MspInit(FMAC_HandleTypeDef* FmacHandle)
{
  /* Enable FMAC clock */
  __HAL_RCC_FMAC_CLK_ENABLE();

  /* Configuration of DMAs */
  /* DMA clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMAMUX1_CLK_ENABLE();

  /* Configure DMA handle init parameters */
  DmaFmacInHandle.Instance                     = DMA1_Channel5;
  DmaFmacInHandle.Init.Request                 = DMA_REQUEST_FMAC_WRITE;
  DmaFmacInHandle.Init.Direction               = DMA_MEMORY_TO_PERIPH;
  DmaFmacInHandle.Init.PeriphInc               = DMA_PINC_DISABLE;
  DmaFmacInHandle.Init.MemInc                  = DMA_MINC_ENABLE;
  DmaFmacInHandle.Init.PeriphDataAlignment     = DMA_PDATAALIGN_WORD;
  DmaFmacInHandle.Init.MemDataAlignment        = DMA_MDATAALIGN_HALFWORD;
  DmaFmacInHandle.Init.Mode                    = DMA_NORMAL;
  DmaFmacInHandle.Init.Priority                = DMA_PRIORITY_HIGH;
  HAL_DMA_Init(&DmaFmacInHandle);
  /* Link DMA handle to FMAC */
  __HAL_LINKDMA(FmacHandle, hdmaIn, DmaFmacInHandle);

  /* Configure DMA handle init parameters */
  DmaFmacOutHandle.Instance                     = DMA1_Channel6;
  DmaFmacOutHandle.Init.Request                 = DMA_REQUEST_FMAC_READ;
  DmaFmacOutHandle.Init.Direction               = DMA_PERIPH_TO_MEMORY;
  DmaFmacOutHandle.Init.PeriphInc               = DMA_PINC_DISABLE;
  DmaFmacOutHandle.Init.MemInc                  = DMA_MINC_ENABLE;
  DmaFmacOutHandle.Init.PeriphDataAlignment     = DMA_PDATAALIGN_WORD;
  DmaFmacOutHandle.Init.MemDataAlignment        = DMA_MDATAALIGN_HALFWORD;
  DmaFmacOutHandle.Init.Mode                    = DMA_NORMAL;
  DmaFmacOutHandle.Init.Priority                = DMA_PRIORITY_HIGH;
  HAL_DMA_Init(&DmaFmacOutHandle);
  /* Link DMA handle to FMAC */
  __HAL_LINKDMA(FmacHandle, hdmaOut, DmaFmacOutHandle);

  /* Configure DMA handle init parameters */
  DmaFmacPreloadHandle.Instance                 = DMA1_Channel7;
  DmaFmacPreloadHandle.Init.Request             = 0;
  DmaFmacPreloadHandle.Init.Direction           = DMA_MEMORY_TO_MEMORY;
  DmaFmacPreloadHandle.Init.PeriphInc           = DMA_PINC_ENABLE;
  DmaFmacPreloadHandle.Init.MemInc              = DMA_MINC_DISABLE;
  DmaFmacPreloadHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  DmaFmacPreloadHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  DmaFmacPreloadHandle.Init.Mode                = DMA_NORMAL;
  DmaFmacPreloadHandle.Init.Priority            = DMA_PRIORITY_HIGH;
  HAL_DMA_Init(&DmaFmacPreloadHandle);
  /* Link DMA handle to FMAC */
  __HAL_LINKDMA(FmacHandle, hdmaPreload, DmaFmacPreloadHandle);

  /* Configure DMA interruptions to use FMAC with DMA In */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0x00, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

  /* Configure DMA interruptions to use FMAC with DMA Out */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0x00, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

  /* Configure DMA interruptions to use FMAC with Memory to Memory */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0x00, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

/**
  * @brief  De-Initializes the FMAC BSP.
  * @param  FmacHandle pointer to the HAL FMAC handle
  * @retval None
  */
void HAL_FMAC_MspDeInit(FMAC_HandleTypeDef* FmacHandle)
{
  /* Disable FMAC clock */
  __HAL_RCC_FMAC_CLK_DISABLE();

  /* Reset FMAC */
  __HAL_RCC_FMAC_FORCE_RESET();
  __HAL_RCC_FMAC_RELEASE_RESET();

  /* Deinitialization of DMAs */
  HAL_DMA_DeInit(FmacHandle->hdmaIn);
  HAL_DMA_DeInit(FmacHandle->hdmaOut);

  /* Deinitialization of interruptions */
  HAL_NVIC_DisableIRQ(DMA1_Channel5_IRQn);
  HAL_NVIC_DisableIRQ(DMA1_Channel6_IRQn);
  HAL_NVIC_DisableIRQ(DMA1_Channel7_IRQn);
}


#undef __APP_MATH_C
#endif

