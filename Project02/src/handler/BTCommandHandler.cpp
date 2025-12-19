#include "BTCommandHandler.h"
#include <ArduinoJson.h>
#include "WiFi.h"

BTCommandHandler::BTCommandHandler(BluetoothModule* bt, StorageModule* storage, LCDModule* lcd): bt(bt), storage(storage), lcd(lcd) {}

void BTCommandHandler::saveConfig(const char* path, const char* key, const String& value) {
    ArduinoJson::JsonDocument doc;
    storage->readJSON(path, doc);
    doc[key] = value;
    storage->writeJSON(path, doc);
    lcd->setText("BT TX: ", "config updated: " + String(key));
}

void BTCommandHandler::handle(const String& msg) {
    Serial.println("[Handler] Received: " + msg);
    lcd->setText("Handler RX:", msg);

    if (msg == "read-config") {
        ArduinoJson::JsonDocument doc;
        if (storage->readJSON("/config.json", doc)) {
            String out;
            serializeJson(doc, out);
            send(out + "\n");
            lcd->setText("BT TX: ", out);
            Serial.println("[BT TX] config sent");
        } else {
            send("{\"error\":\"failed to read config\"}\n");
            lcd->setText("BT TX: ", "read config error");
        }
    }

    else if (msg.startsWith("add-config")) {
        int jsonStart = msg.indexOf('{');
        if (jsonStart == -1) {
            send("{\"error\":\"invalid format\"}\n");
            lcd->setText("BT TX: ", "invalid format");
        } else {
            String jsonPart = msg.substring(jsonStart);
            ArduinoJson::JsonDocument incoming;
            DeserializationError err = deserializeJson(incoming, jsonPart);
            if (err) {
                send("{\"error\":\"invalid json\"}\n");
                lcd->setText("BT TX: ", "invalid json");
            } else {
                String key = incoming["key"] | "";
                String value = incoming["value"] | "";

                if (key.length() == 0) {
                    send("{\"error\":\"missing key\"}\n");
                    lcd->setText("BT TX: ", "missing key");
                    return;
                }

                BTCommandHandler::saveConfig("/config.json", key.c_str(), value);
                send("{\"success\":\"config updated\"}\n");
            }
        }
    }

    else if (msg == "get-ip") {
        IPAddress ip = WiFi.localIP();
        String json = "{\"info\":\"" + ip.toString() + "\"}\n";
        send("{\"info\":\""+ ip.toString() + "\"}\n");
        lcd->setText("BT TX: ", "IP sent: " + ip.toString());
        Serial.println("[BT] IP sent: " + ip.toString());
    }

    else if (msg == "restart") {
        send("{\"info\":\"restarting ESP32\"}\n");
        Serial.println("[BT] Restarting ESP32");
        lcd->setText("BT TX: ", "restarting ESP32");
        delay(100); // beri waktu untuk kirim data sebelum restart

        ESP.restart();
    }

    else if(msg == "*123#") {
        send("{\"info\":\"Menjalankan motor\"}\n");
        Serial.println("[BT] Menjalankan motor");
        lcd->setText("BT TX: ", "Menjalankan motor");
        delay(100); // beri waktu untuk kirim data sebelum reset
    }

    else {
        send("Unknown command: " + msg + "\n");
        lcd->setText("BT TX: ", "Unknown command: " + msg);
    }


}

void BTCommandHandler::send(const String& msg) {
    bt->send(msg + "\n");
    lcd->setText("Handler TX:", msg);
}

