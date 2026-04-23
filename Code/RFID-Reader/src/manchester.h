/**
 * manchester.h - Schnittstelle fuer Manchester-Kodierung.
 *
 * Kodierung:
 *   Datenbit 0 -> 10
 *   Datenbit 1 -> 01
 *
 * Ein logisches Datenbyte wird dadurch zu 16 Bits und auf der UART-Leitung
 * als zwei normale Bytes uebertragen.
 */

#pragma once

#include <cstdint>

uint16_t manchesterEncodeByte(uint8_t value);
bool manchesterDecodeWord(uint16_t encoded, uint8_t& valueOut);
