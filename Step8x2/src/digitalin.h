#pragma once

#include <Arduino.h>

class DigitalIn
{
public:
    DigitalIn();

    void begin(int pin, bool pullup);

    void Read();
    int Get() const;
    bool IsRising() const;
    bool IsFalling() const;

private:
    int m_Pin;
    int m_Curr;
    int m_Prev;
};
