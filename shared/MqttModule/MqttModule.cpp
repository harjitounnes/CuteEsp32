#include "MqttModule.h"

MqttModule::MqttModule(
    WiFiClient* client,
    WiFiModule* wifi,
    StorageModule* storage,
    LCDModule* lcd
) {
    this->client = client;
    this->wifi = wifi;
    this->storage = storage;
    this->lcd = lcd;
}

void MqttModule::begin() {
    ArduinoJson ::JsonDocument doc;

    if (!storage->readJSON("/config.json", doc)) {
        Serial.println("[MQTT] Config not found");
        return;
    }

    broker = doc["mqtt_host"] | "";
    port = doc["mqtt_port"] | 1883;
    clientId = doc["mqtt_client_id"] | "esp32-device";
    username = doc["mqtt_username"] | "";
    password = doc["mqtt_password"] | "";
    topicSubscribe = doc["mqtt_subscribe_topic"] | "esp32/subscribe";
    topicPublish = doc["mqtt_publish_topic"] | "esp32/publish";

    lcd->setText("MQTT", "Connecting...");

    publishQueue = xQueueCreate(5, sizeof(MqttPublishMsg));

    xTaskCreatePinnedToCore(
        mqttTaskWrapper,
        "MQTT_Task",
        6144,
        this,
        1,
        &mqttTaskHandle,
        1   // core 1 (aman dari WiFi core 0)
    );

}

void MqttModule::mqttTaskWrapper(void* param) {
    static_cast<MqttModule*>(param)->mqttTask();
}

void MqttModule::mqttTask() {
    uint32_t lastReconnect = 0;

    for (;;) {
        if (!wifi->isConnected()) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (!client->connected()) {
            if (millis() - lastReconnect > 3000) {
                lastReconnect = millis();
                connectBroker();
            }
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        MqttPublishMsg msg;
        if (xQueueReceive(publishQueue, &msg, pdMS_TO_TICKS(10)) == pdTRUE) {
            publishRaw(msg.topic, msg.payload);
        }
        // === baca incoming ===
        processIncoming();


        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void MqttModule::processIncoming() {
    if (!client->available()) return;

    uint8_t header = client->read();
    if ((header & 0xF0) != 0x30) return;

    uint8_t len = client->read();

    uint16_t topicLen = (client->read() << 8) | client->read();
    char topic[64];
    client->readBytes(topic, topicLen);
    topic[topicLen] = 0;

    char payload[128];
    int payloadLen = len - topicLen - 2;
    client->readBytes(payload, payloadLen);
    payload[payloadLen] = 0;

    handleMessage(topic, payload);
}

bool MqttModule::connectBroker() {
    if (!wifi->isConnected()) return false;
    if (!client) return false;

    Serial.printf("[MQTT] TCP connect %s:%d\n", broker.c_str(), port);

    if (!client->connect(broker.c_str(), port)) {
        lcd->setText("MQTT", "TCP Failed");
        return false;
    }

    // ===== CONNECT FLAGS =====
    uint8_t connectFlags = 0x02; // Clean Session
    if (username.length()) connectFlags |= 0x80; // Username flag
    if (password.length()) connectFlags |= 0x40; // Password flag

    // ===== Remaining Length =====
    uint16_t remainingLen =
          10
        + 2 + clientId.length()
        + (username.length() ? 2 + username.length() : 0)
        + (password.length() ? 2 + password.length() : 0);

    // ===== FIXED HEADER =====
    client->write((uint8_t)0x10);              // CONNECT
    sendRemainingLength(remainingLen);

    // ===== VARIABLE HEADER =====
    client->write((uint8_t)0x00);
    client->write((uint8_t)0x04);
    client->write("MQTT", 4);                  // Protocol Name
    client->write((uint8_t)0x04);              // MQTT 3.1.1
    client->write(connectFlags);               // 🔥 CONNECT FLAGS (BENAR)
    client->write((uint8_t)0x00);
    client->write((uint8_t)60);                // KeepAlive 60s

    // ===== PAYLOAD =====
    sendString(clientId.c_str());
    if (username.length()) sendString(username.c_str());
    if (password.length()) sendString(password.c_str());

    // ===== CONNACK =====
    uint8_t ack[4];
    uint32_t timeout = millis() + 3000;
    while (!client->available()) {
        if (millis() > timeout) {
            Serial.println("[MQTT] CONNACK timeout");
            client->stop();
            return false;
        }
        delay(10);
    }

    client->readBytes(ack, 4);

    if (ack[0] != 0x20 || ack[1] != 0x02 || ack[3] != 0x00) {
        Serial.printf("[MQTT] CONNACK error code=%d\n", ack[3]);
        client->stop();
        return false;
    }

    subscribe(topicSubscribe.c_str());

    lcd->setText("MQTT", "Connected");
    Serial.println("[MQTT] Connected OK");

    return true;
}

bool MqttModule::publish(const char* topic, const char* payload) {
    if (!publishQueue) return false;

    MqttPublishMsg msg;
    strncpy(msg.topic, topic, sizeof(msg.topic) - 1);
    strncpy(msg.payload, payload, sizeof(msg.payload) - 1);
    msg.topic[sizeof(msg.topic) - 1] = 0;
    msg.payload[sizeof(msg.payload) - 1] = 0;

    return xQueueSend(publishQueue, &msg, 0) == pdTRUE;
}

void MqttModule::subscribe(const char* topic) {
    if (!client->connected()) return;

    client->write((uint8_t)0x82);
    sendRemainingLength(strlen(topic) + 5);
    client->write((uint8_t)0x00);
    client->write((uint8_t)0x01);
    sendString(topic);
    client->write((uint8_t)0x00);
}

void MqttModule::handleMessage(const char* topic, const char* payload) {
    lcd->setText(topic, payload);

    if (messageCallback) {
        Serial.printf("[MQTT] Message received on topic %s: %s\n", topic, payload);
        messageCallback(topic, payload);
    }
}

void MqttModule::sendString(const char* str) {
    uint16_t len = strlen(str);
    client->write(len >> 8);
    client->write(len & 0xFF);
    client->write(str);
}

void MqttModule::sendRemainingLength(uint32_t len) {
    do {
        uint8_t digit = len % 128;
        len /= 128;
        if (len > 0) digit |= 0x80;
        client->write(digit);
    } while (len > 0);
}

void MqttModule::onMessage(MessageCallback cb) {
    Serial.println("[MQTT] Message callback set");
    messageCallback = cb;
}

bool MqttModule::publishRaw(const char* topic, const char* payload) {
    if (!client || !client->connected()) return false;

    uint16_t len = strlen(topic) + strlen(payload) + 2;

    client->write((uint8_t)0x30);     
    sendRemainingLength(len);
    sendString(topic);
    client->write(payload);

    client->flush();                  

    return true;
}
