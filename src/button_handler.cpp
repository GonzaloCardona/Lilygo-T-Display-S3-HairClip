#include "button_handler.h"

ButtonHandler::ButtonHandler()
    : _lastBootState(HIGH),
      _bootPressStart(0),
      _bootHandled(false),
      _lastUserState(HIGH),
      _userPressStart(0),
      _userHandled(false) {}

void ButtonHandler::begin() {
    pinMode(PIN_BUTTON_BOOT, INPUT_PULLUP);
    pinMode(PIN_BUTTON_USER, INPUT_PULLUP);
}

void ButtonHandler::onConfigPress(std::function<void()> callback) {
    _configCallback = callback;
}

void ButtonHandler::onSleepPress(std::function<void()> callback) {
    _sleepCallback = callback;
}

void ButtonHandler::update() {
    unsigned long now = millis();

    // 1. Lectura del botón BOOT (GPIO 0) -> Modo Configuración
    bool currentBootState = digitalRead(PIN_BUTTON_BOOT);
    if (currentBootState == LOW && _lastBootState == HIGH) {
        // Acaba de pulsarse
        _bootPressStart = now;
        _bootHandled = false;
    } else if (currentBootState == LOW && !_bootHandled) {
        if (now - _bootPressStart >= BUTTON_DEBOUNCE_MS) {
            _bootHandled = true;
            if (_configCallback) {
                _configCallback();
            }
        }
    }
    _lastBootState = currentBootState;

    // 2. Lectura del botón USER (GPIO 14) -> Deep Sleep (Pulsación larga)
    bool currentUserState = digitalRead(PIN_BUTTON_USER);
    if (currentUserState == LOW && _lastUserState == HIGH) {
        // Acaba de pulsarse
        _userPressStart = now;
        _userHandled = false;
    } else if (currentUserState == LOW && !_userHandled) {
        if (now - _userPressStart >= LONG_PRESS_MS) {
            _userHandled = true;
            if (_sleepCallback) {
                _sleepCallback();
            }
        }
    }
    _lastUserState = currentUserState;
}
