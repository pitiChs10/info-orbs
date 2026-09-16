#include <Arduino.h>
#include <IRremote.hpp>

constexpr uint8_t kReceiverPin = 22;
uint32_t receivedFrames = 0;
uint32_t lastStatus = 0;
volatile uint32_t signalEdges = 0;
uint32_t previousEdges = 0;

// Count electrical transitions independently of protocol decoding.
void IRAM_ATTR onSignalEdge() {
    ++signalEdges;
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    IrReceiver.begin(kReceiverPin, DISABLE_LED_FEEDBACK);
    attachInterrupt(kReceiverPin, onSignalEdge, CHANGE);
    Serial.println("\nKY-022 test v2 (edge counter): VCC=3V3, GND=GND, S=GPIO22");
    Serial.println("Point an IR remote at the receiver and press a button.");
    Serial.println("The Orbs displays are not initialized by this test.");
}

void loop() {
    if (IrReceiver.decode()) {
        ++receivedFrames;
        Serial.print("Frame #");
        Serial.print(receivedFrames);
        Serial.print(": ");
        IrReceiver.printIRResultShort(&Serial);
        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW) {
            Serial.println("Receive buffer overflow: try a short button press.");
        } else if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
            Serial.println("Signal detected, but protocol unknown (or noise).");
        }
        IrReceiver.resume();
    }

    if (millis() - lastStatus >= 2000) {
        lastStatus = millis();
        Serial.print("Alive | GPIO22=");
        Serial.print(digitalRead(kReceiverPin) ? "HIGH" : "LOW");
        Serial.print(" | received frames=");
        Serial.print(receivedFrames);
        const uint32_t currentEdges = signalEdges;
        Serial.print(" | edges/2s=");
        Serial.print(currentEdges - previousEdges);
        Serial.print(" | total edges=");
        Serial.println(currentEdges);
        previousEdges = currentEdges;
    }
    delay(1);
}
