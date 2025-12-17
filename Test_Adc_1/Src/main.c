/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdlib.h"
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
__IO uint16_t adcx[4]={0};
float temp0,temp1,temp2,temp3;

/* 模拟传感器数据 */
float simulated_temperature = 25.0f;
float simulated_light = 500.0f;
float simulated_smoke = 0.0f;
float simulated_flame = 0.0f;
uint8_t simulated_door_status = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* 辅助函数声明 */
void SimulateSensorData(EnvData_t *env_data);
void PrintEnvData(const EnvData_t *env_data);

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
  MX_DMA_Init();
  MX_ADC3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_ADC_Start_DMA(&hadc3,(uint32_t*)adcx,4);//启动ADC转换
  
  /* 初始化环境数据 */
  EnvData_t env_data;
  
  printf("\r\n=== Environmental Monitoring System Started ===\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* 环境采集任务 - 模拟Task_Sensor功能 */
    SimulateSensorData(&env_data);
    
    /* 控制任务 - 模拟Task_Control功能 */
    printf("\r\n[Control Task] Processing environmental data...\r\n");
    PrintEnvData(&env_data);
    
    /* 界面任务 - 模拟Task_UI功能 */
    printf("[UI Task] Updating display...\r\n");
    
    /* 日志任务 - 模拟Task_Log功能 */
    printf("[Log Task] Recording data...\r\n");
    
    /* 显示ADC原始数据 */
    temp0 = (float)adcx[0]*(3.3/4096);  // 将ADC值转换为电压值
    temp1 = (float)adcx[1]*(3.3/4096);  // 将ADC值转换为电压值
    temp2 = (float)adcx[2]*(3.3/4096);  // 将ADC值转换为电压值
    temp3 = (float)adcx[3]*(3.3/4096);  // 将ADC值转换为电压值
    printf("\r\nADC Data: Channel 1=%fV, Channel 2=%fV, Channel 3=%fV, Channel 4=%fV\r\n",temp0,temp1,temp2,temp3);
    
    printf("\r\n=== Waiting for next acquisition ===\r\n");
    HAL_Delay(2000);  // 延迟2秒
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (uint8_t) ch;      
	return ch;
}

/**
 * @brief 模拟传感器数据采集
 * @param env_data 指向EnvData结构体的指针
 */
void SimulateSensorData(EnvData_t *env_data)
{
    static uint32_t counter = 0;
    
    /* 模拟温度变化 (20-30度范围) - 使用简单的周期函数 */
    simulated_temperature = 25.0f + 5.0f * ((counter % 20) / 20.0f);
    
    /* 模拟光照变化 (白天/夜晚) - 使用简单的周期函数 */
    simulated_light = 300.0f + 200.0f * ((counter % 40) / 40.0f);
    
    /* 模拟烟雾浓度 (随机波动) */
    simulated_smoke = (float)(rand() % 100) / 10.0f;
    
    /* 模拟火焰强度 (随机波动) */
    simulated_flame = (float)(rand() % 50) / 10.0f;
    
    /* 模拟门状态 (每10次切换一次) */
    if (counter % 10 == 0) {
        simulated_door_status = !simulated_door_status;
    }
    
    /* 填充环境数据结构 */
    env_data->temperature = simulated_temperature;
    env_data->light_intensity = simulated_light;
    env_data->smoke_level = simulated_smoke;
    env_data->flame_level = simulated_flame;
    env_data->door_status = simulated_door_status;
    env_data->timestamp = HAL_GetTick();
    
    counter++;
}

/**
 * @brief 打印环境数据
 * @param env_data 指向EnvData结构体的指针
 */
void PrintEnvData(const EnvData_t *env_data)
{
    printf("=== Environmental Data ===\r\n");
    printf("Temperature: %.2f°C\r\n", env_data->temperature);
    printf("Light Intensity: %.2f lux\r\n", env_data->light_intensity);
    printf("Smoke Level: %.2f ppm\r\n", env_data->smoke_level);
    printf("Flame Level: %.2f\r\n", env_data->flame_level);
    printf("Door Status: %s\r\n", env_data->door_status ? "Open" : "Closed");
    printf("Timestamp: %lu ms\r\n", env_data->timestamp);
    printf("==========================\r\n");
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */