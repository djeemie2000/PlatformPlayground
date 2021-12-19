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
    bool Read(const char* path, uint8_t& value);
    bool Read(const char* path, uint8_t* values, int capacity, int& size);
    bool Remove(const char* path);//only files!
    

private:
    bool m_IsOpen;
};

