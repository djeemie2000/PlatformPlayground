#ifndef VOLTAGE_OUT_H_INCLUDE
#define VOLTAGE_OUT_H_INCLUDE

//TODO MCP4x22 implementation

class VoltageOut
{
public:
//  virtual ~VoltageOut() =default;
  virtual void WriteVoltage(float voltage){}
  virtual void WriteValue(float value){}
};

#endif //VOLTAGE_OUT_H_INCLUDE
