#include "Mpr121InBank.h"

Mpr121InBank::Mpr121InBank(PinName sda, PinName sdc)
 : m_I2C(sda, sdc)
 , m_Touchpad(&m_I2C, Mpr121::ADD_VSS)//assume sparkfun board!
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
    m_TouchState = m_Touchpad.readTouchData();
}
