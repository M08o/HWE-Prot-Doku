/**
 * ============================================================
 *  config.h – ESP32 TAG
 *
 *  PINBELEGUNG:
 *  ┌─────────────────────────────────────────────────┐
 *  │  GPIO 16  →  UART2 RX  (Empfang vom Reader)     │
 *  │  GPIO 17  →  UART2 TX  (Senden zum Reader)      │
 *  │                                                  │
 *  │  LEDs invertiert direkt an UART-Leitungen:      │
 *  │  3.3V ── 470Ω ── LED ── GPIO 17  (TX-LED)       │
 *  │  3.3V ── 470Ω ── LED ── GPIO 16  (RX-LED)       │
 *  └─────────────────────────────────────────────────┘
 *
 *  CPU-FREQUENZ: 10 MHz (kleinste offizielle ESP32-Stufe)
 *  UART: 1200 Baud, 8 Datenbits, No Parity, 1 Stopbit
 * ============================================================
 */

#pragma once

#include <cstdint>
#include <Arduino.h>
// ─── CPU ─────────────────────────────────────────────────────
// Fallback-Kette: 10 → 13 → 20 → 26 MHz
static constexpr uint32_t CPU_TARGET_MHZ     = 10;

// ─── UART ────────────────────────────────────────────────────
static constexpr int      UART_RX_PIN        = 16;
static constexpr int      UART_TX_PIN        = 17;
static constexpr uint32_t BAUD_RATE          = 2400;
static constexpr uint32_t UART_CONFIG        = SERIAL_8N1;
static constexpr uint8_t  UART_PORT          = 2;

// ─── LED ─────────────────────────────────────────────────────
static constexpr int      LED_TX_PIN         = 18;
static constexpr int      LED_RX_PIN         = 19;
static constexpr int      INTERANL_LED_PIN   = 2;   // Interne LED des ESP32 (GPIO2)
static constexpr uint32_t LED_PULSE_MS       = 80;

// ─── PROTOKOLL ───────────────────────────────────────────────
static constexpr uint8_t  START_BYTE         = 0xAA;  // Startbyte das Tag sendet
static constexpr uint8_t  ACK_BYTE           = 0xAC;  // Erwartetes ACK vom Reader
static constexpr uint8_t  CMD_TOGGLE_LEDS    = 0xF0;  // Befehl: beide LEDs toggeln
static constexpr uint8_t  DATA_BYTE_MIN      = 0xC0;  // Nutzdaten-Bereich Start
static constexpr uint8_t  DATA_BYTE_MAX      = 0xCF;  // Nutzdaten-Bereich Ende
static constexpr uint32_t REPLY_DELAY_MS     = 3000;
static constexpr uint32_t ACK_TIMEOUT_MS     = 4000;
