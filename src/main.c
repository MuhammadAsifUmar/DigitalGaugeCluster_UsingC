#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "data/vehicle_data.h"
#include "obd/obd_manager.h"
#include "ui/ui_manager.h"

static const char *TAG = "APP_MAIN";

/*
 * Production Firmware Entry Point
 * 
 * This file initializes the system and launches the User Interface.
 * The UI is the primary interaction point.
 * The OBD communication runs in the background to feed data to the UI.
 */
void app_main(void) {
    ESP_LOGI(TAG, "System Booting...");

    // 1. Initialize Non-Volatile Storage (needed for WiFi/BT/Settings)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Initialize Shared Data Model
    vehicle_data_init();
    
    // 3. Start Tasks
    
    // UI Task: Handles Display and Touch.
    // We give it higher priority so interactions feel snappy.
    // It will render the gauge cluster and settings.
    xTaskCreate(ui_task, "UI_Manager", 8192, NULL, 5, NULL);

    // OBD Task: Handles Vehicle Communication.
    // Runs slightly lower priority to ensure UI doesn't freeze,
    // but high enough to process UART buffers.
    xTaskCreate(obd_manager_task, "OBD_Manager", 4096, NULL, 4, NULL);

    ESP_LOGI(TAG, "System Running. UI is active.");
    
    // Main loop can be used for Watchdog or Deep Sleep management
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10000));
        // Optional: Monitor stack high water marks
        // UBaseType_t hwm = uxTaskGetStackHighWaterMark(NULL);
        // ESP_LOGD(TAG, "Main Task Heartbeat");
    }
}
