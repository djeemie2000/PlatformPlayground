#pragma once

#include <Arduino.h>

/*!
Midi note stack 
remark: note + channel = 2 bytes
*/
class MidiNoteStack
{
public:
  static const int Capacity = 12;

  MidiNoteStack();

  void NoteOn(uint8_t midiNote, uint8_t midiChannel);
  void NoteOff(uint8_t midiNote, uint8_t midiChannel);

  int Size() const { return m_Size; }
  uint16_t Current() const
  {
    return 0 < m_Size ? m_Stack[m_Size - 1] : 0x00;
  }
  uint16_t Pop();
  uint16_t Get(int idx) const;
  bool Contains(uint8_t midiNote, uint8_t midiChannel);

  uint8_t ToNote(uint16_t value);
  uint8_t ToChannel(uint16_t value);

private:
  uint16_t Concat(uint8_t midiNote, uint8_t midiChannel);

  uint16_t m_Stack[Capacity];
  int m_Size; //{0};
};
