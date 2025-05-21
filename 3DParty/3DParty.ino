#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include "config.h"
#include "Games/IGame.hpp"
#include "Games/Example.hpp"
#include "Games/Breakout.hpp"
#include "Menu.hpp"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

IGame *games[] = {
    new ExampleGame(display),
    new BreakoutGame(display)
};
Menu menu(display, games);
int currentGameIndex = 0;

void setup() {
  Serial.begin(115200);
  // Config
  rgbLedWrite(PIN_RGB_LED, 32, 0, 0);
  pinMode(PIN_BUTTON_A, INPUT_PULLUP);
  pinMode(PIN_BUTTON_B, INPUT_PULLUP);
  pinMode(PIN_JOYSTICK_BUTTON, INPUT_PULLUP);
  Wire.begin(PIN_SDA, PIN_SCL);

  delay(250);
  rgbLedWrite(PIN_RGB_LED, 0, 0, 0);
  Serial.println("Starting...");

  // Init OLED display
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.setRotation(SCREEN_ROTATION);

  // Init Games
  for (int i = 0; i < sizeof(games) / sizeof(games[0]); i++) games[i]->init();
  
  currentGameIndex = menu.run();

  Serial.print("Selected game: ");
  Serial.print(currentGameIndex);
  Serial.print(" -> ");
  Serial.println(games[currentGameIndex]->getName().c_str());
  Serial.print("Multiplayer: ");
  Serial.println(games[currentGameIndex]->isNetworkGame() ? "Yes" : "No");
  Serial.println("Starting game...");
}

void loop() {
  games[currentGameIndex]->run();
}
