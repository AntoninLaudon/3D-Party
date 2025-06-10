#pragma once

#include <Adafruit_SSD1306.h>
#include <esp_wifi.h>
#include <Preferences.h>
#include "config.h"
#include "Network/GameProtocol.hpp"
#include "tools.hpp"

namespace Game {

// ################################################################
// ####  GAME CONSTANTS  ##########################################
// ################################################################

enum GameReturnCode {
    QUIT,
    RESTART,
    CONTINUE
};

// ################################################################
// ######  IGAME CLASS DEFINITION  ################################
// ################################################################

class IGame {
public:
    IGame(Adafruit_SSD1306 &display) : _display(display) {}
    
    virtual void init() = 0;
    virtual void step() = 0;
    GameReturnCode run();
    String getName() const;
    bool isNetworkGame() const;
    GameReturnCode getGameState() const;
    bool connectionLobby(int id);
    void setHost(bool isHost);
    
protected:
    virtual void _firstStep() {};
    void _retrieveInputs();
    virtual void _update(unsigned long deltaTime) = 0;
    virtual void _draw() = 0;
    Adafruit_SSD1306 &_display;
    String _name;
    bool _isNetworkGame = false;
    bool _isHost = false;
    unsigned long _lastUpdate = 0;
    GameReturnCode _gameState = CONTINUE;
    Preferences _NVSData;

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

    bool _imReady = false;
    bool _peerReady = false;

};


// ################################################################
// ######  IGAME CLASS IMPLEMENTATION  ############################
// ################################################################

GameReturnCode IGame::run() {
    while (_gameState == CONTINUE) {
        step();
    }
    return _gameState;
}

String IGame::getName() const {
    return _name;
}

bool IGame::isNetworkGame() const {
    return _isNetworkGame;
}

GameReturnCode IGame::getGameState() const {
    return _gameState;
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

bool IGame::connectionLobby(int id) {
    Networking::GameProtocol &protocol = Networking::GameProtocol::get();

    if (protocol.isConnected()) protocol.reset();

    protocol.begin();
    protocol.invite(id);

    _display.clearDisplay();
    _display.setTextColor(SSD1306_WHITE);
    _display.setTextSize(1);
    _display.setCursor(SCREEN_WIDTH / 2 - (_name.length() / 2.0) * 5, 0);
    _display.print(_name);
    _display.setCursor(14, 20);
    _display.print("Waiting for peer");
    _display.drawLine(0, 12, SCREEN_WIDTH, 12, SSD1306_WHITE);
    _display.display();

    unsigned long startTime = millis();
    while (!protocol.isConnected()) {
        protocol.loop();
        _display.fillRect(0, 40, 128, 16, SSD1306_BLACK);
        int nbrPoints = (millis() - startTime) / 500 % 4;
        if (nbrPoints == 0) {
            _display.fillCircle(64, 48, 4, SSD1306_WHITE);
        } else if (nbrPoints == 1 || nbrPoints == 3) {
            _display.fillCircle(56, 48, 4, SSD1306_WHITE);
            _display.fillCircle(72, 48, 4, SSD1306_WHITE);
        } else if (nbrPoints == 2) {
            _display.fillCircle(48, 48, 4, SSD1306_WHITE);
            _display.fillCircle(64, 48, 4, SSD1306_WHITE);
            _display.fillCircle(80, 48, 4, SSD1306_WHITE);
        }
        _display.display();
        _retrieveInputs();
        if (_buttonANew || _buttonBNew) {
            Serial.println("Connection cancelled by user.");
            protocol.reset();
            return false;
        }
    }
    uint8_t peerMac[6];
    protocol.getPeerMac(peerMac);
    uint8_t myMac[6];
    esp_wifi_get_mac(WIFI_IF_STA, myMac);
    Serial.printf("My MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", myMac[0], myMac[1], myMac[2], myMac[3], myMac[4], myMac[5]);
    Serial.printf("Peer MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", peerMac[0], peerMac[1], peerMac[2], peerMac[3], peerMac[4], peerMac[5]);
    setHost(isMacBigger(peerMac, myMac));

    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setCursor(SCREEN_WIDTH / 2 - (_name.length() / 2) * 5, 0);
    _display.print(_name);
    _display.drawLine(0, 12, SCREEN_WIDTH, 12, SSD1306_WHITE);
    _display.setCursor(31, 20);
    _display.print("Connected !");

    while (_imReady == false || _peerReady == false) {
        protocol.loop();
        _retrieveInputs();
        _display.fillRect(0, 40, 128, 24, SSD1306_BLACK);
        
        if (_buttonANew) {
            _imReady = true;
            protocol.sendData("READY", 5, true);
        }

        if (_imReady) {
            _display.fillRoundRect(32, 40, 64, 16, 8, SSD1306_WHITE);
            _display.setTextColor(SSD1306_BLACK);
            _display.setCursor(50, 44);
            _display.print("Ready");
        } else {
            _display.drawRoundRect(32, 40, 64, 16, 8, SSD1306_WHITE);
            _display.setTextColor(SSD1306_WHITE);
            _display.setCursor(38, 44);
            _display.print("Not ready");
        }

        if (_peerReady) {
            _display.setTextColor(SSD1306_WHITE);
            _display.setCursor(25, 30);
            _display.print("Peer is ready");
        }

        while (protocol.hasPacket()) {
            Networking::GameProtocol::DataPacket pkt;
            protocol.getPacket(pkt);
            if (strncmp((char*)pkt.data, "READY", pkt.len) == 0) _peerReady = true;
        }
        _display.display();
    }
    
    Serial.println("Game starting...");
    return true;
}

void IGame::setHost(bool isHost) {
    _isHost = isHost;
    Serial.printf("Role : %s\n", isHost ? "Host" : "Client");
    rgbLedWrite(PIN_RGB_LED, 0, isHost ? 0 : 16, isHost ? 16 : 0);
}

} // namespace Game
