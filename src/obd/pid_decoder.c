#include "pid_decoder.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Helper to convert 2 hex chars to uint8
static uint8_t hex2byte(const char* hex) {
    char buf[3] = {hex[0], hex[1], '\0'};
    return (uint8_t)strtol(buf, NULL, 16);
}

float decode_pid(uint8_t pid, const char* response) {
    // Expected format: "41 XX A B ..." or "41 XX A ..."
    // We assume the response string has stripped prompts and whitespace/newlines might be present,
    // but the STN1110 driver usually returns cleaned data or we need to handle spaces.
    // For simplicity, let's assume the driver implementation strips header/spaces or we parse carefully.
    
    // Simple parser: Find "41 <PID>" sequence
    char pid_hex[3];
    sprintf(pid_hex, "%02X", pid);
    
    // Find "41 " + PID
    // Note: Use strstr to skip headers if present
    const char* data_ptr = response;
    
    // Basic validation (skipping rigorous implementation for brevity)
    // In a real scenario, we'd tokenize by space.
    
    // Check if response contains "NO DATA"
    if (strstr(response, "NO DATA") != NULL) return -1.0f;

    // ELM327 responses are space separated hex bytes, e.g. "41 0C 1A F8"
    // We need to parse the A, B, C, D bytes.
    int A = 0, B = 0;
    
    // Pointer math to skip "41 PID " (6 chars if space separated)
    // This is BRITTLE and needs robust parsing in production
    // e.g. "41 0C 1A F8" -> offset 6 is '1'
    
    // A robust way: sscanf
    int mode, ret_pid;
    int a_val, b_val, c_val, d_val;
    // Try 2 bytes
    int matches = sscanf(response, "%x %x %x %x", &mode, &ret_pid, &a_val, &b_val);
    
    if (matches < 3) return -1.0f; // Need at least A
    if (mode != 0x41 || ret_pid != pid) return -1.0f;

    A = a_val;
    if (matches >= 4) B = b_val;

    switch (pid) {
        case PID_RPM:
            // ((A*256)+B)/4
            return ((float)(A * 256 + B)) / 4.0f;
            
        case PID_SPEED:
            // A
            return (float)A;
            
        case PID_COOLANT_TEMP:
            // A - 40
            return (float)(A - 40);
            
        case PID_THROTTLE_POS:
            // (A*100)/255
            return ((float)A * 100.0f) / 255.0f;
            
        case PID_BATTERY_VOLTAGE:
            // ((A*256)+B)/1000  (This is PID 42 which is Control Module Voltage)
            return ((float)(A * 256 + B)) / 1000.0f;
            
        default:
            return 0.0f;
    }
}
