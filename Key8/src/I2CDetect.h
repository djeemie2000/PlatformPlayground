#pragma once

#include <Arduino.h>
#include <Wire.h>

void FindI2CDevices()
{
    Serial.println("Finding I2C devices...");
    for(uint8_t Address = 0; Address<127; ++Address)
    {
        Wire.beginTransmission(Address);
        uint8_t Error = Wire.endTransmission(Address);
        if(Error==0)
        {
            // found I2C device!
            Serial.print("Found I2C device at 0x");
            Serial.print(Address, HEX);
            Serial.println();
        }
    }
    Serial.println("Done!");
}
