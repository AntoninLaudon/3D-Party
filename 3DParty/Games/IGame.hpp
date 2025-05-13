#pragma once

#include <string>
#include <Adafruit_SSD1306.h>
#include "config.h"

class IGame {
public:
    IGame(Adafruit_SSD1306 &display) : _display(display) {}
    
    virtual void init() = 0;
    virtual int run() = 0;
    virtual std::string getName() const { return _name; }
    virtual bool isNetworkGame() const { return _isNetworkGame; }
    
    protected:
    void _retrieveInputs() {
        _jx = (analogRead(PIN_JOYSTICK_X) - 2048) / 2048.0;
        _jy = (analogRead(PIN_JOYSTICK_Y) - 2048) / -2048.0;
        _jPressed = digitalRead(PIN_JOYSTICK_BUTTON) == LOW;
        _buttonA = digitalRead(PIN_BUTTON_A) == LOW;
        _buttonB = digitalRead(PIN_BUTTON_B) == LOW;

        // Normalize joystick vector
        float length = sqrt(_jx * _jx + _jy * _jy);
        if (length > 1.0) {
            _jx /= length;
            _jy /= length;
        }
        if (abs(_jx) < DEADZONE) _jx = 0;
        if (abs(_jy) < DEADZONE) _jy = 0;
    }
    virtual void _update(unsigned long deltaTime) = 0;
    virtual void _draw() = 0;
    Adafruit_SSD1306 &_display;
    std::string _name;
    bool _isNetworkGame = false;
    unsigned long _lastUpdate = 0;

    float _jx = 0;
    float _jy = 0;
    bool _jPressed = false;
    bool _buttonA = false;
    bool _buttonB = false;

};
