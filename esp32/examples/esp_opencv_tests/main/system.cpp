#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_spiffs.h>
#include <esp_log.h>

#include "system.hpp"


#define TAG "SYSTEM"


esp_err_t init_spiffs() {
    ESP_LOGI("spiffs_init", "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
            .base_path = "/spiffs",
            .partition_label = NULL,
            .max_files = 10,
            .format_if_mount_failed = false
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE("spiffs_init", "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE("spiffs_init", "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE("spiffs_init", "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE("spiffs_init", "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI("spiffs_init", "Partition size: total: %d, used: %d", total, used);
    }
    return ESP_OK;
}

void wait_msec(uint16_t v) {
    vTaskDelay(v / portTICK_PERIOD_MS);
}

void wait_sec(uint16_t v) {
    vTaskDelay(v * 1000 / portTICK_PERIOD_MS);
}

void disp_mem_infos() {
    ESP_LOGI(TAG, "task stack left: %d Bytes", uxTaskGetStackHighWaterMark(NULL));
    ESP_LOGI(TAG, "heap left: %d Bytes", esp_get_free_heap_size());
}