/**
  ******************************************************************************
  * @file    app_audio.c
  * @author  MCD Application Team
  * @brief   Audio application implementation.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
#define __APP_AUDIO_C

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "app_audio.h"
#include "wave_player.h"
#include "wave_recorder.h"
#include "stm32g474e_eval.h"
#include "stm32g474e_eval_lcd.h"
#include "utils.h"
#include "k_config.h"
#include "k_module.h"
#include "k_menu.h"

/* Private typedef -----------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static KMODULE_RETURN _AudioDemoExec(void);

/* Private Variable ----------------------------------------------------------*/

/* Audio menu */
static const tMenuItem AudioMenuMenuItems[] =
{
    {"Wave Player"  , 14, 30, SEL_EXEC,    MODULE_NONE, WavePlayer_Demo,   WavePlayer_UserAction,   NULL, NULL, NULL },
    {"Wave Recorder", 14, 30, SEL_EXEC,    MODULE_NONE, WaveRecorder_Demo, WaveRecorder_UserAction, NULL, NULL, NULL },
    {"Return"       ,  0,  0, SEL_EXIT,    MODULE_NONE, NULL,              NULL,                    NULL, NULL, NULL }
};

static const tMenu AudioMenu = {
  "Audio", AudioMenuMenuItems, countof(AudioMenuMenuItems), TYPE_TEXT, 1, 1 };

/* Private typedef -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
const K_ModuleItem_Typedef ModuleAudio =
{
  MODULE_AUDIO,
  NULL,
  _AudioDemoExec,
  NULL,
  NULL
};

/* WAVE file descriptor */
FIL WaveFile = {0};

/* Audio samples buffer */
uint32_t Buffer[BUFFER_HALF_SIZE_WORD * 2];

/* Progress bar handle */
GLOBAL void * hProgressBar = NULL;

/* Exported variables --------------------------------------------------------*/
/**
  * @brief  Run the applications
  * @param  None.
  * @retval KMODULE_RETURN status.
  */
static KMODULE_RETURN _AudioDemoExec(void)
{
  /* Prepare the main MMI */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  kMenu_Execute(AudioMenu);

  /* Execute the app 8uart */
  /* App initialization    */
  return KMODULE_OK;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
