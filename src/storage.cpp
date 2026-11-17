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
    doc["fontSize"] = config.fontSize;
    doc["scrollSpeed"] = config.scrollSpeed;
    doc["scrollMode"] = config.scrollMode;
    doc["brightness"] = config.brightness;
    doc["rotation"] = config.rotation;
    doc["swapBytes"] = config.swapBytes;
    doc["hasImage"] = config.hasImage;

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
    config.fontSize = doc["fontSize"] | 3;
    config.scrollSpeed = doc["scrollSpeed"] | 3;
    config.scrollMode = doc["scrollMode"] | true;
    config.brightness = doc["brightness"] | DEFAULT_BRIGHTNESS;
    config.rotation = doc["rotation"] | 1;
    config.swapBytes = doc["swapBytes"] | true;
    config.hasImage = doc["hasImage"] | false;

    Serial.println("[Storage] Configuración cargada con éxito");
    return config;
}

bool StorageManager::openImageForWriting() {
    if (!_fsMounted) return false;
    _imgFile = LittleFS.open(IMAGE_FILE, "w");
    return _imgFile;
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

bool StorageManager::hasImage() {
    return _fsMounted && LittleFS.exists(IMAGE_FILE);
}

void StorageManager::deleteImage() {
    if (_fsMounted && LittleFS.exists(IMAGE_FILE)) {
        LittleFS.remove(IMAGE_FILE);
    }
}
