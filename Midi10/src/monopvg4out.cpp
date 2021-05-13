#include "monopvg4out.h"
#include "cvfunctions.h"
#include "midi10ui.h"
#include "EEPROM.h"

MonoPVG4Out::MonoPVG4Out()
    : m_Idx(0), m_Learn(false), m_Channel(0xFF), m_BaseNote(0x00), m_Stack()
{
}

void MonoPVG4Out::Begin(int idx)
{
    m_Idx = idx;
    m_Learn = false; //??
    m_Channel = 0xFF;
    m_BaseNote = 0x00;
    m_Stack.Clear();
}

void MonoPVG4Out::NoteOn(uint8_t channel, uint8_t midiNote, uint8_t velocity)
{
    if (m_Learn)
    {
        m_Stack.Clear();
        m_Channel = channel;
        m_BaseNote = midiNote;
        m_Learn = false;
    }
    else if (m_Channel == channel)
    {
        m_Stack.NoteOn(midiNote, velocity);
    }
}

void MonoPVG4Out::NoteOff(uint8_t channel, uint8_t midiNote, uint8_t velocity)
{
    if (m_Channel == channel)
    {
        m_Stack.NoteOff(midiNote);
    }
}

void MonoPVG4Out::updateUI(Midi10UI *ui)
{
    if (m_Learn)
    {
        LedOut(ui->ledsOut, m_Idx, LedOutBank::Off);       //gate
        LedOut(ui->ledsOut, m_Idx + 1, LedOutBank::Off);   //gate
        LedOut(ui->ledsOut, m_Idx + 4, LedOutBank::Blink); //led
        LedOut(ui->ledsOut, m_Idx + 5, LedOutBank::Blink); //led
        // pitch/velocity??
    }
    else
    {
        // outputs
        int gate = 0 < m_Stack.Size() ? LedOutBank::On : LedOutBank::Off;
        ui->ledsOut.set(m_Idx, gate);         ///gate
        ui->ledsOut.set(m_Idx + 1, gate);     //gate
        LedOut(ui->ledsOut, m_Idx + 4, gate); //led
        LedOut(ui->ledsOut, m_Idx + 5, gate); //led
        if (gate)
        {
            PitchOut(ui->cvOut, m_Idx, m_Stack.Note(0), m_BaseNote);
            VelocityOut(ui->cvOut, m_Idx + 1, m_Stack.Velocity(0));
        }
    }
}

void MonoPVG4Out::Learn(bool learn)
{
    m_Learn = learn;
}

bool MonoPVG4Out::IsLearning() const
{
    return m_Learn;
}

void MonoPVG4Out::saveParams(int offset)
{
    int off = offset;
    EEPROM.update(off++, m_Channel);
    EEPROM.update(off++, m_BaseNote);
}

int MonoPVG4Out::paramSize() const
{
    return 2;
}

void MonoPVG4Out::loadParams(int offset)
{
    int off = offset;
    m_Channel = EEPROM.read(off++);
    m_BaseNote = EEPROM.read(off++);
}
