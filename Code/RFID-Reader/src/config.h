/**
 * config.h - Zentrale Einstellungen fuer das ESP32-Lesegeraet.
 *
 * Wichtiger Hinweis zur Manchester-Kodierung:
 * Ein logisches Datenbyte wird als zwei UART-Bytes uebertragen.
 * Deshalb laeuft die physische UART mit 2400 Baud, waehrend die logische
 * Nutzdatenrate weiter 1200 Bit/s entspricht.
 */

#pragma once

#include <Arduino.h>
#include <cstdint>

// UART-Verbindung zum Tag.
static constexpr int      UART_RX_PIN          = 16;
static constexpr int      UART_TX_PIN          = 17;
static constexpr uint32_t BAUD_RATE            = 2400;
static constexpr uint32_t UART_CONFIG          = SERIAL_8N1;
static constexpr uint8_t  UART_PORT            = 2;

// Protokoll auf logischer Datenebene.
static constexpr uint8_t  START_BYTE           = 0xAA;
static constexpr uint8_t  ACK_BYTE             = 0xAC;
static constexpr uint8_t  LED_TOGGLE_BYTE      = 0xF0;
static constexpr uint32_t REPLY_DELAY_MS       = 3000;
static constexpr uint8_t  REPLY_BYTE_START     = 0xB0;
static constexpr uint32_t TAG_TIMEOUT_MS       = 4000;

// Aktivitaets-LEDs: kurzer Impuls bei jedem logisch empfangenen oder gesendeten Byte.
static constexpr int      RX_ACTIVITY_LED_PIN  = 18;
static constexpr int      TX_ACTIVITY_LED_PIN  = 19;
static constexpr uint32_t ACTIVITY_LED_MS      = 60;

// Serieller Monitor fuer Debug-Ausgaben am PC.
static constexpr uint32_t MONITOR_BAUD         = 115200;
