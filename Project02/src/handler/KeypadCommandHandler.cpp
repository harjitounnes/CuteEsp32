#include "KeypadCommandHandler.h"

#define CMD_TIMEOUT 5000
#define MAX_CMD_LEN 10

KeypadCommandHandler::KeypadCommandHandler(LCDModule* lcd) : _lcd(lcd) {}

void KeypadCommandHandler::onCommand(CommandCallback cb) {
    _cmdCallback = cb;
}

void KeypadCommandHandler::reset() {
    _buffer = "";
    _commandMode = false;
}

void KeypadCommandHandler::onKey(char key) {
    _lastKeyTime = millis();

    // START COMMAND
    if (key == '*') {
        _buffer = "*";
        _commandMode = true;
        _lcd->setText("Command Mode", "Enter command...");
        return;
    }

    if (!_commandMode) {
        _lcd->setText("Info", "Not in command mode.");  
        return;
    }

    // ENTER
    if (key == '#') {
        _buffer += '#';
        process();
        reset();
        _lcd->setText("Command Mode", "Command processed.");
        return;
    }

    _buffer += key;
    _lcd->setText("Command Mode", _buffer);
    if (_buffer.length() > MAX_CMD_LEN) {
        reset();
    }
}

void KeypadCommandHandler::process() {
    if (!_cmdCallback) return;

    // valid minimal: *x#
    if (_buffer.length() < 3) return;

    _cmdCallback(_buffer);
}
