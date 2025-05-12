#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include "config.h"
#include "Games/IGame.hpp"
#include "Games/Example.hpp"

uint8_t broadcastAddress[] = MAC_ADDRESS;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

IGame *games[] = {
    new ExampleGame(display)
};
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
  // Init RGB LED


  // Init OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;) delay(1000); // Don't proceed, loop forever
  }
  display.setRotation(SCREEN_ROTATION);

  // Init Game
  games[currentGameIndex]->init();
}

void loop() {
  games[currentGameIndex]->run();
}
