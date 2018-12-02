#pragma once

#include <mbed.h>

// "Interface"
class DigitalInBank
{
public:
    DigitalInBank(){}

    virtual int Size() const{return 1;}
    virtual int Get(int idx) const{return 0;}
    virtual void Read(){}
};
