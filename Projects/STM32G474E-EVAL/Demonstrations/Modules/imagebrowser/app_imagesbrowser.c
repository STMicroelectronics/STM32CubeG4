/**
  ******************************************************************************
  * @file    app_imagesbrowser.c
  * @author  MCD Application Team
  * @brief   Images browser application implementation
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
#define __APP_IMAGEBROWSER_C

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32g4xx.h"
#include "stm32g474e_eval.h"
#include "k_config.h"
#include "k_module.h"
#include "k_menu.h"
#include "k_storage.h"

/* Private typedef ----------------------------------------------------------*/
/* Private constants ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
KMODULE_RETURN _ImageBrowserDemoExec(void);

void ImageBrowserDemo(void);
void ImageBrowserMenuUserAction(uint8_t sel);
void ImageBrowserUserInformation(int16_t tempvalue);
void ImageBrowserUserHeader(void);

/* Private Variable ----------------------------------------------------------*/
const tMenuItem ImageBrowserMenuMenuItems[] =
{
    {NULL, 14, 30, TYPE_EXEC, MODULE_NONE, ImageBrowserDemo, ImageBrowserMenuUserAction, NULL, NULL, NULL  },
};

const tMenu ImageBrowserMenu = {
  NULL, ImageBrowserMenuMenuItems, countof(ImageBrowserMenuMenuItems), TYPE_EXEC, 1, 1 };

/* used to exit application */
static __IO uint8_t user_event=0;
static __IO uint8_t user_action=0;

/* Private typedef -----------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
const K_ModuleItem_Typedef ModuleImageBrowser =
{
  MODULE_TSENSOR,
  NULL,
  _ImageBrowserDemoExec,
  NULL,
  NULL
};

/**
  * @brief  Run the 8 uart application
  * @param  None.
  * @note   run and display information about the uart transaction.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _ImageBrowserDemoExec(void)
{
  /* Prepare the main MMI */
  kMenu_Execute(ImageBrowserMenu);

  /* Execute the app 8uart */
  /* App initialization    */
  return KMODULE_OK;
}


/**
  * @brief  Run the image browser
  * @param  None.
  * @note   run and display image according the user action.
  * @retval None.
  */
void ImageBrowserDemo(void)
{
  uint8_t  filename[15];
  uint8_t  PathImage[50];

  /* Lecture du folder source */
  kStorage_GetDirectoryFiles((uint8_t *)"USER", KSTORAGE_FINDFIRST, filename, (uint8_t *)"bmp");

  /* Affichage de la premiere image */
  sprintf((char*)PathImage,"USER/%s",filename);
  kStorage_OpenFileDrawPixel(0,0,PathImage);

  /* reset the user action event handler */
  user_action = 0;

  /* Wait User event                    */
  /*       JOY_LEFT  : previous picture     */
  /*       JOY_RIGHT : next picture     */
  /*       JOY_SEL   : exit application */
  do
  {
    while(user_action == 0);

    if((user_event == JOY_RIGHT) || (user_event == JOY_LEFT))
    {

      if(kStorage_GetDirectoryFiles((uint8_t *)"USER", ((user_event == JOY_LEFT) ? KSTORAGE_FINDPREV : KSTORAGE_FINDNEXT) , filename, (uint8_t *)"bmp") == KSTORAGE_NOERROR)
      {
        /* display the picture */
        sprintf((char *)PathImage,"USER/%s",filename);
        kStorage_OpenFileDrawPixel(0,0,PathImage);
      }
    }

    /* exit on UP or Down */
    if((user_event == JOY_UP) || (user_event == JOY_DOWN))
    {
      user_event = JOY_SEL;
    }

    /* reset the user action event handler */
    user_action = 0;
  }
  while(user_event != JOY_SEL);

  /* Close the find */
  kStorage_GetDirectoryFiles((uint8_t *)"USER", KSTORAGE_FINDCLOSE, filename, (uint8_t *)"bmp");
}

/**
  * @brief  Get User action
  * @param  sel   User selection (JOY_SEL,...)
  * @note   This example is the only way to get user information.
  * @retval None
  */
void ImageBrowserMenuUserAction(uint8_t sel)
{
  if (user_action == 0 )
  {
    user_action = 1;
    user_event = sel;
  }
}


