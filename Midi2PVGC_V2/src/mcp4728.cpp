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
        m_Configured[ch] = false;
        m_Gain[ch] = MCP4728Dac::gain::MCP4728_GAIN_1X;
        m_PdMode[ch] = MCP4728Dac::pd_mode::MCP4728_PD_MODE_NORMAL;
        m_Vref[ch] = MCP4728Dac::vref::MCP4728_VREF_INTERNAL;
    }
}

void MCP4728Dac::Begin(uint8_t address)
{
    m_Address = address;
    Wire.begin(); // call here??
}

bool MCP4728Dac::CheckConfigured(int channel, 
                        vref new_vref,
                        gain new_gain,
                        pd_mode new_pd_mode)
{
    if(!m_Configured[channel]
    || m_Vref[channel] != new_vref
    || m_Gain[channel] != new_gain
    || m_PdMode[channel] != new_pd_mode)
    {
        m_Vref[channel] = new_vref;
        m_Gain[channel] = new_gain;
        m_PdMode[channel] = new_pd_mode;
        m_Configured[channel] = true;
        return false;
    }
    return true;
}

void MCP4728Dac::ConfigureChannel(int channel,
                                  vref new_vref,
                                  gain new_gain,
                                  pd_mode new_pd_mode)
{
    if (0 <= channel && channel < NumChannels 
    && !CheckConfigured(channel, new_vref, new_gain, new_pd_mode))
    {
        // configure one channel with options
        uint8_t output_buffer[3];

        // build the setter header/ "address"
        // 0 1 0 0 0 DAC1 DAC0 UDAC[A]
        uint8_t sequential_write_cmd = MCP4728_MULTI_IR_CMD;
        sequential_write_cmd |= (channel << 1);
        // no udac support (yet)
        bool udac = false;
        sequential_write_cmd |= udac;
        output_buffer[0] = sequential_write_cmd;

        // VREF PD1 PD0 Gx D11 D10 D9 D8 [A] D7 D6 D5 D4 D3 D2 D1 D0 [A]
        uint16_t new_value = m_Value[channel];
        new_value |= (new_vref << 15);
        new_value |= (new_pd_mode << 13);
        new_value |= (new_gain << 12);

        output_buffer[1] = new_value >> 8;
        output_buffer[2] = new_value & 0xFF;


        WriteBuffer(output_buffer, 3);
    }
}

void MCP4728Dac::SetValue(int channel, uint16_t value)
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

    WriteBuffer(output_buffer, 8);
}

void MCP4728Dac::WriteBuffer(const uint8_t* output_buffer, int size)
{
    // for(int idx = 0; idx<size; ++idx)
    // {
    //     Serial.print(output_buffer[idx], HEX);
    //     Serial.write(' ');
    // }

    Wire.beginTransmission(m_Address);
    Wire.write(output_buffer, size);
    // uint8_t tmp = 
    Wire.endTransmission(m_Address);

    //Serial.println(tmp, HEX);
}

void AllOff(MCP4728Dac& bank)
{
    for(int channel = 0;channel<MCP4728Dac::NumChannels; ++ channel)
    {
        bank.SetValue(channel, 0);
    }
    bank.Update();
}

void TestMCP4728(MCP4728Dac& bank, int repeats, bool internal)
{
    if(internal)
    {
        Serial.println("Testing mcp4728 bank internal VREF...");
    }
    else
    {
        Serial.println("Testing mcp4728 bank external VREF...");
    }

    for(int repeat = 0; repeat<repeats; ++repeat)
    {
        for(int channel = 0;channel<MCP4728Dac::NumChannels; ++ channel)
        {
            if(internal)
            {
                bank.ConfigureChannel(channel, MCP4728Dac::MCP4728_VREF_INTERNAL, MCP4728Dac::MCP4728_GAIN_2X, MCP4728Dac::MCP4728_PD_MODE_NORMAL);
            }
            else
            {
                bank.ConfigureChannel(channel, MCP4728Dac::MCP4728_VREF_VDD, MCP4728Dac::MCP4728_GAIN_1X, MCP4728Dac::MCP4728_PD_MODE_NORMAL);
            }

            for(int value = 0; value < 4096; value += 256)
            {
                Serial.print(channel);
                Serial.print(" ");
                Serial.println(value);

                bank.SetValue(channel, value);

                bank.Update();

                delay(250);
            }

            bank.SetValue(channel, 2048);
        }
    }
}