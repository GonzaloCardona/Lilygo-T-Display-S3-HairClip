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

    // 3. Subida de imágenes (multipart/form-data)
    _server.on("/api/image", HTTP_POST, [this]() {
        _server.send(200, "application/json", "{\"status\":\"ok\"}");
    }, [this]() {
        handleImageUpload();
    });

    // 4. Captive portal catch-all
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
    doc["fontSize"] = _config->fontSize;
    doc["scrollSpeed"] = _config->scrollSpeed;
    doc["scrollMode"] = _config->scrollMode;
    doc["brightness"] = _config->brightness;
    doc["hasImage"] = _config->hasImage;

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
    if (doc["fontSize"].is<uint8_t>()) _config->fontSize = doc["fontSize"].as<uint8_t>();
    if (doc["scrollSpeed"].is<uint8_t>()) _config->scrollSpeed = doc["scrollSpeed"].as<uint8_t>();
    if (doc["scrollMode"].is<bool>()) _config->scrollMode = doc["scrollMode"].as<bool>();
    
    _config->hasImage = false; // El usuario eligió texto

    if (_storage) {
        _storage->saveConfig(*_config);
    }

    Serial.printf("[WebServer] Texto actualizado: '%s', color: #%06X\n", _config->text.c_str(), _config->color);
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
