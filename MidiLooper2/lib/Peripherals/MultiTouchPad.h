#pragma once
#include <Arduino.h>
#include "MPR121TouchPad.h"

class MultiTouchPad
{
public:
    MultiTouchPad();

    void Begin(MPR121TouchPad* touchPad1, MPR121TouchPad* touchPad2 = nullptr, MPR121TouchPad* touchPad3 = nullptr);
    void Read();

    int GetNumPads() const;
    bool Get(int Pad) const;
    bool IsPushed(int Pad) const;
    bool IsClicked(int Pad) const;
    bool IsReleased(int Pad) const;

private:
    static const int Capacity = 3;
    MPR121TouchPad* m_TouchPad[Capacity];
};
