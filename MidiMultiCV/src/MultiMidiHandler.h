#ifndef MULTIMIDIHANDLER_H_INCLUDE
#define MULTIMIDIHANDLER_H_INCLUDE

#include <mbed.h>
#include "MidiHandler.h"

class MultiMidiHandler : public MidiHandler
{
public:
  MultiMidiHandler(MidiHandler& handler1, MidiHandler& handler2, MidiHandler& handler3)
  : m_handler1(handler1)
  , m_handler2(handler2)
  , m_handler3(handler3)
  {}

  void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override
  {
    m_handler1.NoteOn(Channel, MidiNote, Velocity);
    m_handler2.NoteOn(Channel, MidiNote, Velocity);
    m_handler3.NoteOn(Channel, MidiNote, Velocity);
  }
 void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override
 {
   m_handler1.NoteOff(Channel, MidiNote, Velocity);
   m_handler2.NoteOff(Channel, MidiNote, Velocity);
   m_handler3.NoteOff(Channel, MidiNote, Velocity);
 }
 void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value)
 {
   m_handler1.ContinuousController(Channel, Controller, Value);
   m_handler2.ContinuousController(Channel, Controller, Value);
   m_handler3.ContinuousController(Channel, Controller, Value);
 }
 void PitchWheel(uint8_t Channel, int Bend)
 {
   m_handler1.PitchWheel(Channel, Bend);
   m_handler2.PitchWheel(Channel, Bend);
   m_handler3.PitchWheel(Channel, Bend);
 }
 void ActiveSense()
 {
   m_handler1.ActiveSense();
   m_handler2.ActiveSense();
   m_handler3.ActiveSense();
 }

private:
  MidiHandler& m_handler1;
  MidiHandler& m_handler2;
  MidiHandler& m_handler3;
};

#endif /* end of include guard: MULTIMIDIHANDLER_H_INCLUDE */
