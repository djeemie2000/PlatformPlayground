#pragma once

#include <Arduino.h>

class MidiHandler
{
public:
  virtual ~MidiHandler() {} // =default;

  // voice commands
  virtual void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) {}
  virtual void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) {}
  virtual void AfterTouch(uint8_t Channel, uint8_t MidiNote, uint8_t Pressure) {}
  virtual void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value) {}
  virtual void ProgramChange(uint8_t Channel, uint8_t Program) {}
  virtual void ChannelPressure(uint8_t Channel, uint8_t Pressure) {}
  virtual void PitchWheel(uint8_t Channel, int Bend) {} // centered = 0 ! range +/- 8192 ( 2^13 )

  // system common
  virtual void QuarterFrameMessage(uint8_t TimeCode) {}
  virtual void SongPositionPointer(int MidiBeat) {}
  virtual void SongSelect(uint8_t SongNumber) {}
  virtual void TuneRequest() {}

  // system realtime -> simplified
  virtual void SystemRealtime(uint8_t byte) {}
  // virtual void MidiClock() {}
  // virtual void MidiStart() {}
  // virtual void MidiContinue() {}
  // virtual void MidiStop() {}
  // virtual void ActiveSense() {}
  // virtual void SystemReset() {}

  // system exclusive (sysex)
};
