#pragma once

#include <Arduino.h>
#include <cstdint>

// Manchester mapping:
// 0 -> 10
// 1 -> 01

uint16_t manchesterEncodeByte(uint8_t value);
bool manchesterDecodeWord(uint16_t encoded, uint8_t& valueOut);
void manchesterWriteByte(HardwareSerial& serial, uint8_t value);
bool manchesterReadByte(HardwareSerial& serial, uint8_t& valueOut);
