/**
  ******************************************************************************
  * @file    BSP/Src/lcd.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use LCD drawing features in the
  *          STM32G474E EVAL driver
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>

/** @addtogroup STM32G4xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LCD_FEATURES_NUM                6
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t LCD_Feature = 0;
/* Private function prototypes -----------------------------------------------*/
static void LCD_SetHint(void);
static void LCD_Show_Feature(uint8_t feature);
static void ReadPixelTest(void);
static void BrightnessTest(void);
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  LCD demo
  * @param  None
  * @retval 0 if ok, else value < 0.
  */
int32_t Lcd_demo(void)
{
  int32_t   result = 0;
  uint32_t  Brightness;
  uint32_t  Xsize, Ysize;
  uint32_t  k;
  uint32_t  Orientation[4] = {LCD_ORIENTATION_PORTRAIT, LCD_ORIENTATION_LANDSCAPE, LCD_ORIENTATION_PORTRAIT_ROT180, LCD_ORIENTATION_LANDSCAPE_ROT180};
  char*     Text[4] = {"portrait", "landscape", "portrait with 180 degree rotation", "landscape with 180 degree rotation"};

  Point     triangle1[3] = {{145, 5}, {145, 45}, {185, 45}};
  Point     triangle2[3] = {{190, 45}, {230, 45}, {230, 5}};
  uint32_t  Pixel;

  for (k = 0; k < 4; k++)
  {
    /********************/
    /* TEST WITHOUT GUI */
    /********************/
    printf("\r\nTest with orientation %s\n", Text[k]);

    /* Initialize LCD */
    if (BSP_LCD_Init(0, Orientation[k]) != BSP_ERROR_NONE) result--;

    /* Set the brightness */
    if (BSP_LCD_SetBrightness(0, 50) != BSP_ERROR_COMPONENT_FAILURE) result--;

    /* Get the brightness */
    if (BSP_LCD_GetBrightness(0, &Brightness) != BSP_ERROR_COMPONENT_FAILURE) result--;

    /* Get the display Xsize */
    if (BSP_LCD_GetXSize(0, &Xsize) != BSP_ERROR_NONE) result--;
    if ((Orientation[k] == LCD_ORIENTATION_PORTRAIT) || (Orientation[k] == LCD_ORIENTATION_PORTRAIT_ROT180))
    {
      if (Xsize != 240U) result--;
    }
    else
    {
      if (Xsize != 320U) result--;
    }

    /* Get the display Ysize */
    if (BSP_LCD_GetYSize(0, &Ysize) != BSP_ERROR_NONE) result--;
    if ((Orientation[k] == LCD_ORIENTATION_PORTRAIT) || (Orientation[k] == LCD_ORIENTATION_PORTRAIT_ROT180))
    {
      if (Ysize != 320U) result--;
    }
    else
    {
      if (Ysize != 240U) result--;
    }

    /* Clean display */
    uint32_t i, j;

    for (i = 0; i < Xsize; i++)
    {
      for (j = 0; j < Ysize; j++)
      {
        if (BSP_LCD_WritePixel(0, i, j, LCD_COLOR_RGB565_WHITE) != BSP_ERROR_NONE) result--;
      }
    }

    /* Set the display on */
    if (BSP_LCD_DisplayOn(0) != BSP_ERROR_NONE) result--;

    /* Display a brown rectangle pixel by pixel */
    for (i = 0; i < 60; i++)
    {
      for (j = 0; j < 120; j++)
      {
        if (BSP_LCD_WritePixel(0, i, j, LCD_COLOR_RGB565_DARKGREEN) != BSP_ERROR_NONE) result--;
      }
    }

    /* Get pixel */
    if (BSP_LCD_ReadPixel(0, 20, 21, &Pixel) != BSP_ERROR_NONE) result--;
    if (Pixel != LCD_COLOR_RGB565_DARKGREEN) result--;

    /* Display a red rectangle */
    if (BSP_LCD_FillRect(0, 80, 20, 80, 60, LCD_COLOR_RGB565_RED) != BSP_ERROR_NONE) result--;

    /* Display a green horizontal line */
    if (BSP_LCD_DrawHLine(0, 80, 120, 90, LCD_COLOR_RGB565_GREEN) != BSP_ERROR_NONE) result--;

    /* Display a green vertical line */
    if (BSP_LCD_DrawVLine(0, 80, 120, 90, LCD_COLOR_RGB565_GREEN) != BSP_ERROR_NONE) result--;

    /* Display a green horizontal line */
    if (BSP_LCD_DrawHLine(0, 80, 210, 90, LCD_COLOR_RGB565_GREEN) != BSP_ERROR_NONE) result--;

    /* Display a green vertical line */
    if (BSP_LCD_DrawVLine(0, 170, 120, 90, LCD_COLOR_RGB565_GREEN) != BSP_ERROR_NONE) result--;

    /* Display a bitmap */
    if (BSP_LCD_DrawBitmap(0, 85, 135, (uint8_t *)stlogo) != BSP_ERROR_NONE) result--;

    HAL_Delay(1000);

    /* Set the display off */
    if (BSP_LCD_DisplayOff(0) != BSP_ERROR_NONE) result--;

    HAL_Delay(1000);

    /* Set the display on */
    if (BSP_LCD_DisplayOn(0) != BSP_ERROR_NONE) result--;

    /* Check result */
    if (CheckResult() != 0U) result--;

    /* De-initialize LCD */
    if (BSP_LCD_DeInit(0) != BSP_ERROR_NONE) result--;

    HAL_Delay(1000);

    /*****************/
    /* TEST WITH GUI */
    /*****************/
    printf("\r\nTest with orientation %s and GUI\n", Text[k]);

    /* Initialize LCD */
    if (BSP_LCD_Init(0, Orientation[k]) != BSP_ERROR_NONE) result--;

    /* Set GUI functions */
     GUI_SetFuncDriver(&LCD_Driver); /* SetFunc before setting device */
     GUI_SetDevice(0);/* SetDevice after funcDriver is set */

    /* Clear screen */
    BSP_LCD_Clear(0, LCD_COLOR_RGB565_WHITE);

    /* Set the display on */
    if (BSP_LCD_DisplayOn(0) != BSP_ERROR_NONE) result--;

    /* Draw a black pixel */
    GUI_SetPixel(25, 25, GUI_COLOR_BLACK);

    /* Draw a yellow circle */
    GUI_DrawCircle(25, 25, 20, GUI_COLOR_YELLOW);

    /* Fill a magenta circle */
    GUI_FillCircle(70, 25, 20, GUI_COLOR_MAGENTA);

    /* Draw a blue vertical line */
    GUI_DrawVLine(95, 5, 40, GUI_COLOR_BLUE);

    /* Draw a red horizontal line */
    GUI_DrawHLine(100, 25, 40, GUI_COLOR_RED);

    /* Draw a green triangle */
    GUI_DrawPolygon(triangle1, 3, GUI_COLOR_GREEN);

    /* Fill a cyan triangle */
    GUI_FillPolygon(triangle2, 3, GUI_COLOR_CYAN);

    /* Draw a gray rectangle */
    GUI_DrawRect(5, 50, 40, 20, GUI_COLOR_GRAY);

    /* Fill a brown rectangle */
    GUI_FillRect(50, 50, 40, 20, GUI_COLOR_BROWN);

    /* Draw a orange ellipse */
    GUI_DrawEllipse(115, 60, 20, 10, GUI_COLOR_ORANGE);

    /* Fill a dark blue ellipse */
    GUI_FillEllipse(170, 60, 20, 10, GUI_COLOR_DARKBLUE);

    /* Display text */
    GUI_SetFont(&Font24);
    GUI_SetBackColor(GUI_COLOR_BLACK);
    GUI_SetTextColor(GUI_COLOR_GREEN);
    GUI_DisplayStringAtLine(3, "Test of LCD");
    GUI_SetFont(&Font12);
    GUI_SetBackColor(GUI_COLOR_BLUE);
    GUI_SetTextColor(GUI_COLOR_ORANGE);
    GUI_DisplayStringAt(0, 112, "Test of LCD", CENTER_MODE);

    /* Draw bitmap */
    GUI_DrawBitmap(5, 130, (uint8_t *)stlogo);

    /* Check result */
    if (CheckResult() != 0U) result--;

    /* De-initialize LCD */
    if (BSP_LCD_DeInit(0) != BSP_ERROR_NONE) result--;
  }

  return result;
}

/**
  * @brief  LCD demo
  * @param  None
  * @retval None
  */
void LCD_demo (void)
{
  LCD_SetHint();
  LCD_Feature = 0;
  LCD_Show_Feature (LCD_Feature);

  while (1)
  {

    if(CheckForUserInput() > 0)
    {
      if(++LCD_Feature < LCD_FEATURES_NUM)
      {
       LCD_Show_Feature (LCD_Feature);
      }
      else
      {
        return;
      }
    }
    HAL_Delay(100);
  }
}

/**
  * @brief  Display LCD demo hint
  * @param  None
  * @retval None
  */
static void LCD_SetHint(void)
{
  uint32_t x_size, y_size;
  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  /* Clear the LCD */
  GUI_Clear(GUI_COLOR_WHITE);
  /* Set LCD Demo description */

  BSP_LCD_FillRect(0, 0, 0, x_size, 80, GUI_COLOR_BLUE );

  GUI_SetTextColor(GUI_COLOR_WHITE);
  GUI_SetBackColor(GUI_COLOR_BLUE);
  GUI_SetFont(&Font24);
  GUI_DisplayStringAt(0, 0, (uint8_t *)"LCD", CENTER_MODE);
  GUI_SetFont(&Font12);
  GUI_DisplayStringAt(0, 30, (uint8_t *)"This example shows the different", CENTER_MODE);
  GUI_DisplayStringAt(0, 45, (uint8_t *)"LCD Features, use Tamper push-button to display", CENTER_MODE);
  GUI_DisplayStringAt(0, 60, (uint8_t *)"next page", CENTER_MODE);

   /* Set the LCD Text Color */
  GUI_DrawRect(10, 90, x_size - 20, y_size- 100, GUI_COLOR_BLUE);
  GUI_DrawRect(11, 91, x_size - 22, y_size- 102, GUI_COLOR_BLUE);
 }

/**
  * @brief  Show LCD Features
  * @param  feature : feature index
  * @retval None
  */
static void LCD_Show_Feature(uint8_t feature)
{
  Point Points[]= {{220, 220}, {280, 180}, {320, 180}, {370, 220}, {370, 260}, {320, 310}, {280, 310}, {220, 260}};
  Point Points2[3];
  uint32_t x_size, y_size;

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  Points2[0].X = x_size - 80;
  Points2[0].Y = 150;
  Points2[1].X = x_size - 20;
  Points2[1].Y = 150;
  Points2[2].X = x_size - 20;
  Points2[2].Y = 200;

  BSP_LCD_FillRect(0, 12, 92, x_size - 24, y_size- 104, LCD_COLOR_RGB565_WHITE );
  GUI_SetTextColor(GUI_COLOR_BLACK);

  switch (feature)
  {
  case 0:
    /* Text Feature */
    GUI_DisplayStringAt(14, 100, (uint8_t *)"Left aligned Text", LEFT_MODE);
    GUI_DisplayStringAt(0, 115, (uint8_t *)"Center aligned Text", CENTER_MODE);
    GUI_DisplayStringAt(14, 130, (uint8_t*)"Right aligned Text", RIGHT_MODE);
    GUI_SetFont(&Font24);
    GUI_DisplayStringAt(14, 180, (uint8_t *)"Font24", LEFT_MODE);
    GUI_SetFont(&Font20);
    GUI_DisplayStringAt(x_size/2 -20, 180, (uint8_t *)"Font20", LEFT_MODE);
    GUI_SetFont(&Font16);
    GUI_DisplayStringAt(x_size - 80, 184, (uint8_t *)"Font16", LEFT_MODE);
    HAL_Delay(2000);
    break;

  case 1:

    /* Draw misc. Shapes */
    GUI_DrawRect ( 20, 100, 60 , 40, GUI_COLOR_BLACK);
    BSP_LCD_FillRect(0, 100, 100, 60 , 40, LCD_COLOR_RGB565_BLACK);

//    GUI_DrawCircle( x_size - 120, 120, 20);
//    GUI_FillCircle( x_size - 40, 120, 20, GUI_COLOR_GRAY);

    GUI_FillPolygon(Points, 8, GUI_COLOR_GREEN);
    GUI_DrawEllipse(130, 170, 30, 20, GUI_COLOR_RED);
    GUI_FillEllipse(200, 170, 30, 20, GUI_COLOR_RED);

    BSP_LCD_DrawHLine(0, 20, y_size - 30, x_size / 5, LCD_COLOR_RGB565_BLACK);
    GUI_DrawLine ( 100, y_size - 20, 230, y_size- 50, GUI_COLOR_BLACK);
    GUI_DrawLine ( 100, y_size- 50, 230, y_size- 20, GUI_COLOR_BLACK);

    GUI_DrawPolygon(Points2, 3, GUI_COLOR_YELLOW);
    HAL_Delay(2000);
    break;

  case 2:
    /* Draw Bitmap */
    BSP_LCD_DrawBitmap(0, 20, 100, (uint8_t *)stlogo);
    HAL_Delay(500);

    BSP_LCD_DrawBitmap(0, x_size/2 - 40, 100, (uint8_t *)stlogo);
    HAL_Delay(500);

    BSP_LCD_DrawBitmap(0, x_size-100, 100, (uint8_t *)stlogo);
    HAL_Delay(500);

    BSP_LCD_DrawBitmap(0, 20, y_size- 80, (uint8_t *)stlogo);
    HAL_Delay(500);

    BSP_LCD_DrawBitmap(0, x_size/2 - 40, y_size- 80, (uint8_t *)stlogo);
    HAL_Delay(500);

    BSP_LCD_DrawBitmap(0, x_size-100, y_size- 80, (uint8_t *)stlogo);
    HAL_Delay(500);
    break;

    case 3:
    /* Set LCD Brightness */
     BrightnessTest();
    break;

    case 4:
    /* LCD Read Pixel */
     ReadPixelTest();
    break;
  case 5:

    //LCD_SetFuncDriver(&LCD_Driver);
    GUI_SetFont(&Font20);
    /* Clear the LCD */
    GUI_SetBackColor(GUI_COLOR_BLUE);
    GUI_Clear(GUI_COLOR_BLUE);
    BSP_LCD_GetXSize(0, &x_size);

    BSP_LCD_FillRect(0,0, 0, x_size, 80, GUI_COLOR_WHITE);

    /* Set the LCD Text Color */
    GUI_SetTextColor(GUI_COLOR_WHITE);
    GUI_DisplayStringAt(0, y_size/2, (uint8_t *)"Orientation: LANDSCAPE", CENTER_MODE);
    GUI_DisplayStringAt(0, y_size/2 + 45, (uint8_t *)"Pixel Format: RGB565", CENTER_MODE);
    HAL_Delay(2000);
    //LCD_SetFuncDriver(&LCD_Driver);
    GUI_SetFont(&Font20);
    /* Clear the LCD */
    GUI_SetBackColor(GUI_COLOR_BLUE);
    GUI_Clear(GUI_COLOR_BLUE);
    BSP_LCD_GetXSize(0, &x_size);

#if 0
    BSP_LCD_FillRect(0, 0, 0, x_size, 80, LCD_COLOR_RGB565_WHITE);
    /* Set the LCD Text Color */
    GUI_SetTextColor(GUI_COLOR_WHITE);
    GUI_DisplayStringAt(0, y_size/2, (uint8_t *)"Orientation: PORTRAIT", CENTER_MODE);
    GUI_DisplayStringAt(0, y_size/2 + 45, (uint8_t *)"Pixel Format: RGB565", CENTER_MODE);
    HAL_Delay(2000);

    BSP_LCD_DeInit(0);
    BSP_LCD_Init(0, LCD_ORIENTATION_PORTRAIT);
    //LCD_SetFuncDriver(&LCD_Driver);
    GUI_SetFont(&LCD_DEFAULT_FONT);
    /* Clear the LCD */
    GUI_SetBackColor(GUI_COLOR_BLUE);
    GUI_Clear(GUI_COLOR_BLUE);
    BSP_LCD_GetXSize(0, &x_size);

    BSP_LCD_FillRect(0, 0, 0, x_size, 80, LCD_COLOR_RGD565_WHITE);
    /* Set the LCD Text Color */
    GUI_SetTextColor(GUI_COLOR_WHITE);
    GUI_DisplayStringAt(0, y_size/2, (uint8_t *)"Orientation: PORTRAIT", CENTER_MODE);
    GUI_DisplayStringAt(0, y_size/2 + 45, (uint8_t *)"Pixel Format: RGB888", CENTER_MODE);
    HAL_Delay(2000);
#endif
    BSP_LCD_DeInit(0);
    BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);
    //LCD_SetFuncDriver(&LCD_Driver);
    GUI_SetFont(&Font20);
    /* Clear the LCD */
    GUI_SetBackColor(GUI_COLOR_BLUE);
    GUI_Clear(GUI_COLOR_BLUE);
    BSP_LCD_GetXSize(0, &x_size);

    BSP_LCD_FillRect(0, 0, 0, x_size, 80, LCD_COLOR_RGB565_WHITE);
    /* Set the LCD Text Color */
    GUI_SetTextColor(GUI_COLOR_WHITE);
    GUI_DisplayStringAt(0, y_size/2, (uint8_t *)"Orientation: LANDSCAPE", CENTER_MODE);
    GUI_DisplayStringAt(0, y_size/2 + 45, (uint8_t *)"Pixel Format: RGB888", CENTER_MODE);
    HAL_Delay(2000);

    break;
  }
}

/**
  * @brief  LCD Read Pixel test
  * @param  None
  * @retval None
  */
static void ReadPixelTest(void)
{
  uint16_t i, j, k;
  uint32_t x_size, y_size, read_pixel;
  uint32_t colors[23] = {GUI_COLOR_BLUE, GUI_COLOR_GREEN, GUI_COLOR_RED, GUI_COLOR_CYAN, GUI_COLOR_MAGENTA, GUI_COLOR_YELLOW,
  GUI_COLOR_LIGHTBLUE, GUI_COLOR_LIGHTGREEN, GUI_COLOR_LIGHTRED, GUI_COLOR_LIGHTMAGENTA,
  GUI_COLOR_LIGHTYELLOW, GUI_COLOR_DARKBLUE, GUI_COLOR_DARKGREEN, GUI_COLOR_DARKRED, GUI_COLOR_DARKCYAN,
  GUI_COLOR_DARKMAGENTA, GUI_COLOR_DARKYELLOW, GUI_COLOR_LIGHTGRAY, GUI_COLOR_GRAY, GUI_COLOR_DARKGRAY,
  GUI_COLOR_BLACK, GUI_COLOR_BROWN, GUI_COLOR_ORANGE };


  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

#if (LCD_HX8347D == 1)
  for(k = 0; k < 23; k++)
  {
    GUI_Clear(colors[k]);

    BSP_LCD_ReadPixel(0, i++, j++, &read_pixel);
    if(read_pixel != colors[k])
    {
          BSP_LED_On(LED_RED);
    }
  }
#else
  for(k = 0; k < 23; k++)
  {
    GUI_Clear(colors[k]);
    for(j = 0; j < y_size; j++)
    {
      for(i = 0; i < x_size; i++)
      {
        BSP_LCD_ReadPixel(0, i,j, &read_pixel);
        if(read_pixel != colors[k])
        {
          BSP_LED_On(LED_RED);
        }
      }
    }
  }
#endif /* (LCD_HX8347D == 1)*/
}

/**
  * @brief  Show LCD Brightness Feature
  * @param  None
  * @retval None
  */
static void BrightnessTest(void)
{
  uint8_t counter = 100;
  uint8_t text[30];
  uint32_t y_size;

  BSP_LCD_GetXSize(0, &y_size);

#if (LCD_HX8347D == 1)
    /* brightness control is not supported by controller */
    BSP_LCD_SetBrightness(0, counter);
    sprintf((char*)text," Brightness = %d ",counter);
    GUI_DisplayStringAt(0, y_size/2 + 45, (uint8_t *)text, CENTER_MODE);
    HAL_Delay(50);
#else
  while(counter > 0)
  {
    BSP_LCD_SetBrightness(0, counter);
    sprintf((char*)text," Brightness = %d ",counter);
    GUI_DisplayStringAt(0, y_size/2 + 45, (uint8_t *)text, CENTER_MODE);
    counter = counter - 10/*counter--*/;
    HAL_Delay(50);
  }
  while(counter <= 100)
  {
    BSP_LCD_SetBrightness(0, counter);
    sprintf((char*)text," Brightness = %d ",counter);
    GUI_DisplayStringAt(0, y_size/2 + 45, (uint8_t *)text, CENTER_MODE);
    counter = counter + 10/*counter++*/;
    HAL_Delay(50);
  }
#endif /* LCD_HX8347D*/
}

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
