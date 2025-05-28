#pragma once

#include "struct.hpp"

void HSVtoRGB(int h, float s, float v, int &r, int &g, int &b) {
    h = h % 360;
    float c = v * s;  
    float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
    float m = v - c;
    float r1, g1, b1;

    switch (h / 60) {
        case 0:  r1 = c, g1 = x, b1 = 0; break;
        case 1:  r1 = x, g1 = c, b1 = 0; break;
        case 2:  r1 = 0, g1 = c, b1 = x; break;
        case 3:  r1 = 0, g1 = x, b1 = c; break;
        case 4:  r1 = x, g1 = 0, b1 = c; break;
        case 5:  r1 = c, g1 = 0, b1 = x; break;
        default: r1 = 0, g1 = 0, b1 = 0; break;
    }
    r = constrain((r1 + m) * 255, 0, 255);
    g = constrain((g1 + m) * 255, 0, 255);
    b = constrain((b1 + m) * 255, 0, 255);
}

float pow2(float x) {
    return x * x;
}

float dist2(Vector2f v, Vector2f u) {
    return pow2(v.x - u.x) + pow2(v.y - u.y);
}

int randint(int min, int max) {
    return esp_random() % (max - min + 1) + min;
}
