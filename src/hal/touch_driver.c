#include "touch_driver.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include <string.h>

static const char *TAG = "TOUCH";

// I2C Configuration - ADJUST THESE FOR YOUR BOARD
#define I2C_MASTER_SCL_IO           22      // GPIO number for I2C master clock
#define I2C_MASTER_SDA_IO           21      // GPIO number for I2C master data
#define I2C_MASTER_NUM              I2C_NUM_0                          
#define I2C_MASTER_FREQ_HZ          400000                     
#define I2C_MASTER_TX_BUF_DISABLE   0                          
#define I2C_MASTER_RX_BUF_DISABLE   0                          

// Touch Controller Address (Example: FT6236 is 0x38, GT911 is 0x5D or 0x14)
#define TOUCH_ADDR                  0x38  

bool touch_init(void) {
    ESP_LOGI(TAG, "Initializing I2C for Touch...");
    
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);
    
    esp_err_t err = i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C Install Error: %d", err);
        return false;
    }
    
    // Optional: Reset sequence for touch controller if using a Reset Pin
    
    ESP_LOGI(TAG, "Touch Driver Initialized");
    return true;
}

bool touch_read(TouchPoint_t* point) {
    uint8_t data[6];
    
    // Read registers (Example for FT6236-like controllers: start at reg 0x02)
    // 0x02: TD_Status
    // 0x03-0x06: X/Y coordinates
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TOUCH_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x02, true); // Start Register
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TOUCH_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 6, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(50));
    i2c_cmd_link_delete(cmd);

    if (ret == ESP_OK) {
        uint8_t touch_points = data[0] & 0x0F;
        if (touch_points > 0) {
            point->is_pressed = true;
            // High nibble of X/Y masked out
            // Example map: [xH, xL, yH, yL] logic depends on chip
            uint16_t x = ((data[1] & 0x0F) << 8) | data[2];
            uint16_t y = ((data[3] & 0x0F) << 8) | data[4];
            point->x = x;
            point->y = y;
            return true;
        }
    } else {
        // ESP_LOGE(TAG, "I2C Read Failed"); // Uncomment for debug
    }

    point->is_pressed = false;
    return false;
}
