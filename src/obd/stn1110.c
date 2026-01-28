#include "stn1110.h"
#include "driver/uart.h"
#include "esp_log.h"
#include <string.h>

#define STN_UART_NUM UART_NUM_2
#define STN_TX_PIN 17
#define STN_RX_PIN 16
#define RX_BUF_SIZE 1024

static const char *TAG = "STN1110";

bool stn_init(void) {
    uart_config_t uart_config = {
        .baud_rate = 38400, // Default for STN1110
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    
    // Install driver
    if (uart_driver_install(STN_UART_NUM, RX_BUF_SIZE * 2, 0, 0, NULL, 0) != ESP_OK) {
        ESP_LOGE(TAG, "Driver install failed");
        return false;
    }
    
    if (uart_param_config(STN_UART_NUM, &uart_config) != ESP_OK) {
        ESP_LOGE(TAG, "Param config failed");
        return false;
    }

    if (uart_set_pin(STN_UART_NUM, STN_TX_PIN, STN_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK) {
        ESP_LOGE(TAG, "Set pin failed");
        return false;
    }
    
    // Initialization Sequence
    char resp[64];
    
    // 1. Reset
    stn_send_cmd("ATZ", resp, sizeof(resp), 1000);
    vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for reboot
    
    // 2. Echo Off
    if (!stn_send_cmd("ATE0", resp, sizeof(resp), 500)) return false;
    
    // 3. Set Protocol to SAE J1850 VPW (GM)
    // SP 2 = SAE J1850 VPW
    // We increase timeout as protocol initialization might take a moment on the bus
    if (!stn_send_cmd("ATSP2", resp, sizeof(resp), 1000)) return false; 

    // Optional: Set header if needed for specific GM modules (usually not needed for generic OBD)
    // stn_send_cmd("ATSH 6C 10 F1", ...); 
    
    ESP_LOGI(TAG, "STN1110 Initialized");
    return true;
}

bool stn_send_cmd(const char* cmd, char* response_buf, uint16_t buf_len, uint32_t timeout_ms) {
    // Flush previous
    uart_flush_input(STN_UART_NUM);
    
    // Write command
    uart_write_bytes(STN_UART_NUM, cmd, strlen(cmd));
    uart_write_bytes(STN_UART_NUM, "\r", 1); // CR terminator
    
    // Read response
    int len = 0;
    uint32_t start = xTaskGetTickCount();
    int read_bytes = 0;
    
    memset(response_buf, 0, buf_len);
    
    while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(timeout_ms)) {
        int rxBytes = uart_read_bytes(STN_UART_NUM, (uint8_t*)response_buf + len, buf_len - len - 1, pdMS_TO_TICKS(50));
        if (rxBytes > 0) {
            len += rxBytes;
            // Check for prompt '>' which indicates end of response.
            if (strchr(response_buf, '>')) {
                // Remove prompt and trims
                char* p = strchr(response_buf, '>');
                *p = '\0'; 
                return true;
            }
        }
    }
    
    return false; // Timeout
}
