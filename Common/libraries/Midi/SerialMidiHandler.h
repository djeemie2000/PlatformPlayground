#ifndef SERIALMIDIHANDLER_H_INCLUDE
#define SERIALMIDIHANDLER_H_INCLUDE

#include <mbed.h>
#include "MidiHandler.h"

class SerialMidiHandler : public MidiHandler
{
public:
  SerialMidiHandler(Serial& pc);

  void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity); /*override*/
  void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity); /*override*/
  void AfterTouch(uint8_t Channel, uint8_t MidiNote, uint8_t Pressure);
  void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value);
  void PitchWheel(uint8_t Channel, int Bend);
  void ActiveSense();
 
private:
  Serial& m_pc;
};

#endif /* end of include guard: SERIALMIDIHANDLER_H_INCLUDE */
