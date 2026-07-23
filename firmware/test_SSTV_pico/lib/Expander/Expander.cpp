#include "Expander.h"

Expander::Expander()
{
    _address = PCF8574_ADDRESS;
    _state = 0xFF;   // PCF8574 au repos = toutes les lignes à HIGH
}


bool Expander::begin()
{
    Wire.begin();

    updateOutputs();

    return true;
}


void Expander::setBit(uint8_t bit, bool level)
{
    if (bit > 7)
        return;

    if (level == HIGH)
    {
        _state |= (1 << bit);
    }
    else
    {
        _state &= ~(1 << bit);
    }

    updateOutputs();
}


void Expander::updateOutputs()
{
    Wire.beginTransmission(_address);
    Wire.write(_state);
    Wire.endTransmission();
}
