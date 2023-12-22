/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/RTC/RTC_ProgrammingTheWakeUpTimer/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32G4xx RTC LL API to configure
  *          Time and Date.
  *          Peripheral initialization done using LL unitary services functions.
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Constant used to convert in ASCII format */
#define ASCII_CONVERT              ((uint32_t)48)

/* Defines related to Clock configuration */
/* Uncomment to enable the adequate Clock Source */
#define RTC_CLOCK_SOURCE_LSI
/*#define RTC_CLOCK_SOURCE_LSE*/

#ifdef RTC_CLOCK_SOURCE_LSI
/* ck_apre=LSIFreq/(ASYNC prediv + 1) with LSIFreq=37 kHz RC */
#define RTC_ASYNCH_PREDIV          ((uint32_t)0x7F)
/* ck_spre=ck_apre/(SYNC prediv + 1) = 1 Hz */
#define RTC_SYNCH_PREDIV           ((uint32_t)0x122)
#endif

#ifdef RTC_CLOCK_SOURCE_LSE
/* ck_apre=LSEFreq/(ASYNC prediv + 1) = 256Hz with LSEFreq=32768Hz */
#define RTC_ASYNCH_PREDIV          ((uint32_t)0x7F)
/* ck_spre=ck_apre/(SYNC prediv + 1) = 1 Hz */
#define RTC_SYNCH_PREDIV           ((uint32_t)0x00FF)
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint16_t error = 0;  //initialized at 0 and modified by the functions

uint8_t bSend = 0;
uint8_t aStringToSend[20] = "\n00 : 00 : 00 ";
uint8_t RTC_InitializationMode = 0;
uint8_t CharToReceive = 0;
uint8_t CharReceived = 0;
uint8_t Alarm = 1; /* set to 1 for the first print */

#if (USE_TIMEOUT == 1)
uint32_t Timeout = 0; /* Variable used for Timeout management */
#endif /* USE_TIMEOUT */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void     Process(void);
void     Set_RTC_Time(uint32_t Hour, uint32_t Minute, uint32_t Second);
uint32_t Enter_RTC_InitMode(void);
void     LED_Blinking(uint32_t Period);
uint32_t Exit_RTC_InitMode(void);
uint32_t WaitForSynchro_RTC(void);

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
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /** Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
  */
  LL_PWR_DisableUCPDDeadBattery();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  
  /* Enable RXNE and Error interrupts */
  LL_USART_EnableIT_RXNE(USART1);
  /* Enable TC interrupt */
  LL_USART_EnableIT_TC(USART1);
  LL_USART_EnableIT_ERROR(USART1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    Process();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4)
  {
  }
  LL_PWR_EnableRange1BoostMode();
  LL_RCC_HSI_Enable();
   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  LL_RCC_HSI_SetCalibTrimming(64);
  LL_RCC_LSI_Enable();
   /* Wait till LSI is ready */
  while(LL_RCC_LSI_IsReady() != 1)
  {
  }

  LL_PWR_EnableBkUpAccess();
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_4, 85, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_EnableDomain_SYS();
  LL_RCC_PLL_Enable();
   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  /* Insure 1us transition state at intermediate medium speed clock*/
  for (__IO uint32_t i = (170 >> 1); i !=0; i--);

  /* Set AHB prescaler*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  LL_Init1msTick(170000000);

  LL_SetSystemCoreClock(170000000);
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */
  if(LL_RTC_BKP_GetRegister(RTC,LL_RTC_BKP_DR0) != 0x32F2)
  {
  /* USER CODE END RTC_Init 0 */

  LL_RTC_InitTypeDef RTC_InitStruct = {0};
  LL_RTC_TimeTypeDef RTC_TimeStruct = {0};
  LL_RTC_DateTypeDef RTC_DateStruct = {0};

  if(LL_RCC_GetRTCClockSource() != LL_RCC_RTC_CLKSOURCE_LSI)
  {
    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);
  }

  /* Peripheral clock enable */
  LL_RCC_EnableRTC();
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_RTCAPB);

  /* RTC interrupt Init */
  NVIC_SetPriority(RTC_WKUP_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(RTC_WKUP_IRQn);

  /* USER CODE BEGIN RTC_Init 1 */

  /* Configure exti for RTC IT */
  LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_20);
  LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_20);

  /* USER CODE END RTC_Init 1 */
  RTC_InitStruct.HourFormat = LL_RTC_HOURFORMAT_24HOUR;
  RTC_InitStruct.AsynchPrescaler = RTC_ASYNCH_PREDIV;
  RTC_InitStruct.SynchPrescaler = RTC_SYNCH_PREDIV;
  LL_RTC_Init(RTC, &RTC_InitStruct);

  /** Initialize RTC and set the Time and Date
  */
  if(LL_RTC_BKP_GetRegister(RTC,LL_RTC_BKP_DR0) != 0x32F2){

  RTC_TimeStruct.Hours = 0x0;
  RTC_TimeStruct.Minutes = 0x0;
  RTC_TimeStruct.Seconds = 0x0;

  LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BCD, &RTC_TimeStruct);
  RTC_DateStruct.WeekDay = LL_RTC_WEEKDAY_MONDAY;
  RTC_DateStruct.Month = LL_RTC_MONTH_MARCH;
  RTC_DateStruct.Day = 0x1;
  RTC_DateStruct.Year = 0x19;

  LL_RTC_DATE_Init(RTC, LL_RTC_FORMAT_BCD, &RTC_DateStruct);
    LL_RTC_BKP_SetRegister(RTC,LL_RTC_BKP_DR0,0x32F2);
  }

  /** Enable the WakeUp
  */
  LL_RTC_EnableIT_WUT(RTC);
  LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);
  LL_RTC_WAKEUP_SetAutoReload(RTC, 0);
  /* USER CODE BEGIN RTC_Init 2 */
  /* Disable RTC registers write protection */
  LL_RTC_DisableWriteProtection(RTC);

  /** Enable the WakeUp
  */
  LL_RTC_WAKEUP_Enable(RTC);
  LL_RTC_EnableIT_WUT(RTC);
  LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);
  LL_RTC_WAKEUP_SetAutoReload(RTC, 0);

  /* Enable the write protection for RTC registers */
  LL_RTC_EnableWriteProtection(RTC);

  }
  else
  {
    /* Reset Wake up flag */
    LL_RTC_ClearFlag_WUT(RTC);
    /* clear exti line 20 flag */
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_20);

    /* RTC interrupt Init */
    NVIC_SetPriority(RTC_WKUP_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
    NVIC_EnableIRQ(RTC_WKUP_IRQn);
    /* Configure exti and nvic for RTC IT */
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_20);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_20);
  }

  LL_USART_ClearFlag_TC(USART1);

  
  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  /**USART1 GPIO Configuration
  PA9   ------> USART1_TX
  PA10   ------> USART1_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USART1 interrupt Init */
  NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(USART1_IRQn);

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART1, &USART_InitStruct);
  LL_USART_SetTXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_SetRXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_DisableFIFO(USART1);
  LL_USART_DisableOverrunDetect(USART1);
  LL_USART_DisableDMADeactOnRxErr(USART1);
  LL_USART_ConfigAsyncMode(USART1);

  /* USER CODE BEGIN WKUPType USART1 */

  /* USER CODE END WKUPType USART1 */

  LL_USART_Enable(USART1);

  /* Polling USART1 initialisation */
  while((!(LL_USART_IsActiveFlag_TEACK(USART1))) || (!(LL_USART_IsActiveFlag_REACK(USART1))))
  {
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

  /**/
  LL_GPIO_ResetOutputPin(LED2_GPIO_Port, LED2_Pin);

  /**/
  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE13);

  /**/
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_13;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
  LL_EXTI_Init(&EXTI_InitStruct);

  /**/
  LL_GPIO_SetPinPull(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin, LL_GPIO_PULL_NO);

  /**/
  LL_GPIO_SetPinMode(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin, LL_GPIO_MODE_INPUT);

  /**/
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * Brief   This function processes RTC with USART.
  * Param   None
  * Retval  None
  */
void Process(void)
{
  register uint32_t time = 0;
  static uint32_t new_hour = 0, new_minute = 0, new_second = 0;
  __IO uint32_t temp_read = 0;

  LL_mDelay(1000);

  switch (RTC_InitializationMode)
  {
    case 0:
      /* check alarm and synchronisation flag */
      if (Alarm && (LL_RTC_IsActiveFlag_RS(RTC) == 1))
      {
        Alarm = 0;

        /* get time */
        time = LL_RTC_TIME_Get(RTC);
        /* need to read date also to unlock TR register */
        temp_read = READ_REG(RTC->DR);
        ((void)(temp_read));  /* To avoid warning */

        aStringToSend[1] = (uint8_t)((__LL_RTC_GET_HOUR(time) >> 4) + ASCII_CONVERT);/* hour tens */
        aStringToSend[2] = (uint8_t)((__LL_RTC_GET_HOUR(time) & 0x0F) + ASCII_CONVERT);/* hour units */
        aStringToSend[6] = (uint8_t)((__LL_RTC_GET_MINUTE(time) >> 4) + ASCII_CONVERT);/* minute tens */
        aStringToSend[7] = (uint8_t)((__LL_RTC_GET_MINUTE(time) & 0x0F) + ASCII_CONVERT);/* minute units */
        aStringToSend[11] = (uint8_t)((__LL_RTC_GET_SECOND(time) >> 4) + ASCII_CONVERT);/* second tens */
        aStringToSend[12] = (uint8_t)((__LL_RTC_GET_SECOND(time) & 0x0F) + ASCII_CONVERT);/* second units */

        /* start USART transmission */
        LL_USART_TransmitData8(USART1, aStringToSend[bSend++]); /* Will initiate TC if TXE */
      }
      break;
    case 1:
    {
      if (!bSend)
      {
        strcpy((char *)aStringToSend, "\nSend hour tens\n");
        LL_USART_TransmitData8(USART1, aStringToSend[bSend++]); /* Will initiate TC if TXE */
        RTC_InitializationMode = 2;
      }
    }
    break;
    case 2: /* Hour tens */
    {
      if (CharReceived)
      {
        CharReceived = 0;
        CharToReceive -= ASCII_CONVERT;
        if (CharToReceive < 3)
        {
          new_hour = CharToReceive << 4;
          RTC_InitializationMode = 3;
        }
        else
        {
          RTC_InitializationMode = 1;
        }
      }
    }
    break;
    case 3:
    {
      if (!bSend)
      {
        strcpy((char *)aStringToSend, "\nSend hour units\n");
        LL_USART_TransmitData8(USART1, aStringToSend[bSend++]); /* Will initiate TC if TXE */
        RTC_InitializationMode = 4;
      }
    }
    break;
    case 4: /* Hour units */
    {
      if (CharReceived)
      {
        CharReceived = 0;
        CharToReceive -= ASCII_CONVERT;
        if ((((new_hour >> 4) == 2) && (CharToReceive < 4)) ||
            (((new_hour >> 4) < 2) && (CharToReceive < 10)))
        {
          new_hour |= CharToReceive;
          RTC_InitializationMode = 5;
        }
        else
        {
          RTC_InitializationMode = 3;
        }
      }
    }
    break;
    case 5:
    {
      if (!bSend)
      {
        strcpy((char *)aStringToSend, "\nSend minute tens\n");
        LL_USART_TransmitData8(USART1, aStringToSend[bSend++]); /* Will initiate TC if TXE */
        RTC_InitializationMode = 6;
      }
    }
    break;
    case 6: /* Minute tens */
    {
      if (CharReceived)
      {
        CharReceived = 0;
        CharToReceive -= ASCII_CONVERT;
        if (CharToReceive < 6)
        {
          new_minute = CharToReceive << 4;
          RTC_InitializationMode = 7;
        }
        else
        {
          RTC_InitializationMode = 5;
        }
      }
    }
    break;
    case 7:
    {
      if (!bSend)
      {
        strcpy((char *)aStringToSend, "\nSend minute units\n");
        LL_USART_TransmitData8(USART1, aStringToSend[bSend++]); /* Will initiate TC if TXE */
        RTC_InitializationMode = 8;
      }
    }
    break;
    case 8: /* Minute units */
    {
      if (CharReceived)
      {
        CharReceived = 0;
        CharToReceive -= ASCII_CONVERT;
        if (CharToReceive < 10)
        {
          new_minute |= CharToReceive;
          RTC_InitializationMode = 9;
        }
        else
        {
          RTC_InitializationMode = 7;
        }
      }
    }
    break;
    case 9:
    {
      if (!bSend)
      {
        strcpy((char *)aStringToSend, "\nSend second tens\n");
        LL_USART_TransmitData8(USART1, aStringToSend[bSend++]); /* Will initiate TC if TXE */
        RTC_InitializationMode = 10;
      }
    }
    break;
    case 10: /* Second tens */
    {
      if (CharReceived)
      {
        CharReceived = 0;
        CharToReceive -= ASCII_CONVERT;
        if (CharToReceive < 6)
        {
          new_second = CharToReceive << 4;
          RTC_InitializationMode = 11;
        }
        else
        {
          RTC_InitializationMode = 9;
        }
      }
    }
    break;
    case 11:
    {
      if (!bSend)
      {
        strcpy((char *)aStringToSend, "\nSend second units\n");
        LL_USART_TransmitData8(USART1, aStringToSend[bSend++]); /* Will initiate TC if TXE */
        RTC_InitializationMode = 12;
      }
    }
    break;
    case 12: /* Second Units */
    {
      if (CharReceived)
      {
        CharReceived = 0;
        CharToReceive -= ASCII_CONVERT;
        if (CharToReceive < 10)
        {
          new_second |= CharToReceive;
          RTC_InitializationMode = 13;
        }
        else
        {
          RTC_InitializationMode = 11;
        }
      }
    }
    break;
    case 13:
    {
      Set_RTC_Time(new_hour, new_minute, new_second);
      strcpy((char *)aStringToSend, "\n-- : -- : --      ");
      RTC_InitializationMode = 0;
      Alarm = 1;
    }
    break;
  }
}

/**
  * Brief   This function sets the TIME in RTC.
  * Param   Hour   New Hour to set
  * Param   Minute New Minute to set
  * Param   Second New Second to set
  * Retval  None
  */
void Set_RTC_Time(uint32_t Hour, uint32_t Minute, uint32_t Second)
{
  /* Disable RTC registers write protection */
  LL_RTC_DisableWriteProtection(RTC);

  /* Enter in initialization mode */
  if (Enter_RTC_InitMode() != RTC_ERROR_NONE)
  {
    /* Initialization Error */
    LED_Blinking(LED_BLINK_FAST);
  }

  /* New time in TR */
  LL_RTC_TIME_Config(RTC,
                     LL_RTC_TIME_FORMAT_PM,
                     Hour,
                     Minute,
                     Second);

  /* Exit of initialization mode */
  if (Exit_RTC_InitMode() != RTC_ERROR_NONE)
  {
    /* Initialization Error */
    LED_Blinking(LED_BLINK_FAST);
  }

  /* Enable RTC registers write protection */
  LL_RTC_EnableWriteProtection(RTC);
}


/**
  * @brief  Enter in initialization mode
  * @note In this mode, the calendar counter is stopped and its value can be updated
  * @param  None
  * @retval RTC_ERROR_NONE if no error
  */
uint32_t Enter_RTC_InitMode(void)
{
  /* Set Initialization mode */
  LL_RTC_EnableInitMode(RTC);

#if (USE_TIMEOUT == 1)
  Timeout = RTC_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */

  /* Check if the Initialization mode is set */
  while (LL_RTC_IsActiveFlag_INIT(RTC) != 1)
  {
#if (USE_TIMEOUT == 1)
    if (LL_SYSTICK_IsActiveCounterFlag())
    {
      Timeout --;
    }
    if (Timeout == 0)
    {
      return RTC_ERROR_TIMEOUT;
    }
#endif /* USE_TIMEOUT */
  }

  return RTC_ERROR_NONE;
}

/**
  * @brief  Set LED2 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_FAST : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t Period)
{
  /* Toggle IO in an infinite loop */
  while (1)
  {
    LL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    LL_mDelay(Period);
  }
}

/**
  * @brief  Exit Initialization mode
  * @param  None
  * @retval RTC_ERROR_NONE if no error
  */
uint32_t Exit_RTC_InitMode(void)
{
  LL_RTC_DisableInitMode(RTC);

  /* Wait for synchro */
  /* Note: Needed only if Shadow registers is enabled           */
  /*       LL_RTC_IsShadowRegBypassEnabled function can be used */
  return (WaitForSynchro_RTC());
}

/**
  * @brief  Wait until the RTC Time and Date registers (RTC_TR and RTC_DR) are
  *         synchronized with RTC APB clock.
  * @param  None
  * @retval RTC_ERROR_NONE if no error (RTC_ERROR_TIMEOUT will occur if RTC is
  *         not synchronized)
  */
uint32_t WaitForSynchro_RTC(void)
{
  /* Clear RSF flag */
  LL_RTC_ClearFlag_RS(RTC);

#if (USE_TIMEOUT == 1)
  Timeout = RTC_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */

  /* Wait the registers to be synchronised */
  while (LL_RTC_IsActiveFlag_RS(RTC) != 1)
  {
#if (USE_TIMEOUT == 1)
    if (LL_SYSTICK_IsActiveCounterFlag())
    {
      Timeout --;
    }
    if (Timeout == 0)
    {
      return RTC_ERROR_TIMEOUT;
    }
#endif /* USE_TIMEOUT */
  }
  return RTC_ERROR_NONE;
}


/**
  * @brief  Function to manage IRQ Handler
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  RTC_InitializationMode = 1;
}

/**
  * @brief  Function called from USART IRQ Handler when RXNE flag is set
  *         Function is in charge of reading character received on USART RX line.
  * @param  None
  * @retval None
  */
void USART_CharReception_Callback(void)
{
  CharToReceive = LL_USART_ReceiveData8(USART1); /* Receive data, clear flag */
  CharReceived = 1;
}

/**
  * @brief  Function called for achieving next TX Byte bSending
  * @param  None
  * @retval None
  */
void USART_CharTransmitComplete_Callback(void)
{
  if (bSend == sizeof(aStringToSend))
  {
    bSend = 0;
  }
  else
  {
    /* clear transfer complete flag and fill TDR with a new char */
    LL_USART_TransmitData8(USART1, aStringToSend[bSend++]);
  }
}

/**
  * @brief  Function handle the RTC wakeup interruption.
  * @param  None
  * @retval None
  */
void RTC_Wakup_Treatement(void)
{
  /* Toggle Green LED */
  LL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
  Alarm = 1;
}
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

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
