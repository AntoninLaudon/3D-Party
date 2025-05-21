#pragma once

#include "IGame.hpp"
#include "tools.hpp"
#include "struct.hpp"
#include "config.h"

namespace Game {

// ################################################################
// ####  SHOOTER3D GAME CONSTANTS  ################################
// ################################################################

#define SHOOTER_PLAYER_SPEED 0.5
#define SHOOTER_PLAYER_TURN_SPEED 2
#define SHOOTER_FOV 90

#define SHOOTER_MAP_WIDTH 10
#define SHOOTER_MAP_HEIGHT 10

#define WALL_OFFSET 0
#define UI_HEIGHT 54
#define COLLISION_DIST 5

enum Texture {
  CHECK,
  STRIPE_V,
  STRIPE_H,
  STRIPE_D
};

struct Wall {
  Vector2f points[2];
  Texture texture;
};

// ################################################################
// ######  SHOOTER3D CLASS DEFINITION  ############################
// ################################################################

class Shooter3D : public IGame {
public:
    Shooter3D(Adafruit_SSD1306 &display) : IGame(display) {}
    void init() override;
    void step() override;

private:
    void _update(unsigned long deltaTime) override;
    void _draw() override;
    void _verticalLine(int x, int half_length);
    void _checkLine(int x, int half_length, boolean phase);

    Wall _walls[4] = {
        {10, 10, 10, 120, CHECK},
        {10, 120, 120, 120, CHECK},
        {120, 120, 120, 10, CHECK},
        {120, 10, 10, 10, CHECK}
    };
    const uint _numWalls = sizeof(_walls) / sizeof(_walls[0]);
    Vector2f _playerPos;
    float _playerAngle;
};


// ################################################################
// ######  SHOOTER3D CLASS IMPLEMENTATION  ########################
// ################################################################

void Shooter3D::init() {
    _isNetworkGame = false;
    _name = "3D Shooter";
    _lastUpdate = millis();
    _playerPos = {20.0f, 20.0f};
    _playerAngle = 0.0f;
}

void Shooter3D::step() {
    unsigned long currentMillis = millis();
    unsigned long deltaTime = currentMillis - _lastUpdate;
    if (deltaTime < UPDATE_INTERVAL) return;
    _lastUpdate = currentMillis;

    _retrieveInputs();
    _update(deltaTime);
    _draw();
}

void Shooter3D::_update(unsigned long deltaTime) {
    if (_jx != 0) {
        _playerAngle += _jx * SHOOTER_PLAYER_TURN_SPEED;
        if (_playerAngle < 0) _playerAngle += 360;
        if (_playerAngle >= 360) _playerAngle -= 360;
    }

    if (_jy != 0) {
        Vector2f pn = {
            _playerPos.x + 2*cos(_playerAngle * (PI/180)) * -_jy * SHOOTER_PLAYER_SPEED,
            _playerPos.y + 2*sin(_playerAngle * (PI/180)) * -_jy * SHOOTER_PLAYER_SPEED
        };
        _playerPos = pn;
    }
}

void Shooter3D::_draw() {
    _display.clearDisplay();
    // Defines the camera's depth of view and field of view
    float dov = 10.0f;
    float fov = 80.0f;

    float x3 = _playerPos.x;
    float y3 = _playerPos.y;
    float x4, y4;

    // Defines the number of rays
    for (int i = 0; i < 128; i+=2) {

        // Calculates the angle at which the ray is projected
        float angle = (i*(fov/127.0f)) - (fov/2.0f);

        // Projects the endpoint of the ray
        x4 = _playerPos.x + dov*cosf((_playerAngle + angle) * (PI/180));
        y4 = _playerPos.y + dov*sinf((_playerAngle + angle) * (PI/180));

        float dist = 100000.0f;
        Vector2f pt_final = { NULL, NULL };
        Wall cur_wall;
        int cur_edge2pt;

        // Checks if the vector from the camera to the ray's endpoint intersects any walls
        for (int w = 0; w < _numWalls; w++) {

        float x1 = _walls[w].points[0].x;
        float y1 = _walls[w].points[0].y;
        float x2 = _walls[w].points[1].x;
        float y2 = _walls[w].points[1].y;

        float denominator = (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4);

        // vectors do not ever intersect
        if (denominator == 0) continue;

        float t = ((x1-x3)*(y3-y4)-(y1-y3)*(x3-x4))/denominator;
        float u = -((x1-x2)*(y1-y3)-(y1-y2)*(x1-x3))/denominator;

        // Case where the vectors intersect
        if (t > 0 && t < 1 && u > 0) {

            Vector2f pt = { x1 + t * (x2 - x1), y1 + t * (y2 - y1) };
            float ptDist2 = dist2(pt, _playerPos);

            // Checks if the intersected wall is the closest to the camera
            if (ptDist2 < dist) {
            dist = ptDist2;
            pt_final = pt;
            cur_wall = _walls[w];
            cur_edge2pt = dist2(pt, cur_wall.points[0]);
            }
        }
        }

        if (pt_final.x != NULL) {

        int length = 25000 / dist;

        // Draws lines at the edges of walls
        // int wall_len = dist2(cur_wall.points[0], cur_wall.points[1]);
        // if (cur_edge2pt < 2 || wall_len - cur_edge2pt < 2) {
        //     _verticalLine(i, length);
        //     continue;
        // }

        if (cur_wall.texture == CHECK) {
            _checkLine(i, length, (cur_edge2pt % 1000) < 500);
        }
    //      } else if (cur_wall.tex == STRIPE_H) {
    //
    //      } else if (cur_wall.tex == STRIPE_V) {
    //
    //      } else if (cur_wall.tex == STRIPE_D) {
    //
    //      }

        }
    }

    _display.display();
}

void Shooter3D::_verticalLine(int x, int half_length) {
    _display.drawLine(x, SCREEN_HEIGHT / 2 + half_length, x, SCREEN_HEIGHT / 2 - half_length, SSD1306_WHITE);
    // for (int i = 0; i < half_length; i+=2) {
    //     _display.drawPixel(x, UI_HEIGHT/2 + WALL_OFFSET + i, SSD1306_WHITE);
    //     // Ensures that the wall doesnt overlap with the UI
    //     // if (UI_HEIGHT/2 + WALL_OFFSET - i < UI_HEIGHT) {
    //     //     _display.drawPixel(x, UI_HEIGHT/2 + WALL_OFFSET - i, SSD1306_WHITE);
    //     // }
    // }
}

void Shooter3D::_checkLine(int x, int half_length, boolean phase) {
    int lower = UI_HEIGHT/2 - half_length + WALL_OFFSET;
    int upper = UI_HEIGHT/2 + half_length + WALL_OFFSET;

    // for (int i = lower; i < upper; i+=2) {

    //     if (i > UI_HEIGHT) {break;}

    //     if (phase) {
    //     if (i == lower || (i >= lower + half_length && i <= lower + 3*half_length/2)) {
    //         i += half_length/2;
    //     }
    //     } else {
    //     if ((i >= lower + half_length/2 && i <= lower + half_length) || (i >= lower + 3*half_length/2 && i <= upper)) {
    //         i += half_length/2;
    //     }
    //     }

    //     _display.drawPixel(x, i, SSD1306_WHITE);
    // }
    _display.drawPixel(x, UI_HEIGHT/2 - half_length + WALL_OFFSET, SSD1306_WHITE);
    _display.drawPixel(x, UI_HEIGHT/2 + half_length + WALL_OFFSET, SSD1306_WHITE);
}

} // namespace Game

