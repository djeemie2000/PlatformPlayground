#pragma once

#include <mbed.h>

// "Interface"
class DigitalOutBank
{
public:
    virtual ~DigitalOutBank(){}

    virtual int Size() const{return 1;}
    virtual void Set(int idx) {}
    virtual void Clear(int idx) {}
    virtual void Write(){}
};
