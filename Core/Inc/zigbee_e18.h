#ifndef __ZIGBEE_E18_H__
#define __ZIGBEE_E18_H__

#include "stm32f4xx.h"
#include "string.h"
// command types
#define E18_CMD_TYPE_CFG        0x00 // Local configuration commands
#define E18_CMD_TYPE_ZDO_REQ    0x01 // Network management commands
#define E18_CMD_TYPE_ZCL_SEND   0x02 // ZCL send commands
//command codes
#define E18_CMD_GET_STATUS      0x00 // Query module status
#define E18_CMD_START_NWK       0x02 // Start network configuration
#define E18_CMD_RESET           0x04 // Reset/Factory reset
#define E18_CMD_SET_NODE_TYPE   0x05 // Set node type (Coord/Router/EndDevice)
#define E18_CMD_SET_CHANNEL     0x06 // Set channel
#define E18_CMD_SET_PANID       0x08 // Set PANID
#define E18_CMD_SET_TX_POWER    0x0D // Set transmit power
//node types
#define E18_NODE_COORDINATOR    0x00
#define E18_NODE_ROUTER         0x01
#define E18_NODE_END_DEVICE     0x02
#define E18_NODE_SLEEP_END      0x03
#define E18_CMD_SET_ATTR        0x11
#define E18_ATTR_TRANSPARENT_MODE 0x0003

//reset mode types
#define MODULE_RESET			0
#define MODULE_DENETWORK		1
#define MODULE_FACTORY_RST		2

// Function Prototypes
void E18_Init_UART(UART_HandleTypeDef *huart_handle);
void E18_Init(uint8_t type, uint16_t panid, uint8_t channel);
void E18_GetStatus(void);
void E18_SetNodeType(uint8_t type);
void E18_SetPANID(uint16_t panid);
void E18_SetChannel(uint32_t channelMask);
void E18_StartNetwork(void);
void E18_ResetModule(uint8_t mode);
void E18_SwitchToTransparentMode(void);
void E18_SwitchToHexMode(void);
#endif
