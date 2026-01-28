#ifndef VEHICLE_DATA_H
#define VEHICLE_DATA_H

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef struct {
    // Engine Data
    float rpm;              // Engine RPM
    float speed_kmh;        // Vehicle Speed in km/h
    float coolant_temp_c;   // Coolant Temperature in Celsius
    float throttle_pos;     // Throttle Position %
    float engine_load;      // Calculated Engine Load %
    
    // Electrical
    float battery_voltage;  // Module Voltage (PID 42)
    
    // Status
    uint8_t dtc_count;      // Diagnostic Trouble Code count
    bool obd_connected;     // True if communicating with ECU
    uint32_t last_update_ms; // Timestamp of last successful data
} VehicleData_t;

// Global instance getter
VehicleData_t* get_vehicle_data(void);

// Thread-safety
void vehicle_data_lock(void);
void vehicle_data_unlock(void);

// Helper to initialize the data module
void vehicle_data_init(void);

#endif // VEHICLE_DATA_H
