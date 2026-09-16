#include "MatrixWidget.h"

MatrixWidget::MatrixWidget(ScreenManager &manager) : Widget(manager), m_display(manager) {}

void MatrixWidget::setup() {
    m_manager.selectAllScreens();
    m_effect.init(&m_display);
    m_effect.setup(3, 15, 3, 15, 100);
    applyColors();
    m_manager.reset();
}

void MatrixWidget::applyColors() {
    m_effect.setTextColor(((m_textColor >> 11) & 31) * 255 / 31,
                         ((m_textColor >> 5) & 63) * 255 / 63,
                         (m_textColor & 31) * 255 / 31);
    m_effect.setHeadCharColor(((m_headColor >> 11) & 31) * 255 / 31,
                             ((m_headColor >> 5) & 63) * 255 / 63,
                             (m_headColor & 31) * 255 / 31);
}

void MatrixWidget::draw(bool force) {
    m_manager.selectAllScreens();
    m_effect.loop(force);
    m_manager.reset();
}

bool MatrixWidget::setColorPreset(uint8_t preset) {
    if (preset > 9) return false;
    // Each pair is trail color, leading-character color.
    static const uint16_t colors[][2] = {
        {MATRIX_TEXT_COLOR, MATRIX_HEAD_COLOR},
        {0x0200, 0x07E0}, // dark green / green
        {0x4000, 0xF800}, // dark red / red
        {0x001F, 0x07FF}, // blue / cyan
        {0x07FF, 0x001F}, // cyan / blue
        {0x8010, 0xFC18}, // purple / pink
        {0xFD20, 0xFFE0}, // orange / yellow
        {0xF800, 0xFD20}, // red / orange
        {0xFC18, 0x8010}, // pink / purple
        {0x8410, 0xFFFF}  // gray / white
    };
    m_textColor = colors[preset][0];
    m_headColor = colors[preset][1];
    applyColors();
    draw(true);
    return true;
}