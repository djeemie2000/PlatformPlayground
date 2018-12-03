#include "Mpr121InBank.h"

Mpr121InBank::Mpr121InBank(I2C* i2c, PinName irqPin)
 : m_Touchpad(i2c, Mpr121::ADD_VSS)//assume sparkfun board!
 , m_In(irqPin, PullUp)
 , m_TouchState(0)
{
    m_Touchpad.setProximityMode(false);
}

int Mpr121InBank::Size() const
{
    return 12;
}

int Mpr121InBank::Get(int idx) const
{
    // no check on idx in [0,Size[
    return (m_TouchState>>idx)&1;
}

void Mpr121InBank::Read()
{
    m_State.Tick(m_In);
    if(m_State.IsFalling())
    {
        m_TouchState = m_Touchpad.read(0x00);
        m_TouchState += m_Touchpad.read(0x01)<<8;
    }
}
