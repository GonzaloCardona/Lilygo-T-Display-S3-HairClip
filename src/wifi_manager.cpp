#include "wifi_manager.h"
#include <esp_wifi.h>

WiFiManager::WiFiManager()
    : _apActive(false),
      _apIP(AP_IP_1, AP_IP_2, AP_IP_3, AP_IP_4) {}

void WiFiManager::startAP() {
    if (_apActive) return;

    Serial.println("[WiFi] Iniciando punto de acceso...");
    WiFi.mode(WIFI_AP);
    
    // Configurar IP fija (192.168.4.1)
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(_apIP, _apIP, subnet);

    // Iniciar AP abierto
    WiFi.softAP(AP_SSID, AP_PASSWORD);

    // Iniciar servidor DNS para Captive Portal (redirige todo a 192.168.4.1)
    _dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    _dnsServer.start(DNS_PORT, "*", _apIP);

    _apActive = true;
    Serial.printf("[WiFi] AP activo: '%s', IP: %s\n", AP_SSID, _apIP.toString().c_str());
}

void WiFiManager::stopAP() {
    if (!_apActive) return;

    Serial.println("[WiFi] Apagando punto de acceso y radio...");
    _dnsServer.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    esp_wifi_stop();

    _apActive = false;
    Serial.println("[WiFi] WiFi totalmente apagado para ahorro de batería");
}

void WiFiManager::update() {
    if (_apActive) {
        _dnsServer.processNextRequest();
    }
}

String WiFiManager::getIP() const {
    return _apIP.toString();
}
