/**
 * @file env_demo.c
 * @brief 环境数据采集演示程序
 * @author AI Assistant
 * @date 2024
 */

#include "main.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief 模拟传感器数据采集
 * @param env_data 指向EnvData结构体的指针
 */
void SimulateSensorData(EnvData_t *env_data)
{
    static uint32_t counter = 0;
    
    /* 模拟温度变化 (20-30度范围) - 使用简单的周期函数 */
    float temperature = 25.0f + 5.0f * ((counter % 20) / 20.0f);
    
    /* 模拟光照变化 (白天/夜晚) - 使用简单的周期函数 */
    float light_intensity = 300.0f + 200.0f * ((counter % 40) / 40.0f);
    
    /* 模拟烟雾浓度 (随机波动) */
    float smoke_level = (float)(rand() % 100) / 10.0f;
    
    /* 模拟火焰强度 (随机波动) */
    float flame_level = (float)(rand() % 50) / 10.0f;
    
    /* 模拟门状态 (每10次切换一次) */
    uint8_t door_status = (counter % 20 < 10) ? 0 : 1;
    
    /* 填充环境数据结构 */
    env_data->temperature = temperature;
    env_data->light_intensity = light_intensity;
    env_data->smoke_level = smoke_level;
    env_data->flame_level = flame_level;
    env_data->door_status = door_status;
    env_data->timestamp = counter * 1000; // 模拟时间戳
    
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

/**
 * @brief 环境数据采集演示
 */
void EnvDataDemo(void)
{
    EnvData_t env_data;
    
    printf("\r\n=== Environmental Data Acquisition Demo Started ===\r\n");
    
    for(int i = 0; i < 5; i++) {
        printf("\r\n--- Acquisition #%d ---\r\n", i + 1);
        
        /* 采集传感器数据 */
        SimulateSensorData(&env_data);
        
        /* 打印环境数据 */
        PrintEnvData(&env_data);
        
        /* 模拟任务间通信 */
        printf("\r\n[Task Simulation] Data distribution:\r\n");
        printf("- Sent to control task\r\n");
        printf("- Sent to UI task\r\n");
        printf("- Sent to log task\r\n");
        
        /* 模拟控制逻辑 */
        if (env_data.smoke_level > 5.0f) {
            printf("[Control Logic] Warning: Smoke level too high!\r\n");
        }
        if (env_data.temperature > 30.0f) {
            printf("[Control Logic] Warning: Temperature too high!\r\n");
        }
        if (env_data.door_status) {
            printf("[Control Logic] Info: Door is open\r\n");
        }
        
        HAL_Delay(1000); // 等待1秒
    }
    
    printf("\r\n=== Environmental Data Acquisition Demo Completed ===\r\n");
}

/**
 * @brief 环境监测系统主演示
 */
void EnvMonitoringDemo(void)
{
    printf("\r\n");
    printf("****************************************\r\n");
    printf("*      Environmental Monitoring System Demo       *\r\n");
    printf("****************************************\r\n");
    printf("\r\n");
    
    /* 演示EnvData结构体使用 */
    EnvDataDemo();
    
    printf("\r\n");
    printf("****************************************\r\n");
    printf("*              Demo Completed                  *\r\n");
    printf("****************************************\r\n");
}