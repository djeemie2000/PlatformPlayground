#include "ChannelFilterMidiHandler.h"

ChannelFilterMidiHandler::ChannelFilterMidiHandler(MidiHandler& handler)
 : m_Handler(handler)
{
    for(int channel = 0; channel<NumChannels; ++channel)
    {
        m_ChannelAllowed[channel] = true;
    }
}

void ChannelFilterMidiHandler::AllowChannel(uint8_t channel, bool allowed)
{
    //no check on channel range
    m_ChannelAllowed[channel] = allowed;
}

bool ChannelFilterMidiHandler::ChannelAllowed(uint8_t channel) const
{
    return m_ChannelAllowed[channel];
}

void ChannelFilterMidiHandler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    if(ChannelAllowed(Channel))
    {
        m_Handler.NoteOn(Channel, MidiNote, Velocity);
    }
}

void ChannelFilterMidiHandler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    if(ChannelAllowed(Channel))
    {
        m_Handler.NoteOff(Channel, MidiNote, Velocity);
    }
}

void ChannelFilterMidiHandler::AfterTouch(uint8_t Channel, uint8_t MidiNote, uint8_t Pressure)
{
    if(ChannelAllowed(Channel))
    {
        m_Handler.AfterTouch(Channel, MidiNote, Pressure);
    }
}

void ChannelFilterMidiHandler::ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value)
{
    if(ChannelAllowed(Channel))
    {
        m_Handler.ContinuousController(Channel, Controller, Value);
    }
}

void ChannelFilterMidiHandler::ProgramChange(uint8_t Channel, uint8_t Program)
{
    if(ChannelAllowed(Channel))
    {
        m_Handler.ProgramChange(Channel, Program);
    }
}

void ChannelFilterMidiHandler::ChannelPressure(uint8_t Channel, uint8_t Pressure)
{
    if(ChannelAllowed(Channel))
    {
        m_Handler.ChannelPressure(Channel, Pressure);
    }
}

void ChannelFilterMidiHandler::PitchWheel(uint8_t Channel, int Bend)
{
    if(ChannelAllowed(Channel))
    {
        m_Handler.PitchWheel(Channel, Bend);
    }
}

  void ChannelFilterMidiHandler::QuarterFrameMessage(uint8_t TimeCode)
  {
      m_Handler.QuarterFrameMessage(TimeCode);
  }

  void ChannelFilterMidiHandler::SongPositionPointer(int MidiBeat)
  {
      m_Handler.SongPositionPointer(MidiBeat);
  }

  void ChannelFilterMidiHandler::SongSelect(uint8_t SongNumber)
  {
      m_Handler.SongSelect(SongNumber);
  }

  void ChannelFilterMidiHandler::TuneRequest()
  {
      m_Handler.TuneRequest();
  }

  void ChannelFilterMidiHandler::MidiClock()
  {
      m_Handler.MidiClock();
  }

  void ChannelFilterMidiHandler::MidiStart()
  {
      m_Handler.MidiStart();
  }

  void ChannelFilterMidiHandler::MidiContinue()
  {
      m_Handler.MidiContinue();
  }

  void ChannelFilterMidiHandler::MidiStop()
  {
      m_Handler.MidiStop();
  }

  void ChannelFilterMidiHandler::ActiveSense()
  {
      m_Handler.ActiveSense();
  }

  void ChannelFilterMidiHandler::SystemReset()
  {
      m_Handler.SystemReset();
  }
