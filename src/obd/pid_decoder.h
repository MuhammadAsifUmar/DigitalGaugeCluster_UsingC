#ifndef PID_DECODER_H
#define PID_DECODER_H

#include <stdint.h>

// Standard OBD-II PIDs
#define PID_RPM             0x0C
#define PID_SPEED           0x0D
#define PID_COOLANT_TEMP    0x05
#define PID_THROTTLE_POS    0x11
#define PID_BATTERY_VOLTAGE 0x42 

// Parses the ASCII hex response from ELM327/STN1110
// Returns the decoded value as float.
// Returns -1.0f on error.
float decode_pid(uint8_t pid, const char* response);

#endif // PID_DECODER_H
