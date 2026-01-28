#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "../obd/pid_decoder.c"

// Mock definitions
#define TAG "TEST"

void test_pid_decoder() {
    printf("Running PID Decoder Tests...\n");
    
    float val;
    
    // Test 1: RPM (PID 0C)
    // Formula: ((A*256)+B)/4
    // Raw: 41 0C 0F A0
    // A=0F (15), B=A0 (160) -> (3840 + 160) / 4 = 1000 RPM
    val = decode_pid(PID_RPM, "41 0C 0F A0");
    printf("RPM Test 1: '41 0C 0F A0' -> %.2f RPM ", val);
    if (fabs(val - 1000.0f) < 0.01) printf("[PASS]\n"); else printf("[FAIL]\n");

    // Test 2: Speed (PID 0D)
    // Formula: A
    // Raw: 41 0D 3C
    // A=3C (60) -> 60 km/h
    val = decode_pid(PID_SPEED, "41 0D 3C");
    printf("Speed Test 1: '41 0D 3C' -> %.2f km/h ", val);
    if (fabs(val - 60.0f) < 0.01) printf("[PASS]\n"); else printf("[FAIL]\n");
    
    // Test 3: Coolant Temp (PID 05)
    // Formula: A - 40
    // Raw: 41 05 78
    // A=78 (120) -> 120 - 40 = 80 C
    val = decode_pid(PID_COOLANT_TEMP, "41 05 78");
    printf("Temp Test 1: '41 05 78' -> %.2f C ", val);
    if (fabs(val - 80.0f) < 0.01) printf("[PASS]\n"); else printf("[FAIL]\n");
    
    // Test 4: Battery Voltage (PID 42)
    // Formula: ((256*A)+B)/1000
    // Raw: 41 42 30 00
    // A=48, B=0 -> 12288 / 1000 = 12.288
    val = decode_pid(PID_BATTERY_VOLTAGE, "41 42 30 00");
    printf("Volt Test 1: '41 42 30 00' -> %.2f V ", val);
    if (fabs(val - 12.288f) < 0.001) printf("[PASS]\n"); else printf("[FAIL]\n");
    
    // Test 5: Invalid Data
    val = decode_pid(PID_RPM, "NO DATA");
    printf("Error Test 1: 'NO DATA' -> %.2f ", val);
    if (val < 0) printf("[PASS]\n"); else printf("[FAIL]\n");
}

int main() {
    test_pid_decoder();
    return 0;
}
