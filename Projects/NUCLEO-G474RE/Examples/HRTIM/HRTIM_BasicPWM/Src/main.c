/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32g4xx_nucleo.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
HRTIM_HandleTypeDef hhrtim1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
#define HRTIM_INPUT_CLOCK       ((uint64_t)144000000)   /* Value in Hz */

/* Formula below works down to 70.3kHz (with presc ratio = 1) */ 
#define _200KHz_PERIOD ((uint16_t)((HRTIM_INPUT_CLOCK * 32) / 200000))

/* Formula below works down to 70.3kHz (with presc ratio = 1) */ 
#define _100KHz_PERIOD ((uint16_t)((HRTIM_INPUT_CLOCK * 32) / 100000))

/* Formula below works down to 70.3kHz (with presc ratio = 1) */ 
#define _100KHz_Plus_PERIOD ((uint16_t)((HRTIM_INPUT_CLOCK * 32) / 100001))

/* Formula below works down to 17.6kHz (with presc ratio = 4) */ 
#define _33KHz_PERIOD ((uint16_t)((HRTIM_INPUT_CLOCK * 8) / 33333))

/* Define here the snippet to be executed */
#define HRTIM_CHECK
//#define SINGLE_PWM
//#define MULTIPLE_PWM
//#define PWM_MASTER
//#define ARBITRARY_WAVEFORM

HRTIM_HandleTypeDef hhrtim1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_HRTIM1_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
static void GPIO_HRTIM_outputs_Config(void);

#ifdef HRTIM_CHECK
static void HRTIM_Config_HRTIMCheck(void);
#endif /* HRTIM_CHECK */

#ifdef SINGLE_PWM
static void HRTIM_Config_SinglePWM(void);
#endif /* SINGLE_PWM */

#ifdef MULTIPLE_PWM
static void HRTIM_Config_MultiplePWM(void);
#endif  /* MULTIPLE_PWM */

#ifdef PWM_MASTER
static void HRTIM_Config_MasterPWM(void);
#endif /* PWM_MASTER */

#ifdef ARBITRARY_WAVEFORM
static void HRTIM_Config_ArbitraryWaveform(void);
#endif /* ARBITRARY_WAVEFORM */
  
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_HRTIM1_Init();
  /* USER CODE BEGIN 2 */

  /* ---------------------------------------------------------------------------*/
/* ------------------------ EXAMPLE 1: HRTIM_CHECK ---------------------------*/
/* ---------------------------------------------------------------------------*/
#ifdef HRTIM_CHECK
  
  HRTIM_Config_HRTIMCheck();    /* Initialize HRTIM and related inputs */
  GPIO_HRTIM_outputs_Config();  /* Initialize GPIO's HRTIM outputs */
  BSP_LED_On(LED2);
#endif  /* HRTIM_CHECK */
/* ---------------------------------------------------------------------------*/
/* ------------------------ EXAMPLE 2: SINGLE PWM ----------------------------*/
/* ---------------------------------------------------------------------------*/
#ifdef SINGLE_PWM

  HRTIM_Config_SinglePWM();     /* Initialize HRTIM and related inputs */
  GPIO_HRTIM_outputs_Config();  /* Initialize GPIO's HRTIM outputs */

 #endif  /* SINGLE_PWM */
/* ---------------------------------------------------------------------------*/
/* ------------------------ EXAMPLE 3: MULTIPLE PWM --------------------------*/
/* ---------------------------------------------------------------------------*/
#ifdef MULTIPLE_PWM
  
  HRTIM_Config_MultiplePWM();   /* Initialize HRTIM and related inputs */
  GPIO_HRTIM_outputs_Config();  /* Initialize GPIO's HRTIM outputs */

#endif /* MULTIPLE_PWM */
/* ---------------------------------------------------------------------------*/
/* -------------------- SNIPPET 4: PWM with MASTER TIMER ---------------------*/
/* ---------------------------------------------------------------------------*/
#ifdef PWM_MASTER

  HRTIM_Config_MasterPWM();     /* Initialize HRTIM and related inputs */
  GPIO_HRTIM_outputs_Config();  /* Initialize GPIO's HRTIM outputs */

#endif /* PWM_MASTER */
/* ---------------------------------------------------------------------------*/
/* --------------------- EXAMPLE 5: ARBITRARY_WAVEFORM -----------------------*/
/* ---------------------------------------------------------------------------*/
#ifdef ARBITRARY_WAVEFORM

  HRTIM_Config_ArbitraryWaveform();     /* Initialize HRTIM and related inputs */
  GPIO_HRTIM_outputs_Config();  /* Initialize GPIO's HRTIM outputs */

#endif /* ARBITRARY_WAVEFORM */

 
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
/* ---------------------------------------------------------------------------*/
/* ------------------------ EXAMPLE 1: HRTIM_CHECK ---------------------------*/
/* ---------------------------------------------------------------------------*/
#ifdef HRTIM_CHECK

    /* TD1 output toggles at ~430kHz (this frequency can vary depending on compiler and optimizations options) */
    HAL_HRTIM_WaveformSetOutputLevel(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD1, HRTIM_OUTPUTLEVEL_ACTIVE);
    HAL_HRTIM_WaveformSetOutputLevel(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD1, HRTIM_OUTPUTLEVEL_INACTIVE);
    
    /* With direct register access, TD1 output toggles at ~10MHz */
//    HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_D].SETx1R = HRTIM_SET1R_SST;
//    HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_D].RSTx1R = HRTIM_RST1R_SRT;
#endif  /* HRTIM_CHECK */
/* ---------------------------------------------------------------------------*/
/* ------------------------ EXAMPLE 2: SINGLE PWM ----------------------------*/
/* ---------------------------------------------------------------------------*/
#ifdef SINGLE_PWM

    BSP_LED_Toggle(LED2);
    HAL_Delay(100);

#endif  /* SINGLE_PWM */
/* ---------------------------------------------------------------------------*/
/* ------------------------ EXAMPLE 3: MULTIPLE PWM --------------------------*/
/* ---------------------------------------------------------------------------*/
#ifdef MULTIPLE_PWM

    BSP_LED_Toggle(LED2);
    HAL_Delay(100);

#endif /* MULTIPLE_PWM */
/* ---------------------------------------------------------------------------*/
/* -------------------- SNIPPET 4: PWM with MASTER TIMER ---------------------*/
/* ---------------------------------------------------------------------------*/
#ifdef PWM_MASTER

    BSP_LED_Toggle(LED2);
    HAL_Delay(100);

#endif /* PWM_MASTER */
/* ---------------------------------------------------------------------------*/
/* --------------------- EXAMPLE 5: ARBITRARY_WAVEFORM -----------------------*/
/* ---------------------------------------------------------------------------*/
#ifdef ARBITRARY_WAVEFORM

    BSP_LED_Toggle(LED2);
    HAL_Delay(100);

#endif /* ARBITRARY_WAVEFORM */

 
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_8) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief HRTIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_HRTIM1_Init(void)
{

  /* USER CODE BEGIN HRTIM1_Init 0 */

  /* USER CODE END HRTIM1_Init 0 */

  HRTIM_TimeBaseCfgTypeDef pTimeBaseCfg = {0};
  HRTIM_TimerCfgTypeDef pTimerCfg = {0};
  HRTIM_TimerCtlTypeDef pTimerCtl = {0};
  HRTIM_OutputCfgTypeDef pOutputCfg = {0};

  /* USER CODE BEGIN HRTIM1_Init 1 */

  /* USER CODE END HRTIM1_Init 1 */
  hhrtim1.Instance = HRTIM1;
  hhrtim1.Init.HRTIMInterruptResquests = HRTIM_IT_NONE;
  hhrtim1.Init.SyncOptions = HRTIM_SYNCOPTION_NONE;
  if (HAL_HRTIM_Init(&hhrtim1) != HAL_OK)
  {
    Error_Handler();
  }
  pTimeBaseCfg.Period = 0xFFFD;
  pTimeBaseCfg.RepetitionCounter = 0x00;
  pTimeBaseCfg.PrescalerRatio = HRTIM_PRESCALERRATIO_DIV1;
  pTimeBaseCfg.Mode = HRTIM_MODE_CONTINUOUS;
  if (HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_MASTER, &pTimeBaseCfg) != HAL_OK)
  {
    Error_Handler();
  }
  pTimerCfg.InterruptRequests = HRTIM_MASTER_IT_NONE;
  pTimerCfg.DMARequests = HRTIM_MASTER_DMA_NONE;
  pTimerCfg.DMASrcAddress = 0x0000;
  pTimerCfg.DMADstAddress = 0x0000;
  pTimerCfg.DMASize = 0x1;
  pTimerCfg.HalfModeEnable = HRTIM_HALFMODE_DISABLED;
  pTimerCfg.InterleavedMode = HRTIM_INTERLEAVED_MODE_DISABLED;
  pTimerCfg.StartOnSync = HRTIM_SYNCSTART_DISABLED;
  pTimerCfg.ResetOnSync = HRTIM_SYNCRESET_DISABLED;
  pTimerCfg.DACSynchro = HRTIM_DACSYNC_NONE;
  pTimerCfg.PreloadEnable = HRTIM_PRELOAD_DISABLED;
  pTimerCfg.UpdateGating = HRTIM_UPDATEGATING_INDEPENDENT;
  pTimerCfg.BurstMode = HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;
  pTimerCfg.RepetitionUpdate = HRTIM_UPDATEONREPETITION_DISABLED;
  pTimerCfg.ReSyncUpdate = HRTIM_TIMERESYNC_UPDATE_UNCONDITIONAL;
  if (HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_MASTER, &pTimerCfg) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, &pTimeBaseCfg) != HAL_OK)
  {
    Error_Handler();
  }
  pTimerCtl.UpDownMode = HRTIM_TIMERUPDOWNMODE_UP;
  pTimerCtl.DualChannelDacEnable = HRTIM_TIMER_DCDE_DISABLED;
  if (HAL_HRTIM_WaveformTimerControl(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, &pTimerCtl) != HAL_OK)
  {
    Error_Handler();
  }
  pTimerCfg.InterruptRequests = HRTIM_TIM_IT_NONE;
  pTimerCfg.DMARequests = HRTIM_TIM_DMA_NONE;
  pTimerCfg.DMASrcAddress = 0x0000;
  pTimerCfg.DMADstAddress = 0x0000;
  pTimerCfg.DMASize = 0x1;
  pTimerCfg.PushPull = HRTIM_TIMPUSHPULLMODE_DISABLED;
  pTimerCfg.FaultEnable = HRTIM_TIMFAULTENABLE_NONE;
  pTimerCfg.FaultLock = HRTIM_TIMFAULTLOCK_READWRITE;
  pTimerCfg.DeadTimeInsertion = HRTIM_TIMDEADTIMEINSERTION_DISABLED;
  pTimerCfg.DelayedProtectionMode = HRTIM_TIMER_A_B_C_DELAYEDPROTECTION_DISABLED;
  pTimerCfg.UpdateTrigger = HRTIM_TIMUPDATETRIGGER_NONE;
  pTimerCfg.ResetTrigger = HRTIM_TIMRESETTRIGGER_NONE;
  pTimerCfg.ResetUpdate = HRTIM_TIMUPDATEONRESET_DISABLED;
  if (HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, &pTimerCfg) != HAL_OK)
  {
    Error_Handler();
  }
  pTimerCfg.DMASrcAddress = 0x0000;
  pTimerCfg.DMADstAddress = 0x0000;
  pTimerCfg.DMASize = 0x1;
  pTimerCfg.DelayedProtectionMode = HRTIM_TIMER_D_E_DELAYEDPROTECTION_DISABLED;
  if (HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pTimerCfg) != HAL_OK)
  {
    Error_Handler();
  }
  pOutputCfg.Polarity = HRTIM_OUTPUTPOLARITY_HIGH;
  pOutputCfg.SetSource = HRTIM_OUTPUTSET_NONE;
  pOutputCfg.ResetSource = HRTIM_OUTPUTRESET_NONE;
  pOutputCfg.IdleMode = HRTIM_OUTPUTIDLEMODE_NONE;
  pOutputCfg.IdleLevel = HRTIM_OUTPUTIDLELEVEL_INACTIVE;
  pOutputCfg.FaultLevel = HRTIM_OUTPUTFAULTLEVEL_NONE;
  pOutputCfg.ChopperModeEnable = HRTIM_OUTPUTCHOPPERMODE_DISABLED;
  pOutputCfg.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;
  if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_OUTPUT_TA1, &pOutputCfg) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD1, &pOutputCfg) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_OUTPUT_TA2, &pOutputCfg) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_HRTIM_WaveformOutputConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_OUTPUT_TD2, &pOutputCfg) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pTimeBaseCfg) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_HRTIM_WaveformTimerControl(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &pTimerCtl) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN HRTIM1_Init 2 */

  /* USER CODE END HRTIM1_Init 2 */
  HAL_HRTIM_MspPostInit(&hhrtim1);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD_U_Pin|LD_D_Pin|LD_L_Pin|LD_R_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B_USER_Pin */
  GPIO_InitStruct.Pin = B_USER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B_USER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Vin_Sense_Pin Vout_Sense_Pin */
  GPIO_InitStruct.Pin = Vin_Sense_Pin|Vout_Sense_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD_U_Pin LD_D_Pin LD_L_Pin LD_R_Pin */
  GPIO_InitStruct.Pin = LD_U_Pin|LD_D_Pin|LD_L_Pin|LD_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/**
* @brief  Configure GPIO outputs for the HRTIM
* @param  None
* @retval None
*/
static void GPIO_HRTIM_outputs_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable GPIOA clock for timer A outputs */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Configure HRTIM output: TA1 (PA8) and TA2 (PA9)*/
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9; 
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;  
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;;  
  GPIO_InitStruct.Alternate = GPIO_AF13_HRTIM1;
  
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Enable GPIOB clock for timer D outputs */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure HRTIM output: TD1 (PB14) and TD2 (PB15)*/
  GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15; 
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;  
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;;  
  GPIO_InitStruct.Alternate = GPIO_AF13_HRTIM1;
  
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

#ifdef HRTIM_CHECK
/**
* @brief  HRTIM configuration for checking that HRTIM is up and running
* @param  None
* @retval None
*/
static void HRTIM_Config_HRTIMCheck(void)
{

  /* ------------------------------------------------- */
    /* HRTIM Global initialization: Clock and DLL init */
  /* ------------------------------------------------- */
  /* Initialize the HRTIM structure (minimal configuration) */
  hhrtim1.Instance = HRTIM1;
  hhrtim1.Init.HRTIMInterruptResquests = HRTIM_IT_NONE;
  hhrtim1.Init.SyncOptions = HRTIM_SYNCOPTION_NONE;

  /* Initialize HRTIM */
  HAL_HRTIM_Init(&hhrtim1);

  /* HRTIM DLL calibration: periodic calibration, set period to 14µs */
  HAL_HRTIM_DLLCalibrationStart(&hhrtim1, HRTIM_CALIBRATIONRATE_3);

  /* Wait calibration completion*/
  if (HAL_HRTIM_PollForDLLCalibration(&hhrtim1, 100) != HAL_OK)
  {
    Error_Handler(); /* if DLL or clock is not correctly set */
  }

  /* ---------------*/
  /* HRTIM start-up */
  /* ---------------*/
  /* Enable HRTIM's outputs TD1 */
  /* Note: it is necessary to enable also GPIOs to have outputs functional */
  /* This must be done after HRTIM initialization */
  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1); 

}
#endif /* HRTIM_CHECK */


#ifdef SINGLE_PWM
/**
* @brief  HRTIM configuration for a single PWM signal
* @param  None
* @retval None
*/
static void HRTIM_Config_SinglePWM(void)
{
  HRTIM_TimeBaseCfgTypeDef timebase_config;
  HRTIM_TimerCfgTypeDef timer_config;
  HRTIM_OutputCfgTypeDef output_config_TD1;
  HRTIM_CompareCfgTypeDef compare_config;

  /* ----------------------------------------------- */
    /* HRTIM Global initialization: Clock and DLL init */
  /* ----------------------------------------------- */
  /* Initialize the HRTIM structure (minimal configuration) */
  hhrtim1.Instance = HRTIM1;
  hhrtim1.Init.HRTIMInterruptResquests = HRTIM_IT_NONE;
  hhrtim1.Init.SyncOptions = HRTIM_SYNCOPTION_NONE;

  /* Initialize HRTIM */
  HAL_HRTIM_Init(&hhrtim1);

  /* HRTIM DLL calibration: periodic calibration, set period to 14µs */
  HAL_HRTIM_DLLCalibrationStart(&hhrtim1, HRTIM_CALIBRATIONRATE_3);

  /* Wait calibration completion*/
  if (HAL_HRTIM_PollForDLLCalibration(&hhrtim1, 100) != HAL_OK)
  {
    Error_Handler(); /* if DLL or clock is not correctly set */
  }
  
  /* ------------------------------------------------------------ */
  /* TIMERD initialization: set PWM frequency and continuous mode */
  /* ------------------------------------------------------------ */
  timebase_config.Period = _100KHz_PERIOD;
  timebase_config.RepetitionCounter = 0;
  timebase_config.PrescalerRatio = HRTIM_PRESCALERRATIO_MUL32;
  timebase_config.Mode = HRTIM_MODE_CONTINUOUS;
  HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &timebase_config);

  
  /* --------------------------------------------------------------------- */
  /* TIMERD global configuration: all values to default                    */
  /* --------------------------------------------------------------------- */
  timer_config.DMARequests = HRTIM_TIM_DMA_NONE;
  timer_config.DMASrcAddress = 0x0;
  timer_config.DMADstAddress = 0x0;
  timer_config.DMASize = 0x0;
  timer_config.HalfModeEnable = HRTIM_HALFMODE_DISABLED;
  timer_config.StartOnSync = HRTIM_SYNCSTART_DISABLED;
  timer_config.ResetOnSync = HRTIM_SYNCRESET_DISABLED;
  timer_config.DACSynchro = HRTIM_DACSYNC_NONE;
  timer_config.PreloadEnable = HRTIM_PRELOAD_DISABLED;
  timer_config.UpdateGating = HRTIM_UPDATEGATING_INDEPENDENT;
  timer_config.BurstMode = HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;
  timer_config.RepetitionUpdate = HRTIM_UPDATEONREPETITION_DISABLED;
  timer_config.ResetUpdate = HRTIM_TIMUPDATEONRESET_DISABLED;
  timer_config.InterruptRequests = HRTIM_TIM_IT_NONE;
  timer_config.PushPull = HRTIM_TIMPUSHPULLMODE_DISABLED;
  timer_config.FaultEnable = HRTIM_TIMFAULTENABLE_NONE;
  timer_config.FaultLock = HRTIM_TIMFAULTLOCK_READWRITE;
  timer_config.DeadTimeInsertion = HRTIM_TIMDEADTIMEINSERTION_DISABLED;
  timer_config.DelayedProtectionMode = HRTIM_TIMER_A_B_C_DELAYEDPROTECTION_DISABLED;
  timer_config.UpdateTrigger= HRTIM_TIMUPDATETRIGGER_NONE;
  timer_config.ResetTrigger = HRTIM_TIMRESETTRIGGER_NONE;
  HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D,&timer_config);
    
  /* --------------------------------------------------------- */
  /* TD1 waveform description TD1 set on period, reset on CMP1 */
  /* --------------------------------------------------------- */
  output_config_TD1.Polarity = HRTIM_OUTPUTPOLARITY_HIGH;
  output_config_TD1.SetSource = HRTIM_OUTPUTSET_TIMPER;
  output_config_TD1.ResetSource  = HRTIM_OUTPUTRESET_TIMCMP1;
  output_config_TD1.IdleMode = HRTIM_OUTPUTIDLEMODE_NONE;
  output_config_TD1.IdleLevel = HRTIM_OUTPUTIDLELEVEL_INACTIVE;
  output_config_TD1.FaultLevel = HRTIM_OUTPUTFAULTLEVEL_NONE;
  output_config_TD1.ChopperModeEnable = HRTIM_OUTPUTCHOPPERMODE_DISABLED;
  output_config_TD1.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;
  HAL_HRTIM_WaveformOutputConfig(&hhrtim1,
                                 HRTIM_TIMERINDEX_TIMER_D,
                                 HRTIM_OUTPUT_TD1,
                                 &output_config_TD1);

  /* Set compare registers for duty cycle on TD1 */
  compare_config.CompareValue = _100KHz_PERIOD/2;     // 50% duty cycle
  HAL_HRTIM_WaveformCompareConfig(&hhrtim1,
                                  HRTIM_TIMERINDEX_TIMER_D,
                                  HRTIM_COMPAREUNIT_1,
                                  &compare_config);

  /* ---------------*/
  /* HRTIM start-up */
  /* ---------------*/
  /* Enable HRTIM's outputs TD1 */
  /* Note: it is necessary to enable also GPIOs to have outputs functional */
  /* This must be done after HRTIM initialization */
  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1); 

  /* Start HRTIM's TIMER D*/
  HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);

}
#endif /* SINGLE_PWM */

#ifdef MULTIPLE_PWM
/**
* @brief  HRTIM configuration for generating multiple PWM outputs
* @param  None
* @retval None
*/
static void HRTIM_Config_MultiplePWM(void)
{
  HRTIM_TimeBaseCfgTypeDef timebase_config;
  HRTIM_TimerCfgTypeDef timer_config;
  HRTIM_OutputCfgTypeDef output_config_TDx;
  HRTIM_OutputCfgTypeDef output_config_TAx;
  HRTIM_CompareCfgTypeDef compare_config;

  /* ----------------------------------------------- */
  /* HRTIM Global initialization: Clock and DLL init */
  /* ----------------------------------------------- */
  /* Initialize the HRTIM structure (minimal configuration) */
  hhrtim1.Instance = HRTIM1;
  hhrtim1.Init.HRTIMInterruptResquests = HRTIM_IT_NONE;
  hhrtim1.Init.SyncOptions = HRTIM_SYNCOPTION_NONE;

  /* Initialize HRTIM */
  HAL_HRTIM_Init(&hhrtim1);

  /* HRTIM DLL calibration: periodic calibration, set period to 14µs */
  HAL_HRTIM_DLLCalibrationStart(&hhrtim1, HRTIM_CALIBRATIONRATE_3);

  /* Wait calibration completion*/
  if (HAL_HRTIM_PollForDLLCalibration(&hhrtim1, 100) != HAL_OK)
  {
    Error_Handler(); /* if DLL or clock is not correctly set */
  }
  
  /* ------------------------------------------------------------ */
  /* TIMERD initialization: set PWM frequency and continuous mode */
  /* ------------------------------------------------------------ */
  timebase_config.Period = _100KHz_PERIOD;
  timebase_config.RepetitionCounter = 0;
  timebase_config.PrescalerRatio = HRTIM_PRESCALERRATIO_MUL32;
  timebase_config.Mode = HRTIM_MODE_CONTINUOUS;
  HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &timebase_config);

  
  /* --------------------------------------------------------- */
  /* TIMERD global configuration: preload enabled on REP event */
  /* --------------------------------------------------------- */
  timer_config.DMARequests = HRTIM_TIM_DMA_NONE;
  timer_config.HalfModeEnable = HRTIM_HALFMODE_DISABLED;
  timer_config.StartOnSync = HRTIM_SYNCSTART_DISABLED;
  timer_config.ResetOnSync = HRTIM_SYNCRESET_DISABLED;
  timer_config.DACSynchro = HRTIM_DACSYNC_NONE;
  timer_config.PreloadEnable = HRTIM_PRELOAD_ENABLED;
  timer_config.UpdateGating = HRTIM_UPDATEGATING_INDEPENDENT;
  timer_config.BurstMode = HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;
  timer_config.RepetitionUpdate = HRTIM_UPDATEONREPETITION_ENABLED;
  timer_config.ResetUpdate = HRTIM_TIMUPDATEONRESET_DISABLED;
  timer_config.InterruptRequests = HRTIM_TIM_IT_NONE;
  timer_config.PushPull = HRTIM_TIMPUSHPULLMODE_DISABLED;
  timer_config.FaultEnable = HRTIM_TIMFAULTENABLE_NONE;
  timer_config.FaultLock = HRTIM_TIMFAULTLOCK_READWRITE;
  timer_config.DeadTimeInsertion = HRTIM_TIMDEADTIMEINSERTION_DISABLED;
  timer_config.DelayedProtectionMode = HRTIM_TIMER_A_B_C_DELAYEDPROTECTION_DISABLED;
  timer_config.UpdateTrigger= HRTIM_TIMUPDATETRIGGER_NONE;
  timer_config.ResetTrigger = HRTIM_TIMRESETTRIGGER_NONE;
  HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D,&timer_config);
    
  /* ------------------------------------------------------ */
  /* TD1 waveform description: set on period, reset on CMP1 */
  /* ------------------------------------------------------ */
  output_config_TDx.Polarity = HRTIM_OUTPUTPOLARITY_HIGH;
  output_config_TDx.SetSource = HRTIM_OUTPUTSET_TIMPER;
  output_config_TDx.ResetSource  = HRTIM_OUTPUTRESET_TIMCMP1;
  output_config_TDx.IdleMode = HRTIM_OUTPUTIDLEMODE_NONE;
  output_config_TDx.IdleLevel = HRTIM_OUTPUTIDLELEVEL_INACTIVE;
  output_config_TDx.FaultLevel = HRTIM_OUTPUTFAULTLEVEL_NONE;
  output_config_TDx.ChopperModeEnable = HRTIM_OUTPUTCHOPPERMODE_DISABLED;
  output_config_TDx.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;
  HAL_HRTIM_WaveformOutputConfig(&hhrtim1,
                                 HRTIM_TIMERINDEX_TIMER_D,
                                 HRTIM_OUTPUT_TD1,
                                 &output_config_TDx);

  /* ----------------------------------------------------------------------- */
  /* TD2 waveform description: set on CMP2, reset on period */
  /* Note: TDx init structure is re-used, identical parameters not set again */
  /* ----------------------------------------------------------------------- */
  output_config_TDx.SetSource = HRTIM_OUTPUTSET_TIMCMP2;
  output_config_TDx.ResetSource  = HRTIM_OUTPUTRESET_TIMPER;
  HAL_HRTIM_WaveformOutputConfig(&hhrtim1,
                                 HRTIM_TIMERINDEX_TIMER_D,
                                 HRTIM_OUTPUT_TD2,
                                 &output_config_TDx);

  /* Set compare registers for duty cycle on TD1 */
  compare_config.CompareValue = _100KHz_PERIOD/4;     /* 25% duty cycle */
  HAL_HRTIM_WaveformCompareConfig(&hhrtim1,
                                  HRTIM_TIMERINDEX_TIMER_D,
                                  HRTIM_COMPAREUNIT_1,
                                  &compare_config);

  /* Set compare register for duty cycle on TD2 */
  compare_config.AutoDelayedMode = HRTIM_AUTODELAYEDMODE_REGULAR;
  compare_config.AutoDelayedTimeout = 0;
  compare_config.CompareValue = (3*_100KHz_PERIOD)/4;     /* 75% of the period */
  HAL_HRTIM_WaveformCompareConfig(&hhrtim1,
                                  HRTIM_TIMERINDEX_TIMER_D,
                                  HRTIM_COMPAREUNIT_2,
                                  &compare_config);

  /* ------------------------------------------------------------ */
  /* TIMERA initialization: set PWM frequency and continuous mode */
  /* Prescaler ratio is different from Timer A settings */
  /* ------------------------------------------------------------ */
  timebase_config.Period = _33KHz_PERIOD;
  timebase_config.RepetitionCounter = 0;
  timebase_config.PrescalerRatio = HRTIM_PRESCALERRATIO_MUL8;
  timebase_config.Mode = HRTIM_MODE_CONTINUOUS;
  HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, &timebase_config);

  
  /* --------------------------------------------------------- */
  /* TIMERA global configuration: preload enabled on REP event */
  /* Uses Timer D very same configuration                      */
  /* --------------------------------------------------------- */
  HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A,&timer_config);
    
  /* ------------------------------------------------------ */
  /* TA1 waveform description: set on period, reset on CMP1 */
  /* ------------------------------------------------------ */
  output_config_TAx.Polarity = HRTIM_OUTPUTPOLARITY_HIGH;
  output_config_TAx.SetSource = HRTIM_OUTPUTSET_TIMPER;
  output_config_TAx.ResetSource  = HRTIM_OUTPUTRESET_TIMCMP1;
  output_config_TAx.IdleMode = HRTIM_OUTPUTIDLEMODE_NONE;
  output_config_TAx.IdleLevel = HRTIM_OUTPUTIDLELEVEL_INACTIVE;
  output_config_TAx.FaultLevel = HRTIM_OUTPUTFAULTLEVEL_NONE;
  output_config_TAx.ChopperModeEnable = HRTIM_OUTPUTCHOPPERMODE_DISABLED;
  output_config_TAx.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;
  HAL_HRTIM_WaveformOutputConfig(&hhrtim1,
                                 HRTIM_TIMERINDEX_TIMER_A,
                                 HRTIM_OUTPUT_TA1,
                                 &output_config_TAx);

  /* ------------------------------------------------------ */
  /* TA2 waveform description: set on CMP2, reset on period */
  /* Note: TAx init structure is re-used                    */
  /* ------------------------------------------------------ */
  output_config_TAx.SetSource = HRTIM_OUTPUTSET_TIMCMP2;
  output_config_TAx.ResetSource  = HRTIM_OUTPUTRESET_TIMCMP3;
  HAL_HRTIM_WaveformOutputConfig(&hhrtim1,
                                 HRTIM_TIMERINDEX_TIMER_A,
                                 HRTIM_OUTPUT_TA2,
                                 &output_config_TAx);

  /* Set compare registers for duty cycle on TA1 */
  compare_config.CompareValue = _33KHz_PERIOD/4;     /* 25% duty cycle */
  HAL_HRTIM_WaveformCompareConfig(&hhrtim1,
                                  HRTIM_TIMERINDEX_TIMER_A,
                                  HRTIM_COMPAREUNIT_1,
                                  &compare_config);

  /* Set compare register for duty cycle on TA2 */
  compare_config.AutoDelayedMode = HRTIM_AUTODELAYEDMODE_REGULAR;
  compare_config.AutoDelayedTimeout = 0;
  compare_config.CompareValue = _33KHz_PERIOD/2;     /* 50% of the period */
  HAL_HRTIM_WaveformCompareConfig(&hhrtim1,
                                  HRTIM_TIMERINDEX_TIMER_A,
                                  HRTIM_COMPAREUNIT_2,
                                  &compare_config);

  /* Set compare register for duty cycle on TA2 */
  compare_config.CompareValue = (3*_33KHz_PERIOD)/4;     /* 75% of the period */
  HAL_HRTIM_WaveformCompareConfig(&hhrtim1,
                                  HRTIM_TIMERINDEX_TIMER_A,
                                  HRTIM_COMPAREUNIT_3,
                                  &compare_config);

  /* ---------------*/
  /* HRTIM start-up */
  /* ---------------*/
  /* Enable HRTIM's outputs TA1, TA2, TD1 and TD2 */
  /* Note: it is necessary to enable also GPIOs to have outputs functional */
  /* This must be done after HRTIM initialization */
  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1 + HRTIM_OUTPUT_TA2 
                                       + HRTIM_OUTPUT_TD1 + HRTIM_OUTPUT_TD2);

  /* Start HRTIM's TIMER D*/
  HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_A + HRTIM_TIMERID_TIMER_D);

}
#endif  /* MULTIPLE_PWM */

#ifdef PWM_MASTER
/**
* @brief  HRTIM configuration for generating a PWM with the master timer
* @param  None
* @retval None
*/
static void HRTIM_Config_MasterPWM(void)
{
  HRTIM_TimeBaseCfgTypeDef timebase_config;
  HRTIM_TimerCfgTypeDef timer_config;
  HRTIM_OutputCfgTypeDef output_config_TDx;
  HRTIM_CompareCfgTypeDef compare_config;

  /* ----------------------------------------------- */
  /* HRTIM Global initialization: Clock and DLL init */
  /* ----------------------------------------------- */
  /* Initialize the HRTIM structure (minimal configuration) */
  hhrtim1.Instance = HRTIM1;
  hhrtim1.Init.HRTIMInterruptResquests = HRTIM_IT_NONE;
  hhrtim1.Init.SyncOptions = HRTIM_SYNCOPTION_NONE;

  /* Initialize HRTIM */
  HAL_HRTIM_Init(&hhrtim1);

  /* HRTIM DLL calibration: periodic calibration, set period to 14µs */
  HAL_HRTIM_DLLCalibrationStart(&hhrtim1, HRTIM_CALIBRATIONRATE_3);

  /* Wait calibration completion*/
  if (HAL_HRTIM_PollForDLLCalibration(&hhrtim1, 100) != HAL_OK)
  {
    Error_Handler(); /* if DLL or clock is not correctly set */
  }
  

  /* ------------------------------------------------------------ */
  /* TIMERD initialization: set PWM frequency and continuous mode */
  /* ------------------------------------------------------------ */
  timebase_config.Period = _100KHz_PERIOD;
  timebase_config.RepetitionCounter = 0;
  timebase_config.PrescalerRatio = HRTIM_PRESCALERRATIO_MUL32;
  timebase_config.Mode = HRTIM_MODE_CONTINUOUS;
  HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &timebase_config);

  /* ------------------------------------------------------------------- */
  /* MASTER TIMER initialization: set PWM frequency and continuous mode  */
  /* The frequency difference is very low to show that the 2 signals are */
  /* not synchronous                                                     */
  /* Note: Timer D init struture is re-used                              */
  /* ------------------------------------------------------------------- */
  timebase_config.Period = _100KHz_Plus_PERIOD;
  HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_MASTER, &timebase_config);

  
  /* --------------------------------------------------------- */
  /* TIMERD global configuration: preload enabled on REP event */
  /* --------------------------------------------------------- */
  timer_config.DMARequests = HRTIM_TIM_DMA_NONE;
  timer_config.HalfModeEnable = HRTIM_HALFMODE_DISABLED;
  timer_config.StartOnSync = HRTIM_SYNCSTART_DISABLED;
  timer_config.ResetOnSync = HRTIM_SYNCRESET_DISABLED;
  timer_config.DACSynchro = HRTIM_DACSYNC_NONE;
  timer_config.PreloadEnable = HRTIM_PRELOAD_ENABLED;
  timer_config.UpdateGating = HRTIM_UPDATEGATING_INDEPENDENT;
  timer_config.BurstMode = HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;
  timer_config.RepetitionUpdate = HRTIM_UPDATEONREPETITION_ENABLED;
  timer_config.ResetUpdate = HRTIM_TIMUPDATEONRESET_DISABLED;
  timer_config.InterruptRequests = HRTIM_TIM_IT_NONE;
  timer_config.PushPull = HRTIM_TIMPUSHPULLMODE_DISABLED;
  timer_config.FaultEnable = HRTIM_TIMFAULTENABLE_NONE;
  timer_config.FaultLock = HRTIM_TIMFAULTLOCK_READWRITE;
  timer_config.DeadTimeInsertion = HRTIM_TIMDEADTIMEINSERTION_DISABLED;
  timer_config.DelayedProtectionMode = HRTIM_TIMER_A_B_C_DELAYEDPROTECTION_DISABLED;
  timer_config.UpdateTrigger= HRTIM_TIMUPDATETRIGGER_NONE;
  timer_config.ResetTrigger = HRTIM_TIMRESETTRIGGER_NONE;
  HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D,&timer_config);

  /* -------------------------------------------------- */
  /* MASTER TIMER global configuration: same as TIMER D */
  /* -------------------------------------------------- */
  HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_MASTER,&timer_config);
  
  /* ------------------------------------------------------ */
  /* TD1 waveform description: set on period, reset on CMP1 */
  /* ------------------------------------------------------ */
  output_config_TDx.Polarity = HRTIM_OUTPUTPOLARITY_HIGH;
  output_config_TDx.SetSource = HRTIM_OUTPUTSET_TIMPER;
  output_config_TDx.ResetSource  = HRTIM_OUTPUTRESET_TIMCMP1;
  output_config_TDx.IdleMode = HRTIM_OUTPUTIDLEMODE_NONE;
  output_config_TDx.IdleLevel = HRTIM_OUTPUTIDLELEVEL_INACTIVE;
  output_config_TDx.FaultLevel = HRTIM_OUTPUTFAULTLEVEL_NONE;
  output_config_TDx.ChopperModeEnable = HRTIM_OUTPUTCHOPPERMODE_DISABLED;
  output_config_TDx.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;
  HAL_HRTIM_WaveformOutputConfig(&hhrtim1,
                                 HRTIM_TIMERINDEX_TIMER_D,
                                 HRTIM_OUTPUT_TD1,
                                 &output_config_TDx);

  /* ------------------------------------------------------ */
  /* TD2 waveform description: set on CMP2, reset on period */
  /* Note: TDx init structure is re-used */
  /* ------------------------------------------------------ */
  output_config_TDx.SetSource = HRTIM_OUTPUTSET_MASTERPER;
  output_config_TDx.ResetSource  = HRTIM_OUTPUTSET_MASTERCMP1;
  HAL_HRTIM_WaveformOutputConfig(&hhrtim1,
                                 HRTIM_TIMERINDEX_TIMER_D,
                                 HRTIM_OUTPUT_TD2,
                                 &output_config_TDx);

  /* Set compare registers for duty cycle on TD1 in Timer D */
  compare_config.CompareValue = _100KHz_PERIOD/4;     /* 25% duty cycle */
  HAL_HRTIM_WaveformCompareConfig(&hhrtim1,
                                  HRTIM_TIMERINDEX_TIMER_D,
                                  HRTIM_COMPAREUNIT_1,
                                  &compare_config);

  /* Set compare registers for duty cycle on TD2 in Master timer */
  compare_config.CompareValue = _100KHz_Plus_PERIOD/2;     /* 50% duty cycle */
  HAL_HRTIM_WaveformCompareConfig(&hhrtim1,
                                  HRTIM_TIMERINDEX_MASTER,
                                  HRTIM_COMPAREUNIT_1,
                                  &compare_config);

  /* ---------------*/
  /* HRTIM start-up */
  /* ---------------*/
  /* Enable HRTIM's outputs TD1 and TD2 */
  /* Note: it is necessary to enable also GPIOs to have outputs functional */
  /* This must be done after HRTIM initialization */
  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 + HRTIM_OUTPUT_TD2);

  /* Start HRTIM's Master TIMER and TIMER D*/
  HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_MASTER + HRTIM_TIMERID_TIMER_D);

}
#endif /* PWM_MASTER */


#ifdef ARBITRARY_WAVEFORM
/**
* @brief  HRTIM configuration for generating an arbitrary waveform
* @param  None
* @retval None
*/
static void HRTIM_Config_ArbitraryWaveform(void)
{
  HRTIM_TimeBaseCfgTypeDef timebase_config;
  HRTIM_TimerCfgTypeDef timer_config;
  HRTIM_OutputCfgTypeDef output_config_TDx;
  HRTIM_CompareCfgTypeDef compare_config;

  /* ----------------------------------------------- */
  /* HRTIM Global initialization: Clock and DLL init */
  /* ----------------------------------------------- */
  /* Initialize the HRTIM structure (minimal configuration) */
  hhrtim1.Instance = HRTIM1;
  hhrtim1.Init.HRTIMInterruptResquests = HRTIM_IT_NONE;
  hhrtim1.Init.SyncOptions = HRTIM_SYNCOPTION_NONE;

  /* Initialize HRTIM */
  HAL_HRTIM_Init(&hhrtim1);

  /* HRTIM DLL calibration: periodic calibration, set period to 14µs */
  HAL_HRTIM_DLLCalibrationStart(&hhrtim1, HRTIM_CALIBRATIONRATE_3);

  /* Wait calibration completion*/
  if (HAL_HRTIM_PollForDLLCalibration(&hhrtim1, 100) != HAL_OK)
  {
    Error_Handler(); /* if DLL or clock is not correctly set */
  }
  

  /* ------------------------------------------------------------ */
  /* TIMERD initialization: set PWM frequency and continuous mode */
  /* ------------------------------------------------------------ */
  timebase_config.Period = _100KHz_PERIOD;
  timebase_config.RepetitionCounter = 0;
  timebase_config.PrescalerRatio = HRTIM_PRESCALERRATIO_MUL32;
  timebase_config.Mode = HRTIM_MODE_CONTINUOUS;
  HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &timebase_config);
 
  /* --------------------------------------------------------- */
  /* TIMERD global configuration: preload enabled on REP event */
  /* --------------------------------------------------------- */
  timer_config.DMARequests = HRTIM_TIM_DMA_NONE;
  timer_config.HalfModeEnable = HRTIM_HALFMODE_DISABLED;
  timer_config.StartOnSync = HRTIM_SYNCSTART_DISABLED;
  timer_config.ResetOnSync = HRTIM_SYNCRESET_DISABLED;
  timer_config.DACSynchro = HRTIM_DACSYNC_NONE;
  timer_config.PreloadEnable = HRTIM_PRELOAD_ENABLED;
  timer_config.UpdateGating = HRTIM_UPDATEGATING_INDEPENDENT;
  timer_config.BurstMode = HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;
  timer_config.RepetitionUpdate = HRTIM_UPDATEONREPETITION_ENABLED;
  timer_config.ResetUpdate = HRTIM_TIMUPDATEONRESET_DISABLED;
  timer_config.InterruptRequests = HRTIM_TIM_IT_NONE;
  timer_config.PushPull = HRTIM_TIMPUSHPULLMODE_DISABLED;
  timer_config.FaultEnable = HRTIM_TIMFAULTENABLE_NONE;
  timer_config.FaultLock = HRTIM_TIMFAULTLOCK_READWRITE;
  timer_config.DeadTimeInsertion = HRTIM_TIMDEADTIMEINSERTION_DISABLED;
  timer_config.DelayedProtectionMode = HRTIM_TIMER_A_B_C_DELAYEDPROTECTION_DISABLED;
  timer_config.UpdateTrigger= HRTIM_TIMUPDATETRIGGER_NONE;
  timer_config.ResetTrigger = HRTIM_TIMRESETTRIGGER_NONE;
  HAL_HRTIM_WaveformTimerConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D,&timer_config);
 
  /* --------------------------------------------------------------------- */
  /* TD1 waveform description: toggles on TIMD period, CMP1 and CMP2 event */
  /* --------------------------------------------------------------------- */
  output_config_TDx.Polarity = HRTIM_OUTPUTPOLARITY_HIGH;
  output_config_TDx.SetSource = HRTIM_OUTPUTSET_TIMPER + HRTIM_OUTPUTSET_TIMCMP1 + HRTIM_OUTPUTSET_TIMCMP2;
  output_config_TDx.ResetSource  = HRTIM_OUTPUTRESET_TIMPER + HRTIM_OUTPUTRESET_TIMCMP1 + HRTIM_OUTPUTRESET_TIMCMP2;
  output_config_TDx.IdleMode = HRTIM_OUTPUTIDLEMODE_NONE;
  output_config_TDx.IdleLevel = HRTIM_OUTPUTIDLELEVEL_INACTIVE;
  output_config_TDx.FaultLevel = HRTIM_OUTPUTFAULTLEVEL_NONE;
  output_config_TDx.ChopperModeEnable = HRTIM_OUTPUTCHOPPERMODE_DISABLED;
  output_config_TDx.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;
  HAL_HRTIM_WaveformOutputConfig(&hhrtim1,
                                 HRTIM_TIMERINDEX_TIMER_D,
                                 HRTIM_OUTPUT_TD1,
                                 &output_config_TDx);

  /* ------------------------------------------------------ */
  /* TD2 waveform description: set on TIMD PER and CMP2 and */
  /* reset on TIMD CMP1 and CMP3 event                      */
  /* Note: TDx init structure is re-used                    */
  /* ------------------------------------------------------ */
  output_config_TDx.SetSource = HRTIM_OUTPUTSET_TIMPER + HRTIM_OUTPUTSET_TIMCMP2;
  output_config_TDx.ResetSource  = HRTIM_OUTPUTRESET_TIMCMP1 + HRTIM_OUTPUTRESET_TIMCMP3;
  HAL_HRTIM_WaveformOutputConfig(&hhrtim1,
                                 HRTIM_TIMERINDEX_TIMER_D,
                                 HRTIM_OUTPUT_TD2,
                                 &output_config_TDx);

  /* Set compare register 1 in Timer D */
  compare_config.CompareValue = _100KHz_PERIOD/4;     /* 25% of the period */
  HAL_HRTIM_WaveformCompareConfig(&hhrtim1,
                                  HRTIM_TIMERINDEX_TIMER_D,
                                  HRTIM_COMPAREUNIT_1,
                                  &compare_config);

  /* Set compare register 2 in Timer D */
  compare_config.AutoDelayedMode = HRTIM_AUTODELAYEDMODE_REGULAR;
  compare_config.AutoDelayedTimeout = 0;
  compare_config.CompareValue = (3*_100KHz_PERIOD)/8;     /* 37.5% of the period */
  HAL_HRTIM_WaveformCompareConfig(&hhrtim1,
                                  HRTIM_TIMERINDEX_TIMER_D,
                                  HRTIM_COMPAREUNIT_2,
                                  &compare_config);

    /* Set compare register 3 in Timer D */
  compare_config.CompareValue = _100KHz_PERIOD/2;     /* 50% of the period */
  HAL_HRTIM_WaveformCompareConfig(&hhrtim1,
                                  HRTIM_TIMERINDEX_TIMER_D,
                                  HRTIM_COMPAREUNIT_3,
                                  &compare_config);

  /* ---------------*/
  /* HRTIM start-up */
  /* ---------------*/
  /* Enable HRTIM's outputs TD1 and TD2 */
  /* Note: it is necessary to enable also GPIOs to have outputs functional */
  /* This must be done after HRTIM initialization */
  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 + HRTIM_OUTPUT_TD2);

  /* Start HRTIM's TIMER D */
  HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);

}
#endif /* ARBITRARY_WAVEFORM */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
