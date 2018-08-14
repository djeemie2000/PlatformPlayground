#ifndef MCP4X22_H_INCLUDE
#define MCP4X22_H_INCLUDE

#include <mbed.h>

// for now: single chip, fixed SPI port, ...
class MCP4x22
{
public:
  static const int Gain_x1 = 1<<13;
  static const int Gain_x2 = 0;
  static const int Active = 1<<12;
  static const int Channel_A = 0;
  static const int Channel_B = 1<<15;

  MCP4x22();

  void SetOutput(int Value, int Channel, int Gain);//TODO multiple chips

private:
  SPI m_SPI;
};

#endif /* end of include guard: MCP4X22_H_INCLUDE */
