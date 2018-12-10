#include "ChordMemoryMidiHandler.h"

ChordMemoryMidiHandler::ChordMemoryMidiHandler(MidiHandler& handler, uint8_t channel)
 : m_Handler(handler)
 , m_Channel(channel)
{
}

bool ChordMemoryMidiHandler::ChannelAllowed(uint8_t channel) const
{
    return m_Channel == channel;
}

void ChordMemoryMidiHandler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    if(ChannelAllowed(Channel))
    {
        if(m_NotePressed.NumPressed()==0)
        {
            // new chord => all notes off
            while(m_OutStack.Size())
            {
                m_OutStack.NoteOff(m_OutStack.CurrentNote());
                m_Handler.NoteOff(m_Channel, m_OutStack.CurrentNote(), 64);
            }
        }
        m_NotePressed.NoteOn(MidiNote);
        // add note to chord:
        // prevent same note being added twice to stack by first removing it (will do nothing if not in stack)
        m_OutStack.NoteOff(MidiNote);
        m_OutStack.NoteOn(MidiNote);
        m_Handler.NoteOn(Channel, MidiNote, Velocity);
    }
}

void ChordMemoryMidiHandler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    if(ChannelAllowed(Channel))
    {
        m_NotePressed.NoteOff(MidiNote);
    }
}

void ChordMemoryMidiHandler::AfterTouch(uint8_t Channel, uint8_t MidiNote, uint8_t Pressure)
{
    if(ChannelAllowed(Channel))
    {
        m_Handler.AfterTouch(Channel, MidiNote, Pressure);
    }
}

void ChordMemoryMidiHandler::ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value)
{
    if(ChannelAllowed(Channel))
    {
        m_Handler.ContinuousController(Channel, Controller, Value);
    }
}

void ChordMemoryMidiHandler::ProgramChange(uint8_t Channel, uint8_t Program)
{
    if(ChannelAllowed(Channel))
    {
        m_Handler.ProgramChange(Channel, Program);
    }
}

void ChordMemoryMidiHandler::ChannelPressure(uint8_t Channel, uint8_t Pressure)
{
    if(ChannelAllowed(Channel))
    {
        m_Handler.ChannelPressure(Channel, Pressure);
    }
}

void ChordMemoryMidiHandler::PitchWheel(uint8_t Channel, int Bend)
{
    if(ChannelAllowed(Channel))
    {
        m_Handler.PitchWheel(Channel, Bend);
    }
}

  void ChordMemoryMidiHandler::QuarterFrameMessage(uint8_t TimeCode)
  {
      m_Handler.QuarterFrameMessage(TimeCode);
  }

  void ChordMemoryMidiHandler::SongPositionPointer(int MidiBeat)
  {
      m_Handler.SongPositionPointer(MidiBeat);
  }

  void ChordMemoryMidiHandler::SongSelect(uint8_t SongNumber)
  {
      m_Handler.SongSelect(SongNumber);
  }

  void ChordMemoryMidiHandler::TuneRequest()
  {
      m_Handler.TuneRequest();
  }

  void ChordMemoryMidiHandler::MidiClock()
  {
      m_Handler.MidiClock();
  }

  void ChordMemoryMidiHandler::MidiStart()
  {
      m_Handler.MidiStart();
  }

  void ChordMemoryMidiHandler::MidiContinue()
  {
      m_Handler.MidiContinue();
  }

  void ChordMemoryMidiHandler::MidiStop()
  {
      m_Handler.MidiStop();
  }

  void ChordMemoryMidiHandler::ActiveSense()
  {
      m_Handler.ActiveSense();
  }

  void ChordMemoryMidiHandler::SystemReset()
  {
      m_Handler.SystemReset();
  }
