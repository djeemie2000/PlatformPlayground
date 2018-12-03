#include "LogMidiHandler.h"

void PrintUint4(char* dst, uint8_t value)
{
    // value in [0,15] -> 0,1,2,...,9,A,B,...,F
    uint8_t val = value&0x0F;
    if(val<10)
    {
        // char '0' is 0x30, '1' is 0x31, ...
        *dst = char(0x30+val);
    }
    else
    {
        // char 'a' us 0x61, 'b' is 0x62, ...
        // 10 -> 'a', 11 -> 'b', ...
        *dst = char(0x57+val);
    }
}

void PrintHex(char* dst, uint8_t value)
{
    PrintUint4(dst, value>>4);
    PrintUint4(dst+1, value);
}

LogMidiHandler::LogMidiHandler(Serial& pc, int Id) 
: m_pc(pc)
, m_Id(Id)
{
    const char msg[] = "-- : Ch 0x-- : ----- 0x-- 0x--\r\n";
    std::memcpy(m_Message, msg, 32);
    //std::sprintf(m_Message, "%d", Id);
    PrintHex(m_Message, m_Id);
}

void LogMidiHandler::SetChannel(uint8_t Channel)
{
    PrintHex(m_Message+10, Channel);
}

void LogMidiHandler::SetParam1(uint8_t Param)
{
    PrintHex(m_Message+23, Param);
}

void LogMidiHandler::SetParam2(uint8_t Param)
{
    PrintHex(m_Message+28, Param);
}

void LogMidiHandler::SetType(const char* Type)
{
    std::memcpy(m_Message+15, Type, 5);
}

void LogMidiHandler::SendMessage()
{
    //m_pc.write(m_Message, 32);//needs callback, async!
    for(int idx = 0; idx<32;  ++idx)
    {
        m_pc.putc(m_Message[idx]);
    }
}

void LogMidiHandler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) /*override*/
{
    m_pc.printf("%d : Ch 0x%02X : On    0x%02X 0x%02X\r\n", m_Id, Channel, MidiNote, Velocity);
    // SetChannel(Channel);
    // SetParam1(MidiNote);
    // SetParam2(Velocity);
    // SetType("On   ");
    // SendMessage();
}

void LogMidiHandler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity) /*override*/
{
    m_pc.printf("%d : Ch 0x%02X : Off   0x%02X 0x%02X\r\n", m_Id, Channel, MidiNote, Velocity);
    // SetChannel(Channel);
    // SetParam1(MidiNote);
    // SetParam2(Velocity);
    // SetType("Off  ");
    // SendMessage();
}
