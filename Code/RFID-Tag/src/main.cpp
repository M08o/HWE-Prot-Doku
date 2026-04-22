/**
 * main.cpp – ESP32 TAG
 */

#include <Arduino.h>
#include "esp32-hal-cpu.h"
#include "config.h"
#include "tag.h"

// CPU-Frequenz so weit wie möglich reduzieren.
// Fallback-Kette falls 10 MHz nicht unterstützt wird.
static void reduceCpuClock() {
    if (setCpuFrequencyMhz(10)) return;
    if (setCpuFrequencyMhz(13)) return;
    if (setCpuFrequencyMhz(20)) return;
    if (setCpuFrequencyMhz(26)) return;
}

void setup() {
    // CPU zuerst – danach UART-Timing korrekt berechnet
    reduceCpuClock();

    // Kein Serial.begin() auf dem Tag – spart Strom, kein USB nötig
    // Falls Debug gewünscht: Serial.begin(115200) hier einfügen

    tagInit();
}

void loop() {
    tagUpdate();
}
