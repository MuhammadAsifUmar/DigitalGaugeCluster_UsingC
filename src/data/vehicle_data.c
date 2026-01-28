#include "vehicle_data.h"
#include <string.h>

static VehicleData_t v_data;
static SemaphoreHandle_t data_mutex = NULL;

void vehicle_data_init(void) {
    memset(&v_data, 0, sizeof(VehicleData_t));
    data_mutex = xSemaphoreCreateMutex();
}

VehicleData_t* get_vehicle_data(void) {
    return &v_data;
}

void vehicle_data_lock(void) {
    if (data_mutex != NULL) {
        xSemaphoreTake(data_mutex, portMAX_DELAY);
    }
}

void vehicle_data_unlock(void) {
    if (data_mutex != NULL) {
        xSemaphoreGive(data_mutex);
    }
}
