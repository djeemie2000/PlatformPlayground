#pragma once

#include <Arduino.h>

class MemoryBank
{
public:
    virtual ~MemoryBank(){}

    virtual int BankSize() const =0;
    virtual int NumBanks() const =0;
    virtual int WriteBank(int bank, const uint8_t* data, int size) =0;//returns actual # written
    virtual int ReadBank(int bank, uint8_t* data, int size) =0;//returns actual # read
    //TODO offset support in bank
};
