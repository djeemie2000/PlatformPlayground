#pragma once

#include <mbed.h>

// "Interface"
class DigitalOutMatrix
{
public:
    DigitalOutMatrix(){}

    virtual int NumRows() const{return 1;}
    virtual int NumCols() const{return 1;}
    virtual void Set(int row, int col) {}
    virtual void Clear(int row, int col) {}
    virtual void Write(){}
    virtual void Write(int row){}
};
