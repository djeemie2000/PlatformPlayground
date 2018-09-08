#ifndef LOGMIDIHANDLER_H_INCLUDE
#define LOGMIDIHANDLER_H_INCLUDE

#include <mbed.h>
#include "MidiHandler.h"

class LogMidiHandler : public MidiHandler
{
public:
  LogMidiHandler(Serial& pc, int Id) : m_pc(pc), m_Id(Id)
  {}

  void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override
  {
    m_pc.printf("\r\n%d : Ch 0x%2x : On    0x%2x 0x%2x\r\n", m_Id, Channel, MidiNote, Velocity);
  }
 void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override
 {
   m_pc.printf("\r\n%d : Ch 0x%2x : Off   0x%2x 0x%2x\r\n", m_Id, Channel, MidiNote, Velocity);
 }
 void AfterTouch(uint8_t Channel, uint8_t MidiNote, uint8_t Pressure)
 {
   m_pc.printf("\r\n%d : Ch 0x%2x : After 0x%2x 0x%2x\r\n", m_Id, Channel, MidiNote, Pressure);
 }
 void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value)
 {
   m_pc.printf("\r\n%d : Ch 0x%2x : CC    0x%2x 0x%2x\r\n", m_Id, Channel, Controller, Value);
 }
 void PitchWheel(uint8_t Channel, int Bend)
 {
   m_pc.printf("\r\n%d : Ch 0x%2x : Pitch %d\r\n", m_Id, Channel, Bend);
 }
 void ActiveSense()
 {
   //m_pc.printf("\r\n%d : RT : ActiveSense\r\n", m_Id);
 }

private:
  Serial& m_pc;
  int m_Id;
};

#endif /* end of include guard: LOGMIDIHANDLER_H_INCLUDE */
