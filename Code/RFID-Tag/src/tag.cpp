#include "tag.h"

#include "config.h"
#include "led.h"
#include "manchester.h"

#include <Arduino.h>
#include <HardwareSerial.h>

static HardwareSerial ReaderSerial(UART_PORT);

enum class TagState : uint8_t {
    SendStart,
    WaitForAck,
    WaitForByte,
    WaitBeforeReply
};

static TagState s_state = TagState::SendStart;
static uint32_t s_deadlineMs = 0;
static uint8_t s_dataByte = DATA_BYTE_MIN;
static uint8_t s_sendCount = 0;

static void resetHandshake() {
    s_state = TagState::SendStart;
    s_deadlineMs = 0;
}

static uint8_t nextPayload() {
    ++s_sendCount;

    if (s_sendCount >= 5) {
        s_sendCount = 0;
        return CMD_TOGGLE_LEDS;
    }

    const uint8_t value = s_dataByte;
    s_dataByte = (s_dataByte >= DATA_BYTE_MAX)
        ? DATA_BYTE_MIN
        : static_cast<uint8_t>(s_dataByte + 1u);
    return value;
}

static void sendByte(const uint8_t value) {
    triggerTxLed();
    manchesterWriteByte(ReaderSerial, value);
}

void tagInit() {
    ReaderSerial.begin(BAUD_RATE, UART_CONFIG, UART_RX_PIN, UART_TX_PIN);
}

void tagUpdate() {
    uint8_t received = 0;

    switch (s_state) {
        case TagState::SendStart:
            sendByte(START_BYTE);
            s_deadlineMs = millis() + ACK_TIMEOUT_MS;
            s_state = TagState::WaitForAck;
            break;

        case TagState::WaitForAck:
            if (manchesterReadByte(ReaderSerial, received)) {
                triggerRxLed();

                if (received == ACK_BYTE) {
                    sendByte(nextPayload());
                    s_state = TagState::WaitForByte;
                } else {
                    resetHandshake();
                }
            } else if (static_cast<int32_t>(millis() - s_deadlineMs) >= 0) {
                resetHandshake();
            }
            break;

        case TagState::WaitForByte:
            if (manchesterReadByte(ReaderSerial, received)) {
                triggerRxLed();
                s_deadlineMs = millis() + REPLY_DELAY_MS;
                s_state = TagState::WaitBeforeReply;
            }
            break;

        case TagState::WaitBeforeReply:
            if (static_cast<int32_t>(millis() - s_deadlineMs) >= 0) {
                sendByte(nextPayload());
                s_state = TagState::WaitForByte;
            }
            break;
    }
}
