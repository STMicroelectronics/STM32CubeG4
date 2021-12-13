/**
  ******************************************************************************
  * @file    app_files_broswer.c
  * @author  MCD Application Team
  * @brief   File browser application implementation
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#define __APP_FILESBROWSER_C

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "stm32g4xx.h"
#include "stm32g474e_eval.h"
#include "stm32g474e_eval_lcd.h"
#include "utils.h"
#include "k_config.h"
#include "k_module.h"
#include "k_menu.h"
#include "k_storage.h"

/* Private typedef ----------------------------------------------------------*/
/* Private constants --------------------------------------------------------*/
enum {
  FILESBROWSER_INIT,
  FILEBROWSER_GETFILE,
  FILEBROWSER_DISPLAYFILES,
  FILEBROWSER_WAITEVENT,
  FILEBROWSER_EXIT,
	FILEBROWSER_END
};

/* Private function prototypes ----------------------------------------------*/
KMODULE_RETURN _FilesBrowserDemoExec(void);

void FilesBrowserDemo(void);
void FilesBrowserMenuUserAction(uint8_t sel);
void FilesBrowserUserInformation(int16_t tempvalue);
void FilesBrowserUserHeader(void);
static void FilesBrowserDisplayFiles(uint8_t sel);

/* Private Variable ----------------------------------------------------------*/
const tMenuItem FilesBrowserMenuMenuItems[1] =
{
  {"press right: exit", 14, 30, TYPE_EXEC, MODULE_NONE, FilesBrowserDemo, FilesBrowserMenuUserAction, NULL, NULL, NULL  }
};

const tMenu FilesBrowserMenu = {
  "Files Browser", FilesBrowserMenuMenuItems, countof(FilesBrowserMenuMenuItems), TYPE_EXEC, 1, 1 };

/* used to exit application */
static __IO uint8_t user_event=0, user_action=0;


/* Private typedef -----------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
const K_ModuleItem_Typedef ModuleFilesBrowser =
{
  MODULE_FILESBRO,
  NULL,
  _FilesBrowserDemoExec,
  NULL,
  NULL
};

/**
  * @brief  Run the 8 uart application
  * @param  None.
  * @note   run and display information about the uart transaction.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _FilesBrowserDemoExec(void)
{
  /* Prepare the main MMI */
  kMenu_Execute(FilesBrowserMenu);

  /* Execute the app 8uart */
  /* App initialization    */
  return KMODULE_OK;
}

#define MAX_LINE 9
#define FOLDER_LEVEL_MAX 3
FILINFO fileinfo;

uint8_t stringline[MAX_LINE][256];
/**
  * @brief  Run the Files browser
  * @param  None.
  * @note   run and display Files according the user action.
  * @retval None.
  */
void FilesBrowserDemo(void)
{
  uint8_t filename[256];
  uint8_t foldername[256];
  uint8_t fullname[(256*2)+1];
  uint8_t folder_level = 0;
  uint8_t index;
  uint8_t sel = 1;
  uint8_t application_state = FILESBROWSER_INIT;
  uint16_t folderposition= 0, foldertargetposition;
  uint16_t len;

  do
  {
    switch(application_state)
    {
    case FILESBROWSER_INIT :
      /* Help information */
      UTIL_LCD_SetFont(&Font12);
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_GREEN_DARK);
      UTIL_LCD_DisplayStringAt(0, 239 - Font24.Height - Font12.Height, (uint8_t *)"folder: sel enter/left go back", CENTER_MODE);
      UTIL_LCD_SetFont(&Font24);
      /* Lecture du folder source */
      strcpy((char *)stringline[0], "USER");
      strcpy((char *)foldername, "USER");
    case FILEBROWSER_GETFILE :
      memset(stringline[1], 0x00, sizeof(uint8_t)*(MAX_LINE-1)*256);
      folderposition = 0;
      sel = 0;
      if(kStorage_GetDirectoryFiles(foldername, KSTORAGE_FINDFIRST, stringline[1], NULL) == KSTORAGE_NOERROR)
      {
        folderposition = 1;
        sel = 1;
        for(index = 2; index < MAX_LINE; index++)
        {
          if(kStorage_GetDirectoryFiles(foldername, KSTORAGE_FINDNEXT, stringline[index], NULL) == KSTORAGE_NOERROR)
          {
            folderposition++;
          }
        }
      }
      /* Display the files list */
    case FILEBROWSER_DISPLAYFILES :
      FilesBrowserDisplayFiles(sel);
      application_state = FILEBROWSER_WAITEVENT;
      break;

      /* Wait for an user event */
    case FILEBROWSER_WAITEVENT :

      /* User action init */
      user_event = JOY_NONE;
      user_action = 0;
      application_state = FILEBROWSER_WAITEVENT;
      while(user_event == JOY_NONE);

      /* treatment of the user request */
      switch(user_event)
      {
      case JOY_DOWN :
        /* go in down inside the folder */
        if(sel == (MAX_LINE - 1))
        {
          if(kStorage_GetDirectoryFiles(foldername, KSTORAGE_FINDNEXT, filename, NULL) == KSTORAGE_NOERROR)
          {
            folderposition++;
            for(index = 1 ; index < (MAX_LINE - 1); index++)
            {
              len = 0;
              while (stringline[index+1][len] != '\0')
              {
                stringline[index][len] = stringline[index+1][len];
                len++;
              }
              stringline[index][len] = '\0';
            }
            sprintf((char *)stringline[index], "%s",filename);
          }
        }
        else
        {
          sel++;
          if( sel>folderposition) sel--;
        }

        application_state = FILEBROWSER_DISPLAYFILES;
        break;
      case JOY_UP :
        /* go up inside the folder info */
        if(sel == 1)
        {
          if(folderposition > (MAX_LINE - 1))
          {
            foldertargetposition = folderposition;
            kStorage_GetDirectoryFiles(foldername, KSTORAGE_FINDCLOSE, filename, NULL);
            folderposition = 0;
            do
            {
              kStorage_GetDirectoryFiles(foldername, ((folderposition == 0) ? KSTORAGE_FINDFIRST: KSTORAGE_FINDNEXT), stringline[1], NULL);
              folderposition++;
            } while(folderposition != (foldertargetposition - (MAX_LINE - 1)));

            for(index = 2; index < MAX_LINE; index++)
            {
              if(kStorage_GetDirectoryFiles(foldername, KSTORAGE_FINDNEXT, stringline[index], NULL) == KSTORAGE_NOERROR)
              {
                folderposition++;
              }
            }
            application_state = FILEBROWSER_DISPLAYFILES;
          }
        }
        else
        {
          if( sel > 1 )
          {
          sel--;
          application_state = FILEBROWSER_DISPLAYFILES;
          }
        }

        break;
      case JOY_SEL :
        {
          if (sel != 0 )
          {
            strcpy((char *)foldername, (char *)stringline[0]);
            sprintf((char *)fullname,"%s\\%s", foldername, stringline[sel]);
            kStorage_GetFileInfo(fullname, &fileinfo);

            /* enter inside the folder */
            if(((fileinfo.fattrib & AM_DIR)== AM_DIR) && (folder_level < FOLDER_LEVEL_MAX))
            {
              kStorage_GetDirectoryFiles(foldername, KSTORAGE_FINDCLOSE, filename, NULL);
              sprintf((char *)fullname,"%s\\%s", stringline[0], fileinfo.fname);
              strcpy((char *)stringline[0], (char *)fullname);
              strcpy((char *)foldername, (char *)stringline[0]);
              folder_level++;
              application_state = FILEBROWSER_GETFILE;
            }
          }
        }
        break;
      case JOY_LEFT :
        if(folder_level != 0)
        {
          kStorage_GetDirectoryFiles(foldername, KSTORAGE_FINDCLOSE, filename, NULL);
          folder_level--;
          index = strlen((char *)stringline[0]);
            do {
              if (stringline[0][index] == '\\')
              {
                stringline[0][index] = 0x00;
                index = 0;
              }
              else
              {
                index--;
              }
            } while(index != 0);
            strcpy((char *)foldername, (char *)stringline[0]);
            application_state = FILEBROWSER_GETFILE;
        }
        break;
      case JOY_RIGHT :
        application_state = FILEBROWSER_EXIT;
        break;
      }
      break;

    case FILEBROWSER_EXIT :
      /* Close the find */
      kStorage_GetDirectoryFiles(foldername, KSTORAGE_FINDCLOSE, filename, NULL);
      application_state = FILEBROWSER_END;
      break;
    }
  } while(application_state != FILEBROWSER_END);

}

/**
  * @brief  Get User action
  * @param  sel   User selection (JOY_SEL,...)
  * @note   This example is the only way to get user information.
  * @retval None
  */
void FilesBrowserMenuUserAction(uint8_t sel)
{
  if(user_action == 0)
  {
    user_event = sel;
    user_action = 1;
  }
}


/**
  * @brief  Refresh the file list
  * @param  sel   User selection (JOY_SEL,...)
  * @note   display user information
  * @retval None
  */
static void FilesBrowserDisplayFiles(uint8_t sel)
{
  uint32_t pXSize;
  uint8_t index;
  uint8_t len;
  uint8_t buff[256*2+1];
  uint8_t strtmp[13];
  FILINFO fileinfo;

  /* Display information */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
  BSP_LCD_GetXSize(LCD_INSTANCE, &pXSize);
  UTIL_LCD_FillRect(0, 0, pXSize, Font24.Height, UTIL_LCD_COLOR_ST_PINK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAtLine(0, stringline[0]);

  /* Draw the folder area */
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_GREEN_DARK);
  UTIL_LCD_DrawRect(20, 45, 140, 129, UTIL_LCD_COLOR_ST_GREEN_DARK);
  UTIL_LCD_SetFont(&Font16);

  for (index = 1; index < MAX_LINE; index++)
  {
    if(index == sel)
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_PINK);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    }
    else
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
    }
    if(stringline[index][0] == '\0')
    {
      UTIL_LCD_DisplayStringAt(21, 30 + Font16.Height*index, (uint8_t *)"            ", LEFT_MODE);
    }
    else
    {
      /* add space to guarantee right display */
      if(strlen((char*)stringline[index]) < 12)
      {
        len = 0;
        while(stringline[index][len] != '\0')
        {
          len++;
        }
        for(;len<12;len++)
        {
          stringline[index][len] = ' ';
        }
        stringline[index][12]= '\0';
      }
      if(strlen((char*)stringline[index]) >12)
      {
        strncpy((char*)strtmp,(char const*)stringline[index],11);
        strtmp[11] = '*';
        strtmp[12] = '\0';
        UTIL_LCD_DisplayStringAt(21, 30 + Font16.Height*index, strtmp, LEFT_MODE);
      }
      else
      {
        /* write the file name */
        UTIL_LCD_DisplayStringAt(21, 30 + Font16.Height*index, stringline[index], LEFT_MODE);
      }
    }
  }

  if(sel != 0)
  {
    /* Display information about the selected file */
    sprintf((char *)buff,"%s\\%s",stringline[0],stringline[sel]);
    kStorage_GetFileInfo(buff, &fileinfo);
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);

    if((fileinfo.fattrib & AM_DIR) == AM_DIR)
    {
      index = 1;
      UTIL_LCD_DisplayStringAt(170, 46, (uint8_t *)"folder info ", LEFT_MODE);
    }
    else
    {
      index = 0;
      UTIL_LCD_DisplayStringAt(170, 46, (uint8_t *)"file info   ", LEFT_MODE);
    }

    UTIL_LCD_DisplayStringAt(170, 46+Font16.Height, (uint8_t *)"name:", LEFT_MODE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
    UTIL_LCD_DisplayStringAt(186, 46+Font16.Height*2, stringline[sel],LEFT_MODE);
    if((fileinfo.fattrib & AM_DIR) == AM_DIR)
    {
      UTIL_LCD_DisplayStringAt(170, 46+Font16.Height*3, (uint8_t *)"         ", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(186, 46+Font16.Height*4, (uint8_t *)"           ", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(170, 46+Font16.Height*5, (uint8_t *)"           ", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(186, 46+Font16.Height*6, (uint8_t *)"      ", LEFT_MODE);    }
    else
    {
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
      UTIL_LCD_DisplayStringAt(170, 46+Font16.Height*3, (uint8_t *)"fsize   :", LEFT_MODE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);

      /* Convert and Display file size in Bytes, Kilo or Mega Bytes */
      if( fileinfo.fsize < 1024)
      {
        sprintf((char *)buff,"%d Bytes     ", (unsigned int)fileinfo.fsize);
      }
      else if (fileinfo.fsize < 1048576)
      {
        sprintf((char *)buff,"%d KB       ", (unsigned int)fileinfo.fsize/1024);
      }
      else
      {
        sprintf((char *)buff,"%d MB       ", (unsigned int)fileinfo.fsize/1048576);
      }
      UTIL_LCD_DisplayStringAt(186, 46+Font16.Height*4, buff, LEFT_MODE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
      UTIL_LCD_DisplayStringAt(170, 46+Font16.Height*5, (uint8_t *)"fattrib :", LEFT_MODE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE);
      sprintf((char *)buff,"0x%.2x", fileinfo.fattrib);
      UTIL_LCD_DisplayStringAt(186, 46+Font16.Height*6, buff, LEFT_MODE);
    }
  }
  else
  {
    /* Erase information area */
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_PINK);
    UTIL_LCD_DisplayStringAt(170, 46, (uint8_t *)"folder empty", LEFT_MODE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_DisplayStringAt(170, 46+Font16.Height, (uint8_t *)"     ", LEFT_MODE);
    UTIL_LCD_DisplayStringAt(186, 46+Font16.Height*2,(uint8_t *)"            ",LEFT_MODE);
    UTIL_LCD_DisplayStringAt(170, 46+Font16.Height*3, (uint8_t *)"         ", LEFT_MODE);
    UTIL_LCD_DisplayStringAt(186, 46+Font16.Height*4, (uint8_t *)"           ", LEFT_MODE);
    UTIL_LCD_DisplayStringAt(170, 46+Font16.Height*5, (uint8_t *)"           ", LEFT_MODE);
    UTIL_LCD_DisplayStringAt(186, 46+Font16.Height*6, (uint8_t *)"      ", LEFT_MODE);
  }
  UTIL_LCD_SetFont(&Font24);
}


