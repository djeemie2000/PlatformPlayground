#include "MidiParser.h"

MidiParser::MidiParser()
  : m_Command(0x00)
  , m_Param1(0x00)
  , m_Param2(0x00)
  , m_Cntr(0)
{}

bool MidiParser::IsCommand(uint8_t byte) const
{
  return 0x00 != (byte & 0x80);
}

bool MidiParser::IsVoiceMessage(uint8_t byte) const
{
  uint8_t command = byte & 0xF0;
  return  0x80<=command && command<=0xE0;
}

bool MidiParser::HandleSystemRealtime(uint8_t byte, MidiHandler& handler)
{
  bool handled = false;
  if(0xF8<=byte)
  {
    if(byte==0xF8)
    {
      handler.MidiContinue();
    }
    else if(byte==0xFA)
    {
      handler.MidiStart();
    }
    else if(byte==0xFB)
    {
      handler.MidiContinue();
    }
    else if(byte==0xFC)
    {
      handler.MidiStop();
    }
    else if(byte==0xFE)
    {
      handler.ActiveSense();
    }
    else if(byte==0xFF)
    {
        handler.SystemReset();
    }
    handled = true;
  }
  return handled;
}


bool MidiParser::HandleBuffer(MidiHandler& handler) const
{
  bool handled = false;
  if(m_Cntr == 1)
  {
    // commands without params
    if(m_Command==0xF6)
    {
        handler.TuneRequest();
        handled = true;
    }
  }
  else if(m_Cntr == 2)
  {
    // commands with one data byte
    handled = true;
    uint8_t command = m_Command&0xF0;
    uint8_t channel = m_Command&0x0F;
    if(command==0xC0)
    {
        handler.ProgramChange(channel, m_Param1);
    }
    else if(command==0xD0)
    {
        handler.ChannelPressure(channel, m_Param1);
    }
    else if(m_Command==0xF1)
    {
        handler.QuarterFrameMessage(m_Param1);
    }
    else if(m_Command==0xF3)
    {
      handler.SongPositionPointer(m_Param1);
    }
    else
    {
      handled = false;
    }
  }
  else if(m_Cntr == 3)
  {
    handled = true;
    uint8_t command = m_Command&0xF0;
    uint8_t channel = m_Command&0x0F;
    if(command==0x80)
    {
      handler.NoteOff(channel, m_Param1, m_Param2);
    }
    else if(command==0x90)
    {
      // note on with velocity == zero is interpreted as note off,
      // to allow running status for midi notes
      if(m_Param2!=0x00)
      {
        handler.NoteOn(channel, m_Param1, m_Param2);
      }
      else
      {
        handler.NoteOff(channel, m_Param1, m_Param2);
      }
    }
    else if(command==0xA0)
    {
      handler.AfterTouch(channel, m_Param1, m_Param2);
    }
    else if(command==0xB0)
    {
      handler.ContinuousController(channel, m_Param1, m_Param2);
    }
    else if(command==0xE0)
    {
      //TODO function for two uint8_t to int/uint16_t
      int bend = (m_Param2>>1);
      bend <<= 7;
      bend |= (m_Param1>>1);
      // range is [0x0000, 0x7FFF[, so subtract offset of 0x4000
      bend -= 0x1000;
      handler.PitchWheel(channel, bend);
    }
    else if(m_Command==0xF2)
    {
      //TODO function for two uint8_t to int/uint16_t
      int position = (m_Param2>>1);
      position <<= 7;
      position |= (m_Param1>>1);
      handler.SongPositionPointer(position);
    }
    else
    {
      handled = false;
    }
  }
  return handled;
}


bool MidiParser::Parse(uint8_t byte, MidiHandler& handler)
{
  // midi system realtime commands (single byte) can come inbetween other midi commands!
  // handle running status for voice commands, system common cancels running status!
  // http://www.gweep.net/~prefect/eng/reference/protocol/midispec.html
  if(IsCommand(byte))
  {
    if(HandleSystemRealtime(byte, handler))
    {
      return false;//done
    }
    else
    {
      m_Command = byte;
      m_Cntr = 1;

      bool handled = HandleBuffer(handler);
      if(handled)
      {
        m_Cntr = 0;
      }
      return !handled;
    }
  }
  else
  {
    if(m_Cntr==0 && IsVoiceMessage(byte))
    {
      //running status
      m_Param1 = byte;
      m_Cntr = 2;
    }
    else if(m_Cntr==1)
    {
      m_Param1 = byte;
      m_Cntr = 2;
    }
    else if(m_Cntr==2)
    {
      m_Param2 = byte;
      m_Cntr = 3;
    }
    bool handled = HandleBuffer(handler);
    if(handled)
    {
      m_Cntr = 0;
    }
    return !handled;
  }
  //true => continue
  // false => done
  return false;
}
