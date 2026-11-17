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
uint8_t* mediaBuffer = nullptr;
size_t mediaBufferSize = 0;

// Declaraciones de funciones
void enterConfigMode();
void exitConfigMode();
void enterSleep();
void applyConfig();
void freeMediaBuffer();

void freeMediaBuffer() {
    display.stopGif();
    if (mediaBuffer != nullptr) {
        free(mediaBuffer);
        mediaBuffer = nullptr;
        mediaBufferSize = 0;
    }
}

void enterConfigMode() {
    if (currentState == STATE_CONFIG) {
        // Si ya está en config, una nueva pulsación sale del modo config
        exitConfigMode();
        return;
    }

    Serial.println("[App] Entrando en Modo Configuración");
    currentState = STATE_CONFIG;
    freeMediaBuffer();

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
    freeMediaBuffer();
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

    freeMediaBuffer();

    if (config.contentType == CONTENT_GIF && storage.hasGif()) {
        size_t gifSize = storage.getGifSize();
        if (gifSize > 0) {
            mediaBuffer = (uint8_t*)heap_caps_malloc(gifSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
            if (!mediaBuffer) {
                mediaBuffer = (uint8_t*)malloc(gifSize);
            }

            if (mediaBuffer && storage.loadGif(mediaBuffer, gifSize)) {
                mediaBufferSize = gifSize;
                if (display.startGif(mediaBuffer, gifSize, config.bgColor, config.swapBytes)) {
                    return;
                }
            }
            freeMediaBuffer();
        }
    } else if (config.contentType == CONTENT_IMAGE && storage.hasImage()) {
        size_t fileSize = storage.getImageSize();
        if (fileSize > 0) {
            mediaBuffer = (uint8_t*)heap_caps_malloc(fileSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
            if (!mediaBuffer) {
                mediaBuffer = (uint8_t*)malloc(fileSize);
            }

            if (mediaBuffer && storage.loadImage((uint16_t*)mediaBuffer, fileSize)) {
                mediaBufferSize = fileSize;
                display.showBanner((uint16_t*)mediaBuffer, config.imgWidth, config.imgHeight, config.scrollSpeed, config.scrollMode, config.swapBytes, config.bgColor);
                return;
            }
            freeMediaBuffer();
        }
    }

    if (config.scrollMode) {
        display.showScrollText(config.text, config.color, config.fontSize, config.scrollSpeed, config.bgColor);
    } else {
        display.showStaticText(config.text, config.color, config.fontSize, config.bgColor);
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
