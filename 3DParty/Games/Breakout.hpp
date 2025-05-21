#pragma once

#include "IGame.hpp"
#include "struct.hpp"
#include "config.h"

namespace Game {

// ################################################################
// ####  BREAKOUT GAME CONSTANTS  #################################
// ################################################################

#define BREAKOUT_BALL_RADIUS 3
#define BREAKOUT_PADDLE_WIDTH 20
#define BREAKOUT_PADDLE_HEIGHT 5
#define BREAKOUT_PADDLE_Y (SCREEN_HEIGHT - 10)

#define BREAKOUT_PROGRESSION_SPEED 0.1
#define BREAKOUT_PROGRESSION_SPEED_MAX 2.0

#define BREAKOUT_TILE_WIDTH 17
#define BREAKOUT_TILE_HEIGHT 7
#define BREAKOUT_TILE_SPACING_X 4
#define BREAKOUT_TILE_SPACING_Y 4


// ################################################################
// ######  BREAKOUT CLASS DEFINITION  #############################
// ################################################################

class Breakout : public IGame {
public:
    Breakout(Adafruit_SSD1306 &display) : IGame(display) {}
    ~Breakout();
    void init() override;
    void step() override;

    class Tile {
        public:
        Tile(Adafruit_SSD1306 &display, Vector2i pos, Vector2i size) : _pos(pos), _size(size), _display(display) {}
        
        void setPos(Vector2i pos);
        void draw();
        Vector2i isHit(Vector2f ballPos);
        
        private:
        Vector2i _pos;
        Vector2i _size;
        Adafruit_SSD1306 &_display;
        
    };
private:

    void _update(unsigned long deltaTime) override;
    void _draw() override;
    Vector2f _ballPos = {64, SCREEN_HEIGHT - BREAKOUT_PADDLE_HEIGHT - BREAKOUT_BALL_RADIUS - 2};
    Vector2f _ballVel = {-0.0, -1.0};

    int _paddleWidth = BREAKOUT_PADDLE_WIDTH;
    float _paddlePosX = 0.0;
    float _gameSpeed = 1.0;
    Breakout::Tile **_tiles = nullptr;
    int _numTiles = 0;
};


// ################################################################
// ######  BREAKOUT CLASS IMPLEMENTATION  #########################
// ################################################################

Breakout::~Breakout() {
    for (int i = 0; i < _numTiles; i++) {
        delete _tiles[i];
    }
    delete[] _tiles;
}

void Breakout::init() {
    _isNetworkGame = false;
    _name = "Breakout";
    _lastUpdate = millis();

    _numTiles = 12;
    _tiles = new Tile*[_numTiles];
    for (int i = 0; i < _numTiles; i++) {
        int x = (i % 6) * (BREAKOUT_TILE_WIDTH + BREAKOUT_TILE_SPACING_X);
        int y = (i / 6) * (BREAKOUT_TILE_HEIGHT + BREAKOUT_TILE_SPACING_Y) + 10;
        _tiles[i] = new Tile(_display, {x, y}, {BREAKOUT_TILE_WIDTH, BREAKOUT_TILE_HEIGHT});
    }
}

void Breakout::step() {
    unsigned long currentMillis = millis();
    unsigned long deltaTime = currentMillis - _lastUpdate;
    if (deltaTime < UPDATE_INTERVAL) return;
    _lastUpdate = currentMillis;

    _retrieveInputs();
    _update(deltaTime);
    _draw();
}

void Breakout::_update(unsigned long deltaTime) {
    // Update ball position
    Vector2f velNormalized = _ballVel;
    float length = sqrt(velNormalized.x * velNormalized.x + velNormalized.y * velNormalized.y);
    if (length > 1.0) {
        velNormalized.x /= length;
        velNormalized.y /= length;
    }



    _ballPos.x += velNormalized.x * _gameSpeed;
    _ballPos.y += velNormalized.y * _gameSpeed;

    if (_ballPos.x < BREAKOUT_BALL_RADIUS + 1 || _ballPos.x > SCREEN_WIDTH - BREAKOUT_BALL_RADIUS - 1) {
        _ballVel.x = -_ballVel.x;
        _ballPos.x = constrain(_ballPos.x, 0, SCREEN_WIDTH - BREAKOUT_BALL_RADIUS + 1);
    }
    if (_ballPos.y < BREAKOUT_BALL_RADIUS + 1) {
        _ballVel.y = -_ballVel.y;
        _ballPos.y = constrain(_ballPos.y, 0, SCREEN_HEIGHT - BREAKOUT_BALL_RADIUS + 1);
    }

    // Check for collision with paddle
    if (_ballPos.y > SCREEN_HEIGHT - BREAKOUT_PADDLE_HEIGHT - BREAKOUT_BALL_RADIUS - 1 &&
        _ballPos.x > _paddlePosX && _ballPos.x < _paddlePosX + _paddleWidth) {
        // Bounce the ball off the paddle depending on the position
        float paddleCenter = _paddlePosX + _paddleWidth / 2.0;
        float ballOffset = (_ballPos.x - paddleCenter) / (_paddleWidth / 2.0);
        _ballVel.x = ballOffset * 2.0;
        _ballVel.y = -_ballVel.y;
        _ballVel.x = constrain(_ballVel.x, -1.0, 1.0);
        _ballVel.y = constrain(_ballVel.y, -1.0, 1.0);
        _ballPos.x = constrain(_ballPos.x, BREAKOUT_BALL_RADIUS + 1, SCREEN_WIDTH - BREAKOUT_BALL_RADIUS - 1);
        _ballPos.y = SCREEN_HEIGHT - BREAKOUT_PADDLE_HEIGHT - BREAKOUT_BALL_RADIUS - 1;

        // Increase game speed
        if (_gameSpeed < BREAKOUT_PROGRESSION_SPEED_MAX) _gameSpeed += BREAKOUT_PROGRESSION_SPEED;
    }


    if (_ballPos.y > SCREEN_HEIGHT) {
        Serial.println("Game Over");
        _display.clearDisplay();
        _display.setTextSize(2);
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(0, 0);
        _display.print("Game Over");
        _display.display();
        while (true)
        {
            delay(1000);
        }
        
    }

    _paddlePosX += _jx * 5;
    if (_paddlePosX < 1) _paddlePosX = 1;
    if (_paddlePosX > SCREEN_WIDTH - _paddleWidth - 1) _paddlePosX = SCREEN_WIDTH - _paddleWidth - 1;

    // Check for collision with tiles
    for (int i = 0; i < _numTiles; i++) {
        Vector2i hit = _tiles[i]->isHit(_ballPos);
        if (hit.x == 0 && hit.y == 0) continue;
        Serial.print("Tile ");
        Serial.print(i);
        Serial.print(" hit: ");
        Serial.print(hit.x);
        Serial.print(", ");
        Serial.println(hit.y);
        if (hit.x != 0 || hit.y != 0) {
            _ballVel.x *= hit.x;
            _ballVel.y *= hit.y;
            _tiles[i]->setPos({-100, -100}); // Move tile off screen
            break;
        }
    }
}

void Breakout::_draw() {
    _display.clearDisplay();

    // Draw ball
    _display.fillCircle(_ballPos.x, _ballPos.y, BREAKOUT_BALL_RADIUS, SSD1306_WHITE);

    // Draw paddle
    _display.fillRoundRect(_paddlePosX, SCREEN_HEIGHT - BREAKOUT_PADDLE_HEIGHT, _paddleWidth, 5, 2, SSD1306_WHITE);

    // Draw walls
    _display.drawLine(0, 0, 0, SCREEN_HEIGHT - 1, SSD1306_WHITE);
    _display.drawLine(SCREEN_WIDTH - 1, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, SSD1306_WHITE);
    _display.drawLine(0, 0, SCREEN_WIDTH - 1, 0, SSD1306_WHITE);

    for (int i = 0; i < _numTiles; i++) {
        _tiles[i]->draw();
    }

    _display.display();
}

void Breakout::Tile::draw() {
    _display.fillRoundRect(_pos.x, _pos.y, _size.x, _size.y, 2, SSD1306_WHITE);
}

void Breakout::Tile::setPos(Vector2i pos) {
    _pos = pos;
}

Vector2i Breakout::Tile::isHit(Vector2f ballPos) {
    // test each side of the tile, return the direction of the hit
    // 0, 0 = no hit
    // 1, 0 = hit left
    // 0, 1 = hit top
    // -1, 0 = hit right
    // 0, -1 = hit bottom

    if (ballPos.x + BREAKOUT_BALL_RADIUS > _pos.x && ballPos.x - BREAKOUT_BALL_RADIUS < _pos.x + _size.x &&
        ballPos.y + BREAKOUT_BALL_RADIUS > _pos.y && ballPos.y - BREAKOUT_BALL_RADIUS < _pos.y + _size.y) {
        // Ball is inside the tile
        if (ballPos.x < _pos.x) return {1, 0};
        if (ballPos.x > _pos.x + _size.x) return {-1, 0};
        if (ballPos.y < _pos.y) return {0, 1};
        if (ballPos.y > _pos.y + _size.y) return {0, -1};
    }

    // no hit
    return {0, 0};

}

} // namespace Game
