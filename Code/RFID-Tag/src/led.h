/**
 * led.h – LED-Ansteuerung für TX und RX (Tag)
 *
 *  LED_TX_PIN: leuchtet kurz wenn ein Byte gesendet wird
 *  LED_RX_PIN: leuchtet kurz wenn ein Byte empfangen wird
 */

#pragma once

void ledInit();
void triggerTxLed();
void triggerRxLed();
void updateLeds();
