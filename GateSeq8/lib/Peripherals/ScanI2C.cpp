#include "ScanI2C.h"
#include <Arduino.h>
#include <Wire.h>
#include "SerialOut.h"


void ScanI2C(SerialOut& debugSerial)
{
    Wire.begin();

    debugSerial.println("Finding I2C devices...");
    for(uint8_t address = 0; address<127; ++address)
    {
        Wire.beginTransmission(address);
        uint8_t Error = Wire.endTransmission(address);
        if(Error==0)
        {
            // found I2C device!
            debugSerial.printf("Found I2C device at 0x%x\r\n", address);
        }
    }
    debugSerial.println("Done!");
}