#pragma once

#include <Arduino.h>

class AnalogIn
{
public:
    AnalogIn();

    void begin(int pin);

    void Read();
    int Get() const;
    int Prev() const;

private:
    int m_Pin;
    int m_Curr;
    int m_Prev;
};
