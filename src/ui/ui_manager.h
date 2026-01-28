#ifndef UI_MANAGER_H
#define UI_MANAGER_H

// Initializes LVGL, Display Driver, and Touch Driver
void ui_init(void);

// The FreeRTOS task that handles rendering
void ui_task(void *pvParameters);

#endif // UI_MANAGER_H
