#include "web_server.h"
#include "web_interface.h"
#include <ArduinoJson.h>

HairClipWebServer::HairClipWebServer()
    : _server(WEB_SERVER_PORT),
      _display(nullptr),
      _storage(nullptr),
      _config(nullptr),
      _running(false) {}

void HairClipWebServer::begin(DisplayManager* display, StorageManager* storage, HairClipConfig* config, std::function<void()> onExitCallback) {
    _display = display;
    _storage = storage;
    _config = config;
    _onExitCallback = onExitCallback;

    setupRoutes();
    _server.begin();
    _running = true;
    Serial.println("[WebServer] Servidor HTTP iniciado en puerto 80");
}

void HairClipWebServer::stop() {
    if (_running) {
        _server.stop();
        _running = false;
        Serial.println("[WebServer] Servidor HTTP detenido");
    }
}

void HairClipWebServer::handleClient() {
    if (_running) {
        _server.handleClient();
    }
}

void HairClipWebServer::setupRoutes() {
    // 1. Página principal
    _server.on("/", HTTP_GET, [this]() { handleRoot(); });

    // Rutas para detección de Captive Portal de iOS / Android / Windows
    _server.on("/hotspot-detect.html", HTTP_GET, [this]() { handleRoot(); });
    _server.on("/generate_204", HTTP_GET, [this]() { handleRoot(); });
    _server.on("/gen_204", HTTP_GET, [this]() { handleRoot(); });
    _server.on("/ncsi.txt", HTTP_GET, [this]() { handleRoot(); });

    // 2. API REST
    _server.on("/api/status", HTTP_GET, [this]() { handleStatus(); });
    _server.on("/api/text", HTTP_POST, [this]() { handleText(); });
    _server.on("/api/settings", HTTP_POST, [this]() { handleSettings(); });
    _server.on("/api/apply", HTTP_POST, [this]() { handleApply(); });

    // 3. Subida de imágenes y carteles raw (multipart/form-data)
    _server.on("/api/image", HTTP_POST, [this]() {
        if (_config) {
            if (_server.hasArg("w")) _config->imgWidth = _server.arg("w").toInt();
            if (_server.hasArg("h")) _config->imgHeight = _server.arg("h").toInt();
            if (_server.hasArg("scroll")) _config->scrollMode = (_server.arg("scroll") == "1" || _server.arg("scroll") == "true");
            if (_server.hasArg("speed")) _config->scrollSpeed = _server.arg("speed").toInt();
            if (_server.hasArg("bg")) _config->bgColor = strtoul(_server.arg("bg").c_str(), NULL, 16);
            if (_server.hasArg("text")) _config->text = _server.arg("text");
            if (_server.hasArg("fontSize")) _config->fontSize = _server.arg("fontSize").toInt();
            if (_server.hasArg("color")) _config->color = strtoul(_server.arg("color").c_str(), NULL, 16);
            _config->contentType = CONTENT_IMAGE;
            _config->hasImage = true;
            if (_storage) _storage->saveConfig(*_config);
            Serial.printf("[WebServer] Imagen/Cartel procesado: %dx%d, scroll=%d, bg=#%06lX\n",
                          _config->imgWidth, _config->imgHeight, _config->scrollMode, (unsigned long)_config->bgColor);
        }
        _server.send(200, "application/json", "{\"status\":\"ok\"}");
    }, [this]() {
        handleImageUpload();
    });

    // 4. Subida de GIFs animados (multipart/form-data)
    _server.on("/api/gif", HTTP_POST, [this]() {
        if (_config) {
            if (_server.hasArg("bg")) {
                _config->bgColor = strtoul(_server.arg("bg").c_str(), NULL, 16);
            }
            _config->contentType = CONTENT_GIF;
            _config->hasImage = false;
            if (_storage) _storage->saveConfig(*_config);
            Serial.printf("[WebServer] GIF configurado como activo (bg=#%06lX)\n", (unsigned long)_config->bgColor);
        }
        _server.send(200, "application/json", "{\"status\":\"ok\"}");
    }, [this]() {
        handleGifUpload();
    });

    // 5. Captive portal catch-all
    _server.onNotFound([this]() {
        _server.sendHeader("Location", "http://192.168.4.1/", true);
        _server.send(302, "text/plain", "");
    });
}

void HairClipWebServer::handleRoot() {
    _server.send_P(200, "text/html", INDEX_HTML);
}

void HairClipWebServer::handleStatus() {
    if (!_config) {
        _server.send(500, "application/json", "{\"error\":\"config_null\"}");
        return;
    }

    JsonDocument doc;
    doc["text"] = _config->text;
    doc["color"] = _config->color;
    doc["bgColor"] = _config->bgColor;
    doc["fontSize"] = _config->fontSize;
    doc["scrollSpeed"] = _config->scrollSpeed;
    doc["scrollMode"] = _config->scrollMode;
    doc["brightness"] = _config->brightness;
    doc["rotation"] = _config->rotation;
    doc["swapBytes"] = _config->swapBytes;
    doc["contentType"] = _config->contentType;
    doc["hasImage"] = _config->hasImage;
    doc["imgWidth"] = _config->imgWidth;
    doc["imgHeight"] = _config->imgHeight;
    doc["freeSpace"] = _storage ? _storage->getFreeBytes() : 0;

    String response;
    serializeJson(doc, response);
    _server.send(200, "application/json", response);
}

void HairClipWebServer::handleText() {
    if (!_server.hasArg("plain") || !_config) {
        _server.send(400, "application/json", "{\"error\":\"invalid_request\"}");
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, _server.arg("plain"));
    if (err) {
        _server.send(400, "application/json", "{\"error\":\"json_parse_failed\"}");
        return;
    }

    if (doc["text"].is<String>()) _config->text = doc["text"].as<String>();
    if (doc["color"].is<uint32_t>()) _config->color = doc["color"].as<uint32_t>();
    if (doc["bgColor"].is<uint32_t>()) _config->bgColor = doc["bgColor"].as<uint32_t>();
    if (doc["fontSize"].is<uint8_t>()) _config->fontSize = doc["fontSize"].as<uint8_t>();
    if (doc["scrollSpeed"].is<uint8_t>()) _config->scrollSpeed = doc["scrollSpeed"].as<uint8_t>();
    if (doc["scrollMode"].is<bool>()) _config->scrollMode = doc["scrollMode"].as<bool>();
    
    _config->contentType = CONTENT_TEXT;
    _config->hasImage = false; // El usuario eligió texto

    if (_storage) {
        _storage->saveConfig(*_config);
    }

    Serial.printf("[WebServer] Texto actualizado: '%s', color: #%06lX\n", _config->text.c_str(), (unsigned long)_config->color);
    _server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void HairClipWebServer::handleSettings() {
    if (!_server.hasArg("plain") || !_config) {
        _server.send(400, "application/json", "{\"error\":\"invalid_request\"}");
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, _server.arg("plain"));
    if (err) {
        _server.send(400, "application/json", "{\"error\":\"json_parse_failed\"}");
        return;
    }

    if (doc["brightness"].is<uint8_t>()) {
        _config->brightness = doc["brightness"].as<uint8_t>();
        if (_display) _display->setBrightness(_config->brightness);
        if (_storage) _storage->saveConfig(*_config);
    }

    if (doc["rotation"].is<uint8_t>()) {
        _config->rotation = doc["rotation"].as<uint8_t>();
        if (_display) _display->setRotation(_config->rotation);
        if (_storage) _storage->saveConfig(*_config);
    }

    if (doc["swapBytes"].is<bool>()) {
        _config->swapBytes = doc["swapBytes"].as<bool>();
        if (_storage) _storage->saveConfig(*_config);
    }

    _server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void HairClipWebServer::handleApply() {
    _server.send(200, "application/json", "{\"status\":\"ok\"}");
    Serial.println("[WebServer] Petición de guardar y salir recibida");

    if (_onExitCallback) {
        // Ejecutar callback de salida (apaga wifi y vuelve a display)
        _onExitCallback();
    }
}

void HairClipWebServer::handleImageUpload() {
    HTTPUpload& upload = _server.upload();

    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("[WebServer] Inicio de subida de imagen: %s\n", upload.filename.c_str());
        if (_storage) {
            _storage->openImageForWriting();
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (_storage) {
            _storage->writeImageChunk(upload.buf, upload.currentSize);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        Serial.printf("[WebServer] Subida de imagen completada (%u bytes)\n", upload.totalSize);
        if (_storage) {
            _storage->closeImageFile();
            if (_config) {
                _config->contentType = CONTENT_IMAGE;
                _config->hasImage = true;
                _storage->saveConfig(*_config);
            }
        }
    } else if (upload.status == UPLOAD_FILE_ABORTED) {
        Serial.println("[WebServer] Subida de imagen cancelada o interrumpida");
        if (_storage) {
            _storage->closeImageFile();
        }
    }
}

void HairClipWebServer::handleGifUpload() {
    HTTPUpload& upload = _server.upload();

    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("[WebServer] Inicio de subida de GIF: %s\n", upload.filename.c_str());
        if (_storage) {
            _storage->openGifForWriting();
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (_storage) {
            if (!_storage->writeGifChunk(upload.buf, upload.currentSize)) {
                Serial.println("[WebServer] ERROR: Error de escritura en LittleFS (espacio insuficiente)");
            }
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        Serial.printf("[WebServer] Subida de GIF completada (%u bytes)\n", upload.totalSize);
        if (_storage) {
            _storage->closeGifFile();
            if (_config) {
                _config->contentType = CONTENT_GIF;
                _config->hasImage = false;
                _storage->saveConfig(*_config);
            }
        }
    } else if (upload.status == UPLOAD_FILE_ABORTED) {
        Serial.println("[WebServer] Subida de GIF cancelada o interrumpida");
        if (_storage) {
            _storage->closeGifFile();
            _storage->deleteGif();
        }
    }
}
