#pragma once
#include <Arduino.h>

class MidiVoiceMessage;

class MidiNoteParser
{
public:
    MidiNoteParser();

    bool Parse(uint8_t byte, MidiVoiceMessage &message);

private:
    bool m_InsideSysex;
    uint8_t m_StatusByte;
    uint8_t m_Param1;
    uint8_t m_Param2;
    int m_ValueWriteIndex; // 0 <=> ignore value
};
