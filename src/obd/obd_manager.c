#include "obd_manager.h"
#include "stn1110.h"
#include "pid_decoder.h"
#include "../data/vehicle_data.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "OBD_MGR";

// Polling intervals in ticks
#define FAST_INTERVAL  (pdMS_TO_TICKS(100))  // RPM, Speed
#define SLOW_INTERVAL  (pdMS_TO_TICKS(1000)) // Temp, Volt

void obd_manager_task(void *pvParameters) {
    char response[64];
    
    // 1. Initialize Driver
    while (!stn_init()) {
        ESP_LOGE(TAG, "STN Init failed, retrying...");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    
    VehicleData_t* data = get_vehicle_data();
    
    // Main Loop
    TickType_t last_wake_time = xTaskGetTickCount();
    
    uint8_t slow_poll_counter = 0;
    
    while (1) {
        bool comm_success = true;
        
        // --- High Priority PIDs ---
        
        // RPM
        if (stn_send_cmd("010C", response, sizeof(response), 100)) {
            float val = decode_pid(PID_RPM, response);
            if (val >= 0) {
                vehicle_data_lock();
                data->rpm = val;
                vehicle_data_unlock();
            }
        } else comm_success = false;

        // Speed
        if (stn_send_cmd("010D", response, sizeof(response), 100)) {
            float val = decode_pid(PID_SPEED, response);
            if (val >= 0) {
                vehicle_data_lock();
                data->speed_kmh = val;
                vehicle_data_unlock();
            }
        } else comm_success = false;

        // --- Low Priority PIDs (Every 10th cycle -> ~1 sec) ---
        if (++slow_poll_counter >= 10) {
            slow_poll_counter = 0;
            
            // Coolant Temp
            if (stn_send_cmd("0105", response, sizeof(response), 150)) {
                float val = decode_pid(PID_COOLANT_TEMP, response);
                if (val >= 0) {
                    vehicle_data_lock();
                    data->coolant_temp_c = val;
                    vehicle_data_unlock();
                }
            }
            
            // Voltage
            if (stn_send_cmd("0142", response, sizeof(response), 150)) {
                float val = decode_pid(PID_BATTERY_VOLTAGE, response);
                if (val >= 0) {
                    vehicle_data_lock();
                    data->battery_voltage = val;
                    vehicle_data_unlock();
                }
            }
        }

        // Update connection status
        vehicle_data_lock();
        data->obd_connected = comm_success;
        if (comm_success) data->last_update_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
        vehicle_data_unlock();

        // Delay
        vTaskDelayUntil(&last_wake_time, FAST_INTERVAL);
    }
}
