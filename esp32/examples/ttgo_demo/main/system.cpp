#include <esp_log.h>
#include <esp_system.h>

#include <sdkconfig.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <sys/param.h>
#include <esp32/clk.h>

#include "system.h"

#define TAG "SYSTEM"

void wait_msec(uint16_t v) {
    vTaskDelay(v / portTICK_PERIOD_MS);
}

void wait_sec(uint16_t v) {
    vTaskDelay(v * 1000 / portTICK_PERIOD_MS);
}

void disp_infos() {
    /* Print memory information */
    ESP_LOGI(TAG, "task %s stack high watermark: %d Bytes", pcTaskGetTaskName(NULL), uxTaskGetStackHighWaterMark(NULL));
    ESP_LOGI(TAG, "heap left: %d Bytes", esp_get_free_heap_size());
}
