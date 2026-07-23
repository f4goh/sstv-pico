#ifndef EXPANDER_H
#define EXPANDER_H

#include <Arduino.h>
#include <Wire.h>

#define PCF8574_ADDRESS 0x20

class Expander
{
public:
    Expander();

    bool begin();
    void setBit(uint8_t bit, bool level);

private:
    uint8_t _address;
    uint8_t _state;

    void updateOutputs();
};

#endif
