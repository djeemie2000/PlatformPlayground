#pragma once
#include <Arduino.h>

class MCP23017DigitalIOBank
{
public :
    MCP23017DigitalIOBank();

    void Begin(uint8_t address);//TODO settings with defaults?

    // pinmode
    void SetPinMode(int pin, int mode);// OUTPUT, INPUT, INPUT_PULLUP
    void WritePinModes();

    // digital out
    void SetPin(int pin, int value);
    void WritePins();

    // digital in
    int GetPin(int pin) const;
    void ReadPins();

private:
    void WriteRegister8(uint8_t reg, uint8_t byte);
    void WriteRegister16(uint8_t reg, uint16_t value);
    void ReadRegister16(uint8_t reg, uint16_t& value);

    // direction??
    uint8_t m_Address;

    uint16_t m_IoDirMask;
    uint16_t m_PullupMask;
    uint16_t m_IoInvertMask;

    uint16_t m_OutputValues;
    uint16_t m_InputValues;
};
