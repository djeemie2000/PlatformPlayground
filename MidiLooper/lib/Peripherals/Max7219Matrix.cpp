#include "Max7219Matrix.h"
#include <SPI.h>

static const uint8_t NoOpCommand = 0x00;
static const uint8_t Digit0Command = 0x01;
static const uint8_t DecodeModeCommand = 0x09;
static const uint8_t IntensityCommand = 0x0A;
static const uint8_t ScanLimitCommand = 0x0B;
static const uint8_t ShutdownCommand = 0x0C;
static const uint8_t DisplayTestCommand = 0x0D;

Max7219Matrix::Max7219Matrix(int numDevices, uint8_t csPin)
 : m_NumDevices(numDevices)
 , m_CsPin(csPin)
{
    // init digit value for all rows
    for(int row = 0; row<Size;++row)
    {
        for(int device = 0; device<MaxNumDevices; ++device)
        {
            m_Bits[row][device] = 0x00;
        }
    }

    pinMode(m_CsPin, OUTPUT);
}

void Max7219Matrix::Configure()
{
    SPI.begin();//here??

    WriteCommand(ShutdownCommand,0);//off

    WriteCommand(DisplayTestCommand, 0x00);

    WriteCommand(ScanLimitCommand, 0x07);//scan limit 8 <=> all leds used
    WriteCommand(DecodeModeCommand, 0x00);//no decode mode
    WriteCommand(IntensityCommand, 0x01);//lowest intensity 

    WriteCommand(ShutdownCommand,1);//on
}

void Max7219Matrix::WriteCommand(uint8_t command, uint8_t value)
{
    SPI.beginTransaction(SPISettings(4000000ul, MSBFIRST, SPI_MODE0));
    digitalWrite(m_CsPin, LOW);
    delayMicroseconds(5);
    for(int device = 0; device<m_NumDevices; ++device)
    {
        SPI.transfer(command);
        SPI.transfer(value);
    }   
    digitalWrite(m_CsPin, HIGH);
    delayMicroseconds(5);
    SPI.endTransaction();
}

int Max7219Matrix::WriteRow(int row)
{
    if(0<=row && row<Size)
    {
                                         //8000000
        SPI.beginTransaction(SPISettings(4000000ul, MSBFIRST, SPI_MODE0));
        digitalWrite(m_CsPin, LOW);
        delayMicroseconds(5);
        for(int dev = m_NumDevices-1; 0<=dev; --dev)
        {
            SPI.transfer(Digit0Command+row);
            SPI.transfer(m_Bits[row][dev]);
        }
        digitalWrite(m_CsPin, HIGH);
        delayMicroseconds(5);
        SPI.endTransaction();
    }
    return 0;
}

int Max7219Matrix::WriteAll()
{
    for(int row=0; row<Size; ++row)
    {
        WriteRow(row);
    }
    return 0;
}

int Max7219Matrix::Write()
{
    //TODO write per row (internal counter)
    return WriteAll();
}

void Max7219Matrix::SetBits(int device, int row, int col)
{
    //no check on device,row, col
    bitSet(m_Bits[row][device],col);
}

void Max7219Matrix::ClearBits(int device, int row, int col)
{
    //no check on device,row, col
    bitClear(m_Bits[row][device],col);
}

bool Max7219Matrix::Set(int row, int col)
{
    //TODO check row [0,Size[, check col [0, Size*MaxNumDevices]
    int device = col/Size;
    if(0<=device && device<m_NumDevices)
    {
        int r = row;//Size-1-row;
        int c = Size-1-col;//col-device*Size;
        SetBits(device,r,c);//not rotated
        return true;
    }
    return false;
}

bool Max7219Matrix::Clear(int row, int col)
{
    int device = col/Size;
    if(0<=device && device<m_NumDevices)
    {
       
        int r = row;//Size-1-row;
        int c = Size-1-col;//col-device*Size;
        ClearBits(device,r,c);//not rotated
        return true;
    }
    return false;
}
