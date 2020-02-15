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
    // init digit command, digit value for all rows
    for(int row = 0; row<Size;++row)
    {
        for(int device = 0; device<MaxNumDevices; ++device)
        {
            m_Bits[row][2*device] = Digit0Command+row;
            m_Bits[row][2*device+1] = 0x00;
        }
    }

    pinMode(m_CsPin, OUTPUT);
}

void Max7219Matrix::Configure(bool rotated)
{
    m_Rotated = rotated;

    WriteCommand(ShutdownCommand,0);//off

    WriteCommand(ScanLimitCommand, 0x07);//scan limit 8 <=> all leds used
    WriteCommand(DecodeModeCommand, 0x00);//no decode mode
    WriteCommand(IntensityCommand, 0x01);//lowest intensity 

    WriteCommand(ShutdownCommand,1);//off
}

void Max7219Matrix::WriteCommand(uint8_t command, uint8_t value)
{
    uint8_t buffer[2*m_NumDevices];
    for(int device = 0; device<m_NumDevices; ++device)
    {
        buffer[2*device] = command;
        buffer[2*device+1] = value;
    }

    SPI.beginTransaction(SPISettings(8000000ul, MSBFIRST, SPI_MODE0));
    digitalWrite(m_CsPin, LOW);
    SPI.transfer(buffer, 2*m_NumDevices);
    digitalWrite(m_CsPin, HIGH);
    SPI.endTransaction();
}

int Max7219Matrix::WriteRow(int row)
{
    if(0<=row && row<Size)
    {
        SPI.beginTransaction(SPISettings(8000000ul, MSBFIRST, SPI_MODE0));
        digitalWrite(m_CsPin, LOW);
        SPI.transfer(m_Bits[row], 2*m_NumDevices);
        digitalWrite(m_CsPin, HIGH);
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
    bitSet(m_Bits[row][2*device+1],col);
}

void Max7219Matrix::ClearBits(int device, int row, int col)
{
    //no check on device,row, col
    bitClear(m_Bits[row][2*device+1],col);
}

bool Max7219Matrix::Set(int row, int col)
{
    //TODO check row [8,Size[, check col [0, Size*MaxNumDevices]
    int device = col/Size;
    if(0<=device && device<m_NumDevices)
    {
        if(m_Rotated)
        {
            int r = row;
            int c = col-device*Size;
            SetBits(device,c,r);//rotated
        }
        else
        {
            int r = Size-1-row;
            int c = col-device*Size;
            SetBits(device,r,c);//not rotated
        }
        return true;
    }
    return false;
}

bool Max7219Matrix::Clear(int row, int col)
{
    int device = col/Size;
    if(0<=device && device<m_NumDevices)
    {
        if(m_Rotated)
        {
            int r = row;
            int c = col-device*Size;
            ClearBits(device,c,r);//rotated
        }
        else
        {
            int r = Size-1-row;
            int c = col-device*Size;
            ClearBits(device,r,c);//not rotated
        }
        return true;
    }
    return false;
}
