#pragma once

#include "IGame.hpp"
#include "struct.hpp"
#include "config.h"

namespace Game {

// ################################################################
// ####  MEMORY GAME CONSTANTS  ###################################
// ################################################################

#define MEMORY_TILE_DESIGN_0  {0b11111, 0b11111, 0b11111, 0b11111, 0b11111} // square filled
#define MEMORY_TILE_DESIGN_1  {0b01110, 0b10001, 0b10101, 0b10001, 0b01110} // circle empty
#define MEMORY_TILE_DESIGN_2  {0b00000, 0b01110, 0b01110, 0b01110, 0b00000} // small square
#define MEMORY_TILE_DESIGN_3  {0b11100, 0b11100, 0b11111, 0b00111, 0b00111} // 2 small squares
#define MEMORY_TILE_DESIGN_4  {0b00100, 0b01110, 0b01110, 0b00100, 0b00100} // tree ?
#define MEMORY_TILE_DESIGN_5  {0b01010, 0b01010, 0b01010, 0b01010, 0b01010} // 2 lines vertical
#define MEMORY_TILE_DESIGN_6  {0b00000, 0b11111, 0b00000, 0b11111, 0b00000} // 2 lines horizontal
#define MEMORY_TILE_DESIGN_7  {0b11011, 0b11011, 0b00000, 0b10001, 0b01110} // happy smiley
#define MEMORY_TILE_DESIGN_8  {0b11011, 0b11011, 0b00000, 0b01110, 0b10001} // sad smiley
#define MEMORY_TILE_DESIGN_9  {0b00100, 0b00110, 0b00100, 0b11111, 0b01110} // boat
#define MEMORY_TILE_DESIGN_10 {0b00100, 0b11111, 0b11111, 0b00100, 0b01110} // plane
#define MEMORY_TILE_DESIGN_11 {0b01010, 0b10101, 0b10001, 0b01010, 0b00100} // heart
#define MEMORY_TILE_DESIGN_12 {0b01110, 0b11111, 0b11011, 0b11111, 0b01110} // circle filled
#define MEMORY_TILE_DESIGN_13 {0b11111, 0b10001, 0b10001, 0b10001, 0b11111} // square empty
#define MEMORY_TILE_DESIGN_14 {0b00100, 0b01010, 0b10001, 0b01010, 0b00100} // diamond
#define MEMORY_TILE_DESIGN_15 {0b00100, 0b01110, 0b11111, 0b11111, 0b00100} // spique
#define MEMORY_TILE_DESIGN_16 {0b10101, 0b10101, 0b10101, 0b10101, 0b10101} // 3 lines vertical
#define MEMORY_TILE_DESIGN_17 {0b11111, 0b00000, 0b11111, 0b00000, 0b11111} // 3 lines horizontal
#define MEMORY_TILE_DESIGN_18 {0b10001, 0b01010, 0b00100, 0b01010, 0b10001} // diag cross
#define MEMORY_TILE_DESIGN_19 {0b00100, 0b00100, 0b11111, 0b00100, 0b00100} // straight cross
#define MEMORY_TILE_DESIGN_20 {0b00100, 0b01010, 0b10001, 0b10001, 0b11111} // house ?
#define MEMORY_TILE_DESIGN_21 {0b10101, 0b01110, 0b11111, 0b01110, 0b10101} // sun

static const uint8_t DESIGNS[22][5] = {
    MEMORY_TILE_DESIGN_0,
    MEMORY_TILE_DESIGN_1,
    MEMORY_TILE_DESIGN_2,
    MEMORY_TILE_DESIGN_3,
    MEMORY_TILE_DESIGN_4,
    MEMORY_TILE_DESIGN_5,
    MEMORY_TILE_DESIGN_6,
    MEMORY_TILE_DESIGN_7,
    MEMORY_TILE_DESIGN_8,
    MEMORY_TILE_DESIGN_9,
    MEMORY_TILE_DESIGN_10,
    MEMORY_TILE_DESIGN_11,
    MEMORY_TILE_DESIGN_12,
    MEMORY_TILE_DESIGN_13,
    MEMORY_TILE_DESIGN_14,
    MEMORY_TILE_DESIGN_15,
    MEMORY_TILE_DESIGN_16,
    MEMORY_TILE_DESIGN_17,
    MEMORY_TILE_DESIGN_18,
    MEMORY_TILE_DESIGN_19,
    MEMORY_TILE_DESIGN_20,
    MEMORY_TILE_DESIGN_21
};


#define MEMORY_TILE_NBR_DESIGNS 22


// possible tiles style
// Heart
// Circle
// Square
// Diamond
// Pique
// 3 traits H
// 3 traits V

// Triangle
// ################################################################
// ######  MEMORY CLASS DEFINITION  ###############################
// ################################################################

class Memory : public IGame {
public:
    Memory(Adafruit_SSD1306 &display) : IGame(display) {}
    void init() override;
    void step() override;

    class Card {
    public:
        Card(Adafruit_SSD1306 &display, Vector2i pos, int design = 0) : _display(display), _pos(pos), _design(design) {}
        
        void draw(bool isFlipped, bool selected);
        int getDesign() const { return _design; }
        void setFound(bool found) { _wasFound = found; }
        bool wasFound() const { return _wasFound; }
    private:
        Adafruit_SSD1306 &_display;
        Vector2i _pos;
        int _design;
        bool _isFlipped = false;
        bool _wasFound = false;
    };

private:

    void _update(unsigned long deltaTime) override;
    void _draw() override;

    int _currentChoice = 0.0;
    Vector2f _cursorPos = {0.0, 0.0};
    Card **_cards = nullptr;
    unsigned long _startTime = 0;

    int _selection1 = -1;
    int _selection2 = -1;
};


// ################################################################
// ######  MEMORY CLASS IMPLEMENTATION  ###########################
// ################################################################


void Memory::init() {
    _isNetworkGame = false;
    _name = "Memory";
    _NVSData.begin(_name.c_str(), false);
    _lastUpdate = millis();


    // delete previous cards if any
    if (_cards != nullptr) {
        for (int i = 0; i < MEMORY_TILE_NBR_DESIGNS * 2; i++) {
            delete _cards[i];
        }
        delete[] _cards;
    }

    // Initialize cards
    _cards = new Card*[MEMORY_TILE_NBR_DESIGNS * 2];

    // Create an array with two of each design
    int cardDesigns[MEMORY_TILE_NBR_DESIGNS * 2];
    for (int i = 0; i < MEMORY_TILE_NBR_DESIGNS; i++) {
        cardDesigns[i] = i;
        cardDesigns[i + MEMORY_TILE_NBR_DESIGNS] = i;
    }

    // Shuffle the designs using Fisher–Yates
    for (int i = MEMORY_TILE_NBR_DESIGNS * 2 - 1; i > 0; i--) {
        int j = randint(0, i);
        int temp = cardDesigns[i];
        cardDesigns[i] = cardDesigns[j];
        cardDesigns[j] = temp;
    }

    // Create Card objects with shuffled designs and grid positions
    for (int i = 0; i < MEMORY_TILE_NBR_DESIGNS * 2; i++) {
        _cards[i] = new Card(_display, {i % 11, i / 11}, cardDesigns[i]);
    }
}

void Memory::step() {
    if (_startTime == 0) {
        _startTime = millis();
    }
    unsigned long currentMillis = millis();
    unsigned long deltaTime = currentMillis - _lastUpdate;
    if (deltaTime < UPDATE_INTERVAL) return;
    _lastUpdate = currentMillis;

    _retrieveInputs();
    _update(deltaTime);
    _draw();
}

void Memory::_update(unsigned long deltaTime) {
    _cursorPos.x = constrain(_cursorPos.x + _jx * 0.5, 0, 10);
    _cursorPos.y = constrain(_cursorPos.y + _jy * 0.5, 0, 3);
    int newChoice = (int)_cursorPos.x + (int)_cursorPos.y * 11;
    if (newChoice != _currentChoice) tone(PIN_BUZZER, 250, 10);
    _currentChoice = newChoice;

    if (_buttonANew) {
        // If both selection are selected, reset the selection
        if (_selection1 != -1 && _selection2 != -1) {
            _selection1 = -1;
            _selection2 = -1;
        }

        // If the card is already found and we are waiting for a selection, do nothing
        if (_cards[_currentChoice]->wasFound() && (_selection1 == -1 || _selection2 == -1)) return;

        // If the cursor is on the same selection1 do nothing
        if (_selection1 != -1 && _selection1 == _currentChoice) return;

        // If selection 2 is -1, set it to the current choice
        if (_selection1 != -1 && _selection2 == -1) {
            _selection2 = _currentChoice;
        }
        
        // If both selection are -1, set selection 1 to the current choice
        if (_selection1 == -1 && _selection2 == -1) {
            _selection1 = _currentChoice;
        }
        
        // If both selection are selected and not the same, reset the selection
        if (_selection1 != -1 && _selection2 != -1) {
            if (_cards[_selection1]->getDesign() != _cards[_selection2]->getDesign()) {
                // If the two cards are not the same, reset the selection
                tone(PIN_BUZZER, 400, 50);
                delay(100);
                tone(PIN_BUZZER, 250, 50);
            } else {
                // If the two cards are the same, set them as found
                tone(PIN_BUZZER, 250, 50);
                delay(100);
                tone(PIN_BUZZER, 300, 50);
                delay(100);
                tone(PIN_BUZZER, 400, 50);
                _cards[_selection1]->setFound(true);
                _cards[_selection2]->setFound(true);
                _selection1 = -1;
                _selection2 = -1;
            }
        }

        // Check if win
        int tilesLeft = 0;
        for (int i = 0; i < MEMORY_TILE_NBR_DESIGNS * 2; i++) tilesLeft += !_cards[i]->wasFound();

        if (tilesLeft == 0) {
            _display.clearDisplay();
            _display.setTextSize(3);
            _display.setTextColor(SSD1306_WHITE);
            _display.setCursor(0, 0);
            _display.print("YOU WIN");
            float seconds = (millis() - _startTime) / 1000.0;
            _display.setTextSize(1);
            _display.setCursor(0, 30);
            _display.print("Time: ");
            _display.print(seconds);

            if (_NVSData.getFloat("bestTime", 0.0) == 0.0 || seconds < _NVSData.getFloat("bestTime", 0.0)) {
                _NVSData.putFloat("bestTime", seconds);
                _display.setCursor(0, 50);
                _display.print("New best time!");
            } else {
                _display.setCursor(0, 50);
                _display.print("Best time: ");
                _display.print(_NVSData.getFloat("bestTime", 0.0));
            }

            _display.display();
            _buttonANew = false;
            _buttonBNew = false;
            while (!_buttonANew && !_buttonBNew) {
                delay(100);
                _retrieveInputs();
            }
            if (_buttonANew) {
                _gameState = RESTART;
            } else {
                _gameState = QUIT;
            }
            return;
        }
    }
}

void Memory::_draw() {
    _display.clearDisplay();
    // Display the title
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);

    _display.setCursor(0, 0);
    _display.print("Memory Game");
    _display.setCursor(104, 0);
    if (_startTime != 0) {
        int seconds = (millis() - _startTime) / 1000;
        _display.print(seconds);
    }

    _display.drawLine(0, 13, SCREEN_WIDTH, 13, SSD1306_WHITE);

    // Draw cards
    for (int i = 0; i < MEMORY_TILE_NBR_DESIGNS * 2; i++) {
        _cards[i]->draw(!(i == _selection1 || i == _selection2), i == _currentChoice);
    }

    _display.display();
}

void Memory::Card::draw(bool isFlipped, bool selected) {
    // If found, hide it or show the pointer if selected
    if (_wasFound) {
        if (selected) {
            _display.drawPixel(_pos.x * 11 + 8, _pos.y * 11 + 24, SSD1306_WHITE);
        }
        return;
    }

    // Show back of card
    if (isFlipped) {
        if (selected) {
            _display.fillRect(_pos.x * 11 + 3, _pos.y * 11 + 19, 11, 11, SSD1306_WHITE);
            _display.drawRect(_pos.x * 11 + 5, _pos.y * 11 + 21, 7, 7, SSD1306_BLACK);
        } else {
            _display.fillRect(_pos.x * 11 + 3, _pos.y * 11 + 2 + 20, 9, 9, SSD1306_BLACK);
            _display.drawRect(_pos.x * 11 + 5, _pos.y * 11 + 21, 7, 7, SSD1306_WHITE);
        }
        return;
    }

    // Show front of card, inverted if needed
    if (selected) {
        uint8_t invertedDesign[5];
        for (int i = 0; i < 5; i++) {
            invertedDesign[i] = ~DESIGNS[_design][i];
        }
        _display.drawBitmap(_pos.x * 11 + 3, _pos.y * 11 + 2 + 20, invertedDesign, 8, 5, SSD1306_WHITE);
        _display.drawRect(_pos.x * 11 + 5, _pos.y * 11 + 21, 7, 7, SSD1306_WHITE);
        _display.drawRect(_pos.x * 11 + 4, _pos.y * 11 + 20, 9, 9, SSD1306_WHITE);
        _display.drawRect(_pos.x * 11 + 3, _pos.y * 11 + 19, 11, 11, SSD1306_WHITE);
    } else {
        _display.drawBitmap(_pos.x * 11 + 3, _pos.y * 11 + 2 + 20, DESIGNS[_design], 8, 5, SSD1306_WHITE);
        _display.drawRect(_pos.x * 11 + 4, _pos.y * 11 + 20, 9, 9, SSD1306_WHITE);
    }

}

} // namespace Game
