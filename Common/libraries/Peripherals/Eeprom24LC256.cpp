#include "Eeprom24LC256.h"

Eeprom24LC256::Eeprom24LC256(I2C* i2c, Address address)
 : m_I2c(i2c)
 , m_Address(address)
{}

void Eeprom24LC256::WriteAdress(int eepromAddress)
{
    //write address
    uint8_t buffer[2];
    buffer[0] = (eepromAddress >> 8); // MSB
    buffer[1] = (eepromAddress & 0xFF); // LSB
    m_I2c->write(m_Address, (char*)buffer, 2, false);
}

int Eeprom24LC256::writeBank(int bank, const uint8_t* data, int size)
{
    // no check on bank

    int eepromAddress = bank<<6;//*64
    WriteAdress(eepromAddress);

    // write data, but crop write size to bank
    int croppedSize = size<BankSize()?size:BankSize();
    m_I2c->write(m_Address, (char*)data, croppedSize);
    
    //returns actual # written
    return croppedSize;

    //TODO prevent write for approx 5 milliseconds after a write
    // but without blocking the whole application
    // wait_us(6000);
}

int Eeprom24LC256::readBank(int bank, uint8_t* data, int size)
{
    int eepromAddress = bank<<6;//*64
    WriteAdress(eepromAddress);

    // read data, but crop write size to bank
    int croppedSize = size<BankSize()?size:BankSize();
    m_I2c->read(m_Address, (char*)data, croppedSize);
    
    //returns actual # read
    return croppedSize;
}
