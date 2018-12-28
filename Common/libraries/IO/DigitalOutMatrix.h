#pragma once

#include <mbed.h>

// "Interface"
class DigitalOutMatrix
{
public:
    DigitalOutMatrix(){}

    virtual int NumRows() const{return 1;}
    virtual int NumCols() const{return 1;}
    virtual bool Set(int row, int col) {return true;}
    virtual bool Clear(int row, int col) {return true;}
    virtual int Write(){return 0;}
    virtual int Write(int row){return 0;}
};
