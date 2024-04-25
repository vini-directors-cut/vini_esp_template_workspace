#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "sht4x.c"

#define I2C_MASTER_SCL_GPIO 19
#define I2C_MASTER_SDA_GPIO 18

static sht4x_t sht40;

void reading_task(void *params){
  float temperature, humidity;

  TickType_t last_wake_up = xTaskGetTickCount();
  while(true){
    ESP_ERROR_CHECK(sht4x_measure(&sht40, &temperature, &humidity));
    ESP_LOGI("SHT40", "temperature = %.2f, humidity = %.2f", temperature, humidity);
    vTaskDelayUntil(&last_wake_up, pdMS_TO_TICKS(5000));
  }
}

void app_main(void){
  ESP_ERROR_CHECK(i2cdev_init());
  memset(&sht40, 0, sizeof(sht40));

  ESP_ERROR_CHECK(sht4x_init_desc(&sht40, 0, I2C_MASTER_SDA_GPIO, I2C_MASTER_SCL_GPIO));
  ESP_ERROR_CHECK(sht4x_init(&sht40));

  xTaskCreate(reading_task, "SHT40 task", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL);
}
