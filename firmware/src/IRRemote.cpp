#include "IRRemote.h"

#include <Arduino.h>

#ifndef IR_REMOTE_ENABLED
#define IR_REMOTE_ENABLED 1
#endif
#ifndef IR_REMOTE_PIN
#define IR_REMOTE_PIN 22
#endif
#ifndef IR_REMOTE_ADDRESS
#define IR_REMOTE_ADDRESS 0x00
#endif



#if IR_REMOTE_ENABLED
#define DECODE_NEC
#define DISABLE_CODE_FOR_SEND
#include <IRremote.hpp>
#endif

void setupRemote() {
#if IR_REMOTE_ENABLED
    IrReceiver.begin(IR_REMOTE_PIN, DISABLE_LED_FEEDBACK);
    Serial.printf("IR remote ready: GPIO%d, NEC address 0x%X\n", IR_REMOTE_PIN, IR_REMOTE_ADDRESS);
#endif
}

RemoteAction readRemote() {
#if IR_REMOTE_ENABLED
    if (!IrReceiver.decode()) {
        return RemoteAction::None;
    }
    // Log every decoded frame for remote diagnostics.
    IrReceiver.printIRResultShort(&Serial);
    const auto protocol = IrReceiver.decodedIRData.protocol;
    const auto address = IrReceiver.decodedIRData.address;
    const auto command = IrReceiver.decodedIRData.command;
    const auto flags = IrReceiver.decodedIRData.flags;
    // Resume reception before calling widget code, which may perform network requests.
    IrReceiver.resume();
    if (protocol != NEC || address != IR_REMOTE_ADDRESS ||
        (flags & (IRDATA_FLAGS_IS_REPEAT | IRDATA_FLAGS_PARITY_FAILED | IRDATA_FLAGS_WAS_OVERFLOW))) {
        return RemoteAction::None;
    }
    switch (command) {
        case 0x44: return RemoteAction::Previous;
        case 0x43: return RemoteAction::Next;
        case 0x40: return RemoteAction::OK;
        case 0x16: return RemoteAction::Digit0;
        case 0x0C: return RemoteAction::Digit1;
        case 0x18: return RemoteAction::Digit2;
        case 0x5E: return RemoteAction::Digit3;
        case 0x08: return RemoteAction::Digit4;
        case 0x1C: return RemoteAction::Digit5;
        case 0x5A: return RemoteAction::Digit6;
        case 0x42: return RemoteAction::Digit7;
        case 0x52: return RemoteAction::Digit8;
        case 0x4A: return RemoteAction::Digit9;
        default: break;
    }
#endif
    return RemoteAction::None;
}