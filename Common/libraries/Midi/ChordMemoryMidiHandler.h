#pragma once

#include <mbed.h>
#include "MidiHandler.h"
#include "MidiNoteState.h"
#include "MidiNoteStack.h"

class ChordMemoryMidiHandler : public MidiHandler
{
public:
  ChordMemoryMidiHandler(MidiHandler& handler, uint8_t Channel);

  
  // voice commands
  void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity);
  void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity);
  void AfterTouch(uint8_t Channel, uint8_t MidiNote, uint8_t Pressure);
  void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value);
  void ProgramChange(uint8_t Channel, uint8_t Program);
  void ChannelPressure(uint8_t Channel, uint8_t Pressure);
  void PitchWheel(uint8_t Channel, int Bend);// centered = 0 ! range +/- 8192 ( 2^13 )

  // system common
  void QuarterFrameMessage(uint8_t TimeCode);
  void SongPositionPointer(int MidiBeat);
  void SongSelect(uint8_t SongNumber);
  void TuneRequest();

  // system realtime
  void MidiClock();
  void MidiStart();
  void MidiContinue();
  void MidiStop();
  void ActiveSense();
  void SystemReset();

  // system exclusive (sysex)

private:
  bool ChannelAllowed(uint8_t channel) const;
  void OutAllNotesOff();

    MidiHandler& m_Handler;
    uint8_t m_Channel;
    MidiNotePressedState m_NotePressed;
    MidiNoteStack m_OutStack;
};
