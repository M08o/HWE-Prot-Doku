/**
 * led.cpp – LED-Ansteuerung für TX und RX (Tag)
 */

#include "led.h"
#include "config.h"
#include <Arduino.h>

static volatile unsigned long s_txLedOffTime = 0;
static volatile unsigned long s_rxLedOffTime = 0;

void ledInit() {
    pinMode(LED_TX_PIN, OUTPUT);
    pinMode(LED_RX_PIN, OUTPUT);
    pinMode(INTERANL_LED_PIN, OUTPUT);
    digitalWrite(LED_TX_PIN, LOW);
    digitalWrite(LED_RX_PIN, LOW);
    digitalWrite(INTERANL_LED_PIN, LOW);
}

void triggerTxLed() {
    digitalWrite(LED_TX_PIN, HIGH);
    s_txLedOffTime = millis() + LED_PULSE_MS;
}

void triggerRxLed() {
    digitalWrite(LED_RX_PIN, HIGH);
    s_rxLedOffTime = millis() + LED_PULSE_MS;
}

void updateLeds() {
    const unsigned long now = millis();

    if (s_txLedOffTime != 0 && now >= s_txLedOffTime) {
        digitalWrite(LED_TX_PIN, LOW);
        s_txLedOffTime = 0;
    }
    if (s_rxLedOffTime != 0 && now >= s_rxLedOffTime) {
        digitalWrite(LED_RX_PIN, LOW);
        s_rxLedOffTime = 0;
    }
}
