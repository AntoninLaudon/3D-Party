#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include "config.h"
#include "Games/IGame.hpp"
#include "Games/ComponentTest.hpp"
#include "Games/MultiplayerTest.hpp"
#include "Games/Breakout.hpp"
#include "Games/Shooter3D.hpp"
#include "Menu.hpp"
#include "network.hpp"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Game::IGame *games[] = {
    new Game::ComponentTest(display),
    new Game::MultiplayerTest(display),
    new Game::Breakout(display),
    new Game::Shooter3D(display),
};
Menu menu(display, games);
int currentGameIndex = 0;

void setup() {
  Serial.begin(115200);
  // Config
  rgbLedWrite(PIN_RGB_LED, 32, 0, 0);
  pinMode(PIN_BUTTON_A, INPUT);
  pinMode(PIN_BUTTON_B, INPUT);
  pinMode(PIN_JOYSTICK_BUTTON, INPUT);
  Wire.begin(PIN_SDA, PIN_SCL);

  delay(250);
  rgbLedWrite(PIN_RGB_LED, 0, 0, 0);
  Serial.println("Starting...");

  // Init OLED display
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.setRotation(SCREEN_ROTATION);

  // Init Games
  for (Game::IGame *game : games) game->init();

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
  games[currentGameIndex]->step();
}
