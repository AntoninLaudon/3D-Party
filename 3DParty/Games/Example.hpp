#pragma once

#include "IGame.hpp"

void HSVtoRGB(int h, float s, float v, int &r, int &g, int &b) {
    h = h % 360;
    float c = v * s;  
    float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
    float m = v - c;
    float r1, g1, b1;

    switch (h / 60) {
        case 0:  r1 = c, g1 = x, b1 = 0; break;
        case 1:  r1 = x, g1 = c, b1 = 0; break;
        case 2:  r1 = 0, g1 = c, b1 = x; break;
        case 3:  r1 = 0, g1 = x, b1 = c; break;
        case 4:  r1 = x, g1 = 0, b1 = c; break;
        case 5:  r1 = c, g1 = 0, b1 = x; break;
        default: r1 = 0, g1 = 0, b1 = 0; break;
    }
    r = constrain((r1 + m) * 255, 0, 255);
    g = constrain((g1 + m) * 255, 0, 255);
    b = constrain((b1 + m) * 255, 0, 255);
}

class ExampleGame : public IGame {
public:
    ExampleGame(Adafruit_SSD1306 &display) : IGame(display) {}

    void init() override {
        _name = "Example Game";
        _lastUpdate = millis();
    }

    int run() override {
        unsigned long currentMillis = millis();
        unsigned long deltaTime = currentMillis - _lastUpdate;
        if (deltaTime < UPDATE_INTERVAL) return 0;
        _lastUpdate = currentMillis;

        _retrieveInputs();
        _update(deltaTime);
        _draw();

        return 0;
    }

    void _update(unsigned long deltaTime) override {

        _display.clearDisplay();

        // Draw joystick
        _display.drawCircle(32, 32, 20, SSD1306_WHITE);
        if (_jPressed) {
            _display.drawCircle(32 + (int)(_jx * 20), 32 + (int)(_jy * 20), 10, SSD1306_WHITE);
        } else {
            _display.fillCircle(32 + (int)(_jx * 20), 32 + (int)(_jy * 20), 10, SSD1306_WHITE);
        }

        // Draw buttons
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

    void _draw() override {
        _display.display();
    }
private:
    bool _wasJoystickPressed = false;

    bool _isBuzzerOn = false;
    int _hue = 180;
    int _lum = 0;
};