#ifndef BLUETOOTH_MODULE_H
#define BLUETOOTH_MODULE_H

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <LCDModule.h>
#include <functional>

typedef std::function<void(const String& msg)> BTMessageCallback;

class BluetoothModule {
public:
    BluetoothModule(const String& deviceName,  LCDModule* lcd);
    void begin();     
    void loop();
    void send(const String& msg);
    void onMessage(BTMessageCallback cb);    
    
private:
    BluetoothSerial BT;
    BTMessageCallback messageCallback = nullptr;
    LCDModule* lcd;
    String name;
};

#endif
