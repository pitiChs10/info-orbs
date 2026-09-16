#pragma once

#include "DigitalRainAnimation.hpp"
#include "Widget.h"

#ifndef MATRIX_TEXT_COLOR
#define MATRIX_TEXT_COLOR 0x001F
#endif
#ifndef MATRIX_HEAD_COLOR
#define MATRIX_HEAD_COLOR 0x07FF
#endif

class MatrixWidget : public Widget {
public:
    explicit MatrixWidget(ScreenManager &manager);
    void setup() override;
    void update(bool force = false) override {}
    void draw(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override {}
    String getName() override { return "Matrix Screen"; }
    bool setColorPreset(uint8_t preset) override;
private:
    void applyColors();
    MatrixDisplay m_display;
    DigitalRainAnimation m_effect{};
    uint16_t m_textColor = MATRIX_TEXT_COLOR;
    uint16_t m_headColor = MATRIX_HEAD_COLOR;
};