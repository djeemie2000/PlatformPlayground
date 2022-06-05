#pragma once
#include <Arduino.h>

class MCP4728Dac
{
public:
    static const int NumChannels = 4;

    static const uint8_t MCP4728_I2CADDR_DEFAULT = 0x60; ///< MCP4728 default i2c address

    static const uint8_t MCP4728_MULTI_IR_CMD = 0x40;     ///< Command to write to the input register only
    static const uint8_t MCP4728_MULTI_EEPROM_CMD = 0x50; ///< Command to write to the input register and EEPROM
    static const uint8_t MCP4728_FAST_WRITE_CMD = 0xC0;   ///< Command to write all channels at once with

    /**
     * @brief Power status values
     *
     * Allowed values for `setPowerMode`.
     */

    enum pd_mode
    {
        MCP4728_PD_MODE_NORMAL,   ///< Normal; the channel outputs the given value as
                                  ///< normal.
        MCP4728_PD_MODE_GND_1K,   ///< VOUT is loaded with 1 kΩ resistor to ground. Most
                                  ///< of the channel circuits are powered off.
        MCP4728_PD_MODE_GND_100K, ///< VOUT is loaded with 100 kΩ resistor to ground.
                                  ///< Most of the channel circuits are powered off.
        MCP4728_PD_MODE_GND_500K, ///< VOUT is loaded with 500 kΩ resistor to ground.
                                  ///< Most of the channel circuits are powered off.
    };

    /**
     * @brief Example enum values
     *
     * Allowed values for `setGain`.
     */
    enum gain
    {
        MCP4728_GAIN_1X,
        MCP4728_GAIN_2X,
    };

    /**
     * @brief Ex
     *
     * Allowed values for `setVref`.
     */
    enum vref
    {
        MCP4728_VREF_VDD,
        MCP4728_VREF_INTERNAL,
    };

    MCP4728Dac();//default address

    void Begin(uint8_t address);

    // configuration needs to be called at least once for each channel!
    void ConfigureChannel(int channel, 
                        vref new_vref = MCP4728_VREF_VDD,
                        gain new_gain = MCP4728_GAIN_1X,
                        pd_mode new_pd_mode = MCP4728_PD_MODE_NORMAL); // configure one channel with options

    void SetValue(int channel, int value); // 12 bits!
    void Update();                         // writes/updates all channel values

private:
    uint8_t m_Address;
    int m_Value[NumChannels];
};
