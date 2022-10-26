#pragma once 

#include <Arduino.h>

class TTP8229TouchPad
{
public:
    static const int SPIMode = 0;
    static const int I2CMode = 1;

    TTP8229TouchPad();

    void Begin(int mode);

    void Read();
    
    int GetNumPads() const;
    bool Get(int Pad) const;
    bool IsPushed(int Pad) const;
    bool IsClicked(int Pad) const;
    bool IsReleased(int Pad) const;

private:
    int m_Mode;
    uint16_t m_TouchState;// bit = 1 => touched, bit is 0 => not touched
    uint16_t m_PrevTouchState;
};
