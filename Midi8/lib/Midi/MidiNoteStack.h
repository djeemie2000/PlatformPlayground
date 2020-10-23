#pragma once

#include <Arduino.h>

/*!
Midi note stack with latest note priority
*/
class MidiNoteStack
{
public:
  static const int Capacity = 12;

  MidiNoteStack();

  void NoteOn(uint8_t MidiNote);
  void NoteOff(uint8_t MidiNote);

  int Size() const { return m_Size; }
  uint8_t CurrentNote() const
  {
    return 0 < m_Size ? m_Stack[m_Size - 1] : 0x00;
  }
  uint8_t Pop();
  void Clear();
  uint8_t Note(int idx) const;

private:
  uint8_t m_Stack[Capacity];
  int m_Size; //{0};
};

class MidiNoteVelocityStack
{
public:
  static const int Capacity = 12;

  struct Item
  {
    uint8_t m_MidiNote;
    uint8_t m_Velocity;
    Item();
    Item(uint8_t midiNote);
    bool operator==(const Item& other) const;
  };

  MidiNoteVelocityStack();

  void NoteOn(uint8_t MidiNote, uint8_t velocity);
  void NoteOff(uint8_t MidiNote);

  int Size() const { return m_Size; }
  Item Current() const
  {
    return 0 < m_Size ? m_Stack[m_Size - 1] : 0x00;
  }
  Item Pop();
  void Clear();
  uint8_t Note(int idx) const;
  uint8_t Velocity(int idx) const;

private:
  Item m_Stack[Capacity];
  int m_Size; //{0};
};
