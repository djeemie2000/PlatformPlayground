#pragma once

#include <arduino.h>

// "Interface"
class DigitalOutMatrix
{
public:
    DigitalOutMatrix(){}

    virtual int NumRows() const{return 1;}
    virtual int NumCols() const{return 1;}
    virtual bool Set(int row, int col) {return true;}
    virtual bool Clear(int row, int col) {return true;}
    virtual int WriteAll(){return 0;}
    virtual int WriteRow(int row){return 0;}
    virtual int Write(){return 0;}
};
