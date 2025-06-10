#pragma once

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <esp_wifi.h>

#include "esp_random.h"
#include "config.h"


#include "Games/GameTools.hpp"
#include "Games/IGame.hpp"
// #include "Games/ComponentTest.hpp"
// #include "Games/MultiplayerTest.hpp"
#include "Games/Breakout.hpp"
#include "Games/Memory.hpp"
#include "Games/Minesweeper.hpp"
#include "Games/Connect4.hpp"
#include "Games/Pong.hpp"
#include "Games/Shooter3D.hpp"

#include "Menu.hpp"

// ################################################################
// ######  CORE CONSTANTS DEFINITION  #############################
// ################################################################

// ################################################################
// ######  CORE CLASS DEFINITION  #################################
// ################################################################

class Core {
    public:
    Core();
    ~Core();

    bool init();
    void launchMenu();
    void run();

private:
    bool _isInitialized = false;
    Adafruit_SSD1306 *_display = nullptr;
    Game::IGame **_games = nullptr;
    Menu *_menu = nullptr;
    Game::IGame *_currentgame = nullptr;
    int _currentGameIndex = 0;
    int _numGames = 0;

    uint8_t _myMac[6] = {0, 0, 0, 0, 0, 0};
};


// ################################################################
// ######  CORE CLASS IMPLEMENTATION  #############################
// ################################################################

Core::Core() {
    _display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
}

Core::~Core() {
    if (_games) {
        for (int i = 0; i < _numGames; i++) delete _games[i];
        delete[] _games;
    }
    delete _menu;
    delete _display;
}

bool Core::init() {
    if (_isInitialized) return true;

    esp_wifi_get_mac(WIFI_IF_STA, _myMac);
    
    // #### Initialize display ####################################
    Wire.beginTransmission(SCREEN_ADDRESS);
    delay(50); // Wait for the display to be ready
    if (Wire.endTransmission() != 0) {
        Serial.println("Display not found at address " + String(SCREEN_ADDRESS, HEX));
        return false;
    }

    if (!_display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("Display initialization failed!");
        return false;
    }
    _display->setRotation(SCREEN_ROTATION);
    _display->clearDisplay();
    _display->display();

    // ######### Initialize Games #################################
    _numGames = 6;
    _games = new Game::IGame*[_numGames];
    if (!_games) {
        Serial.println("Failed to allocate games array!");
        return false;
    }
    
    _games[0] = new Game::Minesweeper(*_display);
    _games[1] = new Game::Memory(*_display);
    _games[2] = new Game::Breakout(*_display);
    _games[3] = new Game::Connect4(*_display);
    _games[4] = new Game::Pong(*_display);
    _games[5] = new Game::Shooter3D(*_display);

    
    for (int i = 0; i < _numGames; i++) {
        if (!_games[i]) {
            Serial.println("Failed to create game " + String(i));
            return false;
        }
    }
        
    for (int i = 0; i < _numGames; i++) _games[i]->init();

    if (!Game::areGamesNamesUnique(_games, _numGames)) {
        Serial.println("Error: Game names are not unique!");
        return false;
    }


    // ####### Create Menu ########################################
    _menu = new Menu(*_display, _games, _numGames);
    if (!_menu) {
        Serial.println("Failed to create menu!");
        return false;
    }
    
    _isInitialized = true;
    return true;
}

void Core::launchMenu() {
    _currentGameIndex = _menu->run();
    _currentgame = _games[_currentGameIndex];
}

void Core::run() {
    Networking::GameProtocol &protocol = Networking::GameProtocol::get();
    protocol.reset();

    while (true)
    {
        if (_currentgame->isNetworkGame()) {
            if (_currentgame->connectionLobby(_currentGameIndex) == false) {
                launchMenu();
                continue;
            }
        }

        if (_currentgame->run() == Game::QUIT) {
            launchMenu();
        }
        _currentgame->init();
    }
}
