#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#include "main.h"
#include "cmsis_os.h"

#pragma pack(push, 1)

typedef struct {
    float temperature;
    float light_intensity;
    float smoke_value;
    float flame_value;
    uint8_t door_status;
    uint8_t padding[3];
} env_data_t;

typedef struct {
    uint8_t cmd_type;
    uint8_t cmd_data;
    uint8_t padding[2];
} control_cmd_t;

typedef struct {
    uint8_t header[2];
    uint8_t device_id;
    uint8_t frame_type;
    uint32_t timestamp;
    float temperature;
    float light_intensity;
    float smoke_value;
    float flame_value;
    uint8_t door_status;
    uint8_t fan_speed;
    uint8_t light_status;
    uint8_t alarm_status;
    uint8_t system_mode;
    uint16_t crc16;
    uint8_t tail[2];
} data_frame_t;

typedef struct {
    uint8_t header[2];
    uint8_t cmd_type;
    uint8_t cmd_data;
    uint16_t crc16;
    uint8_t tail[2];
} control_frame_t;

#pragma pack(pop)

void uart_send_data(uint8_t *data, uint16_t length);
uint16_t calculate_crc16(const uint8_t *data, uint16_t length);
void process_received_commands(void);
#endif
