/**
 * main.cpp – ESP32 TAG
 */

#include <Arduino.h>
#include "esp32-hal-cpu.h"
#include "config.h"
#include "tag.h"
#include "led.h"




void setup() {
    // CPU-Frequenz so weit wie möglich reduzieren.
    ledInit();
    tagInit();
}
 
void loop() {
    updateLeds();
    tagUpdate();
}
 
