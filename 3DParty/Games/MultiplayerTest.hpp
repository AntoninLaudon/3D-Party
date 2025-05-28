#pragma once

#include "IGame.hpp"
#include "tools.hpp"

namespace Game {

// ################################################################
// ####  MULTIPLAYERTEST GAME CONSTANTS  ##########################
// ################################################################

#define MULTIPLAYERTEST_MAX_DATA 5

// ################################################################
// ######  MULTIPLAYERTEST CLASS DEFINITION  ######################
// ################################################################

class MultiplayerTest : public IGame {
public:
    MultiplayerTest(Adafruit_SSD1306 &display) : IGame(display) {}
    void init() override;
    void step() override;
private:
    void _update(unsigned long deltaTime) override;
    void _draw() override;

    std::string *_dataInfoSend = new std::string[MULTIPLAYERTEST_MAX_DATA];
    std::string *_dataInfoReceive = new std::string[MULTIPLAYERTEST_MAX_DATA];
};


// ################################################################
// ######  MULTIPLAYERTEST CLASS IMPLEMENTATION  ##################
// ################################################################

void MultiplayerTest::init() {
    _isNetworkGame = true;
    _name = "Multi Test";
    _lastUpdate = millis();
}

void MultiplayerTest::step() {
    unsigned long currentMillis = millis();
    unsigned long deltaTime = currentMillis - _lastUpdate;
    if (deltaTime < UPDATE_INTERVAL) return;
    _lastUpdate = currentMillis;

    _retrieveInputs();
    _update(deltaTime);
    _draw();
}

void MultiplayerTest::_update(unsigned long deltaTime) {

    // Send data
    if (_buttonANew || _buttonBNew) {
        for (int i = MULTIPLAYERTEST_MAX_DATA - 1; i > 0; i--) {
            _dataInfoSend[i] = _dataInfoSend[i - 1];
        }
        // add the new data
        if (_buttonANew) {
            _dataInfoSend[0] = "Button A";
        } else if (_buttonBNew) {
            _dataInfoSend[0] = "Button B";
        }
        Serial.print("Sending: ");
        Serial.println(_dataInfoSend[0].c_str());

        // TODO: Implement sending data
    }

    // Receive data
    if (false) { // TODO: Implement receiving data
        for (int i = MULTIPLAYERTEST_MAX_DATA - 1; i > 0; i--) {
            _dataInfoReceive[i] = _dataInfoReceive[i - 1];
        }
        // add the new data
        _dataInfoReceive[0] = "DATA"; // TODO: Replace with actual data
        Serial.print("Receiving: ");
        Serial.println(_dataInfoReceive[0].c_str());
    }
}

void MultiplayerTest::_draw() {
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);
    _display.setCursor(20, 0);
    _display.print("Multiplayer Test");

    _display.setCursor(4, 9);
    _display.print("Sent:");
    _display.setCursor(69, 9);
    _display.print("Received:");
    _display.drawLine(0, 17, SCREEN_WIDTH, 17, SSD1306_WHITE);
    _display.drawLine(SCREEN_WIDTH / 2, 10, SCREEN_WIDTH / 2, SCREEN_HEIGHT, SSD1306_WHITE);

    for (int i = 0; i < MULTIPLAYERTEST_MAX_DATA; i++) {
        _display.setCursor(1, 19 + i * 9);
        _display.print(_dataInfoSend[MULTIPLAYERTEST_MAX_DATA - i - 1].c_str());
        _display.setCursor(66, 19 + i * 9);
        _display.print(_dataInfoReceive[MULTIPLAYERTEST_MAX_DATA - i - 1].c_str());
    }

    _display.display();
}

} // namespace Game
