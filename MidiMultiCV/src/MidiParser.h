#ifndef MIDIPARSER_H
#define MIDIPARSER_H

#include <mbed.h>
#include "MidiHandler.h"

class MidiParser
{
public:
  MidiParser();

  bool Parse(uint8_t byte, MidiHandler& handler);

private:
  bool IsCommand(uint8_t byte) const;
  bool IsSystemMessage(uint8_t byte) const;
  bool IsVoiceMessage(uint8_t byte) const;
  int NumBytes(uint8_t commandByte) const;
  void AddParam1(uint8_t byte);
  void AddParam2(uint8_t byte);
  bool HandleSystemRealtime(uint8_t byte, MidiHandler& handler);
  bool HandleBuffer(MidiHandler& handler) const;

  uint8_t m_Command{0x00};
  uint8_t m_Param1{0x00};
  uint8_t m_Param2{0x00};
  int m_Cntr{0};
};

#endif /* end of include guard: MIDIPARSER_H */
