#ifndef __BSP_SENSOR_DRIVER_H
#define __BSP_SENSOR_DRIVER_H

#include <stdint.h>

void BSP_Sensor_Init(void);
void BSP_Sensor_Read(float *temp, uint16_t *light, uint16_t *smoke, uint16_t *flame);
uint8_t BSP_Sensor_GetDoorStatus(void);

#endif