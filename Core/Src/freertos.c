/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osThreadId_t TaskSensorHandle;
osThreadId_t TaskServerUploadHandle;
osThreadId_t TaskLCDMenuHandle;
osThreadId_t TaskAlertHandle;
osThreadId_t TaskModemHandle;

osMessageQueueId_t sensorQueueHandle;
osMessageQueueId_t alertQueueHandle;
osMessageQueueId_t lcdQueueHandle;
osMutexId_t configMutexHandle;

typedef struct {
  uint32_t timestamp;
  float temperature;
  float smokeLevel;
  uint8_t fireDetected;
} SensorData_t;

typedef struct {
  char msg[64];
} AlertMsg_t;

typedef struct {
  uint8_t userConfigured;
  uint8_t autoUpload;
  char deviceName[32];
} SystemConfig_t;

SystemConfig_t sysConfig;

void Task_Sensor(void *argument);
void Task_ServerUpload(void *argument);
void Task_LCD_Menu(void *argument);
void Task_Alert(void *argument);
void Task_Modem(void *argument);
/* USER CODE END FunctionPrototypes */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void MX_FREERTOS_Init(void)
{
  // Khởi tạo mutex, queue, event flags
  configMutexHandle = osMutexNew(NULL);
  sensorQueueHandle = osMessageQueueNew(8, sizeof(SensorData_t), NULL);
  alertQueueHandle = osMessageQueueNew(4, sizeof(AlertMsg_t), NULL);
  lcdQueueHandle = osMessageQueueNew(6, sizeof(char[64]), NULL);

  // Tạo các task
  const osThreadAttr_t TaskSensor_attributes = {
    .name = "TaskSensor",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
  };
  TaskSensorHandle = osThreadNew(Task_Sensor, NULL, &TaskSensor_attributes);

  const osThreadAttr_t TaskServerUpload_attributes = {
    .name = "TaskServerUpload",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal,
  };
  TaskServerUploadHandle = osThreadNew(Task_ServerUpload, NULL, &TaskServerUpload_attributes);

  const osThreadAttr_t TaskLCDMenu_attributes = {
    .name = "TaskLCDMenu",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal,
  };
  TaskLCDMenuHandle = osThreadNew(Task_LCD_Menu, NULL, &TaskLCDMenu_attributes);

  const osThreadAttr_t TaskAlert_attributes = {
    .name = "TaskAlert",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityHigh,
  };
  TaskAlertHandle = osThreadNew(Task_Alert, NULL, &TaskAlert_attributes);

  const osThreadAttr_t TaskModem_attributes = {
    .name = "TaskModem",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal,
  };
  TaskModemHandle = osThreadNew(Task_Modem, NULL, &TaskModem_attributes);

  // Khởi tạo FreeRTOS kernel
  osKernelStart();
}

// Các task và hàm xử lý

void Task_Sensor(void *argument)
{
  //SensorData_t sensorData;
  for (;;) {
    // Đọc cảm biến và gán dữ liệu cho sensorData
//    sensorData.timestamp = HAL_GetTick();
//    sensorData.temperature = ReadTemperature();
//    sensorData.smokeLevel = ReadSmoke();
//
//    // Kiểm tra cháy
//    sensorData.fireDetected = (sensorData.smokeLevel > SMOKE_THRESHOLD);

    // Gửi dữ liệu vào queue
    //osMessageQueuePut(sensorQueueHandle, &sensorData, 0, 0);
    osDelay(500);  // Đọc mỗi 500ms
  }
}

void Task_ServerUpload(void *argument)
{
  //SensorData_t sensorData;
  for (;;) {
   // if (osMessageQueueGet(sensorQueueHandle, &sensorData, NULL, osWaitForever) == osOK) {
      // Gửi dữ liệu lên server (MQTT)
//      if (sysConfig.autoUpload) {
//        MQTT_Publish("sensor/temperature", &sensorData.temperature, sizeof(sensorData.temperature));
//        MQTT_Publish("sensor/smokeLevel", &sensorData.smokeLevel, sizeof(sensorData.smokeLevel));
//
//        if (sensorData.fireDetected) {
//          osMessageQueuePut(alertQueueHandle, "Fire detected!", 0, 0);
//        }
//      }
//    }
    osDelay(1000);  // Gửi mỗi giây
  }
}

void Task_LCD_Menu(void *argument)
{
  //char lcdMessage[64];
  for (;;) {
//    if (osMessageQueueGet(lcdQueueHandle, lcdMessage, NULL, osWaitForever) == osOK) {
//      LCD_Clear();
//      LCD_Printf(lcdMessage);
//    }
    osDelay(500);
  }
}

void Task_Alert(void *argument)
{
  //AlertMsg_t alertMsg;
  for (;;) {
//    if (osMessageQueueGet(alertQueueHandle, &alertMsg, NULL, osWaitForever) == osOK) {
//      ActivateBuzzer();
//      SendSMS("Alert: Fire detected!");
//    }
    osDelay(500);
  }
}

void Task_Modem(void *argument)
{
  for (;;) {
//    AT_Command("AT+CSQ");
//    if (CheckNetwork()) {
//      AT_Command("AT+CGDCONT=1,\"IP\",\"internet\"");
//      AT_Command("AT+NETOPEN");
//      MQTT_Connect();
//    }
    osDelay(3000);
  }
}

/* USER CODE END Application */

