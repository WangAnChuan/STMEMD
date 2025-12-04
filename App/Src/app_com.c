#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "data_def.h"
#include "bsp_uart_fifo.h" // 引用下面的BSP驱动

// 引用全局队列
extern QueueHandle_t Queue_SensorData;
extern QueueHandle_t Queue_ControlCmd;

// 简单的帧头定义 (示例: 0xAA 0x55)
#define FRAME_HEAD_1 0xAA
#define FRAME_HEAD_2 0x55

/**
 * @brief  简单的校验和计算
 */
static uint8_t CalcChecksum(uint8_t *data, uint16_t len) {
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len; i++) sum += data[i];
    return sum;
}

/**
 * @brief  发送环境数据到上位机 (上传)
 * 协议格式: [AA 55] [Type] [Len] [Data...] [Sum]
 */
static void Upload_EnvData(EnvData_t *data) {
    uint8_t send_buf[32];
    uint8_t idx = 0;

    send_buf[idx++] = FRAME_HEAD_1;
    send_buf[idx++] = FRAME_HEAD_2;
    send_buf[idx++] = 0x01; // 0x01 表示环境数据包
    send_buf[idx++] = sizeof(EnvData_t); // 数据长度

    // 简单序列化结构体 (注意字节序，这里假设PC端能处理)
    uint8_t *pStruct = (uint8_t *)data;
    for(int i=0; i<sizeof(EnvData_t); i++) {
        send_buf[idx++] = pStruct[i];
    }

    send_buf[idx] = CalcChecksum(send_buf, idx); // 计算校验和
    idx++;

    // 调用BSP发送
    BSP_UART_Send(send_buf, idx);
}

/**
 * @brief  解析上位机指令 (下载)
 * 假设上位机发来: [CmdType] [Param4] [Param3] [Param2] [Param1]
 * 共 5 字节简单指令
 */
static void Process_PC_Command(void) {
    uint8_t rx_byte;
    static uint8_t cmd_buf[10];
    static uint8_t cmd_idx = 0;
    
    // 循环读取串口 FIFO 直到为空
    while (BSP_UART_GetByte(&rx_byte)) {
        // 这里实现一个极简的状态机或缓冲区解析
        // 为演示简单，假设每次固定接收 5 个字节作为一条指令
        cmd_buf[cmd_idx++] = rx_byte;

        if (cmd_idx >= 5) {
            ControlCmd_t new_cmd;
            new_cmd.type = (ControlCmdType)cmd_buf[0];
            // 简单的参数组装
            new_cmd.parameter = (cmd_buf[1]<<24) | (cmd_buf[2]<<16) | (cmd_buf[3]<<8) | cmd_buf[4];
            
            // 发送到控制队列
            xQueueSend(Queue_ControlCmd, &new_cmd, 0);
            
            cmd_idx = 0; // 重置
        }
    }
}

/**
 * @brief  成员D的主任务函数
 */
void Task_Com(void *argument) {
    EnvData_t env_data_buffer;

    while (1) {
        // 1. 处理接收 (查询方式，也可改为信号量通知)
        Process_PC_Command();

        // 2. 处理发送 (等待队列数据，超时时间 50ms)
        // 如果 50ms 内没收到传感器数据，就继续循环，不至于卡死
        if (xQueueReceive(Queue_SensorData, &env_data_buffer, pdMS_TO_TICKS(50)) == pdTRUE) {
            Upload_EnvData(&env_data_buffer);
        }
    }
}