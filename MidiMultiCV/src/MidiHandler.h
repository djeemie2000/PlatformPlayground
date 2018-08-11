#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <mbed.h>

class MidiHandler
{
public:
  virtual ~MidiHandler() =default;

  // voice commands
  virtual void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) =0;
  virtual void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) =0;
  virtual void AfterTouch(uint8_t Channel, uint8_t MidiNote, uint8_t Pressure) =0;
  virtual void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value) =0;
  virtual void ProgramChange(uint8_t Channel, uint8_t Program) =0;
  virtual void ChannelPressure(uint8_t Channel, uint8_t Pressure) =0;
  virtual void PitchWheel(uint8_t Channel, int Bend) =0;// centered = 0 ! range +/- 8192 ( 2^13 )

  // system common
  virtual void QuarterFrameMessage(uint8_t TimeCode) =0;
  virtual void SongPositionPointer(int MidiBeat) =0;
  virtual void SongSelect(uint8_t SongNumber) =0;
  virtual void TuneRequest() =0;

  // system realtime
  virtual void MidiClock() =0;
  virtual void MidiStart() =0;
  virtual void MidiContinue() =0;
  virtual void MidiStop() =0;
  virtual void ActiveSense() =0;
  virtual void SystemReset() =0;

  // system exclusive (sysex)



private:
};

#endif /* end of include guard: MIDIHANDLER_H */
