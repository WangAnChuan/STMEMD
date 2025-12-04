#include "main.h"
#include "tim.h"  // 引用 CubeMX 生成的 TIM 头文件
#include "gpio.h" // 引用 CubeMX 生成的 GPIO 头文件

// --- 硬件初始化 ---
// 在 main.c 的 USER CODE BEGIN 2 中调用
void BSP_Actuator_Init(void) {
    // 启动风扇的 PWM 通道 (TIM12 Channel 1)
    // 注意：需要在 CubeMX 中配置 TIM12 CH1 为 PWM Generation
    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);
    
    // 初始化执行器状态 (全关)
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET); // Relay
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET); // Buzzer
}

// --- 风扇控制 (PWM) ---
// speed: 0-100
void Actuator_SetFanSpeed(uint8_t speed) {
    if(speed > 100) speed = 100;
    // 假设 ARR = 999 (0-1000 对应 0-100% 占空比)
    // 成员B的代码里写的是 speed * 10
    __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, speed * 10);
}

// --- 继电器/灯光控制 ---
// state: 0-OFF, 1-ON
void Actuator_SetLight(uint8_t state) {
    // 成员B使用的是 PC0
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// --- 蜂鸣器控制 ---
// mode: 0-OFF, 1-ON (这里简化处理，去掉了复杂的阻塞式叫声，避免卡死RTOS任务)
void Actuator_SetBuzzer(uint8_t state) {
    // 成员B使用的是 PC1
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// --- LED 状态灯 ---
// mask: 位掩码控制 LED1-LED4 (PD0-PD3)
void Actuator_SetLEDs(uint8_t mask) {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, (mask & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, (mask & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, (mask & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, (mask & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}