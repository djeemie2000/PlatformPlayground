#pragma once
#include <Arduino.h>

class RGLed
{
public:
    RGLed();
    void Begin(uint8_t pin0, uint8_t pin1);

    void Set(uint8_t r, uint8_t g);
private:
    uint8_t m_OutputPin[2];
};

void TestRGLed(RGLed& led, int repeats);
void TestRGLed2(RGLed& led, int repeats);
void ColorOut(RGLed& led, uint8_t color);
void TestColorOut(RGLed& led, int repeats);
