#include "zigbee_e18.h"
#include <stdio.h>

static UART_HandleTypeDef *E18_UART_Handle = NULL;

static uint8_t E18_CalculateChecksum(uint8_t cmdType, uint8_t cmdCode, uint8_t* data, uint8_t dataLen) {
    uint8_t checksum = cmdType ^ cmdCode;
    for (int i = 0; i < dataLen; i++) {
        checksum ^= data[i];
    }
    return checksum;
}


static void E18_FlushUART(void) {
    uint8_t temp;

    while(HAL_UART_Receive(E18_UART_Handle, &temp, 1, 0) == HAL_OK);
}


static void E18_SendCommand(uint8_t cmdType, uint8_t cmdCode, uint8_t* data, uint8_t dataLen) {
    if (E18_UART_Handle == NULL) return;

    E18_FlushUART();

    uint8_t frameLen = 1 + 1 + dataLen + 1;
    uint8_t buffer[128];

    buffer[0] = 0x55;
    buffer[1] = frameLen;
    buffer[2] = cmdType;
    buffer[3] = cmdCode;

    if (dataLen > 0 && data != NULL) {
        memcpy(&buffer[4], data, dataLen);
    }

    buffer[4 + dataLen] = E18_CalculateChecksum(cmdType, cmdCode, data, dataLen);
    HAL_UART_Transmit(E18_UART_Handle, buffer, frameLen + 2, 100);
}


static bool E18_WaitHeaderOnly(uint32_t timeout) {
    uint32_t startTick = HAL_GetTick();
    uint8_t byte;

    while ((HAL_GetTick() - startTick) < timeout) {
        // Đọc từng byte
        if (HAL_UART_Receive(E18_UART_Handle, &byte, 1, 10) == HAL_OK) {

            if (byte == 0x55) {
                return true;
            }
        }
    }
    return false; // Timeout
}

void E18_Init_UART(UART_HandleTypeDef *huart_handle) {
    E18_UART_Handle = huart_handle;
}

void E18_SwitchToHexMode(void) {
    if (E18_UART_Handle == NULL) return;
    HAL_Delay(1200);
    HAL_UART_Transmit(E18_UART_Handle, (uint8_t*)"+++", 3, 100);
    HAL_Delay(1000);

    E18_FlushUART();
}

// --- CÁC HÀM CẤU HÌNH ---

bool E18_ResetModule(uint8_t mode) {
    uint8_t data[4] = {mode, 0xFF, 0xFF, 0x00};
    E18_SendCommand(E18_CMD_TYPE_CFG, E18_CMD_RESET, data, 4);


    if (mode == MODULE_RESET) return true;

    return E18_WaitHeaderOnly(2000);
}

bool E18_SetNodeType(uint8_t type) {
    E18_SendCommand(E18_CMD_TYPE_CFG, E18_CMD_SET_NODE_TYPE, &type, 1);
    return E18_WaitHeaderOnly(1000);
}

bool E18_SetPANID(uint16_t panid) {
    uint8_t data[2];
    data[0] = (uint8_t)(panid & 0xFF);
    data[1] = (uint8_t)((panid >> 8) & 0xFF);
    E18_SendCommand(E18_CMD_TYPE_CFG, E18_CMD_SET_PANID, data, 2);
    return E18_WaitHeaderOnly(1000);
}

bool E18_SetChannel(uint32_t channelMask) {
    uint8_t data[5];
    data[0] = 0x01;
    data[1] = (uint8_t)(channelMask & 0xFF);
    data[2] = (uint8_t)((channelMask >> 8) & 0xFF);
    data[3] = (uint8_t)((channelMask >> 16) & 0xFF);
    data[4] = (uint8_t)((channelMask >> 24) & 0xFF);
    E18_SendCommand(E18_CMD_TYPE_CFG, E18_CMD_SET_CHANNEL, data, 5);
    return E18_WaitHeaderOnly(1000);
}

bool E18_GetStatus(void) {
    E18_SendCommand(E18_CMD_TYPE_CFG, E18_CMD_GET_STATUS, NULL, 0);
    return E18_WaitHeaderOnly(500);
}

bool E18_StartNetwork(void) {
    uint8_t mode = 0x00;
    E18_SendCommand(E18_CMD_TYPE_CFG, E18_CMD_START_NWK, &mode, 1);

    return E18_WaitHeaderOnly(3000);
}

bool E18_SwitchToTransparentMode(void) {
    uint8_t val = 1;
    uint8_t data[4];
    data[0] = 0x00; data[1] = 0x03; data[2] = 0x00; data[3] = val;
    E18_SendCommand(E18_CMD_TYPE_CFG, E18_CMD_SET_ATTR, data, 4);
    return E18_WaitHeaderOnly(1000);
}


bool E18_Init(uint8_t type, uint16_t panid, uint8_t channel) {

    // 1. Vào Hex Mode
    E18_SwitchToHexMode();
    HAL_Delay(3000);
    // 2. Denetwork

    E18_ResetModule(MODULE_DENETWORK);
    HAL_Delay(3500);

   // E18_StartNetwork();

    E18_SetNodeType(type);
    HAL_Delay(200);

    E18_SetPANID(panid);
    HAL_Delay(200);

    uint32_t channelMask = (channel == 0) ? 0x07FFF800 : (1 << channel);
    E18_SetChannel(channelMask);
    HAL_Delay(200);

    E18_ResetModule(MODULE_RESET);
    HAL_Delay(3000);

    return E18_GetStatus();
}
