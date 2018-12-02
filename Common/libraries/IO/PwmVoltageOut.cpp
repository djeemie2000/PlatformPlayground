#include "PwmVoltageOut.h"

  PwmVoltageOut::PwmVoltageOut(PinName pin) : m_Out(pin), m_ReferenceVoltage(3.3f)
  {
    m_Out.period_us(400);//2.5kHz
  }

  PwmVoltageOut::PwmVoltageOut(PinName pin, float referenceVoltage) : m_Out(pin), m_ReferenceVoltage(referenceVoltage)
  {
    m_Out.period_us(400);//2.5kHz
  }

  void PwmVoltageOut::WriteValue(float Value)
  {
    if(Value<0.0f)
    {
      Value = 0.0f;
    }
    else if(1.0f<Value)
    {
      Value = 1.0f;
    }
    m_Out.write(Value);

  }

  void PwmVoltageOut::WriteVoltage(float voltage)
  {
    WriteValue(voltage / m_ReferenceVoltage);
  }
