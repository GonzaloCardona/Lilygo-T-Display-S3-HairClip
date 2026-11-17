#include <Arduino.h>
#include <esp_sleep.h>
#include <driver/rtc_io.h>

#include "config.h"
#include "display.h"
#include "button_handler.h"
#include "storage.h"
#include "wifi_manager.h"
#include "web_server.h"

// Estados de la aplicación
enum AppState {
    STATE_DISPLAY, // Modo normal mostrando texto o imagen
    STATE_CONFIG   // Modo configuración con WiFi AP y servidor web
};

AppState currentState = STATE_DISPLAY;

DisplayManager display;
ButtonHandler buttons;
StorageManager storage;
WiFiManager wifi;
HairClipWebServer webServer;
HairClipConfig config;

// Declaraciones de funciones
void enterConfigMode();
void exitConfigMode();
void enterSleep();
void applyConfig();

void enterConfigMode() {
    if (currentState == STATE_CONFIG) {
        // Si ya está en config, una nueva pulsación sale del modo config
        exitConfigMode();
        return;
    }

    Serial.println("[App] Entrando en Modo Configuración");
    currentState = STATE_CONFIG;

    wifi.startAP();
    webServer.begin(&display, &storage, &config, []() {
        exitConfigMode();
    });

    display.showConfigScreen(AP_SSID, wifi.getIP().c_str());
}

void exitConfigMode() {
    Serial.println("[App] Saliendo de Modo Configuración");
    webServer.stop();
    wifi.stopAP();
    currentState = STATE_DISPLAY;
    applyConfig();
}

void enterSleep() {
    Serial.println("[App] Entrando en Deep Sleep...");
    display.showSleepScreen();
    delay(600);

    // Apagar periféricos y retener corte de energía en GPIO 15
    digitalWrite(PIN_LCD_BL, LOW);
    digitalWrite(PIN_POWER_ON, LOW);
    rtc_gpio_hold_en((gpio_num_t)PIN_POWER_ON);

    // Configurar despertar con el botón User (GPIO 14, active LOW)
    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_BUTTON_USER, 0);
    rtc_gpio_pullup_en((gpio_num_t)PIN_BUTTON_USER);
    rtc_gpio_pulldown_dis((gpio_num_t)PIN_BUTTON_USER);

    esp_deep_sleep_start();
}

void applyConfig() {
    display.setRotation(config.rotation);
    display.setBrightness(config.brightness);

    if (config.hasImage && storage.hasImage()) {
        size_t imgSize = SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t);
        uint16_t* imgBuf = (uint16_t*)heap_caps_malloc(imgSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (!imgBuf) {
            imgBuf = (uint16_t*)malloc(imgSize);
        }

        if (imgBuf && storage.loadImage(imgBuf, imgSize)) {
            display.showImage(imgBuf, SCREEN_WIDTH, SCREEN_HEIGHT, config.swapBytes);
            free(imgBuf);
            return;
        }

        if (imgBuf) free(imgBuf);
        // Si falló cargar la imagen, se continúa con texto
    }

    if (config.scrollMode) {
        display.showScrollText(config.text, config.color, config.fontSize, config.scrollSpeed);
    } else {
        display.showStaticText(config.text, config.color, config.fontSize);
    }
}

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.println("\n==================================");
    Serial.println("   🎀 HairClip Digital v1.0");
    Serial.println("==================================");

    // Si viene de Deep Sleep, deshabilitar retención del pin de poder
    rtc_gpio_hold_dis((gpio_num_t)PIN_POWER_ON);

    // 1. Inicializar sistema de archivos
    storage.begin();
    config = storage.loadConfig();

    // 2. Inicializar pantalla
    display.begin();

    // 3. Inicializar botones
    buttons.begin();
    buttons.onConfigPress(enterConfigMode);
    buttons.onSleepPress(enterSleep);

    // 4. Mostrar contenido guardado
    applyConfig();
}

void loop() {
    // Lectura de botones continua
    buttons.update();

    if (currentState == STATE_DISPLAY) {
        // En modo normal actualiza la animación suave de scroll
        display.update();
    } else if (currentState == STATE_CONFIG) {
        // En modo config atiende peticiones DNS y HTTP
        wifi.update();
        webServer.handleClient();
    }
}
