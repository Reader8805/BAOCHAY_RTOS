

#ifndef INC_GATEWAY_BUTTON_H_
#define INC_GATEWAY_BUTTON_H_

#include "stm32f4xx.h"



void button_toggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint8_t button_read(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void button_write(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
#endif /* INC_GATEWAY_BUTTON_H_ */

