#include "ScanI2C.h"
#include <Arduino.h>
#include <Wire.h>

void ScanI2C(HardwareSerial &debugSerial)
{
    Wire.begin();

    debugSerial.println("Finding I2C devices...");
    for (uint8_t address = 0; address < 127; ++address)
    {
        Wire.beginTransmission(address);
        uint8_t Error = Wire.endTransmission(address);
        if (Error == 0)
        {
            // found I2C device!
            debugSerial.print("Found I2C device at 0x");
            debugSerial.println(address, HEX);
        }
    }
    debugSerial.println("Done!");
}
