
#include "4G_MQTT.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// ===== Internal functions =====
static void dbg(const char *fmt, ...);
static void sendAT(const char *cmd, uint32_t timeout);
static int  sendATwait(const char *cmd, const char *waitFor, uint32_t timeout);
static void waitForOK(uint32_t timeout);
static void mqttPublish(const char *topic, const char *payload);

// ===== Common helpers =====
static uint32_t millis(void) { return HAL_GetTick(); }
static void delay_ms(uint32_t ms){ HAL_Delay(ms); }

static void dbg(const char *fmt, ...)
{
    char msg[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 1000);
}

static void uart2_send(const uint8_t *data, uint16_t len)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)data, len, 2000);
}

static void uart2_sendLine(const char *s)
{
    HAL_UART_Transmit(&huart2,(uint8_t*)s,strlen(s),2000);
    const char crlf[2]={0x0D,0x0A};
    HAL_UART_Transmit(&huart2,(uint8_t*)crlf,2,2000);
    dbg(">> %s\r\n", s);
}

// Đọc đến khi thấy chuỗi needle
static int uart2_readUntil(char *buf, size_t buflen,const char *needle,uint32_t timeout)
{
    uint32_t t0 = millis();
    size_t idx  = 0;
    while(millis()-t0 < timeout){
        uint8_t c;
        if(HAL_UART_Receive(&huart2,&c,1,10)==HAL_OK){
            if(idx < buflen-1) buf[idx++] = c;
            buf[idx] = '\0';
            if(strstr(buf,needle)) return 1;
        }
    }
    return 0;
}

static void waitForOK(uint32_t timeout)
{
    char buf[256]={0};
    uint32_t t0 = millis();
    while(millis()-t0 < timeout){
        uint8_t c;
        if(HAL_UART_Receive(&huart2,&c,1,20)==HAL_OK){
            size_t l=strlen(buf);
            if(l < sizeof(buf)-1){
                buf[l]=c;
                buf[l+1]=0;
            }
            if(strstr(buf,"OK")||strstr(buf,"ERROR")||strstr(buf,"+CMQTT"))
                return;
        }
    }
}

static void sendAT(const char *cmd, uint32_t timeout)
{
    uart2_sendLine(cmd);
    waitForOK(timeout);
}

static int sendATwait(const char *cmd,const char *waitFor,uint32_t timeout){
    char buf[128]={0};
    uart2_sendLine(cmd);
    if(uart2_readUntil(buf,sizeof(buf),waitFor,timeout)){
        dbg("%s received\r\n",waitFor);
        return 1;
    }
    dbg("Timeout waiting for %s\r\n",waitFor);
    return 0;
}

// ===== MQTT FLOW =====
static void mqtt_start(void)
{
    sendAT("AT+CMQTTSTART",3000);
    delay_ms(500);
    sendAT("AT+CMQTTACCQ=0,\"client1\",0",3000);

    char cmd[150];
    snprintf(cmd,sizeof(cmd),
        "AT+CMQTTCONNECT=0,\"tcp://eu.thingsboard.cloud:1883\",60,1,\"%s\"",
        TB_ACCESS_TOKEN);
    sendAT(cmd,5000);
}

static void init_network(void)
{
    sendAT("AT",2000);
    sendAT("ATE0",2000);
    sendAT("AT+CPIN?",2000);
    sendAT("AT+CREG?",2000);
    sendAT("AT+CGATT?",2000);

    char apnCmd[64];
    snprintf(apnCmd,sizeof(apnCmd),"AT+CGDCONT=1,\"IP\",\"%s\"",APN_NAME);
    sendAT(apnCmd,3000);

    sendAT("AT+NETOPEN",3000);
}

static void mqttPublish(const char *topic,const char *payload){
    char cmd[64];

    snprintf(cmd,sizeof(cmd),"AT+CMQTTTOPIC=0,%d",(int)strlen(topic));
    if(sendATwait(cmd,">",3000)){
        uart2_send((uint8_t*)topic,strlen(topic));
        uint8_t z=0x1A;
        uart2_send(&z,1);
        waitForOK(3000);
    }

    snprintf(cmd,sizeof(cmd),"AT+CMQTTPAYLOAD=0,%d",(int)strlen(payload));
    if(sendATwait(cmd,">",3000)){
        uart2_send((uint8_t*)payload,strlen(payload));
        uint8_t z=0x1A;
        uart2_send(&z,1);
        waitForOK(3000);
    }

    sendAT("AT+CMQTTPUB=0,1,60",3000);
}

// ===== PUBLIC API =====
void A7640_Init(void)
{
    init_network();
    mqtt_start();
}

void A7640_Publish(int data, const char *c)
{
    char topic[]="v1/devices/me/telemetry";
    char payload[64];

    snprintf(payload, sizeof(payload),
             "{\"%s\":%d}", c, data);

    dbg("\r\nPublish: %s\r\n", payload);
    mqttPublish(topic,payload);


}
//"{\"smoke\":%d}"
