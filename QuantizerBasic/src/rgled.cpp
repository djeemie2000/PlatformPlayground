#include "rgled.h"

RGLed::RGLed()
{}

void RGLed::Begin(uint8_t pin0, uint8_t pin1)
{
    m_OutputPin[0] = pin0;   
    m_OutputPin[1] = pin1;   

    for(int idx = 0; idx<2; ++idx)
    {
        pinMode(m_OutputPin[idx], OUTPUT);
        analogWrite(m_OutputPin[idx], 0);
    }
}

void RGLed::Set(uint8_t r, uint8_t g)
{
    analogWrite(m_OutputPin[0], r);
    analogWrite(m_OutputPin[1], g);
}

void TestRGLed(RGLed& led, int repeats)
{
    Serial.print("TestRGLed... ");
    for(int repeat = 0; repeat<repeats; ++ repeat)
    {
        for(int c = 0; c<256; c+=32)
        {
            led.Set(c, 0);
            delay(200);
            led.Set(0, c);
            delay(200);
        }
    }

    Serial.println("Done");
}

void TestRGLed2(RGLed& led, int repeats)
{
    const int delayTime = 1000;
    Serial.print("TestRGLed2... ");
    for(int repeat = 0; repeat<repeats; ++ repeat)
    {
        led.Set(255, 0);//red
        delay(delayTime);
        led.Set(0, 255);//green
        delay(delayTime);
        led.Set(128, 128);//??
        delay(delayTime);
        led.Set(64, 192);//??
        delay(delayTime);
        led.Set(192, 64);//??
        delay(delayTime);
    }

    Serial.println("Done");
}

void ColorOut(RGLed& led, uint8_t color)
{
    uint8_t c = color>>1;
    led.Set(c, 255-c);
}

void TestColorOut(RGLed& led, int repeats)
{
    const int delayTime = 256;
    Serial.print("TestColorOut... ");
    for(int repeat = 0; repeat<repeats; ++ repeat)
    {
        for(int c = 0; c<=255; c+=32)
        {
            ColorOut(led, c);
            delay(delayTime);
        }
    }
    Serial.println("Done");
}
