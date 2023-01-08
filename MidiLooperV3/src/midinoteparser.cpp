#include "midinoteparser.h"
#include "midivoicemessage.h"

MidiNoteParser::MidiNoteParser()
    : m_InsideSysex(false), m_StatusByte(0x00), m_Param1(0x00), m_Param2(0x00), m_ValueWriteIndex(0)
{
}

bool MidiNoteParser::Parse(uint8_t byte, MidiVoiceMessage &message)
{
    bool handled = false;

    // TODO optimize
    // if sysex => check for sysex end
    // else if < 0x80 => handle value
    // else if >= 0x80 (status byte) => all cases for status bytes

    // TODO sysex -> midi systemrealtime can come inbetween, status byte ends sysex
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
    else if(0xB0 == (byte & 0xF0))
    {
        // handle cc!!
         // => affects current run status / statusByte
        m_StatusByte = byte;
        // do not ignore content !
        m_ValueWriteIndex = 1;
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
