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
        LiveMono,
        StepperRecord,
        StepperPlay
    };

  ModeMidiHandler(uint8_t Channel, MidiHandler& midiOut) 
    : m_Channel(Channel)
    , m_midiOut(midiOut)
    , m_Mode(LivePoly)
    , m_PrevGate(false)
    , m_State()
    , m_Stepper()
  {}

  void Tick(int Gate){
      m_State.Tick();
      m_Stepper.Tick(Gate);

      if(m_Mode==StepperPlay || m_Mode == StepperRecord)
      {
          if(m_PrevGate != m_Stepper.Gate())
          {
              if(m_PrevGate)
              {
                  m_midiOut.NoteOff(m_Channel, m_Stepper.MidiNote(), m_Stepper.Velocity());
              }
              else
              {
                  m_midiOut.NoteOn(m_Channel, m_Stepper.MidiNote(), m_Stepper.Velocity());
              }
              m_PrevGate = !m_PrevGate;
          }
      }
  }

  void SetMode(Mode mode){m_Mode = mode;}


  void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override
  {
     if(m_Channel == Channel)
     {
         m_State.NoteOn(MidiNote, Velocity);
         if(m_Mode==LivePoly || m_Mode==LiveMono)
         {
             m_midiOut.NoteOn(Channel, MidiNote, Velocity);
         } 
         else if(m_Mode==StepperPlay)
         {
             m_Stepper.SetBaseNote(MidiNote);
         }
         else if(m_Mode==StepperRecord)
         {
              // long press => clear
              // shorter press => add (high velocity) or skip (low velocity)
              //    => cannot act upon note on, but need to wait for note off
              // alternative: act in tick???  
              if(Velocity<48)//TODO threshold value?
              {
                  m_Stepper.AddSkipStep();
              }
              else
              {
                m_Stepper.AddStep(MidiNote, Velocity);
              }
         }
     }
  }
 void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) override
 {
     if(m_Channel == Channel)
     {
         m_State.NoteOff(MidiNote, Velocity);
         if(m_Mode==LivePoly)
         {
             m_midiOut.NoteOff(Channel, MidiNote, Velocity);
         }
         else if (m_Mode==LiveMono)
         {
             if(0==m_State.NumPressed())
             {
                 m_midiOut.NoteOff(Channel, MidiNote, Velocity);
             }
         }
         else if(m_Mode==StepperRecord)
         {
              // long press => clear
              // shorter press => add (high velocity) or skip (low velocity)
              //    => cannot act upon note on, but need to wait for note off
              // alternative: act in tick???  
              if(1024<m_State.PressDuration(MidiNote))
              {
                  m_Stepper.Clear();
              }
         }
     }
 }
 void ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value)
 {
     if(m_Channel == Channel)
     {
         m_midiOut.ContinuousController(Channel, Controller, Value);
     }
 }

 void PitchWheel(uint8_t Channel, int Bend) override;

private:
  uint8_t m_Channel;
  MidiHandler& m_midiOut;
  Mode m_Mode;
  bool m_PrevGate;
  MidiNoteState m_State;
  MidiNoteStepper m_Stepper;
};

#endif /* end of include guard: MODEMIDIHANDLER_H_INCLUDE */
