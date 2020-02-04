#include "TTP8229TouchPad.h"

TTP8229InBank::TTP8229InBank(I2C* i2c)
    : m_i2c(i2c)
    , m_TouchState(0)
{
    wait_us(500000); 
    // Allow 500ms for the 8229BSF to get ready after turn-on
}

void TTP8229InBank::Read()
{
    m_TouchState = 0;

    char data[2];
    m_i2c->read(0x57, data, 2);
    
    m_TouchState = (data[0] << 8) | data[1];
    //?? order of bytes?
}

int TTP8229InBank::Size() const
{
    return 16;
}

int TTP8229InBank::Get(int Pad) const
{
    Pad = 15-Pad;
    if(0<=Pad && Pad<Size())
    {
        return m_TouchState & (1<<Pad);
    }
    return 0;
}
