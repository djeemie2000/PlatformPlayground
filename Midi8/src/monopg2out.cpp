#include "monopg2out.h"
#include "cvfunctions.h"
#include "midi8ui.h"
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

void MonoPG2Out::updateUI(Midi8UI *ui)
{
    if (m_Learn)
    {
        ui->ledsOut.set(m_Idx, LedOutBank::Blink);

        ui->gatesOut.set(m_Idx, 0);
    }
    else
    {
        // outputs
        int gate = 0 < m_Stack.Size() ? 1 : 0;
        ui->gatesOut.set(m_Idx, gate);
        if (gate)
        {
            PitchOut(ui->cvOut, m_Idx, m_Stack.Note(0), m_BaseNote);
        }
        ui->ledsOut.set(m_Idx, gate);
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
