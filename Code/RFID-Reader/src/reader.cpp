/**
 * reader.cpp - Zustandsautomat des Readers.
 *
 * Dieser Reader arbeitet jetzt mit Manchester-kodierten Nutzdaten:
 * - logisch wird weiterhin mit einzelnen Bytes gearbeitet
 * - physisch werden pro logischem Byte zwei UART-Bytes uebertragen
 *
 * Manchester-Kodierung:
 *   Datenbit 0 -> 10
 *   Datenbit 1 -> 01
 *
 * Reader-Ablauf:
 * 1. Auf logisches Startbyte 0xAA warten.
 * 2. ACK 0xAC senden.
 * 3. Im aktiven Modus ein logisches Byte empfangen.
 * 4. Nach 3 Sekunden ein Antwortbyte aus der B-Reihe senden.
 * 5. Bei 4 Sekunden Inaktivitaet oder drei 0x00 hintereinander resetten.
 */

#include "reader.h"
#include "config.h"
#include "manchester.h"

#include <Arduino.h>
#include <HardwareSerial.h>

static HardwareSerial TagSerial(UART_PORT);

enum class ReaderState : uint8_t {
    WaitForStart,
    SendAck,
    WaitForByte,
    WaitBeforeReply
};

static ReaderState s_state          = ReaderState::WaitForStart;
static uint32_t    s_replyTimeMs    = 0;
static uint32_t    s_timeoutMs      = 0;
static uint32_t    s_rxLedUntilMs   = 0;
static uint32_t    s_txLedUntilMs   = 0;
static uint8_t     s_replyByte      = REPLY_BYTE_START;
static uint8_t     s_zeroCount      = 0;
static bool        s_ledToggleMode  = false;
static bool        s_rxLedLatched   = false;
static bool        s_txLedLatched   = false;

static void printByte(const char* prefix, const uint8_t value) {
    Serial.print(prefix);
    Serial.print("0x");
    if (value < 0x10) {
        Serial.print('0');
    }
    Serial.print(value, HEX);
    Serial.print(" (dec ");
    Serial.print(value);
    Serial.println(")");
}

static bool timeoutExpired(const uint32_t deadlineMs) {
    return (int32_t)(millis() - deadlineMs) >= 0;
}

static void applyLatchedLedStates() {
    digitalWrite(RX_ACTIVITY_LED_PIN, s_rxLedLatched ? HIGH : LOW);
    digitalWrite(TX_ACTIVITY_LED_PIN, s_txLedLatched ? HIGH : LOW);
}

static void startLedPulse(const int pin, uint32_t& untilMs) {
    if (s_ledToggleMode) {
        return;
    }

    digitalWrite(pin, HIGH);
    untilMs = millis() + ACTIVITY_LED_MS;
}

static void updateLedPulse(const int pin, uint32_t& untilMs) {
    if (untilMs != 0 && timeoutExpired(untilMs)) {
        digitalWrite(pin, LOW);
        untilMs = 0;
    }
}

static void updateActivityLeds() {
    if (s_ledToggleMode) {
        applyLatchedLedStates();
        return;
    }

    updateLedPulse(RX_ACTIVITY_LED_PIN, s_rxLedUntilMs);
    updateLedPulse(TX_ACTIVITY_LED_PIN, s_txLedUntilMs);
}

static void toggleLedOverrideMode() {
    s_ledToggleMode = !s_ledToggleMode;
    s_rxLedLatched = !s_rxLedLatched;
    s_txLedLatched = !s_txLedLatched;
    s_rxLedUntilMs = 0;
    s_txLedUntilMs = 0;
    applyLatchedLedStates();
}

static void noteReceivedByte(const uint8_t value) {
    startLedPulse(RX_ACTIVITY_LED_PIN, s_rxLedUntilMs);
    printByte("[READER] Received:   ", value);
}

// Liest genau ein logisches Manchester-Byte. Erst wenn zwei UART-Bytes voll
// vorliegen und korrekt dekodiert werden koennen, wird true geliefert.
static bool readManchesterByte(uint8_t& valueOut) {
    if (TagSerial.available() < 2) {
        return false;
    }

    const uint8_t hi = static_cast<uint8_t>(TagSerial.read());
    const uint8_t lo = static_cast<uint8_t>(TagSerial.read());
    const uint16_t encoded = static_cast<uint16_t>(hi << 8) | lo;

    if (!manchesterDecodeWord(encoded, valueOut)) {
        Serial.print("[READER] Invalid Manchester word: 0x");
        Serial.println(encoded, HEX);
        return false;
    }

    return true;
}

// Sendet ein logisches Byte als zwei UART-Bytes mit Manchester-Kodierung.
static void sendManchesterByte(const uint8_t value) {
    const uint16_t encoded = manchesterEncodeByte(value);
    const uint8_t hi = static_cast<uint8_t>((encoded >> 8) & 0xFF);
    const uint8_t lo = static_cast<uint8_t>(encoded & 0xFF);

    TagSerial.write(hi);
    TagSerial.write(lo);
    TagSerial.flush();

    // Eventuelles Echo entfernen, damit das eigene Senden nicht als Empfang
    // interpretiert wird.
    while (TagSerial.available() > 0) {
        TagSerial.read();
    }

    startLedPulse(TX_ACTIVITY_LED_PIN, s_txLedUntilMs);
    printByte("[READER] Sent:       ", value);
}

static void advanceReplyByte() {
    if (s_replyByte >= 0xBF) {
        s_replyByte = REPLY_BYTE_START;
    } else {
        s_replyByte++;
    }
}

static void clearInputBuffer() {
    while (TagSerial.available() > 0) {
        TagSerial.read();
    }
}

static void resetToStart(const char* reason) {
    Serial.println(reason);
    clearInputBuffer();
    s_replyByte = REPLY_BYTE_START;
    s_replyTimeMs = 0;
    s_timeoutMs = 0;
    s_zeroCount = 0;
    s_state = ReaderState::WaitForStart;
}

void readerInit() {
    pinMode(RX_ACTIVITY_LED_PIN, OUTPUT);
    pinMode(TX_ACTIVITY_LED_PIN, OUTPUT);
    applyLatchedLedStates();

    TagSerial.begin(BAUD_RATE, UART_CONFIG, UART_RX_PIN, UART_TX_PIN);
    clearInputBuffer();

    Serial.println("UART2 ready: 2400-8-N-1 for Manchester transport");
    Serial.printf("  RX: GPIO %d\n", UART_RX_PIN);
    Serial.printf("  TX: GPIO %d\n", UART_TX_PIN);
    Serial.printf("  RX LED: GPIO %d\n", RX_ACTIVITY_LED_PIN);
    Serial.printf("  TX LED: GPIO %d\n", TX_ACTIVITY_LED_PIN);
    Serial.println("Waiting for logical start byte 0xAA...");
}

void readerUpdate() {
    updateActivityLeds();

    switch (s_state) {
        case ReaderState::WaitForStart: {
            uint8_t received = 0;
            if (readManchesterByte(received)) {
                noteReceivedByte(received);

                if (received == START_BYTE) {
                    Serial.println("[READER] Start byte OK, sending ACK.");
                    s_state = ReaderState::SendAck;
                } else {
                    Serial.println("[READER] Wrong start byte, ignored.");
                }
            }
            break;
        }

        case ReaderState::SendAck:
            sendManchesterByte(ACK_BYTE);
            Serial.println("[READER] Active mode started.");
            s_timeoutMs = millis() + TAG_TIMEOUT_MS;
            s_zeroCount = 0;
            s_state = ReaderState::WaitForByte;
            break;

        case ReaderState::WaitForByte: {
            uint8_t received = 0;
            if (readManchesterByte(received)) {
                noteReceivedByte(received);

                if (received == LED_TOGGLE_BYTE) {
                    toggleLedOverrideMode();
                    Serial.println("[READER] LED toggle mode changed.");
                    s_zeroCount = 0;
                }

                if (received == 0x00) {
                    s_zeroCount++;
                    if (s_zeroCount >= 3) {
                        resetToStart("[READER] Three 0x00 bytes in a row, waiting for 0xAA again.");
                        break;
                    }
                } else {
                    s_zeroCount = 0;
                }

                s_replyTimeMs = millis() + REPLY_DELAY_MS;
                s_timeoutMs = millis() + TAG_TIMEOUT_MS;
                s_state = ReaderState::WaitBeforeReply;
            } else if (timeoutExpired(s_timeoutMs)) {
                resetToStart("[READER] Timeout in WaitForByte, waiting for 0xAA again.");
            }
            break;
        }

        case ReaderState::WaitBeforeReply:
            if (timeoutExpired(s_timeoutMs)) {
                resetToStart("[READER] Timeout during reply delay, waiting for 0xAA again.");
            } else if (timeoutExpired(s_replyTimeMs)) {
                sendManchesterByte(s_replyByte);
                advanceReplyByte();
                s_timeoutMs = millis() + TAG_TIMEOUT_MS;
                s_state = ReaderState::WaitForByte;
            }
            break;

        default:
            resetToStart("[READER] Invalid state, resetting.");
            break;
    }
}
