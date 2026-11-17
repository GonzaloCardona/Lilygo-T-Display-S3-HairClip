#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include <functional>
#include "config.h"
#include "display.h"
#include "storage.h"

class HairClipWebServer {
public:
    HairClipWebServer();
    void begin(DisplayManager* display, StorageManager* storage, HairClipConfig* config, std::function<void()> onExitCallback);
    void stop();
    void handleClient();

private:
    WebServer _server;
    DisplayManager* _display;
    StorageManager* _storage;
    HairClipConfig* _config;
    std::function<void()> _onExitCallback;
    bool _running;

    void setupRoutes();
    void handleRoot();
    void handleStatus();
    void handleText();
    void handleSettings();
    void handleApply();
    void handleImageUpload();
};
