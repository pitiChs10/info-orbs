#include "5zonewidget/5ZoneWidget.h"
#include "GlobalResources.h"
#include "MainHelper.h"
#include "IRRemote.h"
#include "clockwidget/ClockWidget.h"
#include "matrixwidget/MatrixWidget.h"
#include "mqttwidget/MQTTWidget.h"
#include "parqetwidget/ParqetWidget.h"
#include "stockwidget/StockWidget.h"
#include "weatherwidget/WeatherWidget.h"
#include "webdatawidget/WebDataWidget.h"
#include "wifiwidget/WifiWidget.h"
#if INCLUDE_CUSTOM_IMAGE != WIDGET_DISABLED
    #include "CustomImageWidget/CustomImageWidget.h"
#endif
#include <ArduinoLog.h>

TFT_eSPI tft = TFT_eSPI();

GlobalTime *globalTime{nullptr};
WifiWidget *wifiWidget{nullptr};
ScreenManager *sm{nullptr};
ConfigManager *config{nullptr};
OrbsWiFiManager *wifiManager{nullptr};
WidgetSet *widgetSet{nullptr};

void addWidgets() {
    // Always add clock
    widgetSet->add(new ClockWidget(*sm, *config));

#if INCLUDE_CUSTOM_IMAGE != WIDGET_DISABLED
    widgetSet->add(new CustomImageWidget(*sm, *config));
#endif

#if INCLUDE_WEATHER != WIDGET_DISABLED
    widgetSet->add(new WeatherWidget(*sm, *config));
#endif

#if INCLUDE_STOCK != WIDGET_DISABLED
    widgetSet->add(new StockWidget(*sm, *config));
#endif
#if INCLUDE_PARQET != WIDGET_DISABLED
    widgetSet->add(new ParqetWidget(*sm, *config));
#endif
#if INCLUDE_WEBDATA != WIDGET_DISABLED
    #ifdef WEB_DATA_WIDGET_URL
    widgetSet->add(new WebDataWidget(*sm, *config, WEB_DATA_WIDGET_URL));
    #endif
    #ifdef WEB_DATA_STOCK_WIDGET_URL
    widgetSet->add(new WebDataWidget(*sm, *config, WEB_DATA_STOCK_WIDGET_URL));
    #endif
#endif
#if INCLUDE_MQTT != WIDGET_DISABLED
    widgetSet->add(new MQTTWidget(*sm, *config));
#endif
#if INCLUDE_5ZONE != WIDGET_DISABLED
    widgetSet->add(new FiveZoneWidget(*sm, *config));
#endif
#if INCLUDE_MATRIXSCREEN != WIDGET_DISABLED
    widgetSet->add(new MatrixWidget(*sm, *config));
#endif
}

void setup() {
    // Initialize global resources
    initializeGlobalResources();

#ifdef SERIAL_INTERFACE_INIT_DELAY
    // Add a delay to allow the serial interface to initialize
    delay(SERIAL_INTERFACE_INIT_DELAY);
#endif

    Serial.begin(115200);

    // Clear the serial buffer of any garbage
    while (Serial.available() > 0) {
        Serial.read();
    }

#ifdef LOG_TIMESTAMP
    Log.setPrefix(MainHelper::printPrefix);
#endif
    Log.begin(LOG_LEVEL, &Serial);
    Log.noticeln("🚀 Starting up...");
    Log.noticeln("PCB Version: %s", PCB_VERSION);

    wifiManager = new OrbsWiFiManager();
    config = new ConfigManager(*wifiManager);
    sm = new ScreenManager(tft);
    widgetSet = new WidgetSet(sm);

    // Pass references to MainHelper
    MainHelper::init(wifiManager, config, sm, widgetSet);
    MainHelper::setupLittleFS();
    MainHelper::setupConfig();
    MainHelper::setupButtons();
    MainHelper::showWelcome();

    pinMode(BUSY_PIN, OUTPUT);
    Log.noticeln("Connecting to WiFi");
    wifiWidget = new WifiWidget(*sm, *config, *wifiManager);
    wifiWidget->setup();

    globalTime = GlobalTime::getInstance();
    addWidgets();
    config->setupWebPortal();
    MainHelper::resetCycleTimer();
    setupRemote();
}

void checkRemote() {
    const RemoteAction action = readRemote();
    if (action == RemoteAction::None || !wifiWidget->isConnected() || !widgetSet->initialUpdateDone()) {
        return;
    }
    if (action >= RemoteAction::Digit0 && action <= RemoteAction::Digit9) {
        const uint8_t preset = static_cast<uint8_t>(action) - static_cast<uint8_t>(RemoteAction::Digit0);
        if (widgetSet->getCurrent()->setColorPreset(preset)) {
            MainHelper::resetCycleTimer();
            Serial.printf("IR digit %u -> color preset\n", preset);
        }
        return;
    }
    switch (action) {
        case RemoteAction::Previous: MainHelper::buttonPressed(BUTTON_LEFT, BTN_SHORT); break;
        case RemoteAction::Next: MainHelper::buttonPressed(BUTTON_RIGHT, BTN_SHORT); break;
        case RemoteAction::OK: MainHelper::buttonPressed(BUTTON_MIDDLE, BTN_SHORT); break;
        default: break;
    }
}

void loop() {
    checkRemote();
    MainHelper::watchdogReset();
    if (wifiWidget->isConnected() == false) {
        wifiWidget->update();
        wifiWidget->draw();
        widgetSet->setClearScreensOnDrawCurrent(); // Clear screen after wifiWidget
        delay(100);
    } else {
        if (!widgetSet->initialUpdateDone()) {
            widgetSet->initializeAllWidgetsData();
            MainHelper::setupWebPortalEndpoints();
        }
        globalTime->updateTime();

        MainHelper::checkButtons();

        widgetSet->updateCurrent();
        MainHelper::updateBrightnessByTime(globalTime->getHour24());
        widgetSet->drawCurrent();

        MainHelper::checkCycleWidgets();
        wifiManager->process();
        TaskManager::getInstance()->processAwaitingTasks();
        TaskManager::getInstance()->processTaskResponses();
    }
#ifdef MEMORY_DEBUG_INTERVAL
    ShowMemoryUsage::printSerial();
#endif
    MainHelper::restartIfNecessary();
}
