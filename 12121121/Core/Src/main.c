/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <OLED.h>
#include <stdio.h>


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

/* USER CODE BEGIN PV */

uint8_t  state = 1,state_1 = 0;//1表运行，0表暂停
uint8_t  direction = 0,direction_1 = 1;//0表正转，1表反转
uint32_t time = 0;
uint32_t  relative_time = 0;
uint8_t  LED = 0,LED_1 = 0;
uint8_t Button_state_last = 1;
uint8_t Button_state_now = 1;
uint16_t time_interval = 500,time_interval_1 = 0;
uint8_t UART_Back = 0;
uint8_t RXdata[6];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart3)//六位数据包：1、包头AA 2、状态RUN 3、方向DIR 4、时间间隔低字节 5、时间间隔高字节 6、校验位
{
  if (RXdata[0] == 0xAA)
  {
    uint8_t check = 0;
    for (int i = 0; i < 5; i++)
    {
      check += RXdata[i];
    }
    if (RXdata[5] == check)
    {
      state = RXdata[1];
      direction = RXdata[2];
      time_interval = ((uint16_t)RXdata[4] << 8) | RXdata[3];
    }
    else
      goto Here_Error;
    /*启用一次主循环的串口状态返回*/
      UART_Back = 1;
  }

  else
  {
    Here_Error:;
    HAL_UART_Transmit_DMA(huart3, "ERROR", 5);
  }


  /*重启UART接收*/
  HAL_UART_Receive_IT(huart3, RXdata, 6);
}


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
  MX_DMA_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  /*OLED初始化&检查*/
  OLED_Init();
  OLED_Full();
  HAL_Delay(100);
  OLED_Clear();

  /*OLED显示格式初始化*/
  OLED_ShowString(1,1,"STATE:");
  OLED_ShowString(2,3,"DIR:");
  OLED_ShowString(3,2,"STEP:    ms");
  OLED_ShowString(4,3,"LED:");
  HAL_UART_Receive_IT(&huart3,RXdata, 6);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    relative_time = HAL_GetTick() - time;
    if (state != 0)
    {
      if (direction == 0)
      {
       if (relative_time >= time_interval)
       {
         time = HAL_GetTick();
         LED ++;
       }
       if (LED > 5 )
         LED = 1;
      }

      if (direction == 1)
      {
        if (relative_time >= time_interval)
        {
          time = HAL_GetTick();
          LED --;
        }
        if (LED < 1 )
        {
          LED = 5;
        }
      }

    }



   switch (LED)
   {
   case 1:HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,SET);
     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,RESET);
     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,RESET);
     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,RESET);
     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,RESET);
     break;
   case 2:HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,RESET);
     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,SET);
     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,RESET);
     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,RESET);
     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,RESET);
     break;
   case 3:HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,RESET);
     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,RESET);
     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,SET);
     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,RESET);
     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,RESET);
     break;
   case 4:HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,RESET);
     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,RESET);
     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,RESET);
     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,SET);
     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,RESET);
     break;
   case 5:HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,RESET);
     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,RESET);
     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,RESET);
     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,RESET);
     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,SET);
     break;
   }


    //OLED数据显示刷新
    if (LED != LED_1)
    {
      OLED_ShowNum(4,7,LED,1);
      LED_1 = LED;
    }

    if (state != state_1)
    {
      if (state == 0)
        OLED_ShowString(1,7,"Stop");
      else
        OLED_ShowString(1,7,"Run ");
      state_1 = state;
    }

    if (direction != direction_1)
    {
      if (direction == 0)
        OLED_ShowString(2,8,"Forward ");
      else
        OLED_ShowString(2,8,"Backward");
      direction_1 = direction;
    }

    if (time_interval != time_interval_1)
    {
      OLED_ShowNum(3,7,time_interval,4);
      time_interval_1 = time_interval;
    }

    /*串口返回流水灯状态*/
    if (UART_Back)
    {
      static char tx[64];
      UART_Back = 0;

      int n = snprintf(tx, sizeof(tx), "\nSTATE:%s,DIR:%s,STEP:%dms",
                       (state == 0) ? "Stop" : "Run",
                       (direction == 0) ? "Forward" : "Backward",
                       time_interval);

      HAL_UART_Transmit_DMA(&huart3, (uint8_t *)tx, n);
    }


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
