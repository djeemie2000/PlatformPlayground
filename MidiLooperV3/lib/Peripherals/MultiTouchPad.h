#pragma once
#include <Arduino.h>
#include "MPR121TouchPad.h"
#include "TTP8229TouchPad.h"

class MultiTouchPad
{
public:
    MultiTouchPad();

    void Add(MPR121TouchPad* touchPad);
    void Add(TTP8229TouchPad_I2C* touchPad);

    void Begin();
    void Read();

    int GetNumPads() const;
    bool Get(int Pad) const;
    bool IsPushed(int Pad) const;
    bool IsClicked(int Pad) const;
    bool IsReleased(int Pad) const;

private:
    static const int Capacity = 4;
    int m_Size;
    MPR121TouchPad* m_TouchPad[Capacity];
    int m_Size2;
    TTP8229TouchPad_I2C* m_TouchPad2[Capacity];
};
