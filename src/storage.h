#pragma once
#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "config.h"

class StorageManager {
public:
    StorageManager();
    bool begin();

    bool saveConfig(const HairClipConfig& config);
    HairClipConfig loadConfig();

    bool openImageForWriting();
    bool writeImageChunk(const uint8_t* chunk, size_t len);
    void closeImageFile();

    bool loadImage(uint16_t* buffer, size_t maxBytes);
    size_t getImageSize();
    bool hasImage();
    void deleteImage();

    // Gestión de GIF animado
    bool openGifForWriting();
    bool writeGifChunk(const uint8_t* chunk, size_t len);
    void closeGifFile();
    bool hasGif();
    size_t getGifSize();
    bool loadGif(uint8_t* buffer, size_t maxBytes);
    void deleteGif();

    size_t getFreeBytes();

private:
    File _imgFile;
    File _gifFile;
    bool _fsMounted;
};
