#pragma once

#include "IGame.hpp"
#include "struct.hpp"
#include "config.h"

namespace Game {

// ################################################################
// ####  MINESWEEPER GAME CONSTANTS  ##############################
// ################################################################

#define MINESWEEPER_DESIGN_HIDDEN {0b111111, 0b111111, 0b111111, 0b111111, 0b111111}
#define MINESWEEPER_DESIGN_EMPTY {0b00000, 0b00000, 0b00000, 0b00000, 0b00000}
#define MINESWEEPER_DESIGN_BOMB {0b10101, 0b01110, 0b11111, 0b01110, 0b10101}
#define MINESWEEPER_DESIGN_FLAG {0b00110, 0b00111, 0b00100, 0b00100, 0b01110}
#define MINESWEEPER_DESIGN_1 {0b00010, 0b00010, 0b00010, 0b00010, 0b00010}
#define MINESWEEPER_DESIGN_2 {0b01110, 0b00010, 0b01110, 0b01000, 0b01110}
#define MINESWEEPER_DESIGN_3 {0b01110, 0b00010, 0b00110, 0b00010, 0b01110}
#define MINESWEEPER_DESIGN_4 {0b01010, 0b01010, 0b01110, 0b00010, 0b00010}
#define MINESWEEPER_DESIGN_5 {0b01110, 0b01000, 0b01110, 0b00010, 0b01110}
#define MINESWEEPER_DESIGN_6 {0b01110, 0b01000, 0b01110, 0b01010, 0b01110}
#define MINESWEEPER_DESIGN_7 {0b01110, 0b00010, 0b00100, 0b00100, 0b00100}
#define MINESWEEPER_DESIGN_8 {0b01110, 0b01010, 0b01110, 0b01010, 0b01110}

#define MINESWEEPER_NBR_CELLS_X 12
#define MINESWEEPER_NBR_CELLS_Y 5
#define MINESWEEPER_NBR_CELLS (MINESWEEPER_NBR_CELLS_X * MINESWEEPER_NBR_CELLS_Y)

static const uint8_t MINESWEEPER_DESIGNS[12][5] = {
    MINESWEEPER_DESIGN_HIDDEN,
    MINESWEEPER_DESIGN_EMPTY,
    MINESWEEPER_DESIGN_BOMB,
    MINESWEEPER_DESIGN_FLAG,
    MINESWEEPER_DESIGN_1,
    MINESWEEPER_DESIGN_2,
    MINESWEEPER_DESIGN_3,
    MINESWEEPER_DESIGN_4,
    MINESWEEPER_DESIGN_5,
    MINESWEEPER_DESIGN_6,
    MINESWEEPER_DESIGN_7,
    MINESWEEPER_DESIGN_8
};

enum MinesweeperCellType {
    MINESWEEPER_HIDDEN,
    MINESWEEPER_EMPTY,
    MINESWEEPER_BOMB,
    MINESWEEPER_FLAG,
    MINESWEEPER_1,
    MINESWEEPER_2,
    MINESWEEPER_3,
    MINESWEEPER_4,
    MINESWEEPER_5,
    MINESWEEPER_6,
    MINESWEEPER_7,
    MINESWEEPER_8
};

// ################################################################
// ######  MINESWEEPER CLASS DEFINITION  ##########################
// ################################################################

class Minesweeper : public IGame {
public:
    Minesweeper(Adafruit_SSD1306 &display) : IGame(display) {}
    void init() override;
    void step() override;

    class Cell {
    public:
        Cell(Adafruit_SSD1306 &display, Vector2i pos, MinesweeperCellType cellType) : _display(display), _pos(pos), _cellType(cellType) {}

        void draw(bool selected);
        MinesweeperCellType getType() const { return _cellType; }
        void reveal() { _revealed = true; }
        bool isRevealed() const { return _revealed; }
        bool isFlagged() const { return _flagged; }
        void setFlagged(bool flagged) { _flagged = flagged; }
        void setPos(Vector2i pos) { _pos = pos; }
        Vector2i getPos() const { return _pos; }

    private:
        Adafruit_SSD1306 &_display;
        Vector2i _pos;
        MinesweeperCellType _cellType;
        bool _revealed = false;
        bool _flagged = false;
    };
private:

    void _update(unsigned long deltaTime) override;
    void _draw() override;
    void _generateBombMap();
    void _floodReveal(Vector2i pos);

    int _currentChoice = 0;
    Vector2f _cursorPos = {0.0, 0.0};
    unsigned long _startTime = 0;
    Cell **_cells = nullptr;
    uint _nbrMines = 0;
    bool _mapGenerated = false;


};


// ################################################################
// ######  MINESWEEPER CLASS IMPLEMENTATION  ######################
// ################################################################

void Minesweeper::init() {
    _isNetworkGame = false;
    _name = "Minesweeper";
    _NVSData.begin(_name.c_str(), false);
    _lastUpdate = millis();
    _gameState = CONTINUE;

    _nbrMines = randint(12, MINESWEEPER_NBR_CELLS / 4);
    _mapGenerated = false;

    _cells = new Cell*[MINESWEEPER_NBR_CELLS];
    for (int i = 0; i < MINESWEEPER_NBR_CELLS; i++) {
        _cells[i] = new Cell(_display, {i % MINESWEEPER_NBR_CELLS_X, i / MINESWEEPER_NBR_CELLS_X}, MINESWEEPER_HIDDEN);
    }
    _currentChoice = 0;
    _cursorPos = {0.0, 0.0};
    _startTime = 0;
}

void Minesweeper::step() {
    if (_startTime == 0) {
        _startTime = millis();
        delay(250);
    }
    unsigned long currentMillis = millis();
    unsigned long deltaTime = currentMillis - _lastUpdate;
    if (deltaTime < UPDATE_INTERVAL) return;
    _lastUpdate = currentMillis;

    _retrieveInputs();
    _update(deltaTime);
    _draw();
}

void Minesweeper::_update(unsigned long deltaTime) {
    _cursorPos.x = constrain(_cursorPos.x + _jx * 0.5, 0, MINESWEEPER_NBR_CELLS_X - 1);
    _cursorPos.y = constrain(_cursorPos.y + _jy * 0.5, 0, MINESWEEPER_NBR_CELLS_Y - 1);
    int newChoice = (int)_cursorPos.x + (int)_cursorPos.y * MINESWEEPER_NBR_CELLS_X;
    if (newChoice != _currentChoice) tone(PIN_BUZZER, 250, 10);
    _currentChoice = newChoice;

    if (_buttonBNew) {
        if (_cells == nullptr || _cells[_currentChoice]->isRevealed()) return;
        _cells[_currentChoice]->setFlagged(!_cells[_currentChoice]->isFlagged());
    }

    if (_buttonANew) {
        if (_mapGenerated == false) {
            _generateBombMap();
            while (_cells[_currentChoice]->getType() != MINESWEEPER_EMPTY) {
                _generateBombMap();
            }
            _mapGenerated = true;
        }

        if (_cells[_currentChoice]->isFlagged()) return;

        if (_cells[_currentChoice]->getType() == MINESWEEPER_BOMB) {
            for (int i = 0; i < MINESWEEPER_NBR_CELLS; i++) {
                _cells[i]->reveal();
                if (_cells[i]->getType() != MINESWEEPER_BOMB) {
                    _cells[i]->setFlagged(false);
                }
            }
            _currentChoice = -1;
            _draw();
            _display.setTextSize(1);
            _display.setTextColor(SSD1306_WHITE);
            _display.setCursor(35, 0);
            _display.print("GAME OVER");
            _display.display();
            _buttonANew = false;
            _buttonBNew = false;
            tone(PIN_BUZZER, 100, 50);
            delay(100);
            tone(PIN_BUZZER, 250, 50);
            delay(100);
            tone(PIN_BUZZER, 100, 50);
            delay(100);
            tone(PIN_BUZZER, 250, 50);
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
        } else {
            _cells[_currentChoice]->reveal();
            if (_cells[_currentChoice]->getType() == MINESWEEPER_EMPTY) {
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        _floodReveal(_cells[_currentChoice]->getPos() + Vector2i{dx, dy});
                    }
                }
            }
        }
    }
    
    // Check if all cells are revealed
    int revealedCells = 0;
    for (int i = 0; i < MINESWEEPER_NBR_CELLS; i++) {
        if (_cells[i]->isRevealed()) revealedCells++;
    }
    if (revealedCells == MINESWEEPER_NBR_CELLS - _nbrMines) {
        _currentChoice = -1;
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

void Minesweeper::_draw() {
    _display.clearDisplay();

    // Draw cells
    for (int i = 0; i < MINESWEEPER_NBR_CELLS; i++) {
        _cells[i]->draw((i == _currentChoice));
    }

    // Draw nbr of bombs
    int bombsLeft = 0;
    if (_mapGenerated == false) {
        bombsLeft = _nbrMines;
    } else {
        bombsLeft = _nbrMines;
        for (int i = 0; i < MINESWEEPER_NBR_CELLS; i++) {
            if (_cells[i]->isFlagged()) {
                bombsLeft--;
            }
        }
    }


    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);
    _display.drawBitmap(0, 1, MINESWEEPER_DESIGNS[MINESWEEPER_BOMB], 8, 5, SSD1306_WHITE);
    _display.setCursor(10, 0);
    _display.print(bombsLeft);
    _display.setTextSize(1);
    _display.setCursor(104, 0);
    int seconds = (millis() - _startTime) / 1000;
    _display.print(seconds);

    _display.display();
}

void Minesweeper::_floodReveal(Vector2i pos) {
    if (pos.x < 0 || pos.x >= MINESWEEPER_NBR_CELLS_X ||
        pos.y < 0 || pos.y >= MINESWEEPER_NBR_CELLS_Y) return;

    int idx = pos.y * MINESWEEPER_NBR_CELLS_X + pos.x;
    if (_cells[idx]->isRevealed()) return;

    _cells[idx]->reveal();
    if (_cells[idx]->getType() == MINESWEEPER_EMPTY) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                _floodReveal(Vector2i{pos.x + dx, pos.y + dy});
            }
        }
    }
}

void Minesweeper::_generateBombMap() {
    // Create a simple bomb map
    bool bombMap[MINESWEEPER_NBR_CELLS] = { false };
    for (int i = 0; i < _nbrMines; ++i) {
        int pos;
        do {
            pos = random(0, MINESWEEPER_NBR_CELLS);
        } while (bombMap[pos]);
        bombMap[pos] = true;
    }

    // Compute adjacent‐bomb counts
    uint8_t counts[MINESWEEPER_NBR_CELLS] = { 0 };
    for (int idx = 0; idx < MINESWEEPER_NBR_CELLS; ++idx) {
        if (bombMap[idx]) continue;

        int x = idx % MINESWEEPER_NBR_CELLS_X;
        int y = idx / MINESWEEPER_NBR_CELLS_X;
        uint8_t count = 0;

        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx;
                int ny = y + dy;
                if (nx < 0 || nx >= MINESWEEPER_NBR_CELLS_X ||
                    ny < 0 || ny >= MINESWEEPER_NBR_CELLS_Y) continue;
                int nidx = ny * MINESWEEPER_NBR_CELLS_X + nx;
                if (bombMap[nidx]) ++count;
            }
        }
        counts[idx] = count;
    }

    // Delete the previous cells if they exist
    if (_cells != nullptr) {
        for (int idx = 0; idx < MINESWEEPER_NBR_CELLS; ++idx) {
            delete _cells[idx];
        }
        delete[] _cells;
    }

    // Allocate and initialize cells with the proper type
    _cells = new Cell*[MINESWEEPER_NBR_CELLS];
    for (int idx = 0; idx < MINESWEEPER_NBR_CELLS; ++idx) {
        int x = idx % MINESWEEPER_NBR_CELLS_X;
        int y = idx / MINESWEEPER_NBR_CELLS_X;

        MinesweeperCellType type;
        if (bombMap[idx]) {
            type = MINESWEEPER_BOMB;
        } else if (counts[idx] == 0) {
            type = MINESWEEPER_EMPTY;
        } else {
            type = static_cast<MinesweeperCellType>(MINESWEEPER_1 + counts[idx] - 1);
        }

        _cells[idx] = new Cell(_display, Vector2i{x, y}, type);
    }
}

void Minesweeper::Cell::draw(bool selected) {
    if (selected) {
        if (_flagged) {
            uint8_t invertedDesign[5];
            for (int i = 0; i < 5; i++) invertedDesign[i] = ~MINESWEEPER_DESIGNS[MINESWEEPER_FLAG][i];
            _display.drawBitmap(_pos.x * 10 + 3, _pos.y * 10 + 13, invertedDesign, 8, 5, SSD1306_WHITE);
            _display.drawRoundRect(_pos.x * 10 + 3, _pos.y * 10 + 10, 11, 11, 2, SSD1306_WHITE);
            _display.drawRect(_pos.x * 10 + 4, _pos.y * 10 + 11, 9, 9, SSD1306_WHITE);
            _display.drawRect(_pos.x * 10 + 5, _pos.y * 10 + 12, 7, 7, SSD1306_WHITE);
        } else {
            if (_revealed) {
                uint8_t invertedDesign[5];
                for (int i = 0; i < 5; i++) invertedDesign[i] = ~MINESWEEPER_DESIGNS[_cellType][i];
                _display.drawBitmap(_pos.x * 10 + 3, _pos.y * 10 + 13, invertedDesign, 8, 5, SSD1306_WHITE);
                _display.drawRoundRect(_pos.x * 10 + 3, _pos.y * 10 + 10, 11, 11, 2, SSD1306_WHITE);
                _display.drawRect(_pos.x * 10 + 4, _pos.y * 10 + 11, 9, 9, SSD1306_WHITE);
                _display.drawRect(_pos.x * 10 + 5, _pos.y * 10 + 12, 7, 7, SSD1306_WHITE);
            } else {
                _display.drawRoundRect(_pos.x * 10 + 3, _pos.y * 10 + 10, 11, 11, 2, SSD1306_WHITE);
                _display.fillRect(_pos.x * 10 + 5, _pos.y * 10 + 12, 9, 9, SSD1306_BLACK);
                _display.drawRect(_pos.x * 10 + 4, _pos.y * 10 + 11, 9, 9, SSD1306_WHITE);
                _display.drawRect(_pos.x * 10 + 5, _pos.y * 10 + 12, 7, 7, SSD1306_WHITE);
            }
        }
    } else {
        if (_flagged) {
            _display.drawBitmap(_pos.x * 10 + 3, _pos.y * 10 + 13, MINESWEEPER_DESIGNS[MINESWEEPER_FLAG], 8, 5, SSD1306_WHITE);
            _display.drawRoundRect(_pos.x * 10 + 3, _pos.y * 10 + 10, 11, 11, 2, SSD1306_WHITE);
            _display.drawRect(_pos.x * 10 + 4, _pos.y * 10 + 11, 9, 9, SSD1306_BLACK);
            _display.drawRect(_pos.x * 10 + 5, _pos.y * 10 + 12, 7, 7, SSD1306_BLACK);
        } else {
            if (_revealed) {
                _display.drawBitmap(_pos.x * 10 + 3, _pos.y * 10 + 13, MINESWEEPER_DESIGNS[_cellType], 8, 5, SSD1306_WHITE);
                _display.drawRoundRect(_pos.x * 10 + 3, _pos.y * 10 + 10, 11, 11, 2, SSD1306_WHITE);
                _display.drawRect(_pos.x * 10 + 4, _pos.y * 10 + 11, 9, 9, SSD1306_BLACK);
                _display.drawRect(_pos.x * 10 + 5, _pos.y * 10 + 12, 7, 7, SSD1306_BLACK);
            } else {
                _display.drawRoundRect(_pos.x * 10 + 3, _pos.y * 10 + 10, 11, 11, 2, SSD1306_WHITE);
                _display.fillRect(_pos.x * 10 + 5, _pos.y * 10 + 12, 9, 9, SSD1306_WHITE);
                _display.drawRect(_pos.x * 10 + 4, _pos.y * 10 + 11, 9, 9, SSD1306_BLACK);
                _display.drawRect(_pos.x * 10 + 5, _pos.y * 10 + 12, 7, 7, SSD1306_BLACK);
            }
        }
    }
}

} // namespace Game
