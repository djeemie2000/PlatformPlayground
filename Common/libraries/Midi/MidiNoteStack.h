#pragma once

#include <mbed.h>

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
    uint8_t Note(int idx) const;

  private:
    uint8_t m_Stack[Capacity];
    int m_Size; //{0};
};
