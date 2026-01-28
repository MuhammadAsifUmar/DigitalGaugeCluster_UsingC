#ifndef STN1110_DRIVER_H
#define STN1110_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

// Initialize UART and reset logic
bool stn_init(void);

// Send a command and wait for response
// Returns true if response received within timeout
bool stn_send_cmd(const char* cmd, char* response_buf, uint16_t buf_len, uint32_t timeout_ms);

// Check if STN1110 is ready
bool stn_is_ready(void);

#endif // STN1110_DRIVER_H
