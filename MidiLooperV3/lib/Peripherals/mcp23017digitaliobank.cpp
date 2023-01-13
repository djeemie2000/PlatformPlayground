#include "mcp23017digitaliobank.h"
#include "Wire.h"

/**
 * Registers addresses.
 * The library use addresses for IOCON.BANK = 0.
 * See "3.2.1 Byte mode and Sequential mode".
 */
enum class MCP23017Register : uint8_t
{
	IODIR_A		= 0x00, 		///< Controls the direction of the data I/O for port A.
	IODIR_B		= 0x01,			///< Controls the direction of the data I/O for port B.
	IPOL_A		= 0x02,			///< Configures the polarity on the corresponding GPIO_ port bits for port A.
	IPOL_B		= 0x03,			///< Configures the polarity on the corresponding GPIO_ port bits for port B.
	GPINTEN_A	= 0x04,			///< Controls the interrupt-on-change for each pin of port A.
	GPINTEN_B	= 0x05,			///< Controls the interrupt-on-change for each pin of port B.
	DEFVAL_A	= 0x06,			///< Controls the default comparaison value for interrupt-on-change for port A.
	DEFVAL_B	= 0x07,			///< Controls the default comparaison value for interrupt-on-change for port B.
	INTCON_A	= 0x08,			///< Controls how the associated pin value is compared for the interrupt-on-change for port A.
	INTCON_B	= 0x09,			///< Controls how the associated pin value is compared for the interrupt-on-change for port B.
	IOCON		= 0x0A,			///< Controls the device.
	GPPU_A		= 0x0C,			///< Controls the pull-up resistors for the port A pins.
	GPPU_B		= 0x0D,			///< Controls the pull-up resistors for the port B pins.
	INTF_A		= 0x0E,			///< Reflects the interrupt condition on the port A pins.
	INTF_B		= 0x0F,			///< Reflects the interrupt condition on the port B pins.
	INTCAP_A	= 0x10,			///< Captures the port A value at the time the interrupt occured.
	INTCAP_B	= 0x11,			///< Captures the port B value at the time the interrupt occured.
	GPIO_A		= 0x12,			///< Reflects the value on the port A.
	GPIO_B		= 0x13,			///< Reflects the value on the port B.
	OLAT_A		= 0x14,			///< Provides access to the port A output latches.
	OLAT_B		= 0x15,			///< Provides access to the port B output latches.
};

MCP23017DigitalIOBank::MCP23017DigitalIOBank()
{}

void MCP23017DigitalIOBank::Begin(uint8_t address)
{
    // here???
    Wire.begin();

    //TODO settings with defaults?
    m_Address = address;

    m_IoDirMask = 0;
    m_PullupMask= 0xFFFF;
    m_IoInvertMask = 0;

    	//BANK = 	0 : sequential register addresses
	//MIRROR = 	0 : use configureInterrupt 
	//SEQOP = 	1 : sequential operation disabled, address pointer does not increment
	//DISSLW = 	0 : slew rate enabled
	//HAEN = 	0 : hardware address pin is always enabled on 23017
	//ODR = 	0 : active driver output (INTPOL bit sets the polarity.)

	//INTPOL = 	0 : interrupt active low
	//UNIMPLMENTED 	0 : unimplemented: Read as ‘0’

    uint8_t cfg = 0b00100000;
	WriteRegister8(static_cast<uint8_t>(MCP23017Register::IOCON), cfg);

	//enable all pull up resistors (will be effective for input pins only)
	WriteRegister16(static_cast<uint8_t>(MCP23017Register::GPPU_A), m_PullupMask);

}
    // pinmode
void MCP23017DigitalIOBank::SetPinMode(int pin, int mode)
{
    // OUTPUT, INPUT, INPUT_PULLU 
    if(mode == OUTPUT)
    {
        bitClear(m_IoDirMask, pin);
        bitClear(m_PullupMask, pin);
        bitClear(m_IoInvertMask, pin);
    }
    else if (mode == INPUT)
    {
        bitSet(m_IoDirMask, pin);
        bitClear(m_PullupMask, pin);
        bitClear(m_IoInvertMask, pin);
    }
    else if(mode == INPUT_PULLUP)
    {
        bitSet(m_IoDirMask, pin);
        bitSet(m_PullupMask, pin);
        bitClear(m_IoInvertMask, pin);
    }
}

void MCP23017DigitalIOBank::WritePinModes()
{
    WriteRegister16(static_cast<uint8_t>(MCP23017Register::IODIR_A), m_IoDirMask);
	WriteRegister16(static_cast<uint8_t>(MCP23017Register::GPPU_A), m_PullupMask);
	WriteRegister16(static_cast<uint8_t>(MCP23017Register::IPOL_A), m_IoInvertMask);
}

void MCP23017DigitalIOBank::SetPin(int pin, int value)
{
    bitWrite(m_OutputValues, pin, value);
}

void MCP23017DigitalIOBank::WritePins()
{
    WriteRegister16(static_cast<uint8_t>(MCP23017Register::GPIO_A), m_OutputValues);
}

int MCP23017DigitalIOBank::GetPin(int pin) const
{
    return bitRead(m_InputValues, pin);
}

void MCP23017DigitalIOBank::ReadPins()
{
    ReadRegister16(static_cast<uint8_t>(MCP23017Register::GPIO_A), m_InputValues);
}

void MCP23017DigitalIOBank::WriteRegister8(uint8_t reg, uint8_t byte)
{
    Wire.beginTransmission(m_Address);
	Wire.write(reg);
	Wire.write(byte);
	Wire.endTransmission();
}

void MCP23017DigitalIOBank::WriteRegister16(uint8_t reg, uint16_t value)
{
    uint8_t byte1 = value>> 8;
    uint8_t byte2 = value & 0xFF;

    Wire.beginTransmission(m_Address);
	Wire.write(reg);
	Wire.write(byte1);
	Wire.write(byte2);
	Wire.endTransmission();
}

void MCP23017DigitalIOBank::ReadRegister16(uint8_t reg, uint16_t& value)
{
	Wire.beginTransmission(m_Address);
	Wire.write(reg);
	Wire.endTransmission();
	Wire.requestFrom(m_Address, (uint8_t)2);
	uint8_t byte1 = Wire.read();
	uint8_t byte2 = Wire.read();

    value = byte1 << 8 | byte2;
}