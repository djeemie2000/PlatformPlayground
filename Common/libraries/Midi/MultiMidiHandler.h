#ifndef MULTIMIDIHANDLER_H_INCLUDE
#define MULTIMIDIHANDLER_H_INCLUDE

#include <mbed.h>
#include "MidiHandler.h"

class MultiMidiHandler : public MidiHandler
{
public:
  MultiMidiHandler(MidiHandler& handler1, MidiHandler& handler2, MidiHandler& handler3, MidiHandler& handler4) 
  : m_Size(4)
  {
    m_Handler[0] = &handler1;
    m_Handler[1] = &handler2;
    m_Handler[2] = &handler3;
    m_Handler[3] = &handler4;
  }

  MultiMidiHandler() 
  : m_Size(0)
  {
  }

  void AddHandler(MidiHandler* handler)
  {
    m_Handler[m_Size] = handler;
    m_Size++;
  }

  void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) /*override*/
  {
   for(int idx = 0; idx<m_Size; ++idx)
   {
     m_Handler[idx]->NoteOn(Channel, MidiNote, Velocity);
   }
  }
 void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) /*override*/
 {
   for(int idx = 0; idx<m_Size; ++idx)
   {
     m_Handler[idx]->NoteOff(Channel, MidiNote, Velocity);
   }
 }
 void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value)
 {
   for(int idx = 0; idx<m_Size; ++idx)
   {
     m_Handler[idx]->ContinuousController(Channel, Controller, Value);
   }
 }
 void PitchWheel(uint8_t Channel, int Bend)
 {
   for(int idx = 0; idx<m_Size; ++idx)
   {
     m_Handler[idx]->PitchWheel(Channel, Bend);
   }
 }

 void ActiveSense()
 {
   for(int idx = 0; idx<m_Size; ++idx)
   {
     m_Handler[idx]->ActiveSense();
   }
 }

private:
  static const int Capacity = 8;
  int m_Size;
  MidiHandler* m_Handler[Capacity];
};

#endif /* end of include guard: MULTIMIDIHANDLER_H_INCLUDE */
