#pragma once
#ifndef LGFX_USE_V1
#define LGFX_USE_V1
#endif
#include <LovyanGFX.hpp>
#include "config.h"

// ==============================================================
// 🖥️ Driver LGFX oficial para LilyGO T-Display-S3 (Bus Paralelo 8-bit)
// ==============================================================
class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_ST7789      _panel_instance;
    lgfx::Bus_Parallel8     _bus_instance;
    lgfx::Light_PWM         _light_instance;

public:
    LGFX(void) {
        // 1. Configuración del bus paralelo Intel 8080 de 8 bits
        {
            auto cfg = _bus_instance.config();
            cfg.port = 0;
            cfg.freq_write = 20000000;   // 20 MHz
            cfg.pin_wr  = PIN_LCD_WR;    // GPIO 8
            cfg.pin_rd  = PIN_LCD_RD;    // GPIO 9
            cfg.pin_rs  = PIN_LCD_RS;    // GPIO 7 (Data/Command)
            cfg.pin_d0  = PIN_LCD_D0;    // GPIO 39
            cfg.pin_d1  = PIN_LCD_D1;    // GPIO 40
            cfg.pin_d2  = PIN_LCD_D2;    // GPIO 41
            cfg.pin_d3  = PIN_LCD_D3;    // GPIO 42
            cfg.pin_d4  = PIN_LCD_D4;    // GPIO 45
            cfg.pin_d5  = PIN_LCD_D5;    // GPIO 46
            cfg.pin_d6  = PIN_LCD_D6;    // GPIO 47
            cfg.pin_d7  = PIN_LCD_D7;    // GPIO 48
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        // 2. Configuración del panel IPS ST7789V
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs   = PIN_LCD_CS;   // GPIO 6
            cfg.pin_rst  = PIN_LCD_RST;  // GPIO 5
            cfg.pin_busy = -1;
            cfg.memory_width  = 240;     // GRAM real del ST7789 (240x320) para cálculo correcto de offset (35px)
            cfg.memory_height = 320;
            cfg.panel_width   = 170;
            cfg.panel_height  = 320;
            cfg.offset_x      = 35;      // Compensación horizontal del ST7789V (centrado)
            cfg.offset_y      = 0;
            cfg.offset_rotation = 0;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits  = 1;
            cfg.readable   = false;
            cfg.invert     = true;       // Obligatorio para esta pantalla
            cfg.rgb_order  = false;      // false = RGB
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;
            _panel_instance.config(cfg);
        }

        // 3. Control de retroiluminación PWM
        {
            auto cfg = _light_instance.config();
            cfg.pin_bl      = PIN_LCD_BL; // GPIO 38
            cfg.invert      = false;
            cfg.freq        = 44100;
            cfg.pwm_channel = 7;
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        setPanel(&_panel_instance);
    }
};

// ==============================================================
// 🎨 Display Manager para HairClip (texto scroll/estático, imagen)
// ==============================================================
class DisplayManager {
public:
    DisplayManager();
    void begin();
    void setBrightness(uint8_t brightness);
    void setRotation(uint8_t rotation);
    uint8_t getRotation() const { return _rotation; }
    void showStaticText(const String& text, uint32_t color, uint8_t fontSize);
    void showScrollText(const String& text, uint32_t color, uint8_t fontSize, uint8_t speed);
    void showImage(const uint16_t* data, uint16_t w, uint16_t h, bool swapBytes = true);
    void showConfigScreen(const char* ssid, const char* ip);
    void showSleepScreen();
    void update();
    void clear();
    void powerOff();

    LGFX& getTft() { return _tft; }

private:
    LGFX _tft;
    LGFX_Sprite _scrollSprite;
    uint8_t _rotation;
    String _currentText;
    uint32_t _textColor;
    uint8_t _fontSize;
    uint8_t _scrollSpeed;
    bool _scrolling;
    int _scrollX;
    int _textWidth;
    int _textHeight;
    int _spriteY;
    int _spriteHeight;
    unsigned long _lastScrollUpdate;
};
