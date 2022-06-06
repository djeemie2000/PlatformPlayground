#pragma once
#include <Arduino.h>

struct MidiVoiceMessage
{
    uint8_t StatusByte;
    uint8_t Param1;
    uint8_t Param2;
};

uint8_t Command(MidiVoiceMessage& message)
{
    return message.StatusByte & 0xF0;
}

uint8_t Channel(MidiVoiceMessage& message)
{
    return message.StatusByte & 0x0F;
}

uint8_t MidiNote(MidiVoiceMessage& message)
{
    return message.Param1 & 0x7F;
}

uint8_t Velocity(MidiVoiceMessage& message)
{
    return message.Param2 & 0x7F;
}

bool IsNoteOn(MidiVoiceMessage& message)
{
    // note off 0x90
    return 0x90 == Command(message);
}

bool IsNoteOff(MidiVoiceMessage& message)
{
    // note off 0x80 or note on with zero velocity
    return (0x80 == Command(message)) 
    || (0x90 == Command(message) && 0x00 == Velocity(message));
}

class MidiNoteParser
{
public:
    MidiNoteParser()
        : m_InsideSysex(false), m_StatusByte(0x00), m_Param1(0x00), m_Param2(0x00), m_ValueWriteIndex(0)
    {
    }

    bool Parse(uint8_t byte, MidiVoiceMessage &message)
    {
        bool handled = false;

        // TODO optimize
        // if sysex => check for sysex end
        // else if < 0x80 => handle value
        // else if >= 0x80 (status byte) => all cases for status bytes

        //TODO sysex -> midi systemrealtime can come inbetween, status byte ends sysex
        if (m_InsideSysex)
        {
            // ignore everything, except midi sysex end
            if (byte == 0xF7)
            {
                m_InsideSysex = false;
                m_ValueWriteIndex = 0;
            }
        }
        else if (byte == 0xF0)
        {
            // midi sysex start
            m_InsideSysex = true;
            m_ValueWriteIndex = 0;
        }
        else if (0xF8 <= byte)
        {
            // system realtime messages
            // => ignore, does not affect current run status / statusByte
        }
        else if (0xF1 <= byte)
        {
            // system common
            // => affects current run status / statusByte
            m_StatusByte = byte;
            // ignore content
            m_ValueWriteIndex = 0;
        }
        else if (0xA0 <= byte)
        {
            // voice message, not a note on/off
            // => affects current run status / statusByte
            m_StatusByte = byte;
            // but ignore content
            m_ValueWriteIndex = 0;
        }
        else if (0x80 <= byte)
        {
            // note on/off voice message
            // => affects current run status / statusByte
            m_StatusByte = byte;
            // do not ignore content !
            m_ValueWriteIndex = 1;
        }
        else
        {
            // is a value
            if (m_ValueWriteIndex == 1)
            {
                m_Param1 = byte;
                // only handle note on/off messages => always 2 value bytes!
                m_ValueWriteIndex = 2;
            }
            else if (m_ValueWriteIndex == 2)
            {
                m_Param2 = byte;
                // only handle note on/off messages => always 2 value bytes!
                // 2nd valyue byte => handled note on/off message!
                handled = true;
                // could be running status, i.e. more values come in for the same status byte!
                m_ValueWriteIndex = 1;
            }
        }

        if (handled)
        {
            message.StatusByte = m_StatusByte;
            message.Param1 = m_Param1;
            message.Param2 = m_Param2;
        }

        return handled;
    }

private:
    bool m_InsideSysex;
    uint8_t m_StatusByte;
    uint8_t m_Param1;
    uint8_t m_Param2;
    int m_ValueWriteIndex; // 0 <=> ignore value
};
