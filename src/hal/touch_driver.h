#ifndef TOUCH_DRIVER_H
#define TOUCH_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

// Simple point structure
typedef struct {
    int16_t x;
    int16_t y;
    bool is_pressed;
} TouchPoint_t;

// Initialize I2C and touch controller
bool touch_init(void);

// Read current touch state
// Returns true if communication successful
bool touch_read(TouchPoint_t* point);

#endif // TOUCH_DRIVER_H
