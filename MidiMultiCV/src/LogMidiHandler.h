#ifndef LOGMIDIHANDLER_H_INCLUDE
#define LOGMIDIHANDLER_H_INCLUDE

#include <mbed.h>
#include "MidiHandler.h"

class LogMidiHandler : public MidiHandler
{
public:
  LogMidiHandler(Serial& pc) : m_pc(pc)
  {}

  void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override
  {
    m_pc.printf("NoteOn 0x%2x 0x%2x 0x%2x\r\n", Channel, MidiNote, Velocity);
  }
 void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override
 {
   m_pc.printf("NoteOff 0x%2x 0x%2x 0x%2x\r\n", Channel, MidiNote, Velocity);
 }
private:
  Serial& m_pc;
};

#endif /* end of include guard: LOGMIDIHANDLER_H_INCLUDE */
