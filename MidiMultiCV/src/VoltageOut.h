#ifndef VOLTAGE_OUT_H_INCLUDE
#define VOLTAGE_OUT_H_INCLUDE

#include <mbed.h>

//TODO VoltageOut interface, MCP4x22 implementation

class VoltageOut
{
public:
  virtual void write(float voltage){}
};

class PwmVoltageOut : public VoltageOut
{
public:
  PwmVoltageOut(PinName pin) : m_Out(pin)
  {
    m_Out.period_us(400);//2.5kHz
  }

  PwmVoltageOut(PinName pin, float referenceVoltage) : m_Out(pin), m_ReferenceVoltage(referenceVoltage)
  {
    m_Out.period_us(400);//2.5kHz
  }

  void write(float voltage) override
  {
    float value = voltage / m_ReferenceVoltage;
    if(value<0.0f)
    {
      value = 0.0f;
    }
    else if(1.0f<value)
    {
      value = 1.0f;
    }
    m_Out.write(value);
  }

private:
  PwmOut m_Out;
  float m_ReferenceVoltage{3.3f};
};

#endif //VOLTAGE_OUT_H_INCLUDE
