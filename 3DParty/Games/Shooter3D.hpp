#pragma once

#include "IGame.hpp"
#include "struct.hpp"
#include "config.h"
#include <math.h>

namespace Game {

// ################################################################
// ####  SHOOTER3D NETWORK STRUCTURES  ###########################
// ################################################################

typedef struct Shooter3DNetworkData {
    char type[16];
    uint8_t data[200];
} Shooter3DNetworkData;

typedef struct Shooter3DPlayerState {
    Vector2f playerPos;
    Vector2f playerDir;
    bool isShielded;
    bool isShooting;
    int score;
    bool isDead;
} Shooter3DPlayerState;

// ################################################################
// #####  SHOOTER3D GAME CONSTANTS  ##############################
// ################################################################

static constexpr int SHIELD_DURATION = 1000;
static constexpr int SHIELD_COOLDOWN = 3000;
static constexpr int SHOOT_COOLDOWN = 2000;
static constexpr int SPAWN_PROTECTION_DURATION = 2000;
static constexpr int SHOOTER3D_SPAWN_COUNT = 6;

static constexpr int SPRITE_WIDTH = 16;
static constexpr int SPRITE_HEIGHT = 32;
static constexpr int SPRITE_DIRECTIONS = 8;

static constexpr int SHOOTER3D_WALLS_COUNT = 8;
static const Vector2i SHOOTER3D_WALLS_POS[SHOOTER3D_WALLS_COUNT][2] = {
    {{10, 10}, {59, 20}},
    {{69, 20}, {118, 10}},
    {{118, 10}, {118, 54}},
    {{118, 54}, {10, 54}},
    {{10, 54}, {10, 10}},
    {{30, 30}, {40, 40}},
    {{40, 40}, {20, 40}},
    {{20, 40}, {30, 30}}
};

static const Vector2f SHOOTER3D_SPAWN_POSITIONS[SHOOTER3D_SPAWN_COUNT] = {
    {20.0f, 20.0f},
    {108.0f, 20.0f},
    {20.0f, 44.0f},
    {108.0f, 44.0f},
    {64.0f, 15.0f},
    {64.0f, 49.0f}
};

static const Vector2f SHOOTER3D_SPAWN_DIRECTIONS[SHOOTER3D_SPAWN_COUNT] = {
    {1.0f, 0.0f},
    {-1.0f, 0.0f},
    {1.0f, 0.0f},
    {-1.0f, 0.0f},
    {0.0f, 1.0f},
    {0.0f, -1.0f}
};

static const unsigned char player_sprites[SPRITE_DIRECTIONS][SPRITE_HEIGHT][4] = { // check SpriteHexGenerator.py
    { // sprite_1
        {0x00, 0x00, 0x00, 0x00},     {0x00, 0x00, 0x00, 0x00},     {0x00, 0x15, 0x54, 0x00},     {0x00, 0x6A, 0xA9, 0x00},
        {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},
        {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0x96, 0x96, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},
        {0x00, 0x6A, 0xA9, 0x00},     {0x01, 0x9A, 0xA6, 0x40},     {0x06, 0xA5, 0x5A, 0x90},     {0x06, 0xAA, 0xAA, 0x90},
        {0x1A, 0xAA, 0xAA, 0xA4},     {0x1A, 0x9A, 0xA6, 0xA4},     {0x1A, 0x9A, 0xA6, 0xA4},     {0x1A, 0x9A, 0xA6, 0xA4},
        {0x1A, 0xAA, 0xAA, 0xA4},     {0x1A, 0xAA, 0xAA, 0xA4},     {0x1A, 0x6A, 0xA9, 0xA4},     {0x05, 0x69, 0xA9, 0x50},
        {0x00, 0x69, 0xA9, 0x00},     {0x00, 0x69, 0xA9, 0x00},     {0x01, 0xA9, 0xA9, 0x00},     {0x01, 0xA9, 0xAA, 0x40},
        {0x06, 0xA9, 0xAA, 0x90},     {0x1A, 0xA5, 0x55, 0x50},     {0x15, 0x54, 0x00, 0x00},     {0x00, 0x00, 0x00, 0x00}
    },
    { // sprite_2
        {0x00, 0x00, 0x00, 0x00},     {0x00, 0x00, 0x00, 0x00},     {0x00, 0x05, 0x54, 0x00},     {0x00, 0x5A, 0xA9, 0x00},
        {0x01, 0x6A, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},
        {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xA9, 0x69, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x00, 0x6A, 0xAA, 0x40},
        {0x00, 0x6A, 0xA9, 0x00},     {0x01, 0x9A, 0xA6, 0x40},     {0x06, 0xA5, 0x5A, 0x40},     {0x06, 0xAA, 0xAA, 0x90},
        {0x1A, 0xAA, 0xAA, 0x90},     {0x1A, 0xA6, 0xA6, 0x90},     {0x1A, 0xA6, 0xA6, 0x90},     {0x1A, 0xA6, 0xA6, 0x90},
        {0x1A, 0xAA, 0xAA, 0x90},     {0x1A, 0xAA, 0xAA, 0x90},     {0x1A, 0x9A, 0xA9, 0x90},     {0x05, 0x5A, 0x69, 0x40},
        {0x00, 0x1A, 0x69, 0x00},     {0x00, 0x1A, 0x69, 0x00},     {0x00, 0x5A, 0x6A, 0x40},     {0x01, 0xAA, 0x6A, 0x50},
        {0x06, 0xAA, 0x6A, 0x90},     {0x06, 0xA5, 0x55, 0x50},     {0x05, 0x54, 0x00, 0x00},     {0x00, 0x00, 0x00, 0x00}
    },
    { // sprite_3
        {0x00, 0x00, 0x00, 0x00},     {0x00, 0x00, 0x00, 0x00},     {0x00, 0x15, 0x54, 0x00},     {0x01, 0x6A, 0xA9, 0x40},
        {0x01, 0xAA, 0xAA, 0x90},     {0x06, 0xAA, 0xAA, 0x90},     {0x06, 0xAA, 0xAA, 0x90},     {0x06, 0xAA, 0xAA, 0x90},
        {0x06, 0xAA, 0xAA, 0x90},     {0x06, 0xAA, 0x96, 0x90},     {0x06, 0x6A, 0xAA, 0x90},     {0x01, 0x6A, 0xAA, 0x90},
        {0x00, 0x5A, 0xAA, 0x90},     {0x00, 0x16, 0xAA, 0x90},     {0x00, 0x1A, 0x59, 0x40},     {0x00, 0x1A, 0xA9, 0x00},
        {0x00, 0x6A, 0xAA, 0x40},     {0x00, 0x6A, 0x6A, 0x40},     {0x00, 0x6A, 0x6A, 0x40},     {0x00, 0x6A, 0x6A, 0x40},
        {0x00, 0x6A, 0x6A, 0x40},     {0x00, 0x69, 0xAA, 0x40},     {0x00, 0x69, 0xA9, 0x00},     {0x00, 0x16, 0x99, 0x00},
        {0x00, 0x1A, 0x99, 0x00},     {0x00, 0x1A, 0x99, 0x00},     {0x00, 0x6A, 0x9A, 0x40},     {0x00, 0x6A, 0x9A, 0x90},
        {0x00, 0x6A, 0x95, 0x50},     {0x00, 0x6A, 0xA9, 0x00},     {0x00, 0x55, 0x55, 0x00},     {0x00, 0x00, 0x00, 0x00}
    },
    { // sprite_4
        {0x00, 0x00, 0x00, 0x00},     {0x00, 0x00, 0x00, 0x00},     {0x00, 0x05, 0x54, 0x00},     {0x00, 0x5A, 0xA9, 0x00},
        {0x01, 0x6A, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},
        {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xA9, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x00, 0x6A, 0xAA, 0x40},
        {0x00, 0x66, 0xA9, 0x00},     {0x01, 0xA9, 0xAA, 0x40},     {0x06, 0xAA, 0xAA, 0x40},     {0x06, 0xAA, 0xAA, 0x90},
        {0x1A, 0xAA, 0xAA, 0x90},     {0x1A, 0xA6, 0xA6, 0x90},     {0x1A, 0xA6, 0xA6, 0x90},     {0x1A, 0xA6, 0xA6, 0x90},
        {0x1A, 0xAA, 0xAA, 0x90},     {0x1A, 0xAA, 0xAA, 0x90},     {0x1A, 0x9A, 0xA9, 0x90},     {0x05, 0x5A, 0x69, 0x40},
        {0x00, 0x1A, 0x69, 0x00},     {0x00, 0x1A, 0x69, 0x00},     {0x00, 0x5A, 0x6A, 0x40},     {0x01, 0xAA, 0x6A, 0x50},
        {0x06, 0xAA, 0x6A, 0x90},     {0x06, 0xA5, 0x55, 0x50},     {0x05, 0x54, 0x00, 0x00},     {0x00, 0x00, 0x00, 0x00}
    },
    { // sprite_5
        {0x00, 0x00, 0x00, 0x00},     {0x00, 0x00, 0x00, 0x00},     {0x00, 0x15, 0x54, 0x00},     {0x00, 0x6A, 0xA9, 0x00},
        {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},
        {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},
        {0x00, 0x6A, 0xA9, 0x00},     {0x01, 0xAA, 0xAA, 0x40},     {0x06, 0xAA, 0xAA, 0x90},     {0x06, 0xAA, 0xAA, 0x90},
        {0x1A, 0xAA, 0xAA, 0xA4},     {0x1A, 0x9A, 0x86, 0xA4},     {0x1A, 0x9A, 0x86, 0xA4},     {0x1A, 0x9A, 0x86, 0xA4},
        {0x1A, 0xAA, 0xAA, 0xA4},     {0x1A, 0xAA, 0xAA, 0xA4},     {0x1A, 0x6A, 0xA9, 0xA4},     {0x05, 0x6A, 0x69, 0x50},
        {0x00, 0x6A, 0x69, 0x00},     {0x00, 0x6A, 0x69, 0x00},     {0x00, 0x6A, 0x6A, 0x40},     {0x01, 0xAA, 0x6A, 0x40},
        {0x06, 0xAA, 0x6A, 0x90},     {0x05, 0x55, 0x5A, 0xA4},     {0x00, 0x00, 0x15, 0x54},     {0x00, 0x00, 0x00, 0x00}
    },
    { // sprite_6
        {0x00, 0x00, 0x00, 0x00},     {0x00, 0x00, 0x00, 0x00},     {0x00, 0x15, 0x50, 0x00},     {0x00, 0x6A, 0xA5, 0x00},
        {0x01, 0xAA, 0xA9, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},
        {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0x6A, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xA9, 0x00},
        {0x00, 0x6A, 0x99, 0x00},     {0x01, 0xAA, 0x6A, 0x40},     {0x01, 0xAA, 0xAA, 0x90},     {0x06, 0xAA, 0xAA, 0x90},
        {0x06, 0xAA, 0xAA, 0xA4},     {0x06, 0x9A, 0x9A, 0xA4},     {0x06, 0x9A, 0x9A, 0xA4},     {0x06, 0x9A, 0x9A, 0xA4},
        {0x06, 0xAA, 0xAA, 0xA4},     {0x06, 0xAA, 0xAA, 0xA4},     {0x06, 0x6A, 0xA6, 0xA4},     {0x01, 0x69, 0xA5, 0x50},
        {0x00, 0x69, 0xA4, 0x00},     {0x00, 0x69, 0xA4, 0x00},     {0x01, 0xA9, 0xA5, 0x00},     {0x05, 0xA9, 0xAA, 0x40},
        {0x06, 0xA9, 0xAA, 0x90},     {0x05, 0x55, 0x5A, 0x90},     {0x00, 0x00, 0x15, 0x50},     {0x00, 0x00, 0x00, 0x00}
    },
    { // sprite_7
        {0x00, 0x00, 0x00, 0x00},     {0x00, 0x00, 0x00, 0x00},     {0x00, 0x15, 0x54, 0x00},     {0x01, 0x6A, 0xA9, 0x40},
        {0x06, 0xAA, 0xAA, 0x40},     {0x06, 0xAA, 0xAA, 0x90},     {0x06, 0xAA, 0xAA, 0x90},     {0x06, 0xAA, 0xAA, 0x90},
        {0x06, 0xAA, 0xAA, 0x90},     {0x06, 0x96, 0xAA, 0x90},     {0x06, 0xAA, 0xA9, 0x90},     {0x06, 0xAA, 0xA9, 0x40},
        {0x06, 0xAA, 0xA5, 0x00},     {0x06, 0xAA, 0x94, 0x00},     {0x01, 0x65, 0xA4, 0x00},     {0x00, 0x6A, 0xA4, 0x00},
        {0x01, 0xAA, 0xA9, 0x00},     {0x01, 0xA9, 0xA9, 0x00},     {0x01, 0xA9, 0xA9, 0x00},     {0x01, 0xA9, 0xA9, 0x00},
        {0x01, 0xA9, 0xA9, 0x00},     {0x01, 0xAA, 0x69, 0x00},     {0x00, 0x6A, 0x69, 0x00},     {0x00, 0x66, 0x94, 0x00},
        {0x00, 0x66, 0xA4, 0x00},     {0x00, 0x66, 0xA4, 0x00},     {0x01, 0xA6, 0xA9, 0x00},     {0x06, 0xA6, 0xA9, 0x00},
        {0x05, 0x56, 0xA9, 0x00},     {0x00, 0x6A, 0xA9, 0x00},     {0x00, 0x55, 0x55, 0x00},     {0x00, 0x00, 0x00, 0x00}
    },
    { // sprite_8
        {0x00, 0x00, 0x00, 0x00},     {0x00, 0x00, 0x00, 0x00},     {0x00, 0x15, 0x50, 0x00},     {0x00, 0x6A, 0xA5, 0x00},
        {0x01, 0xAA, 0xA9, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xAA, 0x40},
        {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0x69, 0x6A, 0x40},     {0x01, 0xAA, 0xAA, 0x40},     {0x01, 0xAA, 0xA9, 0x00},
        {0x00, 0x6A, 0xA9, 0x00},     {0x01, 0x9A, 0xA6, 0x40},     {0x01, 0xA5, 0x5A, 0x90},     {0x06, 0xAA, 0xAA, 0x90},
        {0x06, 0xAA, 0xAA, 0xA4},     {0x06, 0x9A, 0x9A, 0xA4},     {0x06, 0x9A, 0x9A, 0xA4},     {0x06, 0x9A, 0x9A, 0xA4},
        {0x06, 0xAA, 0xAA, 0xA4},     {0x06, 0xAA, 0xAA, 0xA4},     {0x06, 0x6A, 0xA6, 0xA4},     {0x01, 0x69, 0xA5, 0x50},
        {0x00, 0x69, 0xA4, 0x00},     {0x00, 0x69, 0xA4, 0x00},     {0x01, 0xA9, 0xA5, 0x00},     {0x06, 0xA9, 0xAA, 0x40},
        {0x06, 0xA9, 0xAA, 0x90},     {0x05, 0x55, 0x5A, 0x90},     {0x00, 0x00, 0x15, 0x50},     {0x00, 0x00, 0x00, 0x00}
    },
};

// ################################################################
// ######  SHOOTER3D CLASS DEFINITION  ##########################
// ################################################################

class Shooter3D : public IGame {
public:
    Shooter3D(Adafruit_SSD1306 &display) : IGame(display) {}
    void init() override;
    void step() override;
private:
    void _update(unsigned long deltaTime) override;
    void _draw() override;
    void _drawMinimap();
    void _drawOtherPlayer(float* wallDistances = nullptr, int numRays = 0, float fov = 0.0f);
    int _getSpriteDirectionIndex(const Vector2f& playerPos, const Vector2f& spritePos, float spriteRotation);
    bool _checkWallCollision(const Vector2f& pos);
    float _distancePointToLineSegment(const Vector2f& point, const Vector2i& lineStart, const Vector2i& lineEnd);
    void _raycastMultiple(const Vector2f& origin, const Vector2f& direction, float fov, int numRays, float* distances, int* wallIds = nullptr);
    float _raycastSingle(const Vector2f& origin, const Vector2f& direction, int* hitWallId = nullptr);
    bool _checkPlayerHit(const Vector2f& shooterPos, const Vector2f& shooterDir, const Vector2f& targetPos);
    bool _isProtected();
    void _spawnPlayer();
    
    // Sound effect functions TODO
    // void _playShootSound();
    // void _playHitSound();
    // void _playDeathSound();
    // void _playShieldSound();
    // void _playSpawnSound();
    // void _playUISound();

    unsigned long _startTime = 0;

    // Player state
    Vector2f _playerPos = {64.0, 32.0};
    Vector2f _playerDir = {1.0, 0.0};
    unsigned long _lastShoot = 0;
    unsigned long _lastShield = 0;

    // Networking: Other player's state
    Vector2f _otherPlayerPos = {96.0, 32.0};
    Vector2f _otherPlayerDir = {-1.0, 0.0};
    bool _otherPlayerVisible = false;
    unsigned long _lastOtherPlayerUpdate = 0;
    bool _isOtherPlayerShielded = false;
    bool _isOtherPlayerShooting = false;
    int _otherPlayerScore = 0;
    bool _isOtherPlayerDead = false;
    int _playerScore = 0;
    bool _isPlayerDead = false;
    unsigned long _deathTime = 0;
    unsigned long _spawnTime = 1;

    // Minimap settings
    bool _showMinimap = false;
    float _minimapRadius = 25.0f;
    float _minimapScale = 1.0f;
};


// ################################################################
// ######  SHOOTER3D CLASS IMPLEMENTATION  #######################
// ################################################################

void Shooter3D::init() {
    _isNetworkGame = true;
    _name = "Shooter3D";
    _NVSData.begin(_name.c_str(), false);
    _lastUpdate = millis();
    _spawnPlayer();
}

void Shooter3D::step() {
    Networking::GameProtocol &protocol = Networking::GameProtocol::get();
    if (_startTime == 0) {
        _startTime = millis();
        _spawnTime = millis();
        rgbLedWrite(PIN_RGB_LED, 0, 0, 0);
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

void Shooter3D::_update(unsigned long deltaTime) {
    if (_jPressedNew) {
        _showMinimap = !_showMinimap;
    }
    const float rotationSpeed = 0.05f;
    const float moveSpeed = 1.0f;

    // Rotate direction with _jx
    if (_jx != 0) {
        float currentAngle = atan2(_playerDir.y, _playerDir.x);
        currentAngle += _jx * rotationSpeed;
        
        _playerDir.x = cos(currentAngle);
        _playerDir.y = sin(currentAngle);
    }
      // Move forward/backward with _jy
    Vector2f newPos = _playerPos;
    if (_jy != 0 && !_isPlayerDead) {
        newPos.x -= _playerDir.x * _jy * moveSpeed;
        newPos.y -= _playerDir.y * _jy * moveSpeed;
    }
    if (_buttonBNew && !_isProtected()) {
        if (millis() - _lastShield > SHIELD_COOLDOWN) {
            _lastShield = millis();
        } else {
            newPos = _playerPos;
        }
    }
    
    // Check if player is shooting (only if alive)
    if (_buttonANew && !_isPlayerDead && !_showMinimap) {
        if (millis() - _lastShield > SHIELD_DURATION) {
            if (millis() - _lastShoot > SHOOT_COOLDOWN) {
                _lastShoot = millis();
                if (_otherPlayerVisible && !_isOtherPlayerDead && !_isOtherPlayerShielded) {
                    if (_checkPlayerHit(_playerPos, _playerDir, _otherPlayerPos)) {
                        _playerScore++;
                        _isOtherPlayerDead = true;
                    }
                }
            }
        }
    }

    
    // Check collision and handle wall sliding
    if (_checkWallCollision(newPos)) {
        Vector2f horizontalPos = _playerPos;
        horizontalPos.x -= _playerDir.x * _jy * moveSpeed;
        
        if (!_checkWallCollision(horizontalPos)) {
            _playerPos = horizontalPos;
        } else {
            Vector2f verticalPos = _playerPos;
            verticalPos.y -= _playerDir.y * _jy * moveSpeed;
            
            if (!_checkWallCollision(verticalPos)) {
                _playerPos = verticalPos;
            }
        }
    } else {
        _playerPos = newPos;
    }

    // Handle player death and respawning
    if (_isPlayerDead) {
        if (_deathTime == 0) {
            _deathTime = millis();
        }
        
        if (millis() - _deathTime > 3000) {
            _isPlayerDead = false;
            _deathTime = 0;
            _spawnPlayer();
        }
    }
    if (_isOtherPlayerShooting && !_isPlayerDead) {
        if (!_isProtected() && _checkPlayerHit(_otherPlayerPos, _otherPlayerDir, _playerPos)) {
            _isPlayerDead = true;
            _deathTime = millis();
            _otherPlayerScore++;
        }
    }

    // Networking: Send and receive player state data
    Networking::GameProtocol &protocol = Networking::GameProtocol::get();
    Shooter3DPlayerState playerState;    playerState.playerPos = _playerPos;
    playerState.playerDir = _playerDir;
    playerState.isShielded = _isProtected();
    playerState.isShooting = millis() - _lastShoot < 100;
    playerState.score = _playerScore;
    playerState.isDead = _isPlayerDead;
    
    Shooter3DNetworkData data;
    strcpy(data.type, "PLAYERSTATE");
    memcpy(data.data, &playerState, sizeof(playerState));
    protocol.sendData((uint8_t*)&data, sizeof(data), false);
    
    // Receive other player's state
    while (protocol.hasPacket()) {
        Networking::GameProtocol::DataPacket pkt;
        protocol.getPacket(pkt);
        Shooter3DNetworkData *networkData = (Shooter3DNetworkData*)pkt.data;
          if (strncmp(networkData->type, "PLAYERSTATE", 11) == 0) {
            Shooter3DPlayerState otherPlayerState;
            memcpy(&otherPlayerState, networkData->data, sizeof(otherPlayerState));
              _otherPlayerPos = otherPlayerState.playerPos;
            _otherPlayerDir = otherPlayerState.playerDir;
            _isOtherPlayerShielded = otherPlayerState.isShielded;
            _isOtherPlayerShooting = otherPlayerState.isShooting;
            _otherPlayerScore = otherPlayerState.score;
            _isOtherPlayerDead = otherPlayerState.isDead;

            if (!_otherPlayerVisible) rgbLedWrite(PIN_RGB_LED, 0, 0, 0);
            _otherPlayerVisible = true;
            _lastOtherPlayerUpdate = millis();
            continue;
        }
    }
    
    if (_otherPlayerVisible && (millis() - _lastOtherPlayerUpdate > 3000)) {
        _otherPlayerVisible = false;
        rgbLedWrite(PIN_RGB_LED, 32, 0, 0);
    }

}

void Shooter3D::_draw() {
    _display.clearDisplay();
    
    if (_showMinimap) {
        _drawMinimap();
    } else {
        const int numRays = 64;
        float distances[numRays];
        int wallIds[numRays];
        float fov = PI / 2.0f;
        
        _raycastMultiple(_playerPos, _playerDir, fov, numRays, distances, wallIds);
        for (int i = 0; i < numRays; i++) {
            float distance = distances[i];
            int wallId = wallIds[i];
            
            if (distance >= 1000.0f) continue;      

            float baseAngle = atan2(_playerDir.y, _playerDir.x);
            float angleStep = fov / (numRays - 1);
            float startAngle = baseAngle - fov / 2.0f;
            float rayAngle = startAngle + i * angleStep;
            float angleFromCenter = rayAngle - baseAngle;
            
            float correctedDistance = distance * cos(angleFromCenter);        
            int wallHeight = (int)(1000.0f / correctedDistance);
            int screenX = (i * SCREEN_WIDTH) / numRays;
            int wallTop = (SCREEN_HEIGHT / 2) - (wallHeight / 2);
            int wallBottom = (SCREEN_HEIGHT / 2) + (wallHeight / 2);
            bool isEdge = false;
            int edgeWallHeight = wallHeight;
            
            if (i > 0) {
                int prevWallId = wallIds[i-1];
                float prevDistance = distances[i-1];
                
                if (prevDistance >= 1000.0f && distance < 1000.0f) {
                    isEdge = true;
                }
                else if (wallId != prevWallId && prevDistance < 1000.0f && distance < 1000.0f) {
                    float prevAngleFromCenter = (startAngle + (i-1) * angleStep) - baseAngle;
                    float prevCorrectedDistance = prevDistance * cos(prevAngleFromCenter);
                    int prevWallHeight = (int)(1000.0f / prevCorrectedDistance);
                    
                    if (wallHeight >= prevWallHeight) {
                        isEdge = true;
                        edgeWallHeight = wallHeight;
                    } else {
                        isEdge = true;
                        edgeWallHeight = prevWallHeight;
                    }
                }
                else if (wallId == prevWallId && prevDistance < 1000.0f && distance < 1000.0f) {
                    float distanceDiff = abs(prevDistance - distance);
                    if (distanceDiff > 15.0f) { 
                        isEdge = true;
                    }
                }
            }
            
            if (!isEdge && i < numRays - 1) {
                float nextDistance = distances[i+1];
                if (distance < 1000.0f && nextDistance >= 1000.0f) {
                    isEdge = true;
                }
            }
            if (isEdge) {
                int edgeWallTop = (SCREEN_HEIGHT / 2) - (edgeWallHeight / 2);
                int edgeWallBottom = (SCREEN_HEIGHT / 2) + (edgeWallHeight / 2);
                int clampedTop = edgeWallTop;
                int clampedBottom = edgeWallBottom;
                
                if (clampedTop < 0) clampedTop = 0;
                if (clampedTop >= SCREEN_HEIGHT) clampedTop = SCREEN_HEIGHT - 1;
                if (clampedBottom < 0) clampedBottom = 0;
                if (clampedBottom >= SCREEN_HEIGHT) clampedBottom = SCREEN_HEIGHT - 1;
                
                if (clampedTop != clampedBottom) {
                    _display.drawLine(screenX, clampedTop, screenX, clampedBottom, 1);
                } else {
                    _display.drawPixel(screenX, clampedTop, 1);
                }        } else {
                if (wallTop >= 0 && wallTop < SCREEN_HEIGHT) {
                    _display.drawPixel(screenX, wallTop, 1);
                }
                if (wallBottom >= 0 && wallBottom < SCREEN_HEIGHT && wallBottom != wallTop) {
                    _display.drawPixel(screenX, wallBottom, 1);
                }

                // wall patterns
                if (wallHeight > 15) {
                    int gridSpacing = (int)(correctedDistance * 0.3f);
                    if (gridSpacing < 3) gridSpacing = 3;
                    if (gridSpacing > 20) gridSpacing = 20;
                    
                    for (int y = wallTop + 1; y < wallBottom - 1; y++) {
                        if (y >= 0 && y < SCREEN_HEIGHT) {
                            if ((screenX + y) % gridSpacing == 0) {
                                _display.drawPixel(screenX, y, 1);
                            }
                            if ((screenX - y) % gridSpacing == 0) {
                                _display.drawPixel(screenX, y, 1);
                            }
                        }
                    }
                }
            }
        }

        if (_otherPlayerVisible) {
            _drawOtherPlayer(distances, numRays, fov);
        }
        // draw shield effect
        for (int i = 0; i < 4; i++) {
            int shift = i * 4;
            if (millis() - _lastShield < SHIELD_DURATION - SHIELD_DURATION / 4 * i) {
                _display.drawRoundRect(shift, shift, SCREEN_WIDTH - shift * 2, SCREEN_HEIGHT - shift * 2, 15 - i * 2, 1);
            }
        }

        // draw spawn protection effect
        unsigned long currentTime = millis();
        bool hasSpawnProtection = (_spawnTime > 0) && (currentTime >= _spawnTime) && (currentTime - _spawnTime < SPAWN_PROTECTION_DURATION);
        if (hasSpawnProtection) {
            int pulsePhase = (millis() / 200) % 4;
            for (int i = 0; i < 2; i++) {
                int shift = i * 8 + pulsePhase * 2;
                if (shift < SCREEN_WIDTH / 2) {
                    for (int x = shift; x < SCREEN_WIDTH - shift; x += 4) {
                        _display.drawPixel(x, shift, 1);
                        _display.drawPixel(x, SCREEN_HEIGHT - shift - 1, 1);
                    }
                    for (int y = shift; y < SCREEN_HEIGHT - shift; y += 4) {
                        _display.drawPixel(shift, y, 1);
                        _display.drawPixel(SCREEN_WIDTH - shift - 1, y, 1);
                    }
                }
            }
            unsigned long timeLeft = SPAWN_PROTECTION_DURATION - (currentTime - _spawnTime);
            int spawnProtectionLeft = (timeLeft / 1000) + 1;
            _display.setTextSize(1);
            _display.setTextColor(1);
            _display.setCursor(SCREEN_WIDTH - 20, 12);
            _display.print("SP:");
            _display.print(spawnProtectionLeft);
        }

        // draw cursor in middle of screen, indicating if ready to shoot
        if (millis() - _lastShoot < SHOOT_COOLDOWN) {
            _display.fillCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 3, 0);
            _display.drawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 3, 1);
            _display.drawLine(SCREEN_WIDTH / 2 - 2, SCREEN_HEIGHT / 2 - 2, SCREEN_WIDTH / 2 + 2, SCREEN_HEIGHT / 2 + 2, 1);
            _display.drawLine(SCREEN_WIDTH / 2 + 2, SCREEN_HEIGHT / 2 - 2, SCREEN_WIDTH / 2 - 2, SCREEN_HEIGHT / 2 + 2, 1);
        } else {
            _display.fillCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 3, 0);
            _display.drawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 3, 1);
            _display.drawPixel(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 1);
        }

        // draw muzzle flash effect
        if (millis() - _lastShoot < 150) {
            for (int i = 0; i < 8; i++) {
                float angle = (i * PI) / 4.0f;
                int flashLength = 8 + (rand() % 4);
                int endX = SCREEN_WIDTH / 2 + cos(angle) * flashLength;
                int endY = SCREEN_HEIGHT / 2 + sin(angle) * flashLength;
                _display.drawLine(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, endX, endY, 1);
            }
        }

        // draw death screen
        if (_isPlayerDead) {
            if ((millis() / 200) % 2 == 0) { // blinking effect
                for (int y = 0; y < SCREEN_HEIGHT; y += 4) {
                    for (int x = 0; x < SCREEN_WIDTH; x += 4) {
                        if ((x + y) % 8 == 0) {
                            _display.drawPixel(x, y, 1);
                        }
                    }
                }
            }
            
            _display.setTextSize(2);
            _display.setTextColor(1);
            _display.setCursor(SCREEN_WIDTH / 2 - 24, SCREEN_HEIGHT / 2 - 8);
            _display.print("DEAD");
            int respawnTime = 3 - ((millis() - _deathTime) / 1000);
            if (respawnTime > 0) {
                _display.setTextSize(1);
                _display.setCursor(SCREEN_WIDTH / 2 - 6, SCREEN_HEIGHT / 2 + 12);
                _display.print(respawnTime);
            }
        }
    }

    _display.display();
}

bool Shooter3D::_checkWallCollision(const Vector2f& pos) {
    const float playerRadius = 3.0f;
    
    // check collision with walls
    for (int i = 0; i < SHOOTER3D_WALLS_COUNT; i++) {
        Vector2i wallStart = SHOOTER3D_WALLS_POS[i][0];
        Vector2i wallEnd = SHOOTER3D_WALLS_POS[i][1];
        float distance = _distancePointToLineSegment(pos, wallStart, wallEnd);

        if (distance < playerRadius) {
            return true;
        }
    }
    return false;
}

float Shooter3D::_distancePointToLineSegment(const Vector2f& point, const Vector2i& lineStart, const Vector2i& lineEnd) {
    float A = point.x - lineStart.x;
    float B = point.y - lineStart.y;
    float C = lineEnd.x - lineStart.x;
    float D = lineEnd.y - lineStart.y;
    
    float dot = A * C + B * D;
    float lenSq = C * C + D * D;
    
    float param = -1;
    if (lenSq != 0) {
        param = dot / lenSq;
    }
    
    float xx, yy;
    
    if (param < 0) {
        xx = lineStart.x;
        yy = lineStart.y;
    }
    else if (param > 1) {
        xx = lineEnd.x;
        yy = lineEnd.y;
    }
    else {
        xx = lineStart.x + param * C;
        yy = lineStart.y + param * D;
    }
      float dx = point.x - xx;
    float dy = point.y - yy;
    return sqrt(dx * dx + dy * dy);
}

void Shooter3D::_raycastMultiple(const Vector2f& origin, const Vector2f& direction, float fov, int numRays, float* distances, int* wallIds) {
    float baseAngle = atan2(direction.y, direction.x);
    float angleStep = fov / (numRays - 1);
    float startAngle = baseAngle - fov / 2.0f;
    
    for (int i = 0; i < numRays; i++) {
        float currentAngle = startAngle + i * angleStep;

        // calculate ray direction
        Vector2f rayDir = {cos(currentAngle), sin(currentAngle)};
        int hitWallId;
        float distance = _raycastSingle(origin, rayDir, &hitWallId);
        distances[i] = distance;
        if (wallIds) wallIds[i] = hitWallId;
    }
}

float Shooter3D::_raycastSingle(const Vector2f& origin, const Vector2f& direction, int* hitWallId) {
    float minDistance = 1000.0f;
    int closestWallId = -1;
    
    // check intersection with each wall
    for (int i = 0; i < SHOOTER3D_WALLS_COUNT; i++) {
        Vector2i wallStart = SHOOTER3D_WALLS_POS[i][0];
        Vector2i wallEnd = SHOOTER3D_WALLS_POS[i][1];

        float x1 = origin.x, y1 = origin.y;
        float x2 = origin.x + direction.x, y2 = origin.y + direction.y;
        float x3 = wallStart.x, y3 = wallStart.y;
        float x4 = wallEnd.x, y4 = wallEnd.y;
        
        float denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        
        if (abs(denom) < 0.0001f) continue;
        
        float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
        float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denom;
        
        if (t > 0 && u >= 0 && u <= 1) {
            float intersectX = x1 + t * (x2 - x1);
            float intersectY = y1 + t * (y2 - y1);
            
            float distance = sqrt((intersectX - origin.x) * (intersectX - origin.x) + 
                                (intersectY - origin.y) * (intersectY - origin.y));
            
            if (distance < minDistance) {
                minDistance = distance;
                closestWallId = i;
            }
        }
    }
    
    if (hitWallId) *hitWallId = closestWallId;
    return minDistance;
}

void Shooter3D::_drawMinimap() {
    int offsetX = 32;
    int centerX = SCREEN_WIDTH / 2 + offsetX;
    int centerY = SCREEN_HEIGHT / 2;
    int mapRadius = (centerX < centerY ? centerX : centerY) - 5;
        
    float playerAngle = atan2(_playerDir.y, _playerDir.x);
    playerAngle += PI / 2.0f;
    
    // draw maps walls from above
    for (int i = 0; i < SHOOTER3D_WALLS_COUNT; i++) {
        Vector2i wallStart = SHOOTER3D_WALLS_POS[i][0];
        Vector2i wallEnd = SHOOTER3D_WALLS_POS[i][1];
        
        float distToStart = sqrt(pow(wallStart.x - _playerPos.x, 2) + pow(wallStart.y - _playerPos.y, 2));
        float distToEnd = sqrt(pow(wallEnd.x - _playerPos.x, 2) + pow(wallEnd.y - _playerPos.y, 2));

        // rotate wall endpoints based on player direction
        float angleToStart = atan2(wallStart.y - _playerPos.y, wallStart.x - _playerPos.x);
        float angleToEnd = atan2(wallEnd.y - _playerPos.y, wallEnd.x - _playerPos.x);

        float rotatedStartX = centerX + (cos(angleToStart - playerAngle) * distToStart * _minimapScale);
        float rotatedStartY = centerY + (sin(angleToStart - playerAngle) * distToStart * _minimapScale);
        float rotatedEndX = centerX + (cos(angleToEnd - playerAngle) * distToEnd * _minimapScale);
        float rotatedEndY = centerY + (sin(angleToEnd - playerAngle) * distToEnd * _minimapScale);
        _display.drawLine(rotatedStartX, rotatedStartY, rotatedEndX, rotatedEndY, 1);
    };
    
    // draw other player on minimap if connected
    if (_otherPlayerVisible) {
        float distToOtherPlayer = sqrt(pow(_otherPlayerPos.x - _playerPos.x, 2) + pow(_otherPlayerPos.y - _playerPos.y, 2));
        
        if (distToOtherPlayer < 50.0f) {
            float angleToOtherPlayer = atan2(_otherPlayerPos.y - _playerPos.y, _otherPlayerPos.x - _playerPos.x);
            float rotatedOtherPlayerX = centerX + (cos(angleToOtherPlayer - playerAngle) * distToOtherPlayer * _minimapScale);
            float rotatedOtherPlayerY = centerY + (sin(angleToOtherPlayer - playerAngle) * distToOtherPlayer * _minimapScale);
            
            int otherPlayerSize = 3;
            _display.drawTriangle(
                rotatedOtherPlayerX, rotatedOtherPlayerY - otherPlayerSize,
                rotatedOtherPlayerX - otherPlayerSize, rotatedOtherPlayerY + otherPlayerSize,
                rotatedOtherPlayerX + otherPlayerSize, rotatedOtherPlayerY + otherPlayerSize,
                1
            );
            
            // draw other player's facing direction as a small line
            float otherPlayerFacingAngle = atan2(_otherPlayerDir.y, _otherPlayerDir.x) - playerAngle;
            float dirLineLength = 4;
            int dirEndX = rotatedOtherPlayerX + cos(otherPlayerFacingAngle) * dirLineLength;
            int dirEndY = rotatedOtherPlayerY + sin(otherPlayerFacingAngle) * dirLineLength;
            _display.drawLine(rotatedOtherPlayerX, rotatedOtherPlayerY, dirEndX, dirEndY, 1);
        }
    }
    
    // draw player as triangle
    int playerSize = 3;
    _display.fillTriangle(
        centerX, centerY - playerSize,
        centerX - playerSize, centerY + playerSize,
        centerX + playerSize, centerY + playerSize,
        1
    );
    
    
    // draw minimap border
    _display.drawCircle(centerX, centerY, mapRadius, 1);
    _display.fillRect(0, 0, 64 - mapRadius + offsetX, SCREEN_HEIGHT, 0);
    _display.fillRect(65 + mapRadius + offsetX, 0, 64 - mapRadius, SCREEN_HEIGHT, 0);

    int lenghts[28] = { 26, 23, 20, 18, 17, 15, 14, 13, 12, 11, 10, 9, 9, 8, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 4, 4, 4};
    for (int i = 0; i < 28; i++) {
        _display.drawLine(i + 64 - mapRadius + offsetX, 0, i + 64 - mapRadius + offsetX, lenghts[i], 0);
        _display.drawLine(64 + mapRadius - i + offsetX, 0, 64 + mapRadius - i + offsetX, lenghts[i], 0);
        _display.drawLine(i + 64 - mapRadius + offsetX, SCREEN_HEIGHT, i + 64 - mapRadius + offsetX, SCREEN_HEIGHT - lenghts[i], 0);
        _display.drawLine(64 + mapRadius - i + offsetX, SCREEN_HEIGHT, 64 + mapRadius - i + offsetX, SCREEN_HEIGHT - lenghts[i], 0);
    }

    _display.setTextSize(1);
    _display.setTextColor(1);
    _display.setCursor(2, 2);
    _display.print("SCORE");
    _display.setCursor(2, 12);
    _display.print("You: ");
    _display.print(_playerScore);
    if (_otherPlayerVisible) {
        _display.setCursor(2, 22);
        _display.print("Foe: ");
        _display.print(_otherPlayerScore);
    }

    unsigned long currentTime = millis();
    bool hasSpawnProtection = (_spawnTime > 0) && 
                              (currentTime >= _spawnTime) && 
                              (currentTime - _spawnTime < SPAWN_PROTECTION_DURATION);
    if (hasSpawnProtection) {
        _display.setCursor(2, 32);
        _display.print("SAFE");
    }

    _display.setCursor(2, SCREEN_HEIGHT - 10);
    _display.print("NET: ");
    if ((millis() - _lastOtherPlayerUpdate > 3000)) {
        _display.print("KO");
    } else if ((millis() - _lastOtherPlayerUpdate > 200)) {
        _display.print("...");
    } else {
        _display.print("OK");
    };
}

int Shooter3D::_getSpriteDirectionIndex(const Vector2f& playerPos, const Vector2f& spritePos, float spriteRotation) {
    float dx = playerPos.x - spritePos.x;
    float dy = playerPos.y - spritePos.y;
    float angleToPlayer = atan2(dy, dx);
    float relativeAngle = angleToPlayer - spriteRotation;
    
    while (relativeAngle < 0) relativeAngle += 2 * PI;
    while (relativeAngle >= 2 * PI) relativeAngle -= 2 * PI;
    
    // get index based on relative angle
    int directionIndex = (int)((relativeAngle + PI/8.0f) / (PI/4.0f)) % SPRITE_DIRECTIONS;
    if (directionIndex < 0) directionIndex = 0;
    if (directionIndex >= SPRITE_DIRECTIONS) directionIndex = SPRITE_DIRECTIONS - 1;
      return directionIndex;
}

void Shooter3D::_drawOtherPlayer(float* wallDistances, int numRays, float fov) {
    float spriteX = _otherPlayerPos.x - _playerPos.x;
    float spriteY = _otherPlayerPos.y - _playerPos.y;
    float cosAngle = _playerDir.x;
    float sinAngle = _playerDir.y;
    float transformX = -sinAngle * spriteX + cosAngle * spriteY;
    float transformY = cosAngle * spriteX + sinAngle * spriteY;
    
    if (transformY <= 0.1f) return;
    float otherPlayerRotation = atan2(_otherPlayerDir.y, _otherPlayerDir.x);
    int spriteDirectionIndex = _getSpriteDirectionIndex(_playerPos, _otherPlayerPos, otherPlayerRotation);
    
    int spriteScreenX = (int)(SCREEN_WIDTH / 2 + (transformX * SCREEN_WIDTH) / (transformY * tan(fov / 2) * 2));
    
    // calculate sprite height and width
    int spriteHeight = abs((int)(SCREEN_HEIGHT * 20.0f / transformY));
    int spriteWidth = abs((int)(SCREEN_HEIGHT * 10.0f / transformY));
    
    // calculate draw positions
    int drawStartY = (SCREEN_HEIGHT / 2) - (spriteHeight / 2);
    int drawEndY = (SCREEN_HEIGHT / 2) + (spriteHeight / 2);
    int drawStartX = spriteScreenX - (spriteWidth / 2);
    int drawEndX = spriteScreenX + (spriteWidth / 2);
    
    // skip offscreen
    if (drawEndX < -spriteWidth || drawStartX >= SCREEN_WIDTH + spriteWidth) return;
    
    for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
        if (stripe < 0 || stripe >= SCREEN_WIDTH) continue;
        
        float wallDistance = 1000.0f;
        
        if (wallDistances && numRays > 0) {
            int rayIndex = (stripe * numRays) / SCREEN_WIDTH;
            
            if (rayIndex >= 0 && rayIndex < numRays) {
                wallDistance = wallDistances[rayIndex];
                
                float baseAngle = atan2(_playerDir.y, _playerDir.x);
                float angleStep = fov / (numRays - 1);
                float startAngle = baseAngle - fov / 2.0f;
                float rayAngle = startAngle + rayIndex * angleStep;
                float angleFromCenter = rayAngle - baseAngle;
                wallDistance = wallDistance * cos(angleFromCenter);
            }
        }
        
        float spriteDistance = transformY;
        
        if (wallDistance < spriteDistance - 1.0f) {
            continue;
        }
        
        // calculate texture X coordinate
        int texX = (int)(256 * (stripe - drawStartX) * SPRITE_WIDTH / spriteWidth) / 256;
        if (texX < 0 || texX >= SPRITE_WIDTH) continue;
        
        for (int y = drawStartY; y < drawEndY; y++) {
            if (y < 0 || y >= SCREEN_HEIGHT) continue;
            
            int texY = (int)(256 * (y - drawStartY) * SPRITE_HEIGHT / spriteHeight) / 256;
            if (texY < 0 || texY >= SPRITE_HEIGHT) continue;
            
            int byteIndex = texX / 4;
            int pixelInByte = 3 - (texX % 4);
            int bitShift = pixelInByte * 2;
            
            unsigned char spriteByte = player_sprites[spriteDirectionIndex][texY][byteIndex];
            int pixelValue = (spriteByte >> bitShift) & 0x03;
            
            if (pixelValue == 1) {
                _display.drawPixel(stripe, y, _isOtherPlayerShielded ? 0 : 1);
            } else if (pixelValue == 2) {
                _display.drawPixel(stripe, y, _isOtherPlayerShielded ? 1 : 0);
            }
        }
    }
}

bool Shooter3D::_checkPlayerHit(const Vector2f& shooterPos, const Vector2f& shooterDir, const Vector2f& targetPos) {
    const float playerRadius = 4.0f;
        
    float dx = targetPos.x - shooterPos.x;
    float dy = targetPos.y - shooterPos.y;
    float distanceToTarget = sqrt(dx * dx + dy * dy);
    
    const float maxRange = 100.0f;
    if (distanceToTarget > maxRange) {
        return false;
    }
    
    int hitWallId = -1;
    float wallDistance = _raycastSingle(shooterPos, shooterDir, &hitWallId);
    
    if (wallDistance < distanceToTarget - playerRadius) {
        return false;
    }

    float t = (dx * shooterDir.x + dy * shooterDir.y);
    
    if (t < 0) {
        return false;
    }
    
    Vector2f closestPoint;
    closestPoint.x = shooterPos.x + t * shooterDir.x;
    closestPoint.y = shooterPos.y + t * shooterDir.y;
    
    float hitDx = targetPos.x - closestPoint.x;
    float hitDy = targetPos.y - closestPoint.y;
    float hitDistance = sqrt(hitDx * hitDx + hitDy * hitDy);
    return hitDistance <= playerRadius;
}

bool Shooter3D::_isProtected() {
    unsigned long currentTime = millis();
    
    bool isManuallyShielded = (currentTime >= _lastShield) && 
                              (currentTime - _lastShield < SHIELD_DURATION);

    bool hasSpawnProtection = (_spawnTime > 0) && 
                              (currentTime >= _spawnTime) && 
                              (currentTime - _spawnTime < SPAWN_PROTECTION_DURATION);
    
    return isManuallyShielded || hasSpawnProtection;
}

void Shooter3D::_spawnPlayer() {
    _spawnTime = millis();
    int spawnIndex = random(SHOOTER3D_SPAWN_COUNT);
    int attempts = 0;

    while (attempts < SHOOTER3D_SPAWN_COUNT) {
        Vector2f testPos = SHOOTER3D_SPAWN_POSITIONS[spawnIndex];
          if (!_checkWallCollision(testPos)) {
            _playerPos = testPos;
            _playerDir = SHOOTER3D_SPAWN_DIRECTIONS[spawnIndex];
            return;
        }
        spawnIndex = (spawnIndex + 1) % SHOOTER3D_SPAWN_COUNT;
        attempts++;
    }
    _playerPos = SHOOTER3D_SPAWN_POSITIONS[0];
    _playerDir = SHOOTER3D_SPAWN_DIRECTIONS[0];
}

} // namespace Game
