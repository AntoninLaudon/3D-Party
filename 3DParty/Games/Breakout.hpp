#pragma once

#include "IGame.hpp"

class BreakoutGame : public IGame {
public:
    BreakoutGame(Adafruit_SSD1306 &display) : IGame(display) {}

    void init() override {
        _isNetworkGame = true;
        _name = "Breakout";
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
    }

    void _draw() override {
        _display.display();
    }
private:
};