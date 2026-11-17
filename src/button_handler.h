#pragma once
#include <Arduino.h>
#include <functional>
#include "config.h"

class ButtonHandler {
public:
    ButtonHandler();
    void begin();
    void update();

    void onConfigPress(std::function<void()> callback);
    void onSleepPress(std::function<void()> callback);

private:
    std::function<void()> _configCallback;
    std::function<void()> _sleepCallback;

    // Estado del botón BOOT (GPIO 0)
    bool _lastBootState;
    unsigned long _bootPressStart;
    bool _bootHandled;

    // Estado del botón USER (GPIO 14)
    bool _lastUserState;
    unsigned long _userPressStart;
    bool _userHandled;
};
