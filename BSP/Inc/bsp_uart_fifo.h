#ifndef __BSP_UART_FIFO_H
#define __BSP_UART_FIFO_H

#include <stdint.h>

void BSP_UART_Init(void);
void BSP_UART_Send(uint8_t *data, uint16_t len);
uint8_t BSP_UART_GetByte(uint8_t *byte);

#endif