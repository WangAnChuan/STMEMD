#include "usart.h" // CubeMX生成的头文件
#include "stdint.h"

// 接收缓冲区定义
#define RX_BUFFER_SIZE 128
static uint8_t rx_fifo[RX_BUFFER_SIZE];
static volatile uint16_t rx_head = 0;
static volatile uint16_t rx_tail = 0;
static uint8_t rx_temp_byte; // 单字节接收缓存

// --- 初始化 (在 main.c 中调用) ---
void BSP_UART_Init(void) {
    // 开启中断接收，每次接收 1 字节
    HAL_UART_Receive_IT(&huart1, &rx_temp_byte, 1);
}

// --- 发送函数 ---
void BSP_UART_Send(uint8_t *data, uint16_t len) {
    HAL_UART_Transmit(&huart1, data, len, 100);
}

// --- 读取字节 (从 FIFO 中取出一个字节) ---
// 返回 1 表示读取成功，0 表示缓冲区空
uint8_t BSP_UART_GetByte(uint8_t *byte) {
    if (rx_head == rx_tail) {
        return 0; // 空
    }
    *byte = rx_fifo[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUFFER_SIZE;
    return 1;
}

// --- HAL库中断回调重写 ---
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        // 将数据存入 FIFO
        uint16_t next_head = (rx_head + 1) % RX_BUFFER_SIZE;
        if (next_head != rx_tail) { // 防止溢出
            rx_fifo[rx_head] = rx_temp_byte;
            rx_head = next_head;
        }
        // 重新开启中断
        HAL_UART_Receive_IT(&huart1, &rx_temp_byte, 1);
    }
}