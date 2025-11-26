#include "gateway_button.h"

void button_toggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
}

void button_write(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
	HAL_GPIO_WritePin(GPIOx, GPIO_Pin, PinState);
}

uint8_t button_read(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	uint8_t status = 0;
	if(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == 1) {
		status = 1;
		return status;
	}
	return 0;
}
