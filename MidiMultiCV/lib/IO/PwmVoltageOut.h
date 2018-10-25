#ifndef PMWVOLTAGE_OUT_H_INCLUDE
#define PWMVOLTAGE_OUT_H_INCLUDE

#include <mbed.h>
#include <VoltageOut.h>

class PwmVoltageOut : public VoltageOut
{
public:
  PwmVoltageOut(PinName pin);
  PwmVoltageOut(PinName pin, float referenceVoltage);

  void WriteValue(float Value) /*override*/;
  void WriteVoltage(float voltage) /*override*/;

private:
  PwmOut m_Out;
  float m_ReferenceVoltage;
};

#endif
