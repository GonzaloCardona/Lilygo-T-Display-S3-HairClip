#include "display.h"

DisplayManager::DisplayManager()
    : _scrollSprite(&_tft),
      _textColor(0xFFFFFF),
      _fontSize(3),
      _scrollSpeed(3),
      _scrolling(false),
      _scrollX(0),
      _textWidth(0),
      _textHeight(24),
      _spriteY(0),
      _spriteHeight(80),
      _lastScrollUpdate(0) {}

void DisplayManager::begin() {
    // 1. Activar riel de alimentación (CRÍTICO)
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    delay(50);

    // 2. Inicializar LovyanGFX
    _tft.init();
    _tft.setRotation(1); // Modo horizontal (Landscape: 320 x 170)
    _tft.setBrightness(DEFAULT_BRIGHTNESS);
    _tft.fillScreen(TFT_BLACK);

    // 3. Inicializar sprite para scrolling sin parpadeo
    _scrollSprite.setColorDepth(16);
    _spriteHeight = SCREEN_HEIGHT; // Alto completo para máxima flexibilidad
    _scrollSprite.createSprite(SCREEN_WIDTH, _spriteHeight);
}

void DisplayManager::setBrightness(uint8_t brightness) {
    _tft.setBrightness(brightness);
}

void DisplayManager::clear() {
    _scrolling = false;
    _tft.fillScreen(TFT_BLACK);
}

void DisplayManager::powerOff() {
    _scrolling = false;
    _tft.fillScreen(TFT_BLACK);
    _tft.setBrightness(0);
    digitalWrite(PIN_LCD_BL, LOW);
    digitalWrite(PIN_POWER_ON, LOW);
}

void DisplayManager::showStaticText(const String& text, uint32_t color, uint8_t fontSize) {
    _scrolling = false;
    _currentText = text;
    _textColor = color;
    _fontSize = fontSize;

    _tft.fillScreen(TFT_BLACK);
    _tft.setTextSize(_fontSize);
    
    // Convertir color HEX RGB888 a RGB565
    uint16_t c565 = _tft.color565((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    _tft.setTextColor(c565, TFT_BLACK);
    _tft.setTextDatum(textdatum_t::middle_center);

    _tft.drawString(text, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
}

void DisplayManager::showScrollText(const String& text, uint32_t color, uint8_t fontSize, uint8_t speed) {
    _currentText = text;
    _textColor = color;
    _fontSize = (fontSize < 1) ? 1 : (fontSize > 5 ? 5 : fontSize);
    _scrollSpeed = (speed < 1) ? 1 : (speed > 10 ? 10 : speed);
    
    _tft.setTextSize(_fontSize);
    _textWidth = _tft.textWidth(text);
    _textHeight = _tft.fontHeight();

    _scrollX = SCREEN_WIDTH;
    _scrolling = true;
    _lastScrollUpdate = millis();

    _tft.fillScreen(TFT_BLACK);
}

void DisplayManager::showImage(const uint16_t* data, uint16_t w, uint16_t h) {
    _scrolling = false;
    _tft.fillScreen(TFT_BLACK);
    
    // Centrar si es menor que la pantalla
    int x = (SCREEN_WIDTH - w) / 2;
    int y = (SCREEN_HEIGHT - h) / 2;
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    _tft.pushImage(x, y, w, h, data);
}

void DisplayManager::showConfigScreen(const char* ssid, const char* ip) {
    _scrolling = false;
    _tft.fillScreen(TFT_BLACK);

    // Borde de tarjeta
    _tft.drawRoundRect(10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20, 8, _tft.color565(0, 150, 255));
    _tft.fillRoundRect(12, 12, SCREEN_WIDTH - 24, SCREEN_HEIGHT - 24, 6, _tft.color565(15, 20, 30));

    // Cabecera
    _tft.setTextDatum(textdatum_t::top_center);
    _tft.setTextSize(2);
    _tft.setTextColor(_tft.color565(255, 105, 180)); // Rosa
    _tft.drawString("MODO CONFIGURACION", SCREEN_WIDTH / 2, 22);

    // Línea separadora
    _tft.drawFastHLine(25, 48, SCREEN_WIDTH - 50, _tft.color565(60, 70, 90));

    // Detalles de conexión
    _tft.setTextDatum(textdatum_t::top_left);
    _tft.setTextSize(2);
    _tft.setTextColor(TFT_WHITE);
    _tft.drawString("1. Conecta tu iPhone a:", 25, 60);

    _tft.setTextColor(_tft.color565(255, 215, 0)); // Dorado
    _tft.drawString(String("   WiFi: ") + ssid, 25, 85);

    _tft.setTextColor(TFT_WHITE);
    _tft.drawString("2. Abre en Safari:", 25, 115);

    _tft.setTextColor(_tft.color565(0, 255, 180)); // Cyan
    _tft.drawString(String("   http://") + ip, 25, 138);
}

void DisplayManager::showSleepScreen() {
    _scrolling = false;
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextDatum(textdatum_t::middle_center);
    _tft.setTextSize(2);
    _tft.setTextColor(_tft.color565(120, 120, 150));
    _tft.drawString("Hasta luego... zzz", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
}

void DisplayManager::update() {
    if (!_scrolling) return;

    unsigned long now = millis();
    if (now - _lastScrollUpdate < 20) return; // ~50 FPS
    _lastScrollUpdate = now;

    // Renderizar al sprite
    _scrollSprite.fillSprite(TFT_BLACK);
    _scrollSprite.setTextSize(_fontSize);
    
    uint16_t c565 = _tft.color565((_textColor >> 16) & 0xFF, (_textColor >> 8) & 0xFF, _textColor & 0xFF);
    _scrollSprite.setTextColor(c565);
    _scrollSprite.setTextDatum(textdatum_t::middle_left);

    _scrollSprite.drawString(_currentText, _scrollX, SCREEN_HEIGHT / 2);
    _scrollSprite.pushSprite(0, 0);

    // Mover texto hacia la izquierda
    _scrollX -= _scrollSpeed;

    // Cuando el texto sale completamente de la pantalla, reiniciar a la derecha
    if (_scrollX < -_textWidth) {
        _scrollX = SCREEN_WIDTH;
    }
}
