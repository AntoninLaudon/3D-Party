#pragma once

#include <string>
#include <cmath>
#include <Adafruit_SSD1306.h>
#include "config.h"

namespace Game {

// ################################################################
// ######  IGAME CLASS DEFINITION  ################################
// ################################################################

class IGame {
public:
    IGame(Adafruit_SSD1306 &display) : _display(display) {}

    virtual void init() = 0;
    virtual int run();
    virtual void step() = 0;
    std::string getName() const;
    bool isNetworkGame() const;

    bool isRunning() const;

protected:
    void _retrieveInputs();
    virtual void _update(unsigned long deltaTime) = 0;
    virtual void _draw() = 0;
    Adafruit_SSD1306 &_display;
    std::string _name;
    bool _isNetworkGame = false;
    unsigned long _lastUpdate = 0;
    bool _isRunning = true;

    // Joystick and button states
    float _jx = 0;
    float _jy = 0;
    bool _jPressed = false;
    bool _buttonA = false;
    bool _buttonB = false;

    // button new states (it will be true only once until it is released and pressed again)
    // Example: if you press the button A, _buttonANew will be true for one loop even if the button is still pressed
    bool _buttonANew = false;
    bool _buttonBNew = false;
    bool _jPressedNew = false;
private:
    // Internal state for button press detection
    bool _buttonAWasPressed = false;
    bool _buttonBWasPressed = false;
    bool _jPressedWasPressed = false;

};


// ################################################################
// ######  IGAME CLASS IMPLEMENTATION  ############################
// ################################################################

int IGame::run() {
    while (_isRunning)
    {
        step();
    }
    return 0;
}

std::string IGame::getName() const {
    return _name;
}

bool IGame::isNetworkGame() const {
    return _isNetworkGame;
}

bool IGame::isRunning() const {
    return _isRunning;
}

void IGame::_retrieveInputs() {
    _jx = JOYSTICK_X_ORIENTATION * (analogRead(PIN_JOYSTICK_X) - 2048) / 2048.0;
    _jy = JOYSTICK_Y_ORIENTATION * (analogRead(PIN_JOYSTICK_Y) - 2048) / -2048.0;
    _jPressed = digitalRead(PIN_JOYSTICK_BUTTON) == HIGH;
    _buttonA = digitalRead(PIN_BUTTON_A) == HIGH;
    _buttonB = digitalRead(PIN_BUTTON_B) == HIGH;

    // Normalize joystick vector
    float length = sqrt(_jx * _jx + _jy * _jy);
    if (length > 1.0) {
        _jx /= length;
        _jy /= length;
    }
    if (abs(_jx) < DEADZONE) _jx = 0;
    if (abs(_jy) < DEADZONE) _jy = 0;

    // Button new states
    if (_buttonA && !_buttonAWasPressed) {
        _buttonANew = true;
    } else {
        _buttonANew = false;
    }
    if (_buttonB && !_buttonBWasPressed) {
        _buttonBNew = true;
    } else {
        _buttonBNew = false;
    }
    if (_jPressed && !_jPressedWasPressed) {
        _jPressedNew = true;
    } else {
        _jPressedNew = false;
    }
    // Update button states
    _buttonAWasPressed = _buttonA;
    _buttonBWasPressed = _buttonB;
    _jPressedWasPressed = _jPressed;





}

} // namespace Game
