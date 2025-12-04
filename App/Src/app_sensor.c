#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "data_def.h"      // 公共数据结构
#include "bsp_sensor_driver.h" // 刚刚写的驱动

// 引用全局队列 (sys_core.c)
extern QueueHandle_t Queue_SensorData;

void Task_Sensor(void *argument) {
    EnvData_t env_data;
    
    // 模拟变量 (因为成员A代码里缺少 LM75 温度驱动，我们保留部分模拟功能)
    static float sim_temp = 25.0f;

    while (1) {
        // 1. 读取硬件数据 (从 DMA 缓冲区获取)
        // 注意：temp 这里我们没传指针，因为用模拟值替代
        BSP_Sensor_Read(NULL, &env_data.light_val, &env_data.smoke_val, &env_data.flame_val);
        
        // 2. 补充缺失数据 (模拟温度)
        sim_temp += 0.5f; 
        if (sim_temp > 35.0f) sim_temp = 20.0f;
        env_data.temperature = sim_temp;

        // 3. 读取门状态
        env_data.door_status = BSP_Sensor_GetDoorStatus();
        
        // 4. 填充其他保留位
        env_data.gas_val = 0; // 暂时没有气敏传感器

        // 5. 发送到队列 (等待时间 10ms)
        // 发送给 控制任务(B) 和 通信任务(D)
        xQueueSend(Queue_SensorData, &env_data, 10);

        // 6. 采样周期 500ms
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}