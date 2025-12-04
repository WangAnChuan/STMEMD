#ifndef __BSP_ACTUATOR_DRIVER_H
#define __BSP_ACTUATOR_DRIVER_H

#include <stdint.h>

void BSP_Actuator_Init(void);
void Actuator_SetFanSpeed(uint8_t speed);
void Actuator_SetLight(uint8_t state);
void Actuator_SetBuzzer(uint8_t state);
void Actuator_SetLEDs(uint8_t mask);

#endif