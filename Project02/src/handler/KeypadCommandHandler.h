#pragma once
#include <Arduino.h>
#include <LCDModule.h>

class KeypadCommandHandler {
public:
    typedef void (*CommandCallback)(const String& cmd);

    KeypadCommandHandler(LCDModule* lcd);

    void onKey(char key);                 // dipanggil dari KeypadModule
    void onCommand(CommandCallback cb);   // callback jika command valid

private:
    void reset();
    void process();

    String _buffer;
    bool   _commandMode = false;
    unsigned long _lastKeyTime = 0;

    CommandCallback _cmdCallback = nullptr;
    LCDModule* _lcd = nullptr; 
};
