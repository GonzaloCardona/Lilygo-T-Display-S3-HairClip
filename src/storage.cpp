#include "storage.h"

StorageManager::StorageManager() : _fsMounted(false) {}

bool StorageManager::begin() {
    if (!LittleFS.begin(true)) {
        Serial.println("[Storage] Error al montar LittleFS");
        _fsMounted = false;
        return false;
    }
    _fsMounted = true;
    Serial.println("[Storage] LittleFS montado correctamente");
    return true;
}

bool StorageManager::saveConfig(const HairClipConfig& config) {
    if (!_fsMounted) return false;

    File file = LittleFS.open(CONFIG_FILE, "w");
    if (!file) {
        Serial.println("[Storage] Error al abrir config.json para escribir");
        return false;
    }

    JsonDocument doc;
    doc["text"] = config.text;
    doc["color"] = config.color;
    doc["bgColor"] = config.bgColor;
    doc["fontSize"] = config.fontSize;
    doc["scrollSpeed"] = config.scrollSpeed;
    doc["scrollMode"] = config.scrollMode;
    doc["brightness"] = config.brightness;
    doc["rotation"] = config.rotation;
    doc["swapBytes"] = config.swapBytes;
    doc["contentType"] = config.contentType;
    doc["hasImage"] = config.hasImage;
    doc["imgWidth"] = config.imgWidth;
    doc["imgHeight"] = config.imgHeight;

    if (serializeJson(doc, file) == 0) {
        Serial.println("[Storage] Error al serializar config.json");
        file.close();
        return false;
    }

    file.close();
    Serial.println("[Storage] Configuración guardada en LittleFS");
    return true;
}

HairClipConfig StorageManager::loadConfig() {
    HairClipConfig config;
    if (!_fsMounted) return config;

    if (!LittleFS.exists(CONFIG_FILE)) {
        Serial.println("[Storage] config.json no existe, usando valores por defecto");
        return config;
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("[Storage] Error al abrir config.json para leer");
        return config;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("[Storage] Error al parsear config.json, usando defaults");
        return config;
    }

    config.text = doc["text"] | "🎀 HairClip ✨";
    config.color = doc["color"] | 0xFF69B4;
    config.bgColor = doc["bgColor"] | 0x000000;
    config.fontSize = doc["fontSize"] | 44;
    config.scrollSpeed = doc["scrollSpeed"] | 3;
    config.scrollMode = doc["scrollMode"] | true;
    config.brightness = doc["brightness"] | DEFAULT_BRIGHTNESS;
    config.rotation = doc["rotation"] | 1;
    config.swapBytes = doc["swapBytes"] | true;
    config.hasImage = doc["hasImage"] | false;
    if (doc["contentType"].is<uint8_t>()) {
        config.contentType = doc["contentType"].as<uint8_t>();
    } else {
        config.contentType = config.hasImage ? CONTENT_IMAGE : CONTENT_TEXT;
    }
    config.imgWidth = doc["imgWidth"] | SCREEN_WIDTH;
    config.imgHeight = doc["imgHeight"] | SCREEN_HEIGHT;

    Serial.println("[Storage] Configuración cargada con éxito");
    return config;
}

bool StorageManager::openImageForWriting() {
    if (!_fsMounted) return false;
    _imgFile = LittleFS.open(IMAGE_FILE, "w");
    return (bool)_imgFile;
}

bool StorageManager::writeImageChunk(const uint8_t* chunk, size_t len) {
    if (!_imgFile) return false;
    return _imgFile.write(chunk, len) == len;
}

void StorageManager::closeImageFile() {
    if (_imgFile) {
        _imgFile.close();
        Serial.println("[Storage] Imagen guardada en LittleFS");
    }
}

bool StorageManager::loadImage(uint16_t* buffer, size_t maxBytes) {
    if (!_fsMounted || !hasImage()) return false;

    File file = LittleFS.open(IMAGE_FILE, "r");
    if (!file) return false;

    size_t bytesRead = file.read((uint8_t*)buffer, maxBytes);
    file.close();

    return bytesRead > 0;
}

size_t StorageManager::getImageSize() {
    if (!_fsMounted || !hasImage()) return 0;
    File file = LittleFS.open(IMAGE_FILE, "r");
    if (!file) return 0;
    size_t sz = file.size();
    file.close();
    return sz;
}

bool StorageManager::hasImage() {
    return _fsMounted && LittleFS.exists(IMAGE_FILE);
}

void StorageManager::deleteImage() {
    if (_fsMounted && LittleFS.exists(IMAGE_FILE)) {
        LittleFS.remove(IMAGE_FILE);
    }
}

bool StorageManager::openGifForWriting() {
    if (!_fsMounted) return false;
    _gifFile = LittleFS.open(GIF_FILE, "w");
    return (bool)_gifFile;
}

bool StorageManager::writeGifChunk(const uint8_t* chunk, size_t len) {
    if (!_gifFile) return false;
    return _gifFile.write(chunk, len) == len;
}

void StorageManager::closeGifFile() {
    if (_gifFile) {
        _gifFile.close();
        Serial.println("[Storage] Archivo GIF guardado en LittleFS");
    }
}

bool StorageManager::hasGif() {
    return _fsMounted && LittleFS.exists(GIF_FILE);
}

size_t StorageManager::getGifSize() {
    if (!_fsMounted || !hasGif()) return 0;
    File file = LittleFS.open(GIF_FILE, "r");
    if (!file) return 0;
    size_t sz = file.size();
    file.close();
    return sz;
}

bool StorageManager::loadGif(uint8_t* buffer, size_t maxBytes) {
    if (!_fsMounted || !hasGif()) return false;
    File file = LittleFS.open(GIF_FILE, "r");
    if (!file) return false;
    size_t bytesRead = file.read(buffer, maxBytes);
    file.close();
    return bytesRead > 0;
}

void StorageManager::deleteGif() {
    if (_fsMounted && LittleFS.exists(GIF_FILE)) {
        LittleFS.remove(GIF_FILE);
    }
}
