#pragma once

#include <Adafruit_SSD1306.h>
#include "Games/IGame.hpp"

class Menu {
public:
    Menu(Adafruit_SSD1306 &display, Game::IGame *games[]) : _display(display), _games(games) {
        _numGames = 0;
        while (games[_numGames + 1] != nullptr) {
            _numGames++;
        }

        _currentChoice = 0.0;
    }

    int run() {
        while (true) {
            _display.clearDisplay();
            _update();
            _display.display();

            if (digitalRead(PIN_BUTTON_A) == HIGH) {
                rgbLedWrite(PIN_RGB_LED, 0, 32, 32);
                delay(100);
                rgbLedWrite(PIN_RGB_LED, 0, 0, 0);
                return round(_currentChoice);
            }
        }
    }
private:
    void _update() {
        float jy = (analogRead(PIN_JOYSTICK_Y) - 2048) / -2048.0;
        bool buttonB = digitalRead(PIN_BUTTON_B) == HIGH;

        if (abs(jy) < 0.2) jy = 0;

        _currentChoice = constrain(_currentChoice + _scrollSpeed * jy, 0, _numGames - 1);

        // Scroll indicator
        for (int i = 0; i < 6; i++) {
            _display.drawPixel(SCREEN_WIDTH - 3, 5 + i * 10, SSD1306_WHITE);
            _display.drawPixel(SCREEN_WIDTH - 3, 5 + i * 10 + 1, SSD1306_WHITE);
            _display.drawPixel(SCREEN_WIDTH - 3, 5 + i * 10 + 2, SSD1306_WHITE);
            _display.drawPixel(SCREEN_WIDTH - 3, 5 + i * 10 + 3, SSD1306_WHITE);
            _display.drawPixel(SCREEN_WIDTH - 4, 5 + i * 10, SSD1306_WHITE);
            _display.drawPixel(SCREEN_WIDTH - 4, 5 + i * 10 + 1, SSD1306_WHITE);
            _display.drawPixel(SCREEN_WIDTH - 4, 5 + i * 10 + 2, SSD1306_WHITE);
            _display.drawPixel(SCREEN_WIDTH - 4, 5 + i * 10 + 3, SSD1306_WHITE);
        };
        _display.fillRoundRect(SCREEN_WIDTH - 6, ((_currentChoice * (SCREEN_HEIGHT - 20)) / (_numGames - 1)) + 3, 6, 16, 2, SSD1306_WHITE);

        // Offset for scrolling
        int offset = 0;
        if (_numGames > 3) {
            if (_currentChoice > (_numGames - 2)) offset = (_numGames - 3) * 20;
            else if (_currentChoice > 1.0) offset = (_currentChoice - 1.0) * 20;
        }

        // Display the games with the offset
        for (int i = 0; i < _numGames; i++) {
            if (i == round(_currentChoice)) {
                _display.fillRoundRect(0, i * 20 + 4 - offset, SCREEN_WIDTH - 15, 15, 10, SSD1306_WHITE);
            } else {
                _display.drawRoundRect(0, i * 20 + 4 - offset, SCREEN_WIDTH - 15, 15, 10, SSD1306_WHITE);
            }
            int color = i == round(_currentChoice) ? SSD1306_BLACK : SSD1306_WHITE;
            _display.drawCircle(SCREEN_WIDTH - 23, i * 20 + 11 - offset, 7, color);
            _display.setTextColor(color);

            _display.setTextSize(1);
            _display.setCursor(8, 8 + i * 20 - offset);
            _display.print(_games[i]->getName().c_str());
            _display.setCursor(SCREEN_WIDTH - 25, 8 + i * 20 - offset);
            _display.print(_games[i]->isNetworkGame() ? "M" : "S");
        }

        if (buttonB) _currentChoice = 0.0;
    }

    Adafruit_SSD1306 &_display;
    Game::IGame **_games;
    int _numGames;
    float _currentChoice;
    float _scrollSpeed = 0.25;
};