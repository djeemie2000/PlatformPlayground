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
    m_pc.printf("\r\nNoteOn 0x%2x 0x%2x 0x%2x\r\n", Channel, MidiNote, Velocity);
  }
 void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override
 {
   m_pc.printf("\r\nNoteOff 0x%2x 0x%2x 0x%2x\r\n", Channel, MidiNote, Velocity);
 }
 void AfterTouch(uint8_t Channel, uint8_t MidiNote, uint8_t Pressure)
 {
   m_pc.printf("\r\AfterTouch 0x%2x 0x%2x 0x%2x\r\n", Channel, MidiNote, Pressure);
 }
 void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value)
 {
   m_pc.printf("\r\nContinuousController 0x%2x 0x%2x 0x%2x\r\n", Channel, Controller, Value);
 }
 void PitchWheel(uint8_t Channel, int Bend)
 {
   m_pc.printf("\r\nPitchWheel 0x%2x %d\r\n", Channel, Bend);
 }
 void ActiveSense()
 {
   //m_pc.printf("\r\nActiveSense\r\n");
 }

private:
  Serial& m_pc;
};

#endif /* end of include guard: LOGMIDIHANDLER_H_INCLUDE */
