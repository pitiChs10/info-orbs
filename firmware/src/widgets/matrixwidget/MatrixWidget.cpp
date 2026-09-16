#include "MatrixWidget.h"
#include "MatrixTranslations.h"

MatrixWidget::MatrixWidget(ScreenManager &manager, ConfigManager &config) : Widget(manager, config) {
    m_enabled = (INCLUDE_MATRIXSCREEN == WIDGET_ON);
    m_config.addConfigBool("MatrixWidget", "mtxEnabled", &m_enabled, t_enableWidget);
    m_config.addConfigBool("MatrixWidget", "mtxBigFont", &m_bigFont, t_matrixBigFont, false);
    m_config.addConfigColor("MatrixWidget", "mtxTextColor", &m_textColor, t_matrixTextColor, false);
    m_config.addConfigColor("MatrixWidget", "mtxHeadTxColor", &m_headTextColor, t_matrixHeadTextColor, false);
    m_config.addConfigInt("MatrixWidget", "mtxLineMin", &m_lineMin, t_matrixLineMin, true);
    m_config.addConfigInt("MatrixWidget", "mtxLineMax", &m_lineMax, t_matrixLineMax, true);
    m_config.addConfigInt("MatrixWidget", "mtxSpeedMin", &m_speedMin, t_matrixSpeedMin, true);
    m_config.addConfigInt("MatrixWidget", "mtxSpeedMax", &m_speedMax, t_matrixSpeedMax, true);
    m_config.addConfigInt("MatrixWidget", "mtxUpdateInt", &m_updateInterval, t_matrixUpdateInterval, true);
}

void MatrixWidget::setup() {
    m_manager.selectAllScreens();
    matrix_effect.init(&m_manager, m_bigFont, false);
    matrix_effect.setup(m_lineMin, m_lineMax, m_speedMin, m_speedMax, m_updateInterval);
    applyColors();
    m_manager.reset();
}

void MatrixWidget::applyColors() {
    static const uint16_t colors[][2] = {
        {0, 0}, {0x0200, 0x07E0}, {0x4000, 0xF800}, {0x001F, 0x07FF},
        {0x07FF, 0x001F}, {0x8010, 0xFC18}, {0xFD20, 0xFFE0},
        {0xF800, 0xFD20}, {0xFC18, 0x8010}, {0x8410, 0xFFFF}
    };
    const uint16_t text = m_remotePreset == 0 ? m_textColor : colors[m_remotePreset][0];
    const uint16_t head = m_remotePreset == 0 ? m_headTextColor : colors[m_remotePreset][1];
    matrix_effect.setTextColor(((text >> 11) & 31) * 255 / 31, ((text >> 5) & 63) * 255 / 63, (text & 31) * 255 / 31);
    matrix_effect.setHeadCharColor(((head >> 11) & 31) * 255 / 31, ((head >> 5) & 63) * 255 / 63, (head & 31) * 255 / 31);
}

bool MatrixWidget::setColorPreset(uint8_t preset) {
    if (preset > 9) return false;
    m_remotePreset = preset;
    applyColors();
    draw(true);
    return true;
}

void MatrixWidget::update(bool force) {
}

void MatrixWidget::draw(bool force) {
    m_manager.selectAllScreens();
    applyColors();
    matrix_effect.loop(force);
    m_manager.reset();
}

void MatrixWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
}

String MatrixWidget::getName() {
    return "Matrix Screen";
}
