#pragma once
#include <Arduino.h>

class SDStorage
{
public:
    SDStorage();

    bool Open(int CSPin);
    bool IsOpen() const;

    void PrintState(HardwareSerial& serial);
    void PrintListDir(const char* path, HardwareSerial& serial);

    bool Write(const char* path, uint8_t value);
    bool Write(const char* path, uint8_t* values, int size);
    //TODO read
    

private:
    bool m_IsOpen;
};

