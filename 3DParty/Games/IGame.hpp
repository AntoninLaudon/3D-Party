#pragma once

#include <string>
#include <Adafruit_SSD1306.h>
#include "config.h"

class IGame {
public:
    IGame(Adafruit_SSD1306 &display) : _display(display) {}
    
    virtual void init() = 0;
    virtual int run() = 0;
    virtual std::string getName() const { return _name; }
    
    protected:
    virtual void _update(unsigned long deltaTime) = 0;
    virtual void _draw() = 0;
    Adafruit_SSD1306 &_display;
    std::string _name = "__UNDEFINED__";
    unsigned long _lastUpdate = 0;

};
