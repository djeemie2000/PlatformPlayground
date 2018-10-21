#ifndef GATEMIDIHANDLER_H_INCLUDE
#define GATEMIDIHANDLER_H_INCLUDE

#include <mbed.h>
#include "MidiHandler.h"
#include "MidiNoteState.h"

class GateMidiHandler : public MidiHandler
{
public:
  GateMidiHandler(DigitalOut& gateOut);

  void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override;
  void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override;

private:
  DigitalOut& m_GateOut;
  //MidiNoteState m_State;
  int m_NoteOnCount{0};
};

#endif
