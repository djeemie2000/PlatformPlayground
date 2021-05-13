#include "monopg2out.h"
#include "cvfunctions.h"
#include "midi10ui.h"
#include "EEPROM.h"

MonoPG2Out::MonoPG2Out()
    : m_Idx(0), m_Learn(false), m_Channel(0xFF), m_BaseNote(0x00), m_Stack()
{
}

void MonoPG2Out::Begin(int idx)
{
    m_Idx = idx;
    m_Learn = false; //??
    m_Channel = 0xFF;
    m_BaseNote = 0x00;
    m_Stack.Clear();
}

void MonoPG2Out::NoteOn(uint8_t channel, uint8_t midiNote)
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
        m_Stack.NoteOn(midiNote);
    }
}

void MonoPG2Out::NoteOff(uint8_t channel, uint8_t midiNote)
{
    if (m_Channel == channel)
    {
        m_Stack.NoteOff(midiNote);
    }
}

void MonoPG2Out::updateUI(Midi10UI *ui)
{
    // gate idx
    // led idx+4
    // pitch idx
    if (m_Learn)
    {
        LedOut(ui->ledsOut, m_Idx, LedOutBank::Off);       //gate
        LedOut(ui->ledsOut, m_Idx + 4, LedOutBank::Blink); //led
        ui->cvOut.set(m_Idx, 0);                           //pitch
    }
    else
    {
        // outputs
        int gate = 0 < m_Stack.Size() ? LedOutBank::On : LedOutBank::Off;
        LedOut(ui->ledsOut, m_Idx, gate);     //gate
        LedOut(ui->ledsOut, m_Idx + 4, gate); //led
        if (gate)
        {
            PitchOut(ui->cvOut, m_Idx, m_Stack.Note(0), m_BaseNote);
        }
        // no gate => do not change pitch
    }
}

void MonoPG2Out::Learn(bool learn)
{
    m_Learn = learn;
}

bool MonoPG2Out::IsLearning() const
{
    return m_Learn;
}

void MonoPG2Out::saveParams(int offset)
{
    int off = offset;
    EEPROM.update(off++, m_Channel);
    EEPROM.update(off++, m_BaseNote);
}

int MonoPG2Out::paramSize() const
{
    return 2;
}

void MonoPG2Out::loadParams(int offset)
{
    int off = offset;
    m_Channel = EEPROM.read(off++);
    m_BaseNote = EEPROM.read(off++);
}
