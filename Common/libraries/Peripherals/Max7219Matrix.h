#pragma once

#include <mbed.h>
#include "max7219.h"
#include "DigitalOutMatrix.h"

class Max7219Matrix : public DigitalOutMatrix
{
public:
    Max7219Matrix(int numDevices, SPI* spi, PinName cs);//PinName mosi, PinName miso, PinName sclk, PinName cs);

    void Configure(bool rotated = false);
    void Test();

    int NumRows() const{return Size;}
    int NumCols() const{return Size*m_NumDevices;}
    bool Set(int row, int col);
    bool Clear(int row, int col);
    int Write();
    int Write(int row);

private:
    static const int Size = 8;//8x8 led matrix
    static const int MaxNumDevices = 4;

    struct Device
    {
        Device();
        bool Set(uint8_t row, uint8_t col);
        bool Clear(uint8_t row, uint8_t col);

        uint8_t m_Digit[Size];
    };

    const int m_NumDevices;
    Max7219 m_LedMatrix;
    Device m_Device[MaxNumDevices];
    bool m_Rotated;
};

void TestExtended(Max7219Matrix& matrix, Serial& pc);
