#include "communication.h"
#include "string.h"
#include "freertos.h"
#include <stdio.h>

extern osMessageQId xEnvDataQueueHandle;
extern osMessageQId xControlCmdQueueHandle;
extern osMessageQId xUartRxQueueHandle;
extern osMessageQId xLogQueueHandle;

extern UART_HandleTypeDef huart1;

uint16_t calculate_crc16(const uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    return crc;
}

void uart_send_data(uint8_t *data, uint16_t length) {
    HAL_UART_Transmit(&huart1, data, length, HAL_MAX_DELAY);
}


void process_received_commands(void) {
    static uint8_t rx_buffer[64];
    static uint8_t rx_index = 0;
    osEvent event;

    while ((event = osMessageGet(xUartRxQueueHandle, 0)).status == osEventMessage) {
        uint8_t received_char = (uint8_t)event.value.v;
        rx_buffer[rx_index++] = received_char;
        
        if (rx_index >= 2 && rx_buffer[rx_index-2] == 0x0D && 
            rx_buffer[rx_index-1] == 0x0A) {
            
            if (rx_index >= 8 && rx_buffer[0] == 0xBB && rx_buffer[1] == 0x66) {
                control_frame_t *frame = (control_frame_t*)rx_buffer;
                
                uint16_t calculated_crc = calculate_crc16(rx_buffer, 6);
                if (calculated_crc == frame->crc16) {
                    control_cmd_t cmd;
                    cmd.cmd_type = frame->cmd_type;
                    cmd.cmd_data = frame->cmd_data;
                    osMessagePut(xControlCmdQueueHandle, (uint32_t)&cmd, 0);
                }
            }
            rx_index = 0;
        }
        
        if (rx_index >= sizeof(rx_buffer)) {
            rx_index = 0;
        }
    }
}
