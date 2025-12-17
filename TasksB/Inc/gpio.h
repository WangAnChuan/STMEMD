#ifndef __GPIO_H
#define __GPIO_H

#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
// ???????(?????????:PC0)
#define RELAY_CONTROL_Pin GPIO_PIN_0
#define RELAY_CONTROL_GPIO_Port GPIOC
#define RELAY_CONTROL_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()

// ???????(?????????:PC1)
#define BUZZER_CONTROL_Pin GPIO_PIN_1
#define BUZZER_CONTROL_GPIO_Port GPIOC
#define BUZZER_CONTROL_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()

// LED??(?????????LED:PD0-PD3??D1-D4)
#define LED_D1_Pin GPIO_PIN_0
#define LED_D1_GPIO_Port GPIOD
#define LED_D2_Pin GPIO_PIN_1
#define LED_D2_GPIO_Port GPIOD
#define LED_D3_Pin GPIO_PIN_2
#define LED_D3_GPIO_Port GPIOD
#define LED_D4_Pin GPIO_PIN_3
#define LED_D4_GPIO_Port GPIOD

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

#endif /* __GPIO_H */
