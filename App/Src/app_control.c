#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "data_def.h" // 必须包含公共数据定义
#include "sys_core.h"

// 引用外部定义的驱动函数 (也可以新建一个 .h 文件引用)
extern void Actuator_SetFanSpeed(uint8_t speed);
extern void Actuator_SetLight(uint8_t state);
extern void Actuator_SetBuzzer(uint8_t state);
extern void Actuator_SetLEDs(uint8_t mask);

// 引用全局队列 (来自 sys_core.c)
extern QueueHandle_t Queue_SensorData;
extern QueueHandle_t Queue_ControlCmd;

// 全局状态变量 (用于状态显示)
// 成员B原本叫 g_ControlState，这里为了简化直接定义静态变量或全局变量
static uint8_t current_mode = CMD_MODE_AUTO; // 默认为自动模式

// 阈值定义 (来自成员B的 main.h)
#define TEMP_ALARM_MAX 35.0f
#define SMOKE_ALARM_TH 800
#define FLAME_ALARM_TH 800

/**
 * @brief  控制任务 (成员B的核心逻辑)
 */
void Task_Control(void *argument) {
    EnvData_t env_data;        // 使用 data_def.h 中的结构体
    ControlCmd_t cmd_data;     // 使用 data_def.h 中的结构体
    
    // 初始状态
    Actuator_SetFanSpeed(0);
    Actuator_SetLight(0);
    Actuator_SetLEDs(0x01); // LED1 亮表示待机

    while (1) {
        // ------------------------------------------
        // 1. 处理控制指令 (优先级较高)
        // ------------------------------------------
        if (xQueueReceive(Queue_ControlCmd, &cmd_data, 0) == pdTRUE) {
            switch (cmd_data.type) {
                case CMD_MODE_AUTO:
                    current_mode = CMD_MODE_AUTO;
                    Actuator_SetLEDs(0x01); // 恢复自动模式指示
                    break;
                case CMD_MODE_MANUAL:
                    current_mode = CMD_MODE_MANUAL;
                    Actuator_SetLEDs(0x02); // 手动模式指示
                    break;
                case CMD_FAN_OPEN:
                    // 如果参数不为0，则设置具体速度，否则默认30
                    Actuator_SetFanSpeed(cmd_data.parameter > 0 ? cmd_data.parameter : 30);
                    break;
                case CMD_FAN_CLOSE:
                    Actuator_SetFanSpeed(0);
                    break;
                case CMD_ALARM_CLEAR:
                    Actuator_SetBuzzer(0);
                    // 这里可以添加清除报警标志位的逻辑
                    break;
                case CMD_LED_BLINK:
                    // 简单的交互反馈
                    Actuator_SetLEDs(0x0F);
                    vTaskDelay(100);
                    Actuator_SetLEDs(0x00);
                    break;
                // 注意：如果 data_def.h 里没有 CMD_LIGHT_ON，你需要去加上，或者暂时用预留指令
                default:
                    break;
            }
        }

        // ------------------------------------------
        // 2. 处理环境数据 & 自动控制逻辑
        // ------------------------------------------
        if (xQueueReceive(Queue_SensorData, &env_data, 100) == pdTRUE) {
            
            if (current_mode == CMD_MODE_AUTO) {
                // --- 报警逻辑 ---
                if (env_data.temperature > TEMP_ALARM_MAX || env_data.smoke_val > SMOKE_ALARM_TH || env_data.flame_val > FLAME_ALARM_TH) {
                    // 严重报警：全速风扇 + 蜂鸣器 + LED闪烁
                    Actuator_SetFanSpeed(100);
                    Actuator_SetBuzzer(1);
                    Actuator_SetLEDs(0x08); // LED4 报警
                }
                // --- 正常温控逻辑 ---
                else if (env_data.temperature > 30.0f) {
                    Actuator_SetFanSpeed(60);
                    Actuator_SetBuzzer(0);
                    Actuator_SetLEDs(0x04); // LED3 降温中
                }
                else {
                    // 环境正常
                    Actuator_SetFanSpeed(0);
                    Actuator_SetBuzzer(0);
                    Actuator_SetLEDs(0x01); // LED1 正常
                }
            }
        }
    }
}