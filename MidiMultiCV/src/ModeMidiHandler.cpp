#include "ModeMidiHandler.h"

ModeMidiHandler::ModeMidiHandler(uint8_t Channel, MidiHandler& midiOut) 
    : m_Channel(Channel)
    , m_midiOut(midiOut)
    , m_Mode(LivePoly)
    , m_PrevGate(false)
    , m_State()
    , m_Stepper()
  {}

void ModeMidiHandler::Tick(int Gate)
{
    m_State.Tick();
    m_Stepper.Tick(Gate);

    if(m_Mode==StepperPlay || m_Mode == StepperRecord)
    {
        if(m_PrevGate != m_Stepper.Gate())
        {
            if(m_PrevGate)
            {
                OutAllNotesOff();
            }
            else
            {
                OutNoteOn(m_Stepper.MidiNote(), m_Stepper.Velocity());
            }
            m_PrevGate = !m_PrevGate;
        }
    }
  }

  void ModeMidiHandler::OutNoteOn(uint8_t MidiNote, uint8_t Velocity)
  {
      m_OutStack.NoteOn(MidiNote);
      m_midiOut.NoteOn(m_Channel, MidiNote, Velocity);
  }

  void ModeMidiHandler::OutNoteOff(uint8_t MidiNote, uint8_t Velocity)
  {
      m_OutStack.NoteOff(MidiNote);
      m_midiOut.NoteOff(m_Channel, MidiNote, Velocity);
  }

  void ModeMidiHandler::OutAllNotesOff()
  {
      uint8_t dummyVelocity = 64;
      while(m_OutStack.Size())
      {
          OutNoteOff(m_OutStack.CurrentNote(), dummyVelocity);
      }
  }

  void ModeMidiHandler::SetMode(Mode mode)
  {
      if(mode != m_Mode)
      {
        // when switching mode between live and stepper, make sure no notes get stuck
        // switching between stepper record and play => must play notes without interruption
        if((m_Mode==LivePoly) != (mode==LivePoly))
        {
            OutAllNotesOff();
        }

        m_Mode = mode;
      }
  }


  void ModeMidiHandler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
  {
     if(m_Channel == Channel)
     {
         m_State.NoteOn(MidiNote, Velocity);
         if(m_Mode==LivePoly)
         {
             OutNoteOn(MidiNote, Velocity);
         } 
         else if(m_Mode==StepperPlay)
         {
             //noteoff on midi note with previous basenote??
             m_Stepper.SetBaseNote(MidiNote);
         }
         else if(m_Mode==StepperRecord)
         {
              // long press => clear
              // shorter press => add (high velocity) or skip (low velocity)
              //    => need to wait for note off to be sure it is a clear or add
              const uint8_t SkipStepThreshold = 48;//good threshold value?
              if(Velocity<SkipStepThreshold)
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

 void ModeMidiHandler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
 {
    if(m_Channel == Channel)
    {
        m_State.NoteOff(MidiNote, Velocity);
        if(m_Mode==LivePoly)
        {
            OutNoteOff(MidiNote, Velocity);
        }
        else if(m_Mode==StepperRecord)
        {
            // long press => clear
            // shorter press => add (high velocity) or skip (low velocity)
            //    => cannot act upon note on, but need to wait for note off
            // alternative: act in tick???  
            if(1024<m_State.PressDuration(MidiNote))
            {
                OutAllNotesOff();
                m_Stepper.Clear();
            }
        }
    }
 }

 void ModeMidiHandler::ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value)
 {
     if(m_Channel == Channel)
     {
         //TODO learn controller for gate length
         m_midiOut.ContinuousController(Channel, Controller, Value);
     }
 }

void ModeMidiHandler::PitchWheel(uint8_t Channel, int Bend)
{
    if(Channel == m_Channel)
    {
        m_midiOut.PitchWheel(Channel, Bend);
    }
}
