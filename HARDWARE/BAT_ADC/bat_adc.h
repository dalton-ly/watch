#ifndef _HANDLER_H_
#define _HANDLER_H_

#include "sys.h"

#define BAT_ADC_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
#define BAT_ADC_GPIO (GPIOB)
#define BAT_ADC_PIN (GPIO_PIN_0)

void Bat_ADC_Init(void);
uint16_t Bat_GetValue(void);


#endif
