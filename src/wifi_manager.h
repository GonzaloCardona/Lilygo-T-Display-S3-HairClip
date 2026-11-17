#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include "config.h"

class WiFiManager {
public:
    WiFiManager();
    void startAP();
    void stopAP();
    void update(); // Procesar peticiones DNS del captive portal
    bool isAPActive() const { return _apActive; }
    String getIP() const;

private:
    DNSServer _dnsServer;
    bool _apActive;
    IPAddress _apIP;
};
