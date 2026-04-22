/**
 * main.cpp – ESP32 LESEGERÄT
 */

#include <Arduino.h>
#include "config.h"
#include "reader.h"

void setup() {
    Serial.begin(MONITOR_BAUD);
    delay(200);

    Serial.println();
    Serial.println("esp-lesegeraet startet...");

    readerInit();
}

void loop() {
    readerUpdate();
}
