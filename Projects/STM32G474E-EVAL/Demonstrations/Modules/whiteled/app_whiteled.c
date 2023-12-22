/**
  ******************************************************************************
  * @file    app_whiteled.c
  * @author  MCD Application Team
  * @brief   WhiteLed application implementation.
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
#ifndef __APP_WHITELED_C
#define __APP_WHITELED_C

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx.h"

#include <stdio.h>
#include <arm_math.h>

#include "stm32g474e_eval.h"
#include "stm32g474e_eval_lcd.h"
#include "utils.h"
#include "app_whiteled.h"
#include "k_config.h"
#include "k_module.h"
#include "k_window.h"
#include "k_menu.h"
#include "k_storage.h"

/* Private typedef ----------------------------------------------------------*/


extern tMenuItem psCurrentMenuItem;


/* Private constants ----------------------------------------------------------*/
#define MIN_INTENSITY        625
#define MAX_INTENSITY        0
#define MAX_INTENSITY_DIMMER 0

/* Burst frequency 250kHz with System_clock =170MHz and with Prescaler multiply by 32  */
#define PERIOD ((170000 * 32 ) / 250)
#define DAC_STEP_NB 60  /* Fast DAC support up to 15Msamples per secondes -> 15MHz/250kHz = 60steps */
#define DAC_SAWTOOTH_RANGE 100 /* DAC sawtooth slope range is 100 mv */

/* sawtooth step parameter is 12.4 bit format (12 bits integer part, 4 bits fractional part), thus value is shifted by 4 bits */
#define DAC_STEP (((DAC_SAWTOOTH_RANGE * 4096)<<4) / (3300 * DAC_STEP_NB))

#define DEBUG

/* Private function prototypes -----------------------------------------------*/
KMODULE_RETURN _WhiteLedDemoExec(void);
KMODULE_RETURN _WhiteLedConfig(void);
KMODULE_RETURN _WhiteLedExit(void);

void WhiteLedMenuUserAction(uint8_t sel);
void WhiteLedUserHeader(void);

static void WhiteLed_AutoDimming(void);
static void WhiteLed_ManualDimming(void);
static void WhiteLed_Flashing(void);

static void COMP_Config(void);
static void DAC_Config(void);
#ifdef DEBUG
static void OPAMP_Config(void);
#endif
extern void Error_Handler(void);
static void HRTIM_Config(void);
static void HRTIM_DeConfig(void);
static void HRTIM_SetBurstCompare(float BurstCompare);
void SystemClock_Config(void);

/* Private Variable ----------------------------------------------------------*/

/* Main Menu */
const tMenuItem WhiteLedMenuItems[] =
{
  {"Auto Dimming"    , 14, 30, SEL_EXEC, MODULE_WHITELED, WhiteLed_AutoDimming   , WhiteLedMenuUserAction, NULL, NULL, NULL  },
  {"Manual Dimming"  , 14, 30, SEL_EXEC, MODULE_WHITELED, WhiteLed_ManualDimming , WhiteLedMenuUserAction, NULL, NULL, NULL  },
  {"Flashing"        , 14, 30, SEL_EXEC, MODULE_WHITELED, WhiteLed_Flashing      , WhiteLedMenuUserAction, NULL, NULL, NULL  },
  {"Return"          , 14, 30, SEL_EXIT, MODULE_WHITELED, NULL                   , NULL                  , NULL, NULL, NULL  },
};

const tMenu WhiteLedMenu = {"WhiteLed", WhiteLedMenuItems, countof(WhiteLedMenuItems), TYPE_TEXT, 1, 1 };

/* used to exit application */
static __IO uint8_t UserEvent=0;
static __IO uint8_t UserAction=0;

/* TIM handle declaration */
TIM_HandleTypeDef    TimHandle;


static COMP_HandleTypeDef CompHandle;
static DAC_HandleTypeDef  DacHandle;
static OPAMP_HandleTypeDef OpampHandle;
static __IO  uint8_t      NoWait                = TRUE;
static const uint16_t     DitherTab[8][8]       = {{0,0,0,0,0,0,0,0},
                                                   {0,0,0,0,0,0,0,1},
                                                   {0,0,0,1,0,0,0,1},
                                                   {0,0,1,0,0,1,0,1},
                                                   {0,1,0,1,0,1,0,1},
                                                   {0,1,0,1,1,0,1,1},
                                                   {0,1,1,1,0,1,1,1},
                                                   {0,1,1,1,1,1,1,1}};

static __IO uint32_t  MaxCurrentSense = (DAC_SAWTOOTH_RANGE * 4096) / 3300;
static __IO  float    fIntensity      = MIN_INTENSITY;

/* Private typedef -----------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
const K_ModuleItem_Typedef ModuleWhiteLed =
{
  MODULE_WHITELED,
  _WhiteLedConfig,
  _WhiteLedDemoExec,
  _WhiteLedExit,
  NULL
};

/**
  * @brief  setup the HW for WhiteLed application
  * @param  None.
  * @retval None.
  */
KMODULE_RETURN _WhiteLedConfig(void)
{
  /* DAC Configuration */
  DAC_Config();

  /* COMP Configuration */
  COMP_Config();

#ifdef DEBUG
  /* OPAMP Configuration for DAC4 debug */
  OPAMP_Config();
#endif

  /* Allows temporization */
  NoWait = FALSE;

  return KMODULE_OK;
}

/**
  * @brief  Run the applications
  * @param  None.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _WhiteLedDemoExec(void)
{
  /* App initialization    */
  kMenu_Execute(WhiteLedMenu);
  return KMODULE_OK;
}


/**
  * @brief  Run the applications
  * @param  None.
  * @retval KMODULE_RETURN status.
  */
KMODULE_RETURN _WhiteLedExit(void)
{
#ifdef DEBUG
  /* OPAMP deconfiguration */
  HAL_OPAMP_Stop(&OpampHandle);
  HAL_OPAMP_DeInit(&OpampHandle);
#endif

  /* DAC deconfiguration */
  HAL_DAC_Stop(&DacHandle, DAC_CHANNEL_2);
  HAL_DAC_DeInit(&DacHandle);
  __HAL_RCC_DAC4_CLK_DISABLE();

  /* COMP deconfiguration */
  HAL_COMP_Stop(&CompHandle);
  HAL_COMP_DeInit(&CompHandle);
  __HAL_RCC_SYSCFG_CLK_DISABLE();

  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11);
  __HAL_RCC_GPIOB_CLK_DISABLE();

  return KMODULE_OK;
}


/**
  * @brief  Get User action
  * @param  sel  User selection (JOY_SEL,...)
  * @note   This example is the only way to get user information.
  * @retval None
  */
void WhiteLedMenuUserAction(uint8_t sel)
{
  if (UserAction == 0)
  {
    UserAction = 1;
  }
  /* store user event */
  UserEvent = BSP_JOY_GetState(JOY1);
}


/**
  * @brief  set the Automatic Dimming.
  * @param  None
  * @retval None
  */
static void WhiteLed_AutoDimming(void)
{
  uint32_t maxsense;

  kWindow_Popup("Automatic Dimming", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
    "Press joystick\nto exit\n",                  \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* HRTIM Configuration */
  HRTIM_Config();

  /* Wait a press on any JoyStick pushbuttons */
  while (UserAction == 0x00u)
  {
    /* The LED intensity is automatically controlled */
    /* Levels of current thresholds are progressively set */
    /* Intensity variation and flicker removal are enhanced thanks to dither sequences
    that allows greater precision by steps */

    /* Reset all signal LEDs */
    BSP_LED_Off(LED1);
    BSP_LED_Off(LED2);
    BSP_LED_Off(LED3);
    BSP_LED_Off(LED4);

    NoWait = FALSE;

    /* Set signal LEDs to rising mode */
    BSP_LED_On(LED3);
    BSP_LED_Off(LED4);

    /* Start with current thresholds set to minimum */
    maxsense = DAC_SAWTOOTH_RANGE;

    /* Set fIntensity at minimum intensity */
    fIntensity = MIN_INTENSITY;

    /* Enable HRTIM Master Burst period interrupt */
    /* Each time the interrupt is serviced, the LED is driven */
    __HAL_HRTIM_ENABLE_IT(&HrtimHandle, HRTIM_IT_BMPER);

    /* Intensity changes from Min to Max */
    do
    {
      if(fIntensity > (605 + 0.1))
      {
        /* Increase intensity slowly at low brightness */
        fIntensity -= 0.1f;
        /* Inccrement progressively all thresholds up to the setting value (centered value is ~250mA)*/
        maxsense ++;
        MaxCurrentSense = (maxsense * 4096) / 3300;

        /* Configure Sawtooth waveform generation */
        HAL_DACEx_SawtoothWaveGenerate(&DacHandle, DAC_CHANNEL_2, DAC_SAWTOOTH_POLARITY_DECREMENT, MaxCurrentSense, DAC_STEP);
      }
      else
      {
        /* Increment progressively all thresholds up to the setting value (centered value is ~250mA)*/
        fIntensity -= 0.5f;
      }

      /* Temporisation between 2 steps */
      HAL_Delay(1);

      /* Exit when Joystic key is pressed */
      if (UserAction == 0x1)
      {
        goto End;
      }
    }while((fIntensity >= MAX_INTENSITY_DIMMER) && (NoWait == FALSE));

    /* Set signal LEDs to falling mode */
    /* Intensity falling phase */
    BSP_LED_On(LED4);
    BSP_LED_Off(LED3);

    /* Intensity changes from Max to Min */
    do
    {
      if(fIntensity > 605)
      {
        /* Decrease intensity slowly at low brightness  */
        fIntensity += 0.1f;
        /* Decrement progressively all thresholds down to 0*/
        maxsense --;
        MaxCurrentSense = (maxsense * 4096) /3300;

        /* Configure Sawtooth waveform generation */
        HAL_DACEx_SawtoothWaveGenerate(&DacHandle, DAC_CHANNEL_2, DAC_SAWTOOTH_POLARITY_DECREMENT, MaxCurrentSense, DAC_STEP);
      }
       else
       {
        /* Decrement progressively all thresholds up to the setting value (centered value is ~250mA)*/
         fIntensity += 0.5f;
       }

      /* Temporisation between 2 steps */
      HAL_Delay(1);

      /* Exit when Joystic key is pressed */
      if (UserAction == 0x1)
      {
        goto End;
      }
    }while((fIntensity <= MIN_INTENSITY) && (NoWait == FALSE));
  }

End:
  UserAction = 0x00;

  /* Reset all signal LEDs*/
  BSP_LED_Off(LED3);
  BSP_LED_Off(LED4);

  HRTIM_DeConfig();
}


/**
  * @brief  set the Manual Dimming.
  * @param  None
  * @retval None
  */
static void WhiteLed_ManualDimming(void)
{
  uint32_t exit = 0;
  uint32_t maxsense = DAC_SAWTOOTH_RANGE;

  /* Start with LED off */
  fIntensity = MIN_INTENSITY;

  kWindow_Popup("Manual Dimming", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
    "Press up/down to\nincrease/decrease\nLED brightness\n\nOther Joystick\nbutton to exit\n",                  \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* HRTIM Configuration */
  HRTIM_Config();

  /* Enable HRTIM Master Burst period interrupt */
  __HAL_HRTIM_ENABLE_IT(&HrtimHandle, HRTIM_IT_BMPER);

  HRTIM_SetBurstCompare(fIntensity);

  /* stop the Joystick interrupt */
  BSP_JOY_Init(JOY1, JOY_MODE_GPIO, JOY_ALL);

  /* Wait a press on any JoyStick pushbuttons */
  while(exit == 0)
  {
    UserEvent = BSP_JOY_GetState(JOY1);

    switch(UserEvent)
    {
    case JOY_NONE :
      break;
    case JOY_DOWN :
      {
        /* Display Decrease LED */
        BSP_LED_On(LED4);
        BSP_LED_Off(LED3);

        /* Check if min. intensity reached */
        if (fIntensity >= MIN_INTENSITY)
        {
          fIntensity = MIN_INTENSITY;
          /* Then blink display LED min. intensity */
          BSP_LED_Off(LED4);
          HAL_Delay(100);
        }
        else if(fIntensity > 605)
        {
          /* Decrease intensity slowly at low brightness  */
          fIntensity += 0.1f;
          /* Decrement progressively all thresholds down to 0*/
          maxsense -= 1;
          MaxCurrentSense = (maxsense * 4096) /3300;

          /* Configure Sawtooth waveform generation */
          HAL_DACEx_SawtoothWaveGenerate(&DacHandle, DAC_CHANNEL_2, DAC_SAWTOOTH_POLARITY_DECREMENT, MaxCurrentSense, DAC_STEP);
        }
        else
        {
          /* Decrease intensity quickly at high brightness  */
          fIntensity += 0.5f;
        }

        break;
      }

    case JOY_UP :
      {
        /* Display Increase LED */
        BSP_LED_Off(LED4);
        BSP_LED_On(LED3);

        /* Check if max. intensity reached */
        if (fIntensity <= MAX_INTENSITY)
        {
          fIntensity = MAX_INTENSITY;
          /* Then blink display LED max. intensity */
          BSP_LED_Off(LED3);
          HAL_Delay(100);
          BSP_LED_On(LED3);
          HAL_Delay(50);
        }
        else if(fIntensity > (605 + 0.1))
        {
          /* Inccrease intensity quickly at high brightness  */
          fIntensity -= 0.1f;
          /* Increment progressively all thresholds up to the setting value (centered value is ~250mA)*/
          maxsense +=1;
          MaxCurrentSense = (maxsense * 4096) /3300;

          /* Configure Sawtooth waveform generation */
          HAL_DACEx_SawtoothWaveGenerate(&DacHandle, DAC_CHANNEL_2, DAC_SAWTOOTH_POLARITY_DECREMENT, MaxCurrentSense, DAC_STEP);
        }
        else
        {
          /* Increment progressively all thresholds up to the setting value (centered value is ~250mA)*/
          fIntensity -= 0.5f;
        }

        break;
      }
    default:
      exit = 1;
      break;

    }
    UserAction = 0;
    UserEvent = 0;
  }

  BSP_LED_Off(LED3);
  BSP_LED_Off(LED4);
  HRTIM_DeConfig();
}

/**
  * @brief  make the led flashing.
  * @param  None
  * @retval None
  */
static void WhiteLed_Flashing(void)
{
  uint8_t  toggle      = FALSE;

  kWindow_Popup("Flashing", UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_ST_BLUE,\
                "Press joystick \nto stop\n",                  \
                UTIL_LCD_COLOR_ST_BLUE, UTIL_LCD_COLOR_WHITE );

  /* HRTIM Configuration */
  HRTIM_Config();

  /* Wait a press on any JoyStick pushbuttons */
  while (UserAction == 0x00u)
  {
    /* Flash mode consists to drive the LED @10Hz frequency (20% time ON) at maximum intensity */
    /* Disable HRTIM Master Burst period interrupt */
    __HAL_HRTIM_DISABLE_IT(&HrtimHandle, HRTIM_IT_BMPER);

    /* Reset all signal LEDs*/
    BSP_LED_Off(LED_GREEN);
    BSP_LED_Off(LED_BLUE);
    /* Alternatively signals LED_ORANGE and LED_RED are toggled */
    if (toggle != FALSE)
    {
      BSP_LED_On(LED_ORANGE);
      BSP_LED_Off(LED_RED);
    }
    else
    {
      BSP_LED_Off(LED_ORANGE);
      BSP_LED_On(LED_RED);
    }

    NoWait = FALSE;

    /* freq= 10Hz tON = 20ms; tOFF = 80ms */
    /* BUCK LED ON */
    HRTIM_SetBurstCompare(MAX_INTENSITY);
    /* tON = 20ms */
    HAL_Delay(20);

    /* Alternatively signals LED_ORANGE and LED_RED are toggled */
    if (toggle != FALSE)
    {
      BSP_LED_Off(LED_ORANGE);
      BSP_LED_On(LED_RED);
    }
    else
    {
      BSP_LED_On(LED_ORANGE);
      BSP_LED_Off(LED_RED);
    }

    /* BUCK LED OFF */
    HRTIM_SetBurstCompare(MIN_INTENSITY);
    /* tOFF = 80 ms */
    HAL_Delay(80);
    toggle = (!toggle);
  }

  UserAction = 0x00;

  /* Reset all signal LEDs*/
  BSP_LED_Off(LED_ORANGE);
  BSP_LED_Off(LED_RED);
  HRTIM_DeConfig();
}

/**
* @brief  DAC channel2 configuration
* @param  None
* @retval None
*/
static void DAC_Config(void)
{
  DAC_ChannelConfTypeDef DAC_ConfigStructure = {0};

  /* DAC4 Periph clock enable */
  __HAL_RCC_DAC4_CLK_ENABLE();

  /* DAC4 deinitialize */
  /* DAC4 is a fast DAC. Up to 15M samples per secondes */
  DacHandle.Instance = DAC4;
  if(HAL_DAC_DeInit(&DacHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /* DAC Init */
  if(HAL_DAC_Init(&DacHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /* Fill DAC ConfigStructure */
  /* Sawtooth waveform generation will be triggered by HRTIM */
  DAC_ConfigStructure.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
  DAC_ConfigStructure.DAC_Trigger      = DAC_TRIGGER_HRTIM_RST_TRG5;  // TRG5 correspond to HRTIM E
  DAC_ConfigStructure.DAC_Trigger2     = DAC_TRIGGER_HRTIM_STEP_TRG5;
  DAC_ConfigStructure.DAC_ConnectOnChipPeripheral      = DAC_CHIPCONNECT_INTERNAL;
  DAC_ConfigStructure.DAC_SampleAndHold      = DAC_SAMPLEANDHOLD_ENABLE;
  if(HAL_DAC_ConfigChannel(&DacHandle, &DAC_ConfigStructure, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure Sawtooth waveform generation */
  HAL_DACEx_SawtoothWaveGenerate(&DacHandle, DAC_CHANNEL_2, DAC_SAWTOOTH_POLARITY_DECREMENT, DAC_SAWTOOTH_RANGE, DAC_STEP);

  if(HAL_DAC_Start(&DacHandle, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
}


#ifdef DEBUG
/**
* @brief  OPAMP configuration: follower mode.
*         Used for debug purpose to be able to view DAC output on GPIO
* @param  None
* @retval None
*/
static void OPAMP_Config(void)
{
  GPIO_InitTypeDef       GPIO_InitStructure;

  /* Enable OPAMP clock */
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* OPAMP output configuration :  PA8 */
  /* GPIOA Peripheral clock enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStructure.Pin       = GPIO_PIN_8;
  GPIO_InitStructure.Mode      = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Pull      = GPIO_NOPULL;
  GPIO_InitStructure.Alternate = 0x0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure OPAMP in follower mode to view DAC output on GPIO */
  OpampHandle.Instance = OPAMP5;
  OpampHandle.Init.PowerMode = OPAMP_POWERMODE_HIGHSPEED;
  OpampHandle.Init.Mode = OPAMP_FOLLOWER_MODE;
  OpampHandle.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_DAC ;
  OpampHandle.Init.InvertingInput = 0x0;
  OpampHandle.Init.InternalOutput = DISABLE;
  OpampHandle.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
  OpampHandle.Init.InvertingInputSecondary = 0;
  OpampHandle.Init.NonInvertingInputSecondary = 0;
  OpampHandle.Init.PgaConnect = 0;
  OpampHandle.Init.PgaGain = 0;
  OpampHandle.Init.UserTrimming = OPAMP_TRIMMING_USER;
  OpampHandle.Init.TrimmingValueP = 0;
  OpampHandle.Init.TrimmingValueN = 0;
  HAL_OPAMP_Init(&OpampHandle);

  /* Calibrate OPAMP */
  HAL_OPAMP_SelfCalibrate(&OpampHandle);

  /* Start OPAMP */
  HAL_OPAMP_Start(&OpampHandle);
}
#endif

/**
* @brief  COMP configuration
* @param  None
* @retval None
*/
static void COMP_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOB Peripheral clock enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure PB11 in analog mode: PB11 is connected to COMP6 non inverting input */
  GPIO_InitStructure.Pin  =  GPIO_PIN_11;
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
#ifdef DEBUG
  /* COMP6 output config: PC6 for debug */
  GPIO_InitStructure.Pin       = GPIO_PIN_6;
  GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Pull      = GPIO_NOPULL;
  GPIO_InitStructure.Alternate = GPIO_AF7_COMP6;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif

  /*  COMP6 deinitialize */
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  CompHandle.Instance = COMP6;
  if(HAL_COMP_DeInit(&CompHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /* COMP6 config */
  CompHandle.Init.InputMinus        = COMP_INPUT_MINUS_DAC4_CH2;
  CompHandle.Init.InputPlus         = COMP_INPUT_PLUS_IO1;
  CompHandle.Init.OutputPol         = COMP_OUTPUTPOL_NONINVERTED;
  CompHandle.Init.Hysteresis        = COMP_HYSTERESIS_NONE;
  CompHandle.Init.BlankingSrce      = COMP_BLANKINGSRC_NONE;
  CompHandle.Init.TriggerMode       = COMP_TRIGGERMODE_NONE;
  if(HAL_COMP_Init(&CompHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable COMP6 */
  if(HAL_COMP_Start(&CompHandle) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
* @brief  HRTIM configuration
* @param  None
* @retval None
*/
static void HRTIM_Config(void)
{
  GPIO_InitTypeDef          GPIO_InitStructure;
  HRTIM_BurstModeCfgTypeDef HRTIM_BurstStructure;
  HRTIM_CompareCfgTypeDef   HRTIM_CompareStructure;
  HRTIM_EventCfgTypeDef     HRTIM_ExternalEventStructure;
  HRTIM_OutputCfgTypeDef    HRTIM_OutputStructure;
  HRTIM_TimeBaseCfgTypeDef  HRTIM_TimeBaseStructure;
  HRTIM_TimerCfgTypeDef     HRTIM_TimerWaveStructure;
  HRTIM_TimerCtlTypeDef     HRTIM_TimerCtl;

  /* Configure HRTIM TIM E ****************************************************/
  /* HRTIM output channel configuration : HRTIM_CHE1 (Buck drive) / PC8 */
  /* GPIOC Peripheral clock enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitStructure.Pin       = GPIO_PIN_8;
  GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Pull      = GPIO_NOPULL;
  /* Alternate function configuration : HRTIM_CHE1 / PC8 */
  GPIO_InitStructure.Alternate = GPIO_AF3_HRTIM1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);


  /* Ennable HRTIM clock */
  __HAL_RCC_HRTIM1_CLK_ENABLE();

  HrtimHandle.Instance = HRTIM1;
  HAL_HRTIM_Init(&HrtimHandle);

  /* HRTIM initialization startup */
  if(HAL_HRTIM_DLLCalibrationStart(&HrtimHandle, HRTIM_CALIBRATIONRATE_3) != HAL_OK)
  {
    Error_Handler();
  }

  if(HAL_HRTIM_PollForDLLCalibration(&HrtimHandle, HAL_MAX_DELAY) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure the output features */
  HRTIM_OutputStructure.Polarity              = HRTIM_OUTPUTPOLARITY_HIGH;
  HRTIM_OutputStructure.SetSource             = HRTIM_OUTPUTSET_TIMPER | HRTIM_OUTPUTSET_TIMCMP1;
  HRTIM_OutputStructure.ResetSource           = HRTIM_OUTPUTRESET_EEV_3;
  HRTIM_OutputStructure.IdleMode              = HRTIM_OUTPUTIDLEMODE_IDLE;
  HRTIM_OutputStructure.IdleLevel             = HRTIM_OUTPUTIDLELEVEL_INACTIVE;
  HRTIM_OutputStructure.FaultLevel            = HRTIM_OUTPUTFAULTLEVEL_NONE;
  HRTIM_OutputStructure.ChopperModeEnable     = HRTIM_OUTPUTCHOPPERMODE_DISABLED;
  HRTIM_OutputStructure.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;

  if(HAL_HRTIM_WaveformOutputConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_E,
                                    HRTIM_OUTPUT_TE1, &HRTIM_OutputStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure HRTIM1_TIME waveform */
  HRTIM_TimerWaveStructure.InterruptRequests     = (HRTIM_MASTER_IT_NONE | HRTIM_TIM_IT_NONE);
  HRTIM_TimerWaveStructure.DMARequests           = HRTIM_MASTER_DMA_NONE;
  HRTIM_TimerWaveStructure.DMASrcAddress         = 0;
  HRTIM_TimerWaveStructure.DMADstAddress         = 0;
  HRTIM_TimerWaveStructure.DMASize               = 0;
  HRTIM_TimerWaveStructure.HalfModeEnable        = HRTIM_HALFMODE_DISABLED;
  HRTIM_TimerWaveStructure.InterleavedMode       = HRTIM_INTERLEAVED_MODE_DISABLED;
  HRTIM_TimerWaveStructure.StartOnSync           = HRTIM_SYNCSTART_DISABLED;
  HRTIM_TimerWaveStructure.ResetOnSync           = HRTIM_SYNCRESET_DISABLED;
  HRTIM_TimerWaveStructure.DACSynchro            = HRTIM_DACSYNC_NONE;
  HRTIM_TimerWaveStructure.PreloadEnable         = HRTIM_PRELOAD_DISABLED;
  HRTIM_TimerWaveStructure.UpdateGating          = HRTIM_UPDATEGATING_INDEPENDENT;
  HRTIM_TimerWaveStructure.BurstMode             = HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;
  HRTIM_TimerWaveStructure.RepetitionUpdate      = HRTIM_UPDATEONREPETITION_DISABLED;
  HRTIM_TimerWaveStructure.PushPull              = HRTIM_TIMPUSHPULLMODE_DISABLED;
  HRTIM_TimerWaveStructure.FaultEnable           = HRTIM_TIMFAULTENABLE_NONE;
  HRTIM_TimerWaveStructure.FaultLock             = HRTIM_TIMFAULTLOCK_READWRITE;
  HRTIM_TimerWaveStructure.DeadTimeInsertion     = HRTIM_TIMDEADTIMEINSERTION_DISABLED;
  HRTIM_TimerWaveStructure.DelayedProtectionMode = HRTIM_TIMER_D_E_DELAYEDPROTECTION_DISABLED;
  HRTIM_TimerWaveStructure.BalancedIdleAutomaticResume = HRTIM_OUTPUTBIAR_DISABLED;
  HRTIM_TimerWaveStructure.UpdateTrigger         = HRTIM_TIMUPDATETRIGGER_TIMER_E;
  HRTIM_TimerWaveStructure.ResetTrigger          = HRTIM_TIMRESETTRIGGER_NONE;
  HRTIM_TimerWaveStructure.ResetUpdate           = HRTIM_TIMUPDATEONRESET_DISABLED;
  HRTIM_TimerWaveStructure.ReSyncUpdate          = HRTIM_TIMERESYNC_UPDATE_UNCONDITIONAL;

  if(HAL_HRTIM_WaveformTimerConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_E, &HRTIM_TimerWaveStructure) != HAL_OK)
  {
    Error_Handler();
  }

  HRTIM_CompareStructure.AutoDelayedMode    = HRTIM_AUTODELAYEDMODE_REGULAR;
  HRTIM_CompareStructure.AutoDelayedTimeout = 0;
  HRTIM_CompareStructure.CompareValue       = (PERIOD/(DAC_STEP_NB));
  if(HAL_HRTIM_WaveformCompareConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_E,
                                     HRTIM_COMPAREUNIT_2, &HRTIM_CompareStructure) != HAL_OK)
  {
    Error_Handler();
  }

  HRTIM_TimeBaseStructure.Period            = PERIOD; /* 1 period = 4 µs = 100% time */
  HRTIM_TimeBaseStructure.RepetitionCounter = 0x00;
  HRTIM_TimeBaseStructure.PrescalerRatio    = HRTIM_PRESCALERRATIO_MUL32;
  HRTIM_TimeBaseStructure.Mode              = HRTIM_MODE_CONTINUOUS;

  if(HAL_HRTIM_TimeBaseConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_E, &HRTIM_TimeBaseStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure External Event Source 3 */
  HRTIM_ExternalEventStructure.Source      = HRTIM_EEV3SRC_COMP6_OUT;
  HRTIM_ExternalEventStructure.Polarity    = HRTIM_EVENTPOLARITY_HIGH;
  HRTIM_ExternalEventStructure.Sensitivity = HRTIM_EVENTSENSITIVITY_LEVEL;
  HRTIM_ExternalEventStructure.FastMode    = HRTIM_EVENTFASTMODE_ENABLE;
  HRTIM_ExternalEventStructure.Filter      = HRTIM_EVENTFILTER_NONE;

  if(HAL_HRTIM_EventConfig(&HrtimHandle, HRTIM_EVENT_3, &HRTIM_ExternalEventStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Compare unit 1 is used to force output of HRTIM few cycles after roll over. */
  /* This is to be sure timer output is set despite reset (COMP) event occurring at the very beginning of the period */
  HRTIM_CompareStructure.AutoDelayedMode    = HRTIM_AUTODELAYEDMODE_REGULAR;
  HRTIM_CompareStructure.AutoDelayedTimeout = 0;
  HRTIM_CompareStructure.CompareValue       = 0x200;
  if(HAL_HRTIM_WaveformCompareConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_E,
                                     HRTIM_COMPAREUNIT_1, &HRTIM_CompareStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Burst Mode configuration */
  HRTIM_BurstStructure.Mode          = HRTIM_BURSTMODE_CONTINOUS;
  HRTIM_BurstStructure.ClockSource   = HRTIM_BURSTMODECLOCKSOURCE_TIMER_E;
  HRTIM_BurstStructure.Prescaler     = HRTIM_BURSTMODEPRESCALER_DIV1;
  HRTIM_BurstStructure.PreloadEnable = HRIM_BURSTMODEPRELOAD_ENABLED;
  HRTIM_BurstStructure.Trigger       = HRTIM_BURSTMODETRIGGER_NONE;
  HRTIM_BurstStructure.IdleDuration  = MIN_INTENSITY;
  HRTIM_BurstStructure.Period        = MIN_INTENSITY;

  if(HAL_HRTIM_BurstModeConfig(&HrtimHandle, &HRTIM_BurstStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable the TE1 output */
  if(HAL_HRTIM_WaveformOutputStart(&HrtimHandle, HRTIM_OUTPUT_TE1) != HAL_OK)
  {
    Error_Handler();
  }

  if(HAL_HRTIM_BurstModeCtl(&HrtimHandle, HRTIM_BURSTMODECTL_ENABLED) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure DAC Reset and Step signal for sawtooth generation */
  HRTIM_TimerCtl.UpDownMode = HRTIM_TIMERUPDOWNMODE_UP;
  HRTIM_TimerCtl. TrigHalf = HRTIM_TIMERTRIGHALF_DISABLED;
  HRTIM_TimerCtl.GreaterCMP3 = HRTIM_TIMERGTCMP3_EQUAL;
  HRTIM_TimerCtl.GreaterCMP1 = HRTIM_TIMERGTCMP1_EQUAL;
  HRTIM_TimerCtl.DualChannelDacReset = HRTIM_TIMER_DCDR_COUNTER;
  HRTIM_TimerCtl.DualChannelDacStep = HRTIM_TIMER_DCDS_CMP2;
  HRTIM_TimerCtl.DualChannelDacEnable = HRTIM_TIMER_DCDE_ENABLED;

  if(HAL_HRTIM_TimerDualChannelDacConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_E, &HRTIM_TimerCtl) != HAL_OK)
  {
    Error_Handler();
  }

  /* Start slave*/
  if(HAL_HRTIM_WaveformCounterStart(&HrtimHandle, HRTIM_TIMERID_TIMER_E) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure and enable HRTIM interrupt */
  HAL_NVIC_SetPriority(HRTIM1_Master_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(HRTIM1_Master_IRQn);

  /* Enable Burst mode period completed interrupt */
  HrtimHandle.Init.HRTIMInterruptResquests = HRTIM_IT_BMPER;

  /* Select Burst Trigger */
  if(HAL_HRTIM_BurstModeSoftwareTrigger(&HrtimHandle) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
* @brief  HRTIM deconfiguration
* @param  None
* @retval None
*/
static void HRTIM_DeConfig(void)
{
  /* BUCK LED is OFF */
  HRTIM_SetBurstCompare(MIN_INTENSITY);
  HAL_Delay(10);

  /* Disable HRTIM Master Burst period interrupt */
  __HAL_HRTIM_DISABLE_IT(&HrtimHandle, HRTIM_IT_BMPER);

  if(HAL_HRTIM_WaveformOutputStop(&HrtimHandle, HRTIM_OUTPUT_TE1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_HRTIM_WaveformCounterStop(&HrtimHandle, HRTIM_TIMERID_TIMER_E);

  HAL_HRTIM_BurstModeCtl(&HrtimHandle, HRTIM_BURSTMODECTL_DISABLED);
  HAL_HRTIM_DeInit(&HrtimHandle);

  __HAL_RCC_HRTIM1_CLK_DISABLE();

  /* Deinit GPIO PC8 */
  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8);
}

/**
  * @brief  Callback function invoked when the end of the burst mode period is reached
  * @param  hhrtim: pointer to HAL HRTIM handle
  * @retval None
  */
void HAL_HRTIM_BurstModePeriodCallback(HRTIM_HandleTypeDef * hhrtim)
{
  static uint8_t index1 =0;
  uint32_t i_intensity = 0;
  float index2;

  /* Each Master Burst mode period interrupt, the burst compare is updated */
  i_intensity = (uint32_t) fIntensity;
  index2 = fIntensity - i_intensity;
  index2 *= 8;

  if (DitherTab[(uint32_t)index2][index1] == 0)
  {
    /* If Dither table value is zero then standard drive ...*/
    HRTIM_SetBurstCompare(i_intensity);
  }
  else
  {
    /* or extra period is added */
    HRTIM_SetBurstCompare(i_intensity + 1);
  }

  /* Each interrupt the table data are rolling from 1 to 7 */
  index1++;
  if (index1 > 7)
  {
    index1 = 0;
  }
}

/**
* @brief  Set Burst Compare value
* @param  BurstCompare: Burst Compare value, it can be a value between 0x0 and 0xFFFF
* @retval None
*/
static void HRTIM_SetBurstCompare(float BurstCompare)
{
  /* Set Burst Compare value */
  HrtimHandle.Instance->sCommonRegs.BMCMPR = (uint16_t)BurstCompare;
}


#undef __APP_WHITELED_C
#endif

