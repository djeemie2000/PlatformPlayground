#pragma once

#include <Arduino.h>
#include "DigitalOutMatrix.h"


class Max7219Matrix : public DigitalOutMatrix
{
public:
    Max7219Matrix(int numDevices, uint8_t csPin);

    void Configure();

    int NumRows() const{return Size;}
    int NumCols() const{return Size*m_NumDevices;}
    bool Set(int row, int col);
    bool Clear(int row, int col);
    int Write();
    int WriteRow(int row);
    int WriteAll();
    
private:
    static const int Size = 8;//8x8 led matrix
    static const int MaxNumDevices = 4;

    void WriteCommand(uint8_t command, uint8_t value);
    void SetBits(int device, int row, int col);
    void ClearBits(int device, int row, int col);
    
    const int m_NumDevices;
    uint8_t m_CsPin;
    uint8_t m_Bits[Size][MaxNumDevices];
};

