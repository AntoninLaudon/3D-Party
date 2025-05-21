#pragma once

#include "IGame.hpp"
#include "tools.hpp"

namespace Game {

// ################################################################
// ######  COMPONENTTEST CLASS DEFINITION  ########################
// ################################################################

class ComponentTest : public IGame {
public:
    ComponentTest(Adafruit_SSD1306 &display) : IGame(display) {}
    void init() override;
    void step() override;
private:
    void _update(unsigned long deltaTime) override;
    void _draw() override;

    bool _wasJoystickPressed = false;
    bool _isBuzzerOn = false;
    int _hue = 180;
    int _lum = 0;
};


// ################################################################
// ######  COMPONENTTEST CLASS IMPLEMENTATION  ####################
// ################################################################

void ComponentTest::init() {
    _isNetworkGame = false;
    _name = "Component Test";
    _lastUpdate = millis();
}

void ComponentTest::step() {
    unsigned long currentMillis = millis();
    unsigned long deltaTime = currentMillis - _lastUpdate;
    if (deltaTime < UPDATE_INTERVAL) return;
    _lastUpdate = currentMillis;

    _retrieveInputs();
    _update(deltaTime);
    _draw();
}

void ComponentTest::_update(unsigned long deltaTime){
    _display.clearDisplay();

    // DRAW JOYSTICK
    _display.drawCircle(32, 32, 20, SSD1306_WHITE);
    if (_jPressed) {
        _display.drawCircle(32 + (int)(_jx * 20), 32 + (int)(_jy * 20), 10, SSD1306_WHITE);
    } else {
        _display.fillCircle(32 + (int)(_jx * 20), 32 + (int)(_jy * 20), 10, SSD1306_WHITE);
    }

    // DRAW BUTTONS
    if (_buttonA) {
        _display.drawCircle(80, 40, 10, SSD1306_WHITE);
    } else {
        _display.fillCircle(80, 40, 10, SSD1306_WHITE);
    }
    if (_buttonB) {
        _display.drawCircle(100, 24, 10, SSD1306_WHITE);
    } else {
        _display.fillCircle(100, 24, 10, SSD1306_WHITE);
    }

    // BUZZER & TOGGLE JOYSTICK BUTTON
    if (_jPressed && !_wasJoystickPressed) _isBuzzerOn = !_isBuzzerOn;
    _wasJoystickPressed = _jPressed;

    int frequency = 1000 * abs(_jy) + 5000 * abs(_jx);
    if (_isBuzzerOn) {
        _display.fillCircle(64, 10, 3, SSD1306_WHITE);
        tone(PIN_BUZZER, frequency);
    } else {
        _display.drawCircle(64, 10, 3, SSD1306_WHITE);
        noTone(PIN_BUZZER);
    }
    _display.setTextSize(1);
    _display.setCursor(70, 7);
    _display.print(frequency);
    
    // RGB LED SLIDERS
    _display.drawLine(8, 60, 120, 60, SSD1306_WHITE);
    _display.fillCircle((_hue * 112) / 360 + 8, 60, 2, SSD1306_WHITE);
    
    _display.drawLine(120, 4, 120, 60, SSD1306_WHITE);
    _display.fillCircle(120, ((100 - _lum) * 56) / 100 + 4, 2, SSD1306_WHITE);
    
    // RGB LED
    if (_buttonA) _hue = (_hue + 2) % 360;
    if (_buttonB) _lum = (_lum + 1) % 100;
    int r, g, b;
    HSVtoRGB(_hue, 1.0, _lum / 100.0, r, g, b);
    rgbLedWrite(PIN_RGB_LED, r, g, b);
}

void ComponentTest::_draw() {
    _display.display();
}

} // namespace Game