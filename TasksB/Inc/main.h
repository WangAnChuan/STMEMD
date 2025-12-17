#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
// ??????
typedef enum {
    SYS_NORMAL,    // ????
    SYS_WARNING,   // ????
    SYS_ALARM      // ????
} SysState;

// ??????
typedef enum {
    MODE_AUTO,     // ????
    MODE_MANUAL,   // ????
    MODE_ENERGY    // ????
} SysMode;

// ???????(????)
typedef struct {
    uint8_t FanSpeed;     // ????(0-100)
    uint8_t LightState;   // ????:0-?,1-?
    uint8_t AlarmState;   // ????:0-?,1-??,2-??
    SysMode CurrentMode;  // ??????
    // ??:??????(???????)
    float Temperature;
    uint16_t Light;
    uint16_t Smoke;
    uint16_t Flame;
    uint8_t DoorState;
} ControlState;

// ???????(??A??)
typedef struct {
    float Temperature;    // ??(?)
    uint16_t Light;       // ????(0-4095)
    uint16_t Smoke;       // ????(0-1023)
    uint16_t Flame;       // ?????(0-1023)
    uint8_t DoorState;    // ???:0-??,1-??
} EnvData;

// ????(??C/D??)
typedef enum {
    CMD_MODE_AUTO = 0x01,
    CMD_MODE_MANUAL = 0x02,
    CMD_MODE_ENERGY = 0x03,
    CMD_LIGHT_ON = 0x04,
    CMD_LIGHT_OFF = 0x05,
    CMD_FAN_30 = 0x06,
    CMD_FAN_60 = 0x07,
    CMD_FAN_100 = 0x08,
    CMD_BUZZER_OFF = 0x09,
    // ????(??C/D??)
    CMD_RESET = 0x0A,
    CMD_ALARM_OFF = 0x0B
} CmdType;

/* Exported constants --------------------------------------------------------*/
// ????
#define TEMP_NORMAL_MAX 30.0f
#define TEMP_ALARM_MAX 35.0f
#define SMOKE_WARNING_TH 500
#define SMOKE_ALARM_TH 800
#define FLAME_ALARM_TH 800
#define LIGHT_DARK_TH 200

// ????
#define RELAY_CONTROL_Pin GPIO_PIN_0
#define RELAY_CONTROL_GPIO_Port GPIOC
#define BUZZER_CONTROL_Pin GPIO_PIN_1
#define BUZZER_CONTROL_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOD
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOD
#define LED3_Pin GPIO_PIN_2
#define LED3_GPIO_Port GPIOD
#define LED4_Pin GPIO_PIN_3
#define LED4_GPIO_Port GPIOD

// ?????(??A??)
#define TEMP_ADC_Pin GPIO_PIN_0
#define TEMP_ADC_GPIO_Port GPIOA
#define LIGHT_ADC_Pin GPIO_PIN_1
#define LIGHT_ADC_GPIO_Port GPIOA
#define SMOKE_ADC_Pin GPIO_PIN_2
#define SMOKE_ADC_GPIO_Port GPIOA
#define FLAME_GPIO_Pin GPIO_PIN_3
#define FLAME_GPIO_Port GPIOA
#define DOOR_GPIO_Pin GPIO_PIN_4
#define DOOR_GPIO_Port GPIOA

// ????
#define ENV_DATA_QUEUE_SIZE 16
#define CMD_QUEUE_SIZE 16

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

// ???????
void Motor_PWM_Control(uint8_t speed);
void Relay_Control(uint8_t state);
void Buzzer_Control(uint8_t mode);
void LED_State_Update(SysState state);

// ???????(??A)
EnvData EnvData_Collect(void);

// ??????(??C/D)
uint8_t Cmd_Parse(uint8_t raw_cmd);

// ??????(??D)
void State_Display(ControlState state);

// FreeRTOS????
void StartTask_EnvCollect(void *argument);
void StartTask_Control(void *argument);
void StartTask_CmdParse(void *argument);
void StartTask_StateDisplay(void *argument);

// FreeRTOS?????
void MX_FREERTOS_Init(void);

// ??????
extern ControlState g_ControlState;
extern osMessageQueueId_t EnvData_QueueHandle;
extern osMessageQueueId_t Cmd_QueueHandle;

#endif /* __MAIN_H */
