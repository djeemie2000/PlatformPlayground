#include "SerialMidiHandler.h"

  SerialMidiHandler::SerialMidiHandler(Serial& pc) 
  : m_pc(pc)
  {}

  void SerialMidiHandler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) /*override*/
  {
      m_pc.putc(0x90 | (Channel & 0x0F));
      m_pc.putc(MidiNote & 0x7F);
      m_pc.putc(Velocity & 0x7F);
  }

  void SerialMidiHandler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) /*override*/
  {
        m_pc.putc(0x80 | (Channel & 0x0F));
        m_pc.putc(MidiNote & 0x7F);
        m_pc.putc(Velocity & 0x7F);
  }
  
  void SerialMidiHandler::AfterTouch(uint8_t Channel, uint8_t MidiNote, uint8_t Pressure)
  {
        m_pc.putc(0xA0 | (Channel & 0x0F));
        m_pc.putc(MidiNote & 0x7F);
        m_pc.putc(Pressure & 0x7F);
  }
  
  void SerialMidiHandler::ContinuousController(uint8_t Channel, uint8_t Controller, uint8_t Value)
  {
        m_pc.putc(0xB0 | (Channel & 0x0F));
        m_pc.putc(Controller&0x7F);
        m_pc.putc(Value&0x7F);
  }

  void SerialMidiHandler::PitchWheel(uint8_t Channel, int Bend)
  {
      //TODO
  }
  
  void SerialMidiHandler::ActiveSense()
  {
      m_pc.putc(0xFE);
  }
