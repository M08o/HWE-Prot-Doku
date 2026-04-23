#include "manchester.h"

uint16_t manchesterEncodeByte(const uint8_t value) {
    uint16_t encoded = 0;

    for (int bit = 7; bit >= 0; --bit) {
        encoded <<= 2;
        encoded |= (value & (1u << bit)) ? 0b01u : 0b10u;
    }

    return encoded;
}

bool manchesterDecodeWord(const uint16_t encoded, uint8_t& valueOut) {
    uint8_t decoded = 0;

    for (int pairIndex = 0; pairIndex < 8; ++pairIndex) {
        const uint8_t pair = static_cast<uint8_t>((encoded >> (14 - pairIndex * 2)) & 0x03u);
        decoded <<= 1;

        if (pair == 0b10u) {
            continue;
        }

        if (pair == 0b01u) {
            decoded |= 0x01u;
            continue;
        }

        return false;
    }

    valueOut = decoded;
    return true;
}

void manchesterWriteByte(HardwareSerial& serial, const uint8_t value) {
    const uint16_t encoded = manchesterEncodeByte(value);
    const uint8_t highByte = static_cast<uint8_t>(encoded >> 8);
    const uint8_t lowByte = static_cast<uint8_t>(encoded & 0xFFu);

    serial.write(highByte);
    serial.write(lowByte);
    serial.flush();
}

bool manchesterReadByte(HardwareSerial& serial, uint8_t& valueOut) {
    if (serial.available() < 2) {
        return false;
    }

    const uint8_t highByte = static_cast<uint8_t>(serial.read());
    const uint8_t lowByte = static_cast<uint8_t>(serial.read());
    const uint16_t encoded = static_cast<uint16_t>(highByte) << 8 | lowByte;

    return manchesterDecodeWord(encoded, valueOut);
}