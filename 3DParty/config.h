#pragma once

// PINS
#define PIN_SDA 6
#define PIN_SCL 5

#define PIN_RGB_LED 21
#define PIN_BUZZER 12

#define PIN_JOYSTICK_X 10
#define PIN_JOYSTICK_Y 11
#define PIN_JOYSTICK_BUTTON 9

#define PIN_BUTTON_A 7
#define PIN_BUTTON_B 8

// NETWORK
#define MAC_ADDRESS {0x50, 0x78, 0x7d, 0x17, 0xf2, 0xe8}
#define TCP_RETRY_TIMEOUT 1000
#define TCP_RETRY_COUNT 5

// SCREEN
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define SCREEN_ROTATION 2


// GAME
#define UPDATE_INTERVAL 25 // ms
#define DEADZONE 0.2