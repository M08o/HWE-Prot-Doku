/**
 * ============================================================
 *  config.h – ESP32 LESEGERÄT
 *
 *  PINBELEGUNG:
 *  ┌─────────────────────────────────────────────────┐
 *  │  GPIO 16  →  UART2 RX  (Empfang vom Tag)        │
 *  │  GPIO 17  →  UART2 TX  (Senden zum Tag)         │
 *  │                                                  │
 *  │  LEDs invertiert direkt an UART-Leitungen:      │
 *  │  3.3V ── 470Ω ── LED ── GPIO 17  (TX-LED)       │
 *  │  3.3V ── 470Ω ── LED ── GPIO 16  (RX-LED)       │
 *  └─────────────────────────────────────────────────┘
 *
 *  UART: 1200 Baud, 8 Datenbits, No Parity, 1 Stopbit
 * ============================================================
 */

#pragma once

#include <cstdint>
#include <Arduino.h>

// ─── UART ────────────────────────────────────────────────────
static constexpr int      UART_RX_PIN        = 16;
static constexpr int      UART_TX_PIN        = 17;
static constexpr uint32_t BAUD_RATE          = 1200;
static constexpr uint32_t UART_CONFIG        = SERIAL_8N1;
static constexpr uint8_t  UART_PORT          = 2;

// ─── PROTOKOLL ───────────────────────────────────────────────
static constexpr uint32_t REPLY_DELAY_MS     = 3000;
static constexpr uint8_t  REPLY_BYTE_START   = 0xA0;

// ─── MONITOR ─────────────────────────────────────────────────
static constexpr uint32_t MONITOR_BAUD       = 115200;
