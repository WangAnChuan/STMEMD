/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "communication.h"
#include <string.h>
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
/* USER CODE BEGIN Variables */
typedef struct {
    uint8_t fan_speed;
    uint8_t light_status;
    uint8_t alarm_status;
    uint8_t system_mode;
} control_status_t;

control_status_t control_status = {50, 1, 0, 0}; 
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId Task_SensorHandle;
osThreadId Task_ControlHandle;
osThreadId Task_TouchUIHandle;
osThreadId Task_ComHandle;
osThreadId Task_LoggerHandle;
osMessageQId xEnvDataQueueHandle;
osMessageQId xControlCmdQueueHandle;
osMessageQId xLogQueueHandle;
osMessageQId xUartRxQueueHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);
void StartTask04(void const * argument);
void StartTask05(void const * argument);
void StartTask06(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of xEnvDataQueue */
  osMessageQDef(xEnvDataQueue, 10, 20);
  xEnvDataQueueHandle = osMessageCreate(osMessageQ(xEnvDataQueue), NULL);

  /* definition and creation of xControlCmdQueue */
  osMessageQDef(xControlCmdQueue, 10, 8);
  xControlCmdQueueHandle = osMessageCreate(osMessageQ(xControlCmdQueue), NULL);

  /* definition and creation of xLogQueue */
  osMessageQDef(xLogQueue, 20, 16);
  xLogQueueHandle = osMessageCreate(osMessageQ(xLogQueue), NULL);

  /* definition and creation of xUartRxQueue */
  osMessageQDef(xUartRxQueue, 64, 1);
  xUartRxQueueHandle = osMessageCreate(osMessageQ(xUartRxQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of Task_Sensor */
  osThreadDef(Task_Sensor, StartTask02, osPriorityAboveNormal, 0, 512);
  Task_SensorHandle = osThreadCreate(osThread(Task_Sensor), NULL);

  /* definition and creation of Task_Control */
  osThreadDef(Task_Control, StartTask03, osPriorityHigh, 0, 512);
  Task_ControlHandle = osThreadCreate(osThread(Task_Control), NULL);

  /* definition and creation of Task_TouchUI */
  osThreadDef(Task_TouchUI, StartTask04, osPriorityNormal, 0, 1024);
  Task_TouchUIHandle = osThreadCreate(osThread(Task_TouchUI), NULL);

  /* definition and creation of Task_Com */
  osThreadDef(Task_Com, StartTask05, osPriorityNormal, 0, 512);
  Task_ComHandle = osThreadCreate(osThread(Task_Com), NULL);

  /* definition and creation of Task_Logger */
  osThreadDef(Task_Logger, StartTask06, osPriorityLow, 0, 512);
  Task_LoggerHandle = osThreadCreate(osThread(Task_Logger), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the Task_Sensor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the Task_Control thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the Task_TouchUI thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void const * argument)
{
  /* USER CODE BEGIN StartTask04 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask04 */
}

/* USER CODE BEGIN Header_StartTask05 */
/**
* @brief Function implementing the Task_Com thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask05 */
void StartTask05(void const * argument)
{
  /* USER CODE BEGIN StartTask05 */
  /* Infinite loop */
  data_frame_t data_frame;
  env_data_t env_data;
  uint32_t last_wake_time = osKernelSysTick();
  
  // ??????
  data_frame.header[0] = 0xAA;
  data_frame.header[1] = 0x55;
  data_frame.device_id = 0x01;
  data_frame.frame_type = 0x01;
  data_frame.tail[0] = 0x0D;
  data_frame.tail[1] = 0x0A;
  
  for(;;)
  {
    osDelayUntil(&last_wake_time, 500);
    
    osEvent event = osMessageGet(xEnvDataQueueHandle, 0);
    if (event.status == osEventMessage) {
        memcpy(&env_data, (env_data_t*)event.value.p, sizeof(env_data_t));

        data_frame.timestamp = osKernelSysTick();
        data_frame.temperature = env_data.temperature;
        data_frame.light_intensity = env_data.light_intensity;
        data_frame.smoke_value = env_data.smoke_value;
        data_frame.flame_value = env_data.flame_value;
        data_frame.door_status = env_data.door_status;
        data_frame.fan_speed = control_status.fan_speed;
        data_frame.light_status = control_status.light_status;
        data_frame.alarm_status = control_status.alarm_status;
        data_frame.system_mode = control_status.system_mode;

        data_frame.crc16 = calculate_crc16((uint8_t*)&data_frame, sizeof(data_frame_t) - 4);

        uart_send_data((uint8_t*)&data_frame, sizeof(data_frame_t));
    } else {
        data_frame.timestamp = osKernelSysTick();
        data_frame.temperature = 25.5f;
        data_frame.light_intensity = 300.0f;
        data_frame.smoke_value = 50.0f;
        data_frame.flame_value = 0.0f;
        data_frame.door_status = 0;
        data_frame.fan_speed = control_status.fan_speed;
        data_frame.light_status = control_status.light_status;
        data_frame.alarm_status = control_status.alarm_status;
        data_frame.system_mode = control_status.system_mode;
        data_frame.crc16 = calculate_crc16((uint8_t*)&data_frame, sizeof(data_frame_t) - 4);
        uart_send_data((uint8_t*)&data_frame, sizeof(data_frame_t));
    }

    process_received_commands();
  }
  /* USER CODE END StartTask05 */
}

/* USER CODE BEGIN Header_StartTask06 */
/**
* @brief Function implementing the Task_Logger thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask06 */
void StartTask06(void const * argument)
{
  /* USER CODE BEGIN StartTask06 */
  /* Infinite loop */
  uint32_t log_data;
  for(;;)
  {
    log_data = osKernelSysTick();
    osMessagePut(xLogQueueHandle, log_data, 0);
    osDelay(10000);
  }
  /* USER CODE END StartTask06 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
