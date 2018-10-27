#ifndef MODEMIDIHANDLER_H_INCLUDE
#define MODEMIDIHANDLER_H_INCLUDE

#include <mbed.h>
#include "MidiHandler.h"
#include "MidiNoteState.h"
#include "MidiNoteStepper.h"

class ModeMidiHandler : public MidiHandler
{
public:
    enum Mode
    {
        LivePoly,
        StepperPlay,
        StepperRecord
    };

  ModeMidiHandler(uint8_t Channel, MidiHandler& midiOut);

  void Tick(int Gate);
  void SetMode(Mode mode);

  void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) /*override*/;
  void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) /*override*/;
  void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value) /*override*/;
  void PitchWheel(uint8_t Channel, int Bend) /*override*/;
    void AfterTouch(uint8_t Channel, uint8_t MidiNote, uint8_t Pressure)
    {
        m_midiOut.AfterTouch(Channel, MidiNote, Pressure);
    }
    void ProgramChange(uint8_t Channel, uint8_t Program) 
    {
        m_midiOut.ProgramChange(Channel, Program);
    }
    void ChannelPressure(uint8_t Channel, uint8_t Pressure) 
    {
        m_midiOut.ChannelPressure(Channel, Pressure);
    }
    //TODO pass all other functions to handler!
    void AllNotesOff();

private:
  uint8_t m_Channel;
  MidiHandler& m_midiOut;
  Mode m_Mode;
  bool m_PrevGate;
  MidiNoteState m_State;
  MidiNoteStepper m_Stepper;
};

#endif /* end of include guard: MODEMIDIHANDLER_H_INCLUDE */
