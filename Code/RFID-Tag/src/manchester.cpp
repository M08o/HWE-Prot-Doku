#include "manchester.h"

void manchesterWriteByte(HardwareSerial& serial, const uint8_t value) {
    serial.write(value);
    serial.flush();
}

bool manchesterReadByte(HardwareSerial& serial, uint8_t& valueOut) {
    if (serial.available() < 1) {
        return false;
    }

    valueOut = static_cast<uint8_t>(serial.read());
    return true;
}