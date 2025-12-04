#ifndef __DATA_DEF_H
#define __DATA_DEF_H

#include <stdint.h>

/* =========================================
 * 1. 环境数据结构体 (成员A -> 成员B, C, D)
 * ========================================= */
typedef struct {
    float temperature;      // 温度 (LM75)
    uint16_t light_val;     // 光照 (ADC)
    uint16_t smoke_val;     // 烟雾 (ADC)
    uint16_t gas_val;       // 可燃气体 (ADC)
    uint16_t flame_val;     // 火焰 (ADC)
    uint8_t door_status;    // 门状态 (0:关, 1:开)
} EnvData_t;

/* =========================================
 * 2. 控制指令类型 (成员C, D -> 成员B)
 * ========================================= */
typedef enum {
    CMD_FAN_OPEN = 0x10,
    CMD_FAN_CLOSE,
    CMD_MODE_AUTO = 0x20,
    CMD_MODE_MANUAL,
    CMD_ALARM_CLEAR = 0x30,
    CMD_LED_BLINK = 0x40
} ControlCmdType;

/* =========================================
 * 3. 控制指令包 (队列传输单元)
 * ========================================= */
typedef struct {
    ControlCmdType type;    // 指令类型
    uint32_t parameter;     // 参数 (如风扇PWM值，若无则填0)
} ControlCmd_t;

#endif // __DATA_DEF_H