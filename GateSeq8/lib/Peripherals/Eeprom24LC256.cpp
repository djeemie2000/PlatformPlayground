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
    Wire.write(buffer[0]);
    Wire.write(buffer[1]);    
//    Wire.endTransmission();// continue transmission in write or read
}

int Eeprom24LC256::WriteBank(int bank, const uint8_t* data, int size, int offset)
{
    if(0<=bank && bank<NumBanks() && 0<=offset && offset<BankSize())
    {
        ack_pooling();

        // no check on bank
        int eepromAddress = bank*BankSize()+offset;

        // write data, but crop write size to bank
        int croppedSize = min(size, BankSize()-offset);//size<BankSize()?size:BankSize();

        Wire.beginTransmission(m_Address);
        WriteEepromAdress(eepromAddress);
        //Wire.write(data,croppedSize);
        const uint8_t* d = data;
        while(d<data+croppedSize)
        {
            Wire.write(*d);
            ++d;
        }
        Wire.endTransmission();
    
        //TODO prevent write for approx 5 milliseconds after a write
        // but without blocking the whole application
//        delay(20);

        //returns actual # written
        return croppedSize;
    }
    return 0;
}

int Eeprom24LC256::ReadBank(int bank, uint8_t* data, int size, int offset)
{
    if(0<=bank && bank<NumBanks() && 0<=offset && offset<BankSize())
    {
        ack_pooling();

        // no check on bank
        int eepromAddress = bank*BankSize()+offset;

        Wire.beginTransmission(m_Address);
        WriteEepromAdress(eepromAddress);
        Wire.endTransmission();//??

        // read data, but crop write size to bank
        int croppedSize = min(size, BankSize()-offset);//size<BankSize()?size:BankSize();
        Wire.requestFrom(m_Address,croppedSize);
        //return Wire.readBytes(data, croppedSize);

        int numRead = 0;
        uint8_t* d = data;
        while(Wire.available())
        {
        // read one byte, advance readTo pointer
        *d = Wire.read();
        ++d;
        ++numRead;
        }
        return numRead;
        
        //returns actual # read
        //return croppedSize;
    }
    return 0;
}

/*
	Acknowledge Pooling Algoritihm recomended by Manufacturer	
*/

void Eeprom24LC256::ack_pooling()
{
	uint8_t code = -1;
	do
	{
        const uint8_t value = 0;
		Wire.beginTransmission(m_Address);
		Wire.write(value);
		code = Wire.endTransmission();
	}
	while(code != 0);
}