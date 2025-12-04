#include "sys_core.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// --- 1. 全局队列句柄定义 ---
QueueHandle_t Queue_SensorData = NULL;
QueueHandle_t Queue_ControlCmd = NULL;

// --- 2. 外部任务函数声明 (由其他成员实现) ---
extern void Task_Sensor(void *argument);   // 成员A
extern void Task_Control(void *argument);  // 成员B
extern void Task_HMI(void *argument);      // 成员C
extern void Task_Com(void *argument);      // 成员D (你自己)

// --- 3. 任务句柄 ---
TaskHandle_t xHandle_Sensor;
TaskHandle_t xHandle_Control;
TaskHandle_t xHandle_HMI;
TaskHandle_t xHandle_Com;

// --- 4. 系统资源初始化函数 ---
void RTOS_Structure_Init(void)
{
    // A. 创建队列
    // 存储环境数据，深度5，每个单元为 EnvData_t 大小
    Queue_SensorData = xQueueCreate(5, sizeof(EnvData_t));
    
    // 存储控制指令，深度10，每个单元为 ControlCmd_t 大小
    Queue_ControlCmd = xQueueCreate(10, sizeof(ControlCmd_t));

    // B. 创建任务 (优先级：控制 > 采集 > 通信 > 界面)
    xTaskCreate(Task_Control, "Control", 128 * 4, NULL, osPriorityNormal + 2, &xHandle_Control);
    xTaskCreate(Task_Sensor,  "Sensor",  128 * 4, NULL, osPriorityNormal + 1, &xHandle_Sensor);
    xTaskCreate(Task_Com,     "Com",     128 * 4, NULL, osPriorityNormal,     &xHandle_Com);
    xTaskCreate(Task_HMI,     "HMI",     128 * 4, NULL, osPriorityBelowNormal,&xHandle_HMI);
}