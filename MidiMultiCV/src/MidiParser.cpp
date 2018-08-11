#include "MidiParser.h"

MidiParser::MidiParser()
{}

bool MidiParser::IsCommand(uint8_t byte) const
{
  return 0x00 != (byte & 0x80);
}

bool MidiParser::IsSystemMessage(uint8_t byte) const
{
  return 0 != (0xF0 & byte);
}
int MidiParser::NumBytes(uint8_t commandByte) const
{
  uint8_t command = commandByte & 0xF0;
  int numBytes = 3;
  if(command==0xC0 || command == 0xD0 || commandByte==0xF1 || commandByte==0xF3)
  {
    numBytes = 2;
  }
  else if(commandByte == 0xF6 ||commandByte == 0xF8 ||commandByte == 0xFA ||commandByte == 0xFA ||commandByte == 0xFC ||commandByte == 0xFE || commandByte == 0xFF)
  {
    numBytes = 1;
  }
  return numBytes;
}

bool MidiParser::HandleBuffer(MidiHandler& handler) const
{
  bool handled = false;
  if(m_Cntr == 1)
  {
    // commands without params
    handled = true;
    if(m_Command==0xF6)
    {
        handler.TuneRequest();
    }
    else if(m_Command==0xF8)
    {
      handler.MidiContinue();
    }
    else if(m_Command==0xFA)
    {
      handler.MidiStart();
    }
    else if(m_Command==0xFB)
    {
      handler.MidiContinue();
    }
    else if(m_Command==0xFC)
    {
      handler.MidiStop();
    }
    else if(m_Command==0xFE)
    {
      handler.ActiveSense();
    }
    else if(m_Command==0xFF)
    {
        handler.SystemReset();
    }
    else
    {
      handled = false;
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
      int bend = (m_Param1>>1);
      bend <<= 7;
      bend |= (m_Param2>>1);
      // range is [0x0000, 0x4000[, so subtract offset of 0x2000
      bend -= 0x2000;
      handler.PitchWheel(channel, bend);
    }
    else if(m_Command==0xF2)
    {
      //TODO function for two uint8_t to int/uint16_t
      int position = (m_Param1>>1);
      position <<= 7;
      position |= (m_Param2>>1);
      handler.SongPositionPointer(position);
    }
    else
    {
      handled = false;
    }
  }
  return handled;
}


bool MidiParser::Parse(uint8_t byte)
{
  //TODO midi system realtime commands (single byte) can come inbetween other midi commands!
  //TODO handle running status for voice commands, system common cancels running status!
  // http://www.gweep.net/~prefect/eng/reference/protocol/midispec.html
  bool handled = false;
  if(m_Cntr==0)
  {
    if(IsCommand(byte))
    {
      m_Command = byte;
      // check if  1 byte buffer => immediate callback
      if(1==NumBytes(m_Command))
      {
        // callback TODO
        m_Cntr = 0;
        handled = true;
      }
      else
      {
        m_Cntr = 1;
      }
    }
    else if(!IsSystemMessage(m_Command))
    {  // running status?

      //voice command => assume running status => fill params
      AddParam1(byte);
    }
  }
  else if(m_Cntr == 1)
  {
    AddParam1(byte);
  }
  else
  {
    AddParam2(byte);
  }
  //true => continue
  // false => done
  return !handled;
}

void MidiParser::AddParam1(uint8_t byte)
{
  m_Param1 = byte;
  // check if buffer is full depending on command
  if(2==NumBytes(m_Command))
  {
    //TODO callbacks
    // 0xC0 and 0xD0 only have 2 byte, the other voice commands have 3 byte
    if((m_Command & 0xF0) == 0xD0)
    {

    }
    else if(( m_Command & 0xF0) == 0xC0)
    {

    }
    m_Cntr = 0;
  }
  else
  {
    m_Cntr = 2;
  }
}

void MidiParser::AddParam2(uint8_t byte)
{
  // if we end up here, it means a 3 byte commmand
  if(3==NumBytes(m_Command))
  {
    // callback TODO
    m_Cntr = 0;
  }
}
