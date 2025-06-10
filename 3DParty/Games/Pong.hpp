#pragma once

#include "IGame.hpp"
#include "struct.hpp"
#include "config.h"

namespace Game {

// ################################################################
// ####  PONG GAME STRUCTURES  ####################################
// ################################################################

typedef struct PongNetworkData {
    char type[16];
    uint8_t data[200];
} PongNetworkData;

typedef struct PongGameState {
    float paddleY;
    Vector2f ballPos;
    int scorePlayer;
    int scoreOpponent;
} PongGameState;

// ################################################################
// ####  PONG GAME CONSTANTS  #####################################
// ################################################################

#define PONG_PADDLE_WIDTH 4
#define PONG_PADDLE_HEIGHT 16
#define PONG_PADDLE_SPEED 2

#define PONG_BALL_SIZE 2
#define PONG_BALL_INITIAL_SPEED 1.0
#define PONG_BALL_MAX_SPEED 3.0
#define PONG_BALL_SPEED_INCREMENT 0.05

#define PONG_TIME_BETWEEN_ROUNDS 2000

// ################################################################
// ######  PONG CLASS DEFINITION  #################################
// ################################################################

class Pong : public IGame {
public:
    Pong(Adafruit_SSD1306 &display) : IGame(display) {}
    void init() override;
    void step() override;
private:
    void _firstStep() override;
    void _update(unsigned long deltaTime) override;
    void _draw() override;

    bool _paddleCollision();

    float _paddleYplayer = SCREEN_HEIGHT / 2.0 - PONG_PADDLE_HEIGHT / 2.0;
    float _paddleYopponent = SCREEN_HEIGHT / 2.0 - PONG_PADDLE_HEIGHT / 2.0;

    Vector2f _ballPos = {SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0};
    Vector2f _ballDir = {1.0, 1.0};
    float _ballSpeedX = PONG_BALL_INITIAL_SPEED;

    int _scorePlayer = 0;
    int _scoreOpponent = 0;

    int _timeSinceLastScore = 0;

    unsigned long _startTime = 0;
};


// ################################################################
// ######  PONG CLASS IMPLEMENTATION  #########################
// ################################################################

void Pong::init() {
    _isNetworkGame = true;
    _name = "Pong";
    _lastUpdate = millis();
}

void Pong::step() {
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

void Pong::_firstStep() {
    rgbLedWrite(PIN_RGB_LED, 0, 0, 0);
    Networking::GameProtocol &protocol = Networking::GameProtocol::get();
    Networking::GameProtocol::DataPacket pkt = {};
    if (_isHost) {
        _ballDir.x = randint(0, 1) ? 1.0 : -1.0;
        _ballDir.y = randint(0, 1) ? 1.0 : -1.0;
        _scorePlayer = 1;
    }
}

void Pong::_update(unsigned long deltaTime) {
    Networking::GameProtocol &protocol = Networking::GameProtocol::get();

    _paddleYplayer += _jy * PONG_PADDLE_SPEED;
    _paddleYplayer = constrain(_paddleYplayer, 0, SCREEN_HEIGHT - PONG_PADDLE_HEIGHT);

    protocol.loop();

    if (_isHost && _timeSinceLastScore >= 0) {
        _ballPos.x += _ballDir.x * _ballSpeedX;
        _ballPos.y += _ballDir.y * _ballSpeedX;
    
        if (_ballPos.y <= PONG_BALL_SIZE || _ballPos.y >= SCREEN_HEIGHT - PONG_BALL_SIZE) {
            _ballDir.y = -_ballDir.y;
        }

        if (_ballPos.x <= PONG_BALL_SIZE) {
            // Ball hit left wall, opponent scores
            _scoreOpponent++;
            _timeSinceLastScore = PONG_TIME_BETWEEN_ROUNDS;
            _ballPos = {SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0};
            _ballDir.x = 1.0; // Reset ball direction to right
            _ballDir.y = randint(0, 1) ? 1.0 : -1.0; // Randomize vertical direction
            _ballSpeedX = PONG_BALL_INITIAL_SPEED; // Reset ball speed
        } else if (_ballPos.x >= SCREEN_WIDTH - PONG_BALL_SIZE) {
            // Ball hit right wall, player scores
            _scorePlayer++;
            _timeSinceLastScore = 0;
            _ballPos = {SCREEN_WIDTH / 2.0, SCREEN_HEIGHT / 2.0};
            _ballDir.x = -1.0; // Reset ball direction to left
            _ballDir.y = randint(0, 1) ? 1.0 : -1.0; // Randomize vertical direction
            _ballSpeedX = PONG_BALL_INITIAL_SPEED; // Reset ball speed
        }

        _paddleCollision();
    }

    if (_timeSinceLastScore > 0) {
        _timeSinceLastScore -= deltaTime;
        if (_timeSinceLastScore < 0) _timeSinceLastScore = 0;
    }

    // Send combined game state data (both paddle and ball if host)
    PongGameState gameState;
    gameState.paddleY = _paddleYplayer;
    gameState.ballPos = _ballPos;
    gameState.scorePlayer = _scorePlayer;
    gameState.scoreOpponent = _scoreOpponent;
    
    PongNetworkData data;
    strcpy(data.type, "GAMESTATE");
    memcpy(data.data, &gameState, sizeof(gameState));
    protocol.sendData((uint8_t*)&data, sizeof(data), false);

    while (protocol.hasPacket()) {
        Networking::GameProtocol::DataPacket pkt;
        protocol.getPacket(pkt);
        PongNetworkData *data = (PongNetworkData*)pkt.data;

        if (strncmp(data->type, "GAMESTATE", 9) == 0) {
            PongGameState gameState;
            memcpy(&gameState, data->data, sizeof(gameState));
            
            _paddleYopponent = gameState.paddleY;
            
            if (!_isHost) {
                _ballPos.y = gameState.ballPos.y;
                _ballPos.x = SCREEN_WIDTH - gameState.ballPos.x;

                _scorePlayer = gameState.scoreOpponent;
                _scoreOpponent = gameState.scorePlayer;
            }
            continue;
        }
    }
}

void Pong::_draw() {
    _display.clearDisplay();

    // draw paddles
    _display.fillRoundRect(0, _paddleYplayer, PONG_PADDLE_WIDTH, PONG_PADDLE_HEIGHT, 2, SSD1306_WHITE);
    _display.fillRoundRect(SCREEN_WIDTH - PONG_PADDLE_WIDTH, _paddleYopponent, PONG_PADDLE_WIDTH, PONG_PADDLE_HEIGHT, 2, SSD1306_WHITE);

    // draw middle dashed line
    for (int y = 0; y < SCREEN_HEIGHT; y += 4) {
        _display.drawLine(SCREEN_WIDTH / 2, y, SCREEN_WIDTH / 2, y + 2, SSD1306_WHITE);
    }

    // draw scores
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);
    _display.setCursor(SCREEN_WIDTH / 2 - 13, 0);
    _display.print(_scorePlayer);

    _display.setCursor(SCREEN_WIDTH / 2 + 3, 0);
    _display.print(_scoreOpponent);

    // draw ball
    _display.fillCircle(_ballPos.x, _ballPos.y, PONG_BALL_SIZE, SSD1306_WHITE);

    _display.display();
}

bool Pong::_paddleCollision() {
    // Check collision with player paddle
    if (_ballPos.x <= PONG_PADDLE_WIDTH + PONG_BALL_SIZE && 
        _ballPos.y >= _paddleYplayer && 
        _ballPos.y <= _paddleYplayer + PONG_PADDLE_HEIGHT) {
        _ballDir.x = 1.0; // Bounce to the right
        _ballSpeedX = min(_ballSpeedX + PONG_BALL_SPEED_INCREMENT, PONG_BALL_MAX_SPEED);
        return true;
    }

    // Check collision with opponent paddle
    if (_ballPos.x >= SCREEN_WIDTH - PONG_PADDLE_WIDTH - PONG_BALL_SIZE && 
        _ballPos.y >= _paddleYopponent && 
        _ballPos.y <= _paddleYopponent + PONG_PADDLE_HEIGHT) {
        _ballDir.x = -1.0; // Bounce to the left
        _ballSpeedX = min(_ballSpeedX + PONG_BALL_SPEED_INCREMENT, PONG_BALL_MAX_SPEED);
        return true;
    }

    return false;
}

} // namespace Game
