#include "mcp4728.h"
#include <Wire.h>

MCP4728Dac::MCP4728Dac()
    : m_Address(MCP4728_I2CADDR_DEFAULT) // default address
      ,
      m_Value()
{
    for (int ch = 0; ch < NumChannels; ++ch)
    {
        m_Value[ch] = 0;
    }
}

void MCP4728Dac::Begin(uint8_t address)
{
    m_Address = address;
    Wire.begin(); // call here??
}

void MCP4728Dac::ConfigureChannel(int channel,
                                  vref new_vref = MCP4728_VREF_VDD,
                                  gain new_gain = MCP4728_GAIN_1X,
                                  pd_mode new_pd_mode = MCP4728_PD_MODE_NORMAL)
{
    if (0 <= channel && channel < NumChannels)
    {
        // configure one channel with options
        uint8_t output_buffer[3];

        // build the setter header/ "address"
        // 0 1 0 0 0 DAC1 DAC0 UDAC[A]
        uint8_t sequential_write_cmd = MCP4728_MULTI_IR_CMD;
        sequential_write_cmd |= (channel << 1);
        // no udac support (yet)
        // sequential_write_cmd |= udac;
        output_buffer[0] = sequential_write_cmd;

        // VREF PD1 PD0 Gx D11 D10 D9 D8 [A] D7 D6 D5 D4 D3 D2 D1 D0 [A]
        uint16_t new_value = m_Value[channel];
        new_value |= (new_vref << 15);
        new_value |= (new_pd_mode << 13);
        new_value |= (new_gain << 12);

        output_buffer[1] = new_value >> 8;
        output_buffer[2] = new_value & 0xFF;

        Wire.beginTransmission(m_Address);
        Wire.write(output_buffer, 3);
        Wire.endTransmission(m_Address);
    }
}

void MCP4728Dac::SetValue(int channel, int value)
{
    if (0 <= channel && channel < NumChannels)
    {
        m_Value[channel] = value;
    }
}

void MCP4728Dac::Update()
{
    // writes/updates all channel values
    uint8_t output_buffer[8];

    output_buffer[0] = m_Value[0] >> 8;
    output_buffer[1] = m_Value[0] & 0xFF;

    output_buffer[2] = m_Value[1] >> 8;
    output_buffer[3] = m_Value[1] & 0xFF;

    output_buffer[4] = m_Value[2] >> 8;
    output_buffer[5] = m_Value[2] & 0xFF;

    output_buffer[6] = m_Value[3] >> 8;
    output_buffer[7] = m_Value[3] & 0xFF;

    Wire.beginTransmission(m_Address);
    Wire.write(output_buffer, 8);
    Wire.endTransmission(m_Address);
}
