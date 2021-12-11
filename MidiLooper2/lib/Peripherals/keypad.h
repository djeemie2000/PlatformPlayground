#pragma once
#include <Arduino.h>

class KeyPad
{
public:
    KeyPad();

    void Begin(int pinOutRow1, int pinOutRow2, int pinOutRow3, int pinOutRow4, 
                int pinInCol1, int pinInCol2, int pinInCol3, int pinInCol4);
    void Read();
    void ReadRow(int row);

    int GetNumPads() const;
    bool Get(int Pad) const;
    bool IsPushed(int Pad) const;
    bool IsClicked(int Pad) const;
    bool IsReleased(int Pad) const;

private:
    static const int NumRows = 4;
    static const int NumCols = 4;
    static const int NumPads = NumRows * NumCols;

    int m_PinOutRow[NumRows];
    int m_PinInCol[NumCols];

    // TODO int/bool [NumPads]
    int m_TouchState[NumPads];// bit = 1 => touched, bit is 0 => not touched
    int m_PrevTouchState[NumPads];
};
