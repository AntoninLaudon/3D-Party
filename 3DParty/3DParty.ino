#include <Wire.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "config.h"
#include "Core.hpp"

Core core;

void setup() {
  Serial.begin(115200);

  // Config
  rgbLedWrite(PIN_RGB_LED, 0, 32, 32);
  pinMode(PIN_BUTTON_A, INPUT);
  pinMode(PIN_BUTTON_B, INPUT);
  pinMode(PIN_JOYSTICK_BUTTON, INPUT);

  Wire.begin(PIN_SDA, PIN_SCL);
  WiFi.mode(WIFI_STA);
  WiFi.STA.begin();
  
  delay(250);
  
  rgbLedWrite(PIN_RGB_LED, 0, 0, 0);
  Serial.println("Starting...");

  if (!core.init()) {
    Serial.println("Core initialization failed!");
    while(1) {
      rgbLedWrite(PIN_RGB_LED, 32, 0, 0);
      delay(100);
      rgbLedWrite(PIN_RGB_LED, 0, 0, 0);
      delay(100);
    };
  }
  core.launchMenu();
}

void loop() {
  core.run();
  delay(10);
}
