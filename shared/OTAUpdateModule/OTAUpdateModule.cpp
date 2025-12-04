#include "OTAUpdateModule.h"

OTAUpdateModule::OTAUpdateModule(StorageModule* storage, LCDModule* lcd): 
    lcd(lcd), storage(storage) {
}

bool OTAUpdateModule::begin() {
    Serial.println("[OTA] Starting OTA module...");

    if (!loadConfig()) {
        Serial.println("[OTA] Config load failed.");
        return false;
    }

    server.serveStatic("/home.css", LittleFS, "/web/home.css");
    server.serveStatic("/home.js", LittleFS, "/web/home.js");
    server.serveStatic("/update.css", LittleFS, "/web/update.css");
    server.serveStatic("/update.js", LittleFS, "/web/update.js");

    server.on("/", HTTP_GET,  std::bind(&OTAUpdateModule::handleHomePage,  this));
    server.on("/update",  HTTP_GET, std::bind(&OTAUpdateModule::handleProgressUpdate,   this));

    server.begin();
    IPAddress ip = WiFi.localIP();
    Serial.println("[OTA] WebServer started on " + ip.toString() + " port 80");
    lcd->setText("OTA WebServer:", ip.toString() + ":80");

    return true;
}

void OTAUpdateModule::loop() {
    server.handleClient();
}

bool OTAUpdateModule::loadConfig() {
    ArduinoJson::JsonDocument doc;

    if (!storage->readJSON("/config.json", doc)) {
        Serial.println("[OTA] ERROR: cannot read /config.json");
        lcd->setText("OTA Error", "Cannot read config");
        return false;
    }

    firmwareVersion = doc["version"] | "";
    updateJsonUrl   = doc["update_url"] | "";

    if (firmwareVersion == "" || updateJsonUrl == "") {
        Serial.println("[OTA] ERROR: config incomplete");
        lcd->setText("OTA Error", "Config incomplete");
        return false;
    }
    lcd->setText("OTA Config", "Loaded");
    return true;
}


bool OTAUpdateModule::checkUpdate() {
    if (updateJsonUrl == "") {
        Serial.println("[OTA] ERROR: updateJsonUrl empty");
        lcd->setText("OTA Error", "Update URL empty");
        return false;
    }

    HTTPClient http;
    http.begin(updateJsonUrl);

    int code = http.GET();
    if (code != 200) {
        Serial.println("[OTA] ERROR: Cannot GET update JSON");
        lcd->setText("OTA Error", "Cannot GET update JSON");
        return false;
    }

    String payload = http.getString();
    http.end();

    ArduinoJson::JsonDocument doc;
    if (deserializeJson(doc, payload)) {
        Serial.println("[OTA] JSON parse error");
        lcd->setText("OTA Error", "JSON parse error");
        return false;
    }

    latestVersion = doc["latest_version"] | "";
    String url    = doc["firmware_url"]  | "";

    if (latestVersion == "" || url == "") {
        Serial.println("[OTA] JSON missing fields");    
        lcd->setText("OTA Error", "JSON missing fields");
        return false;
    }

    Serial.println("[OTA] Server version: " + latestVersion);
    lcd->setText("OTA Server Version", latestVersion);
    Serial.println("[OTA] Local version : " + firmwareVersion);
    lcd->setText("OTA Local Version", firmwareVersion);

    return (latestVersion != firmwareVersion);
}

void OTAUpdateModule::handleHomePage() {
    Serial.println("[OTA] Serving Home Page");
    lcd->setText("OTA", "Serving Home Page");

    HTTPClient http;
    if (!http.begin(updateJsonUrl)) {
        server.send(500, "text/html",
            "<h3>Gagal mengakses update server!</h3>"
            "<a href='/'><button>Kembali</button></a>"
        );
        return;
    }

    int code = http.GET();
    if (code != 200) {
        server.send(500, "text/html",
            "<h3>Update server error: " + String(code) + "</h3>"
            "<a href='/'><button>Kembali</button></a>"
        );
        http.end();
        return;
    }

    String payload = http.getString();
    http.end();

    ArduinoJson::JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload);

    if (err) {
        server.send(500, "text/html",
            "<h3>Gagal parse JSON update!</h3>"
            "<p>Error: " + String(err.c_str()) + "</p>"
            "<a href='/'><button>Kembali</button></a>"
        );
        return;
    }

    String latest = doc["latest_version"] | "";
    String fwUrl  = doc["firmware_url"]   | "";

    bool updateAvailable = (latest != firmwareVersion);

    File htmlFile = LittleFS.open("/web/home.html", "r"); 
    String htmlContent = htmlFile.readString();
    htmlFile.close();
    String serverVars =  "window.currentVersion='" + firmwareVersion + "';"
      "window.latestVersion='" + latest + "';"
      "window.updateAvailable=" + String(updateAvailable ? "true" : "false");

    htmlContent.replace("<!--SERVER_VARS-->", serverVars);

    server.send(200, "text/html", htmlContent);
    
}

void OTAUpdateModule::handleProgressUpdate() {
    File file = LittleFS.open("/web/update.html", "r");
    if (!file) {
        server.send(500, "text/plain", "Gagal membuka halaman update!");
        return;
    }

    server.streamFile(file, "text/html");
    file.close();
    startOTAUpdate();
}

void OTAUpdateModule::startOTAUpdate() {
    if (updating) return;
    updating = true;

    // Jalankan OTA di task / thread terpisah jika menggunakan FreeRTOS
    xTaskCreate([](void* param) {
        OTAUpdateModule* self = (OTAUpdateModule*) param;
        self->performOTA();
        vTaskDelete(NULL);
    }, "OTAUpdateTask", 8192, this, 1, NULL);
}

void OTAUpdateModule::performOTA() {
    HTTPClient http;
    http.begin(updateJsonUrl);
    int code = http.GET();

    if (code != 200) {
        return;
    }

    String payload = http.getString();
    http.end();

    ArduinoJson::JsonDocument doc;
    if (deserializeJson(doc, payload)) {
        return;
    }

    latestVersion = doc["latest_version"] | "";
    String url    = doc["firmware_url"]  | "";

    WiFiClient client;
    http.begin(client, url);

    Serial.println("[OTA] Mengunduh firmware dari: " + url);
    lcd->setText("OTA", "Downloading firmware");

    int httpCode = http.GET();
    if (httpCode != 200) {
        Serial.printf("[OTA] Gagal download firmware, code: %d\n", httpCode);
        lcd->setText("OTA Error", "Download failed");
        http.end();
        updating = false;
        return;
    }

    int contentLength = http.getSize();
    WiFiClient* stream = http.getStreamPtr();

    Serial.println("[OTA] HTTP Code = " + String(httpCode));
    Serial.println("[OTA] Content-Type = " + http.header("Content-Type"));
    Serial.println("[OTA] Content-Length = " + String(contentLength));
    Serial.println("[OTA] Memulai update...");
    lcd->setText("OTA", "Starting update");

    if (!Update.begin(contentLength)) {
        Serial.println("[OTA] Gagal memulai Update");
        lcd->setText("OTA Error", "Update start failed");
        http.end();
        updating = false;
        return;
    }

    // baca firmware dari stream dan tulis ke flash
    size_t written = Update.writeStream(*stream);
    if (written == contentLength) {
        Serial.println("[OTA] Firmware ditulis penuh");
        lcd->setText("OTA", "Firmware written");
    } else {
        Serial.printf("[OTA] Firmware tertulis %u dari %d\n", written, contentLength);
        lcd->setText("OTA Error", "Write incomplete");
    }

    if (Update.end()) {
        if (Update.isFinished()) {
            Serial.println("[OTA] Update selesai, rebooting...");
            lcd->setText("OTA", "Update finished, rebooting");
            ArduinoJson::JsonDocument config;
            storage->readJSON("/config.json", config);
            config["version"] = latestVersion;
            Serial.println("[OTA] New version: " + latestVersion);
            Serial.println("[OTA] Before version: " + firmwareVersion);

            storage->writeJSON("/config.json", config);

            delay(1000);
            ESP.restart();
        } else {
            Serial.println("[OTA] Update gagal!");
            lcd->setText("OTA Error", "Update failed");
        }
    } else {
        Serial.printf("[OTA] Update error: %s\n", Update.errorString());
        lcd->setText("OTA Error", "Update error");
    }

    http.end();
    updating = false;
}