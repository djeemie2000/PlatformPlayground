#include "Eeprom24LC256.h"
#include <Wire.h>
//#include "Wait.h"

Eeprom24LC256::Eeprom24LC256(Address address)
 : m_Address(address)
{}

void Eeprom24LC256::WriteEepromAdress(int eepromAddress)
{
    //write address
    uint8_t buffer[2];
    buffer[0] = (eepromAddress >> 8); // MSB
    buffer[1] = (eepromAddress & 0xFF); // LSB

//    m_I2c->write(m_Address, (char*)buffer, 2, false);
//    Wire.beginTransmission(m_Address);
    Wire.write(buffer,2);
//    Wire.endTransmission();// continue transmission in write or read
}

int Eeprom24LC256::WriteBank(int bank, const uint8_t* data, int size)
{
    // no check on bank
    int eepromAddress = bank<<6;//*64

    // write data, but crop write size to bank
    int croppedSize = size<BankSize()?size:BankSize();

    Wire.beginTransmission(m_Address);
    WriteEepromAdress(eepromAddress);
    Wire.write(data,croppedSize);
    Wire.endTransmission();
  
    //TODO prevent write for approx 5 milliseconds after a write
    // but without blocking the whole application
    delay(6);

    //returns actual # written
    return croppedSize;
}

int Eeprom24LC256::ReadBank(int bank, uint8_t* data, int size)
{
    // no check on bank
    int eepromAddress = bank<<6;//*64

    Wire.beginTransmission(m_Address);
    WriteEepromAdress(eepromAddress);
    Wire.endTransmission();//??

    // read data, but crop write size to bank
    int croppedSize = size<BankSize()?size:BankSize();
    Wire.requestFrom(m_Address,croppedSize);
   return Wire.readBytes(data, croppedSize);

//    while(Wire.available())
  //  {
      // read one byte, advance readTo pointer
    //}
    
    //returns actual # read
    //return croppedSize;
}
