#include "zigbee_e18.h"


static UART_HandleTypeDef *E18_UART_Handle = NULL;


static uint8_t E18_CalculateChecksum(uint8_t cmdType, uint8_t cmdCode, uint8_t* data, uint8_t dataLen) {
    uint8_t checksum = cmdType ^ cmdCode;
    for (int i = 0; i < dataLen; i++) {
        checksum ^= data[i];
    }
    return checksum;
}


static void E18_SendCommand(uint8_t cmdType, uint8_t cmdCode, uint8_t* data, uint8_t dataLen) {
    if (E18_UART_Handle == NULL) return;

    // Frame Length = Type(1) + Code(1) + Data(N) + Checksum(1)
    uint8_t frameLen = 1 + 1 + dataLen + 1;
    uint8_t buffer[64];

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


static void E18_SetLocalAttribute(uint16_t attrID, void* value, uint8_t dataLen) {
    uint8_t data[16]; // Buffer tạm

    data[0] = 0x00; // Port Index (Mặc định 0)

    // Attribute ID (Little Endian)
    data[1] = (uint8_t)(attrID & 0xFF);
    data[2] = (uint8_t)((attrID >> 8) & 0xFF);

    // Copy giá trị vào payload
    if(value != NULL && dataLen > 0) {
        memcpy(&data[3], value, dataLen);
    }

    // Tổng độ dài data gửi đi = 1 (Port) + 2 (AttrID) + dataLen (Value)
    E18_SendCommand(E18_CMD_TYPE_CFG, E18_CMD_SET_ATTR, data, 3 + dataLen);
}


void E18_Init_UART(UART_HandleTypeDef *huart_handle) {
    E18_UART_Handle = huart_handle;
}

void E18_SwitchToHexMode(void) {
    if (E18_UART_Handle == NULL) return;
    HAL_Delay(1000);
    uint8_t cmd[] = "+++";
    HAL_UART_Transmit(E18_UART_Handle, cmd, 3, 100);
    HAL_Delay(1000);
}

void E18_ResetModule(uint8_t mode) {
    uint8_t data[4] = {mode, 0xFF, 0xFF, 0x00};
    E18_SendCommand(E18_CMD_TYPE_CFG, E18_CMD_RESET, data, 4);
}

void E18_SetNodeType(uint8_t type) {
    E18_SendCommand(E18_CMD_TYPE_CFG, E18_CMD_SET_NODE_TYPE, &type, 1);
}

void E18_SetPANID(uint16_t panid) {
    uint8_t data[2];
    data[0] = (uint8_t)(panid & 0xFF);
    data[1] = (uint8_t)((panid >> 8) & 0xFF);
    E18_SendCommand(E18_CMD_TYPE_CFG, E18_CMD_SET_PANID, data, 2);
}

void E18_SetChannel(uint32_t channelMask) {
    uint8_t data[5];
    data[0] = 0x01; // Mode: 1 (Enable channel)
    // Little Endian 32bit channel mask
    data[1] = (uint8_t)(channelMask & 0xFF);
    data[2] = (uint8_t)((channelMask >> 8) & 0xFF);
    data[3] = (uint8_t)((channelMask >> 16) & 0xFF);
    data[4] = (uint8_t)((channelMask >> 24) & 0xFF);
    E18_SendCommand(E18_CMD_TYPE_CFG, E18_CMD_SET_CHANNEL, data, 5);
}


void E18_Init(uint8_t type, uint16_t panid, uint8_t channel) {

    E18_SwitchToHexMode();

    E18_ResetModule(MODULE_DENETWORK);
    HAL_Delay(3000);

    E18_SetNodeType(type);
    HAL_Delay(500);

    E18_SetPANID(panid);
    HAL_Delay(500);

    uint32_t channelMask;

    if (channel == 0) {
        channelMask = 0x07FFF800;
    } else {
        channelMask = (1 << channel);
    }

    E18_SetChannel(channelMask);
    HAL_Delay(500);

    E18_ResetModule(MODULE_RESET);
    HAL_Delay(3000);
}

void E18_GetStatus(void) {
    E18_SendCommand(E18_CMD_TYPE_CFG, E18_CMD_GET_STATUS, NULL, 0);
}

void E18_StartNetwork(void) {
    uint8_t mode = 0x00;
    E18_SendCommand(E18_CMD_TYPE_CFG, E18_CMD_START_NWK, &mode, 1);
}

void E18_SwitchToTransparentMode(void) {
    uint8_t val = 1;

    E18_SetLocalAttribute(E18_ATTR_TRANSPARENT_MODE, &val, 1);
}
