/**
 * manchester.cpp - Implementierung der Manchester-Kodierung.
 */

#include "manchester.h"

uint16_t manchesterEncodeByte(const uint8_t value) {
    uint16_t encoded = 0;

    // MSB zuerst kodieren, damit Sender und Empfaenger dieselbe Bitreihenfolge nutzen.
    for (int bit = 7; bit >= 0; --bit) {
        encoded <<= 2;

        if ((value & (1U << bit)) != 0U) {
            encoded |= 0b01;
        } else {
            encoded |= 0b10;
        }
    }

    return encoded;
}

bool manchesterDecodeWord(const uint16_t encoded, uint8_t& valueOut) {
    uint8_t decoded = 0;

    for (int pairIndex = 0; pairIndex < 8; ++pairIndex) {
        const uint8_t pair = static_cast<uint8_t>((encoded >> (14 - pairIndex * 2)) & 0b11);
        decoded <<= 1;

        if (pair == 0b10) {
            // Datenbit 0
        } else if (pair == 0b01) {
            decoded |= 0x01;
        } else {
            return false;
        }
    }

    valueOut = decoded;
    return true;
}
