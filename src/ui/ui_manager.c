#include "ui_manager.h"
#include "../data/vehicle_data.h"
#include "../hal/touch_driver.h"
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdio.h>

/*
 * Note: specific display drivers (ILI9341, ST7789) often use
 * "esp_lcd_panel_io" or generic LVGL drivers.
 */

// Widgets
static lv_obj_t *lbl_rpm_val;
static lv_obj_t *lbl_speed_val;
static lv_obj_t *arc_rpm;

static void create_dashboard(void) {
    lv_obj_t *scr = lv_scr_act();

    // RPM Gauge
    arc_rpm = lv_arc_create(scr);
    lv_obj_set_size(arc_rpm, 200, 200);
    lv_arc_set_rotation(arc_rpm, 135);
    lv_arc_set_bg_angles(arc_rpm, 0, 270);
    lv_arc_set_value(arc_rpm, 0);
    lv_arc_set_range(arc_rpm, 0, 8000); // 8k max rpm
    lv_obj_center(arc_rpm);

    // RPM Label
    lbl_rpm_val = lv_label_create(scr);
    lv_label_set_text(lbl_rpm_val, "0 RPM");
    lv_obj_align_to(lbl_rpm_val, arc_rpm, LV_ALIGN_CENTER, 0, 40);

    // Speed Label
    lbl_speed_val = lv_label_create(scr);
    lv_label_set_text(lbl_speed_val, "0 km/h");
    lv_obj_align(lbl_speed_val, LV_ALIGN_TOP_MID, 0, 20);
}

void ui_init(void) {
    // Hardware init (SPI / Display) would go here
    lv_init();
    
    // ... LVGL buffer and driver registration ...
    // lv_disp_drv_register(...)
    // lv_indev_drv_register(...)
    
    create_dashboard();
}

void ui_task(void *pvParameters) {
    ui_init();
    
    VehicleData_t* data = get_vehicle_data();
    char buf[32];
    
    while (1) {
        // Read Data (Non-blocking or minimal blocking)
        vehicle_data_lock();
        float rpm = data->rpm;
        float speed = data->speed_kmh;
        vehicle_data_unlock();
        
        // Update UI
        lv_arc_set_value(arc_rpm, (int32_t)rpm);
        
        snprintf(buf, sizeof(buf), "%d RPM", (int)rpm);
        lv_label_set_text(lbl_rpm_val, buf);
        
        snprintf(buf, sizeof(buf), "%d km/h", (int)speed);
        lv_label_set_text(lbl_speed_val, buf);
        
        // LVGL Tick
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
