#pragma once
#include <Arduino.h>

// ==========================================
// 📌 PINES HARDWARE (LilyGO T-Display-S3)
// ==========================================

// Alimentación y retroiluminación
#define PIN_POWER_ON        15   // Enable alimentación pantalla/periféricos (HIGH para encender)
#define PIN_LCD_BL          38   // Backlight PWM

// Botones físicos (ambos active LOW con pull-up interno)
#define PIN_BUTTON_BOOT     0    // Botón BOOT -> Modo Configuración
#define PIN_BUTTON_USER     14   // Botón User -> Deep Sleep (pulsación larga) + Wakeup

// Medición de batería
#define PIN_BAT_VOLT        4    // ADC divisor de voltaje (Vreal = lectura * 2)

// Pantalla ST7789V - Bus Paralelo 8-bit (I8080)
#define PIN_LCD_RD          9    // Read strobe
#define PIN_LCD_WR          8    // Write strobe
#define PIN_LCD_RS          7    // Data / Command (D/C)
#define PIN_LCD_CS          6    // Chip Select
#define PIN_LCD_RST         5    // Reset
#define PIN_LCD_D0          39
#define PIN_LCD_D1          40
#define PIN_LCD_D2          41
#define PIN_LCD_D3          42
#define PIN_LCD_D4          45
#define PIN_LCD_D5          46
#define PIN_LCD_D6          47
#define PIN_LCD_D7          48

// ==========================================
// 🖥️ CONFIGURACIÓN DE PANTALLA
// ==========================================
#define SCREEN_WIDTH        320  // Orientación horizontal (landscape)
#define SCREEN_HEIGHT       170  // Orientación horizontal
#define DEFAULT_BRIGHTNESS  160  // 0 a 255

// ==========================================
// 📶 WIFI ACCESS POINT & WEB SERVER
// ==========================================
#define AP_SSID             "HairClip"
#define AP_PASSWORD         ""   // Red abierta para conexión directa rápida desde iPhone
#define AP_IP_1             192
#define AP_IP_2             168
#define AP_IP_3             4
#define AP_IP_4             1
#define WEB_SERVER_PORT     80
#define DNS_PORT            53

// ==========================================
// ⏱️ TIEMPOS Y BOTONES
// ==========================================
#define BUTTON_DEBOUNCE_MS  50
#define LONG_PRESS_MS       1200 // > 1.2s para evitar apagar accidentalmente

// ==========================================
// 💾 ARCHIVOS DE ALMACENAMIENTO (LittleFS)
// ==========================================
#define CONFIG_FILE         "/config.json"
#define IMAGE_FILE          "/image.raw"

// Estructura de configuración persistente
struct HairClipConfig {
    String text = "HairClip Digital";
    uint32_t color = 0xFF69B4;      // Hot pink (#FF69B4)
    uint8_t fontSize = 3;           // Escala de fuente (1 a 5)
    uint8_t scrollSpeed = 3;        // 1 a 10
    bool scrollMode = true;         // true = en movimiento, false = estático
    uint8_t brightness = DEFAULT_BRIGHTNESS;
    uint8_t rotation = 1;           // 1 = normal landscape, 3 = landscape 180°
    bool hasImage = false;          // true = muestra imagen, false = texto
};
