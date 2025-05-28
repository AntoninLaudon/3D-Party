#pragma once

typedef struct Vector2i
{
    int x;
    int y;
} Vector2i;

Vector2i operator+(const Vector2i &v1, const Vector2i &v2) {
    return {v1.x + v2.x, v1.y + v2.y};
}

typedef struct Vector2f
{
    float x;
    float y;
} Vector2f;

Vector2f operator+(const Vector2f &v1, const Vector2f &v2) {
    return {v1.x + v2.x, v1.y + v2.y};
}
