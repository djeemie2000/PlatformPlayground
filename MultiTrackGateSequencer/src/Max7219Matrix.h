#pragma once

#include <mbed.h>
#include "max7219.h"
#include "DigitalOutMatrix.h"

class Max7219Matrix : public DigitalOutMatrix
{
public:
    Max7219Matrix(int numDevices, PinName mosi, PinName miso, PinName sclk, PinName cs);

    void Init();
    void Test();

    int NumRows() const{return Size;}
    int NumCols() const{return Size*m_NumDevices;}
    void Set(int row, int col);
    void Clear(int row, int col);
    void Write();

private:
    static const int Size = 8;//8x8 led matrix
    const int m_NumDevices;
    Max7219 m_LedMatrix;
    uint32_t m_Pattern[Size];//max 4 devices
};
