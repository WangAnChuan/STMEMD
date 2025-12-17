#include "main.h"
#include "cmsis_os2.h"
#include "tim.h"
#include "gpio.h"

/* Private variables ---------------------------------------------------------*/
// ????
osMessageQueueId_t EnvData_QueueHandle;
osMessageQueueId_t Cmd_QueueHandle;

// ????
osThreadId_t Task_EnvCollectHandle;
osThreadId_t Task_ControlHandle;
osThreadId_t Task_CmdParseHandle;
osThreadId_t Task_StateDisplayHandle;

// ?????
static EnvData EnvData_QueueBuffer[ENV_DATA_QUEUE_SIZE];
static uint8_t Cmd_QueueBuffer[CMD_QUEUE_SIZE];

// ????
const osMessageQueueAttr_t EnvData_QueueAttr = {
    .name = "EnvDataQueue",
    .mq_mem = EnvData_QueueBuffer,
    .mq_size = sizeof(EnvData_QueueBuffer)
};

const osMessageQueueAttr_t Cmd_QueueAttr = {
    .name = "CmdQueue",
    .mq_mem = Cmd_QueueBuffer,
    .mq_size = sizeof(Cmd_QueueBuffer)
};

// ??????(??????)
ControlState g_ControlState = {0, 0, 0, MODE_AUTO, 0.0f, 0, 0, 0, 0};

/* Private function prototypes -----------------------------------------------*/
// ??????
void StartTask_EnvCollect(void *argument);
void StartTask_Control(void *argument);
void StartTask_CmdParse(void *argument);
void StartTask_StateDisplay(void *argument);

// ???????
void Motor_PWM_Control(uint8_t speed);
void Relay_Control(uint8_t state);
void Buzzer_Control(uint8_t mode);
void LED_State_Update(SysState state);

// ?????+????+????
EnvData EnvData_Collect(void);
uint8_t Cmd_Parse(uint8_t raw_cmd);
void State_Display(ControlState state);

/**
  * @brief  FreeRTOS???:?????????
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  // ???OS??
  osKernelInitialize();

  // 1. ??????
  EnvData_QueueHandle = osMessageQueueNew(ENV_DATA_QUEUE_SIZE, sizeof(EnvData), &EnvData_QueueAttr);
  Cmd_QueueHandle = osMessageQueueNew(CMD_QUEUE_SIZE, sizeof(uint8_t), &Cmd_QueueAttr);

  // 2. ????(????????)
  // ????(?????)
  const osThreadAttr_t Task_ControlAttr = {
    .name = "Task_Control",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityHigh,
  };
  Task_ControlHandle = osThreadNew(StartTask_Control, NULL, &Task_ControlAttr);

  // ??????(????)
  const osThreadAttr_t Task_CmdParseAttr = {
    .name = "Task_CmdParse",
    .stack_size = 256 * 4,
    .priority = (osPriority_t) osPriorityNormal,
  };
  Task_CmdParseHandle = osThreadNew(StartTask_CmdParse, NULL, &Task_CmdParseAttr);

  // ??????(?????)
  const osThreadAttr_t Task_StateDisplayAttr = {
    .name = "Task_StateDisplay",
    .stack_size = 256 * 4,
    .priority = (osPriority_t) osPriorityBelowNormal,
  };
  Task_StateDisplayHandle = osThreadNew(StartTask_StateDisplay, NULL, &Task_StateDisplayAttr);

  // ??????(?????)
  const osThreadAttr_t Task_EnvCollectAttr = {
    .name = "Task_EnvCollect",
    .stack_size = 256 * 4,
    .priority = (osPriority_t) osPriorityLow,
  };
  Task_EnvCollectHandle = osThreadNew(StartTask_EnvCollect, NULL, &Task_EnvCollectAttr);

  // ?????
  osKernelStart();

  // ???????
  while (1) {
    osDelay(1);
  }
}

/**
  * @brief  ??1:????(??A)
  * @param  argument: ???
  * @retval None
  */
void StartTask_EnvCollect(void *argument)
{
  EnvData env_data = {0};
  for(;;)
  {
    // 1. ???????(???????????)
    env_data = EnvData_Collect();

    // 2. ??????????(???????)
    osMessageQueuePut(EnvData_QueueHandle, &env_data, 0, osWaitForever);

    // 3. ???????(???????)
    g_ControlState.Temperature = env_data.Temperature;
    g_ControlState.Light = env_data.Light;
    g_ControlState.Smoke = env_data.Smoke;
    g_ControlState.Flame = env_data.Flame;
    g_ControlState.DoorState = env_data.DoorState;

    // ????:200ms
    osDelay(200);
  }
}

/**
  * @brief  ??2:????(??B)
  * @param  argument: ???
  * @retval None
  */
void StartTask_Control(void *argument)
{
  EnvData recv_env_data = {0};
  uint8_t recv_cmd = 0;
  SysState current_state = SYS_NORMAL;
  osStatus_t status;

  // ??????
  Motor_PWM_Control(0);
  Relay_Control(0);
  Buzzer_Control(0);
  LED_State_Update(current_state);

  for(;;)
  {
    // 1. ??????(????)
    status = osMessageQueueGet(EnvData_QueueHandle, &recv_env_data, NULL, osWaitForever);
    if(status != osOK) continue;

    // 2. ??????(10ms??,???)
    status = osMessageQueueGet(Cmd_QueueHandle, &recv_cmd, NULL, 10);
    if(status == osOK)
    {
      // ????(??C/D?????)
      switch(recv_cmd)
      {
        case CMD_MODE_AUTO: g_ControlState.CurrentMode = MODE_AUTO; break;
        case CMD_MODE_MANUAL: g_ControlState.CurrentMode = MODE_MANUAL; break;
        case CMD_MODE_ENERGY: g_ControlState.CurrentMode = MODE_ENERGY; break;
        case CMD_LIGHT_ON: Relay_Control(1); break;
        case CMD_LIGHT_OFF: Relay_Control(0); break;
        case CMD_FAN_30: Motor_PWM_Control(30); break;
        case CMD_FAN_60: Motor_PWM_Control(60); break;
        case CMD_FAN_100: Motor_PWM_Control(100); break;
        case CMD_BUZZER_OFF: Buzzer_Control(0); break;
        case CMD_RESET: g_ControlState = (ControlState){0,0,0,MODE_AUTO,0.0f,0,0,0,0}; break;
        case CMD_ALARM_OFF: current_state = SYS_NORMAL; Buzzer_Control(0); break;
        default: break;
      }
    }

    // 3. ????????
    if(g_ControlState.CurrentMode == MODE_AUTO)
    {
      // ??????
      if(recv_env_data.Flame > FLAME_ALARM_TH) {
        current_state = SYS_ALARM;
      } else if(recv_env_data.Temperature > TEMP_ALARM_MAX || recv_env_data.Smoke > SMOKE_ALARM_TH) {
        current_state = SYS_ALARM;
      } else if(recv_env_data.Temperature > TEMP_NORMAL_MAX || recv_env_data.Smoke > SMOKE_WARNING_TH || recv_env_data.DoorState == 1) {
        current_state = SYS_WARNING;
      } else {
        current_state = SYS_NORMAL;
      }

      // ?????
      switch(current_state)
      {
        case SYS_NORMAL:
          Motor_PWM_Control(recv_env_data.Temperature > 25.0f ? 30 : 0);
          Relay_Control(recv_env_data.Light < LIGHT_DARK_TH ? 1 : 0);
          Buzzer_Control(0);
          break;
        case SYS_WARNING:
          Motor_PWM_Control(60);
          Relay_Control(1);
          Buzzer_Control(2);
          break;
        case SYS_ALARM:
          Motor_PWM_Control(100);
          Relay_Control(1);
          Buzzer_Control(3);
          break;
      }
    }
    // ????
    else if(g_ControlState.CurrentMode == MODE_ENERGY)
    {
      Motor_PWM_Control(recv_env_data.Temperature > 28.0f ? 20 : 0);
      Relay_Control(recv_env_data.Light < 150 ? 1 : 0);
      Buzzer_Control(0);
      current_state = SYS_NORMAL;
    }

    // ??????
    g_ControlState.AlarmState = current_state;
    LED_State_Update(current_state);

    osDelay(100);
  }
}

/**
  * @brief  ??3:????(??C/D)
  * @param  argument: ???
  * @retval None
  */
void StartTask_CmdParse(void *argument)
{
  uint8_t raw_cmd = 0;
  uint8_t parsed_cmd = 0;
  for(;;)
  {
    // 1. ??????(???????/??????)
    // ??:??????(????????HAL_UART_Receive/????)
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET) {
      raw_cmd = 0x01; // ????"????"??
    } else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_SET) {
      raw_cmd = 0x02; // ????"????"??
    } else {
      raw_cmd = 0x00;
    }

    // 2. ????
    if(raw_cmd != 0x00) {
      parsed_cmd = Cmd_Parse(raw_cmd);
      // ???????(???????)
      osMessageQueuePut(Cmd_QueueHandle, &parsed_cmd, 0, 10);
    }

    // ??????:50ms
    osDelay(50);
  }
}

/**
  * @brief  ??4:????(??D)
  * @param  argument: ???
  * @retval None
  */
void StartTask_StateDisplay(void *argument)
{
  for(;;)
  {
    // 1. ?????????(?????LCD/??????)
    State_Display(g_ControlState);

    // ??????:500ms
    osDelay(500);
  }
}

/* ?????????(?????????) */
// ??????(??A)
EnvData EnvData_Collect(void)
{
  EnvData data = {0};
  // ??:???????(????????ADC??/?????)
  static float temp = 25.0f;
  static uint16_t light = 300;
  static uint16_t smoke = 200;
  static uint16_t flame = 100;
  static uint8_t door = 0;

  // ??????
  temp += 0.1f;
  if(temp > 36.0f) temp = 25.0f;
  light -= 5;
  if(light < 100) light = 400;

  data.Temperature = temp;
  data.Light = light;
  data.Smoke = smoke;
  data.Flame = flame;
  data.DoorState = door;

  return data;
}

// ????(??C/D)
uint8_t Cmd_Parse(uint8_t raw_cmd)
{
  // ??:???????????
  switch(raw_cmd)
  {
    case 0x01: return CMD_MODE_AUTO;
    case 0x02: return CMD_MODE_MANUAL;
    case 0x03: return CMD_MODE_ENERGY;
    case 0x04: return CMD_LIGHT_ON;
    case 0x05: return CMD_LIGHT_OFF;
    default: return 0x00;
  }
}

// ????(??D)
void State_Display(ControlState state)
{
  // ??:??????(????????LCD??)
  // HAL_UART_Transmit(&huart1, (uint8_t*)"Temp: %.1f\r\n", sprintf(buf, "Temp: %.1f\r\n", state.Temperature), 100, 10);
  // ??????,???????????
}

// ??PWM??(??B)
void Motor_PWM_Control(uint8_t speed)
{
  if(speed > 100) speed = 100;
  // ??:??TIM12 CH1???
  __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, speed * 10);
  g_ControlState.FanSpeed = speed;
}

// ?????(??B)
void Relay_Control(uint8_t state)
{
  HAL_GPIO_WritePin(RELAY_CONTROL_GPIO_Port, RELAY_CONTROL_Pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
  g_ControlState.LightState = state;
}

// ?????(??B)
void Buzzer_Control(uint8_t mode)
{
  switch(mode)
  {
    case 0: HAL_GPIO_WritePin(BUZZER_CONTROL_GPIO_Port, BUZZER_CONTROL_Pin, GPIO_PIN_RESET); break;
    case 1: HAL_GPIO_WritePin(BUZZER_CONTROL_GPIO_Port, BUZZER_CONTROL_Pin, GPIO_PIN_SET); HAL_Delay(100); HAL_GPIO_WritePin(BUZZER_CONTROL_GPIO_Port, BUZZER_CONTROL_Pin, GPIO_PIN_RESET); break;
    case 2: for(uint8_t i=0; i<3; i++) { HAL_GPIO_WritePin(BUZZER_CONTROL_GPIO_Port, BUZZER_CONTROL_Pin, GPIO_PIN_SET); HAL_Delay(200); HAL_GPIO_WritePin(BUZZER_CONTROL_GPIO_Port, BUZZER_CONTROL_Pin, GPIO_PIN_RESET); HAL_Delay(300); } break;
    case 3: HAL_GPIO_WritePin(BUZZER_CONTROL_GPIO_Port, BUZZER_CONTROL_Pin, GPIO_PIN_SET); break;
    default: break;
  }
}

// LED????(??B/D)
void LED_State_Update(SysState state)
{
  switch(state)
  {
    case SYS_NORMAL:
      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
      break;
    case SYS_WARNING:
      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
      break;
    case SYS_ALARM:
      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
      break;
  }

  // ????LED4
  static uint32_t last_tick = 0;
  if(g_ControlState.CurrentMode == MODE_AUTO) {
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
  } else if(g_ControlState.CurrentMode == MODE_ENERGY) {
    if(HAL_GetTick() - last_tick > 500) {
      HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
      last_tick = HAL_GetTick();
    }
  } else {
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);
  }
}
