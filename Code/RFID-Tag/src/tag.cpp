/**
 * tag.cpp – Zustandsautomat des Tags
 */

#include "tag.h"
#include "config.h"
#include <Arduino.h>
#include <HardwareSerial.h>

static HardwareSerial ReaderSerial(UART_PORT);

enum class TagState : uint8_t {
    WaitForByte,
    WaitBeforeReply
};

static TagState s_state          = TagState::WaitForByte;
static uint32_t s_replyTimeMs    = 0;
static uint8_t  s_nextByte       = TAG_BYTE_START;

// ─── Hilfsfunktion: Byte senden ──────────────────────────────
static void sendByte(const uint8_t value) {
    ReaderSerial.write(value);
    ReaderSerial.flush();
}

// ─── Init ────────────────────────────────────────────────────
void tagInit() {
    ReaderSerial.begin(BAUD_RATE, UART_CONFIG, UART_RX_PIN, UART_TX_PIN);

    // Sofort erstes Byte senden – kein delay, kein Warten
    sendByte(s_nextByte);
    s_nextByte++;
}

// ─── Zustandsautomat ─────────────────────────────────────────
void tagUpdate() {
    switch (s_state) {

        case TagState::WaitForByte:
            if (ReaderSerial.available() > 0) {
                // Byte lesen (Inhalt für Demo nicht weiter verwendet)
                (void)ReaderSerial.read();

                s_replyTimeMs = millis() + REPLY_DELAY_MS;
                s_state = TagState::WaitBeforeReply;
            }
            break;

        case TagState::WaitBeforeReply:
            // Overflow-sicherer Vergleich
            if ((int32_t)(millis() - s_replyTimeMs) >= 0) {
                sendByte(s_nextByte);
                s_nextByte++;
                s_state = TagState::WaitForByte;
            }
            break;

        default:
            s_state = TagState::WaitForByte;
            break;
    }
}
