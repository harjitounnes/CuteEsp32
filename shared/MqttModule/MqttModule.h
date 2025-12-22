#pragma once

#include <WiFi.h>
#include "WiFiModule.h"
#include "StorageModule.h"
#include "LCDModule.h"
#include <ArduinoJson.h>

struct MqttPublishMsg {
    char topic[64];
    char payload[128];
};

class MqttModule {
public:
    using MessageCallback = std::function<void(const char* topic, const char* payload)>;

    MqttModule(
        WiFiClient* client,
        WiFiModule* wifi,
        StorageModule* storage,
        LCDModule* lcd
    );

    void begin();
    bool publish(const char* topic, const char* payload);
    void subscribe(const char* topic);
    void onMessage(MessageCallback cb);
    bool publishRaw(const char* topic, const char* payload);


private:
    WiFiClient* client;
    WiFiModule* wifi;
    StorageModule* storage;
    LCDModule* lcd;

    String broker;
    uint16_t port;
    String clientId;

    String username;
    String password;
    String topicSubscribe;
    String topicPublish;
    
    MessageCallback messageCallback;

    // MQTT internal
    bool connectBroker();
    void handleMessage(const char* topic, const char* payload);

    // low-level MQTT
    void sendString(const char* str);
    void sendRemainingLength(uint32_t len);

    static void mqttTaskWrapper(void* param);
    void mqttTask();

    void processIncoming();
    TaskHandle_t mqttTaskHandle = nullptr;
    QueueHandle_t publishQueue  = nullptr;
};
