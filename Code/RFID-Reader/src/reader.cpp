/**
 * reader.cpp – Zustandsautomat des Lesegeräts
 */

#include "reader.h"
#include "config.h"
#include <Arduino.h>
#include <HardwareSerial.h>

static HardwareSerial TagSerial(UART_PORT);

enum class ReaderState : uint8_t {
    WaitForByte,
    WaitBeforeReply
};

static ReaderState s_state       = ReaderState::WaitForByte;
static uint32_t    s_replyTimeMs = 0;
static uint8_t     s_replyByte   = REPLY_BYTE_START;

// ─── Hilfsfunktion: Byte formatiert ausgeben ─────────────────
static void printByte(const char* prefix, const uint8_t value) {
    Serial.print(prefix);
    Serial.print("0x");
    if (value < 0x10) Serial.print('0');
    Serial.print(value, HEX);
    Serial.print(" (dez ");
    Serial.print(value);
    Serial.println(")");
}

// ─── Hilfsfunktion: Byte senden ──────────────────────────────
static void sendByte(const uint8_t value) {
    TagSerial.write(value);
    TagSerial.flush();
    printByte("[READER] Gesendet:   ", value);
}

// ─── Init ────────────────────────────────────────────────────
void readerInit() {
    TagSerial.begin(BAUD_RATE, UART_CONFIG, UART_RX_PIN, UART_TX_PIN);

    Serial.println("UART2 bereit: 1200-8-N-1");
    Serial.printf("  RX: GPIO %d\n", UART_RX_PIN);
    Serial.printf("  TX: GPIO %d\n", UART_TX_PIN);
    Serial.println("Warte auf Tag...");
}

// ─── Zustandsautomat ─────────────────────────────────────────
void readerUpdate() {
    switch (s_state) {

        case ReaderState::WaitForByte:
            if (TagSerial.available() > 0) {
                const uint8_t received = static_cast<uint8_t>(TagSerial.read());
                printByte("[READER] Empfangen:  ", received);

                // Zeitstempel setzen, dann nicht-blockierend warten
                s_replyTimeMs = millis() + REPLY_DELAY_MS;
                s_state = ReaderState::WaitBeforeReply;
            }
            break;

        case ReaderState::WaitBeforeReply:
            // Overflow-sicherer Vergleich
            if ((int32_t)(millis() - s_replyTimeMs) >= 0) {
                sendByte(s_replyByte);
                s_replyByte++;
                s_state = ReaderState::WaitForByte;
            }
            break;

        default:
            s_state = ReaderState::WaitForByte;
            break;
    }
}
