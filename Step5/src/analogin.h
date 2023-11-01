#pragma once

#include <Arduino.h>

class AnalogIn
{
public:
    AnalogIn();

    void Begin(int pin);

    void Read();
    int Get() const;
    int Prev() const;

private:
    int m_Pin;
    int m_Curr;
    int m_Prev;
};

void TestAnalogIn(AnalogIn& analogIn, int repeats);
void TestAnalogIn(AnalogIn& analogIn0, AnalogIn& analogIn1, int repeats);
void TestAnalogIn(AnalogIn& analogIn0, AnalogIn& analogIn1, AnalogIn& analogIn2, int repeats);
