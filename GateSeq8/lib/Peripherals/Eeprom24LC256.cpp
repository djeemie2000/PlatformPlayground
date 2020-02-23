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

    Wire.write(buffer, 2);
}

int Eeprom24LC256::WriteBank(int bank, const uint8_t* data, int size, int offset)
{
    if(0<=bank && bank<NumBanks() && 0<=offset && offset<BankSize())
    {
        ack_pooling();

        // no check on bank
        int eepromAddress = bank*BankSize()+offset;
        // write data, but crop write size to bank
        int croppedSize = min(size, BankSize()-offset);

        Wire.beginTransmission(m_Address);
        WriteEepromAdress(eepromAddress);
        int numWritten = Wire.write(data,croppedSize);
        Wire.endTransmission();
        //returns actual # written
        return numWritten;
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
        // read data, but crop read size to bank
        int croppedSize = min(size, BankSize()-offset);

        Wire.beginTransmission(m_Address);
        WriteEepromAdress(eepromAddress);
        Wire.endTransmission();//??

        Wire.requestFrom(m_Address,croppedSize);
        return Wire.readBytes(data, croppedSize);
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