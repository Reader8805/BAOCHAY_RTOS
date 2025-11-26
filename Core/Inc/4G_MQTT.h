///*
// * 4G_MQTT.h
// *
// *  Created on: Nov 3, 2025
// *      Author: DANG
// */
//
//#ifndef INC_4G_MQTT_H_
//#define INC_4G_MQTT_H_
//#include "stm32f4xx.h"
//#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>
//#define VIETTEL  "v-internet"
//#define VINAPHONE "m3-world"
//#define TB_HOST         "demo.thingsboard.io"
//#define TB_PORT         1883
//#define ACCESS_TOKEN    "FDxdurpOltHZcwzozd6e"     // <-- token CE của bạn
//
//extern char txBuf[256];
//extern char rxBuf[256];
//uint8_t waitForPrompt(UART_HandleTypeDef *uart_sim, char ch);
//void sendAT(UART_HandleTypeDef *uart_sim, const char *cmd);
//void waitForOK(UART_HandleTypeDef *uart_sim, uint32_t timeout);
//uint8_t sendATwait(UART_HandleTypeDef *uart_sim, const char *cmd, const char *expect, uint32_t timeout);
//void mqttPublish(UART_HandleTypeDef *uart_sim, const char *topic, const char *payload);
//void delayMs(uint32_t ms);
//void mqttt4GConfig(UART_HandleTypeDef *uart_sim, const char *apn, const char *accessToken);
//#endif /* INC_4G_MQTT_H_ */
//
//
#ifndef __4G_MQTT_H__
#define __4G_MQTT_H__

#include "stm32f4xx_hal.h" // sửa theo chip của bạn
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
extern UART_HandleTypeDef huart1; // DEBUG UART
extern UART_HandleTypeDef huart2; // SIM UART

// Cấu hình người dùng
#define TB_ACCESS_TOKEN "fye0jv1quop9nb7c7u9o"
#define APN_NAME        "v-internet"   // Viettel -> "v-internet"

// ===== Public APIs =====
void A7640_Init(void);
void A7640_Publish(int data, const char *c);

#endif /* __4G_MQTT_H__ */
