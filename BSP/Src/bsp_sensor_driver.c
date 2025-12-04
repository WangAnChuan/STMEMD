#include "adc.h" // 引用 CubeMX 生成的 ADC 头文件
#include "dma.h"
#include "gpio.h"
#include "main.h" // 获取 HAL 库引用

// --- 1. 定义 DMA 缓冲区 ---
// 成员 A 原代码: __IO uint16_t adcx[4]={0};
volatile uint16_t adc_dma_buffer[4] = {0}; 

// --- 2. 硬件初始化 ---
void BSP_Sensor_Init(void) {
    // 启动 ADC3 的 DMA 传输，循环读取 4 个通道
    // 对应成员 A 代码: HAL_ADC_Start_DMA(&hadc3,(uint32_t*)adcx,4);
    if (HAL_ADC_Start_DMA(&hadc3, (uint32_t*)adc_dma_buffer, 4) != HAL_OK) {
        // 如果启动失败，可以在这里处理错误
    }
}

// --- 3. 获取传感器原始值 ---
// 返回电压值 (0.0 - 3.3V) 或 原始值 (0 - 4095)
// 这里我们为了通用，返回计算后的物理量模拟值
void BSP_Sensor_Read(float *temp, uint16_t *light, uint16_t *smoke, uint16_t *flame) {
    // 将 12位 ADC 值 (0-4096) 转换为电压或对应物理量
    
    // 通道 0: 假设是 光照 (关联 adcx[0])
    // 简单的转换逻辑：ADC值越大，光照越强 (仅作示例)
    *light = adc_dma_buffer[0]; 

    // 通道 1: 假设是 烟雾 (关联 adcx[1])
    *smoke = adc_dma_buffer[1];

    // 通道 2: 假设是 火焰 (关联 adcx[2])
    *flame = adc_dma_buffer[2];
    
    // 通道 3: 假设是 燃气/其他，这里暂时没用上，或者用于温度
    // 如果没有 LM75，我们可以用这个通道模拟温度
    // *temp = (float)adc_dma_buffer[3] * (3.3f / 4096.0f) * 100.0f; // 假设 10mV/C
}

// --- 4. 获取门状态 ---
uint8_t BSP_Sensor_GetDoorStatus(void) {
    // 假设门磁传感器接在 PA4 (根据之前的 main.h 定义)
    // 如果没有硬件，这里返回 0 (关)
    // return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4); 
    return 0; // 暂时默认关闭
}