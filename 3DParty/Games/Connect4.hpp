#pragma once

#include "IGame.hpp"
#include "struct.hpp"
#include "config.h"

namespace Game {

// ################################################################
// ####  CONNECT4 GAME STRUCTURES  ################################
// ################################################################

typedef struct Connect4NetworkData {
    String type;
    uint8_t data[200];
} Connect4NetworkData;

// ################################################################
// ####  CONNECT4 GAME CONSTANTS  #################################
// ################################################################

#define CONNECT4_NBR_CELLS_X 7
#define CONNECT4_NBR_CELLS_Y 6
#define CONNECT4_NBR_CELLS (CONNECT4_NBR_CELLS_X * CONNECT4_NBR_CELLS_Y)

#define CONNECT4_CELL_SIZE 10

// ################################################################
// ######  CONNECT4 CLASS DEFINITION  #############################
// ################################################################

class Connect4 : public IGame {
public:
    Connect4(Adafruit_SSD1306 &display) : IGame(display) {}
    void init() override;
    void step() override;
private:
    void _firstStep() override;
    void _update(unsigned long deltaTime) override;
    void _draw() override;
    bool _playToken(int column, bool isMyToken);

    bool _isWinCondition(Vector2i winLine[2]);
    bool _isBoardFull();

    unsigned long _startTime = 0;
    float _currentChoice = CONNECT4_NBR_CELLS_X / 2.0;
    int _board[CONNECT4_NBR_CELLS_Y][CONNECT4_NBR_CELLS_X];

    bool _isMyTurn = true;
    bool _isStarting = false;
    Vector2i _lastTokenPos = {-1, -1};
};


// ################################################################
// ######  CONNECT4 CLASS IMPLEMENTATION  #########################
// ################################################################

void Connect4::init() {
    _isNetworkGame = true;
    _name = "Connect4";
    _lastUpdate = millis();

    for (int y = 0; y < CONNECT4_NBR_CELLS_Y; ++y) {
        for (int x = 0; x < CONNECT4_NBR_CELLS_X; ++x) {
            _board[y][x] = 0;
        }
    }
}

void Connect4::step() {
    Networking::GameProtocol &protocol = Networking::GameProtocol::get();
    if (_startTime == 0) {
        _startTime = millis();
        _firstStep();
    }
    if (_isNetworkGame) protocol.loop();
    unsigned long currentMillis = millis();
    unsigned long deltaTime = currentMillis - _lastUpdate;
    if (deltaTime < UPDATE_INTERVAL) return;
    _lastUpdate = currentMillis;

    _retrieveInputs();
    _update(deltaTime);
    _draw();
}

void Connect4::_firstStep() {
    rgbLedWrite(PIN_RGB_LED, 0, 0, 0);
    Networking::GameProtocol &protocol = Networking::GameProtocol::get();
    Networking::GameProtocol::DataPacket pkt = {};
    if (_isHost) {
        _isMyTurn = randint(0, 1) == 0;
        Connect4NetworkData data;
        data.type = "SET_TURN";
        data.data[0] = _isMyTurn ? 1 : 0;
        protocol.sendData((uint8_t*)&data, sizeof(data), true);
        Serial.printf("Host: isMyTurn: %d\n", _isMyTurn);
        _isStarting = true;
    }
}

void Connect4::_update(unsigned long deltaTime) {
    Networking::GameProtocol &protocol = Networking::GameProtocol::get();
    float newChoice = _currentChoice + _jx * 0.5;
    if (round(newChoice) != round(_currentChoice)) {
        tone(PIN_BUZZER, 250, 10);
    }
    _currentChoice = constrain(newChoice, 0, CONNECT4_NBR_CELLS_X - 1);

    Vector2i winLine[2];
    if (_isWinCondition(winLine)) {
        _display.fillRect(0, 20, 60, 44, SSD1306_BLACK);

        _display.setTextSize(1);
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(0, 20);
        if (!_isMyTurn) {
            _display.print("You win!");
            tone(PIN_BUZZER, 1000, 500);
        } else {
            _display.print("You lose!");
            tone(PIN_BUZZER, 500, 500);
        }
        _display.display();
        while (true) {
            protocol.loop();
            delay(100);
        }
    } else if (_isBoardFull()) {
        Serial.println("Board is full, it's a draw!");
        _display.fillRect(0, 20, 60, 44, SSD1306_BLACK);
        _display.setTextSize(2);
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(0, 20);
        _display.print("It's a draw!");
        tone(PIN_BUZZER, 500, 500);

        _display.display();
        while (true) {
            protocol.loop();
            delay(100);
        }
    }

    if (_buttonANew && _isMyTurn && _isStarting) {
        Serial.printf("Button A pressed, current choice: %d\n", (int)round(_currentChoice));
        int column = round(_currentChoice);
        if (!_playToken(column, _isHost)) return;

        Connect4NetworkData data;
        data.type = "PLAY_TOKEN";
        memcpy(data.data, &column, sizeof(column));
        
        protocol.sendData((uint8_t*)&data, sizeof(data), true);
        _isMyTurn = false;
        Serial.printf("Played column: %d\n", column);
    }

    while (protocol.hasPacket()) {
        Networking::GameProtocol::DataPacket pkt;
        protocol.getPacket(pkt);
        Connect4NetworkData *data = (Connect4NetworkData*)pkt.data;
        Serial.printf("Received packet: %s, len: %d\n", pkt.data, pkt.len);
        if (strncmp(data->type.c_str(), "SET_TURN", pkt.len) == 0) {
            _isMyTurn = data->data[0] == 0;
            Serial.printf("Received SET_TURN: isMyTurn: %d\n", _isMyTurn);
            _isStarting = true;
            continue;
        }
        if (strncmp(data->type.c_str(), "PLAY_TOKEN", pkt.len) == 0) {
            int column = 0;
            memcpy(&column, data->data, sizeof(column));
            Serial.printf("Received PLAY_TOKEN: column: %d\n", column);
            if (_playToken(column, !_isHost)) {
                _isMyTurn = true;
            } else {
                Serial.printf("Failed to play token in column: %d\n", column);
            }
            continue;
        }
    }
}

void Connect4::_draw() {
    _display.clearDisplay();

    // Draw the checkboard
    for (int x = 0; x < CONNECT4_NBR_CELLS_X; ++x) {
        for (int y = 0; y < CONNECT4_NBR_CELLS_Y; ++y) {
            int cellX = x * (CONNECT4_CELL_SIZE - 1);
            int cellY = y * (CONNECT4_CELL_SIZE - 1);

            _display.drawRoundRect(cellX + SCREEN_WIDTH / 2 - 2, cellY + 8, CONNECT4_CELL_SIZE, CONNECT4_CELL_SIZE, 2, SSD1306_WHITE);

            if (_board[y][x] == 1) {
                _display.fillRoundRect(cellX + SCREEN_WIDTH / 2, cellY + 10, CONNECT4_CELL_SIZE - 4, CONNECT4_CELL_SIZE - 4, 3, SSD1306_WHITE);
            } else if (_board[y][x] == 2) {
                _display.drawRoundRect(cellX + SCREEN_WIDTH / 2, cellY + 10, CONNECT4_CELL_SIZE - 4, CONNECT4_CELL_SIZE - 4, 3, SSD1306_WHITE);
            }

            if ((_lastTokenPos.x == x && _lastTokenPos.y == y) && millis() % 1000 < 500) {
                 _display.fillRect(cellX + SCREEN_WIDTH / 2 + 2, cellY + 12, 2, 2, _board[y][x] == 1 ? SSD1306_BLACK : SSD1306_WHITE);
            }
        }
    }

    // Draw the cursor
    _display.fillRoundRect(round(_currentChoice) * (CONNECT4_CELL_SIZE - 1) + SCREEN_WIDTH / 2 - 1, 2, 8, 4, 3, SSD1306_WHITE);

    // Draw the current turn
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);
    _display.setCursor(5, 0);
    _display.print("Connect 4");
    _display.drawLine(8, 12, 48, 12, SSD1306_WHITE);

    _display.setCursor(0, 20);
    _display.print("Color: ");
    if (_isHost) {
        _display.fillCircle(40, 24, 3, SSD1306_WHITE);
    } else {
        _display.drawCircle(40, 24, 3, SSD1306_WHITE);
    }

    _display.setCursor(0, 35);
    _display.print(_isMyTurn ? "Turn: You" : "Turn: Peer");
    _display.display();
}

bool Connect4::_playToken(int column, bool isMyToken) {
    if (column < 0 || column >= CONNECT4_NBR_CELLS_X) {
        Serial.printf("Invalid column: %d\n", column);
        return false;
    }

    for (int y = CONNECT4_NBR_CELLS_Y - 1; y >= 0; --y) {
        if (_board[y][column] == 0) {
            _board[y][column] = isMyToken ? 1 : 2;
            _lastTokenPos = {column, y};
            tone(PIN_BUZZER, 300, 50);
            return true;
        }
    }

    tone(PIN_BUZZER, 150, 100);
    Serial.printf("Column %d is full\n", column);
    return false;
}

bool Connect4::_isWinCondition(Vector2i winLine[2]) {
    for (int y = 0; y < CONNECT4_NBR_CELLS_Y; ++y) {
        for (int x = 0; x < CONNECT4_NBR_CELLS_X; ++x) {
            if (_board[y][x] == 0) continue;

            // Horizontal check
            if (x <= CONNECT4_NBR_CELLS_X - 4 &&
                _board[y][x] == _board[y][x + 1] &&
                _board[y][x] == _board[y][x + 2] &&
                _board[y][x] == _board[y][x + 3]) {
                winLine[0] = {x, y};
                winLine[1] = {x + 3, y};
                return true;
            }

            // Vertical check
            if (y <= CONNECT4_NBR_CELLS_Y - 4 &&
                _board[y][x] == _board[y + 1][x] &&
                _board[y][x] == _board[y + 2][x] &&
                _board[y][x] == _board[y + 3][x]) {
                winLine[0] = {x, y};
                winLine[1] = {x, y + 3};
                return true;
            }

            // Diagonal check (top-left to bottom-right)
            if (y <= CONNECT4_NBR_CELLS_Y - 4 && x <= CONNECT4_NBR_CELLS_X - 4 &&
                _board[y][x] == _board[y + 1][x + 1] &&
                _board[y][x] == _board[y + 2][x + 2] &&
                _board[y][x] == _board[y + 3][x + 3]) {
                winLine[0] = {x, y};
                winLine[1] = {x + 3, y + 3};
                return true;
            }

            // Diagonal check (bottom-left to top-right)
            if (y >= 3 && x <= CONNECT4_NBR_CELLS_X - 4 &&
                _board[y][x] == _board[y - 1][x + 1] &&
                _board[y][x] == _board[y - 2][x + 2] &&
                _board[y][x] == _board[y - 3][x + 3]) {
                winLine[0] = {x, y};
                winLine[1] = {x + 3, y - 3};
                return true;
            }
        }
    }
    return false;
}


bool Connect4::_isBoardFull() {
    for (int x = 0; x < CONNECT4_NBR_CELLS_X; ++x) {
        if (_board[0][x] == 0) return false;
    }
    return true;
}

} // namespace Game
