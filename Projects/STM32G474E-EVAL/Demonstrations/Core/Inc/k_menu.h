/**
  ******************************************************************************
  * @file    k_menu.h
  * @author  MCD Application Team
  * @brief   Header for k_menu.c file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __K_MENU_H
#define __K_MENU_H

#ifdef __cplusplus
 extern "C" {
#endif

   
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef void (*tExecFunc)(void);
typedef void (*tExecAction)(uint8_t);

typedef struct sMenuItem tMenuItem;
typedef struct sMenu tMenu;
struct sMenuItem
{
  char *pszTitle;
  uint16_t x;
  uint16_t y;
  uint8_t SelType;
  uint8_t ModuleId;
  tExecFunc pfExecFunc;
  tExecAction pfActionFunc;
  const tMenu *psSubMenu;
  char *pIconPath;
  char *pIconSelectedPath;
};

struct sMenu
{
  char* pszTitle;
  const tMenuItem *psItems;
  uint32_t nItems;
  uint32_t nType;
  uint8_t line;
  uint8_t column;
};

enum{
  IDLE_DEMO,
  /* Type de menu */
  TYPE_TEXT,
  TYPE_ICON,
  TYPE_EXEC,
  /* MENU SEL */
  SEL_SUBMENU,
  SEL_MODULE,
  SEL_EXEC,
  SEL_EXIT,
  
  KMENU_RESET,
  KMENU_UPDATE,
};

/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void kMenu_Init(void);
void kMenu_Execute(tMenu psCurrentMenu);
void kMenu_EventHandler(uint16_t GPIO_Pin);
void kMenu_Header(char *string);

#ifdef __cplusplus
}
#endif

#endif /*__K_MENU_H */


