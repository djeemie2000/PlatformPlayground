#include "mono2handler.h"
#include "midi8ui.h"
#include "cvfunctions.h"
#include "EEPROM.h"

Mono2Handler::Mono2Handler()
    : m_LearnIdx(-1)
{
    for (int idx = 0; idx < Size; ++idx)
    {
        int column = idx;
        m_Out[idx].Begin(column);
    }
}

void Mono2Handler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t /*Velocity*/)
{
    //
    // Serial.print("M2 On  ");
    // Serial.print(Channel, HEX);
    // Serial.print(" ");
    // Serial.print(MidiNote, HEX);
    // //
    // Serial.print(" L ");
    // Serial.println(m_LearnIdx);
    //

    if (m_LearnIdx == -1)
    {
        // normal operation
        for (int idx = 0; idx < Size; ++idx)
        {
            m_Out[idx].NoteOn(Channel, MidiNote);
        }
    }
    else
    {
        //learning
        m_Out[m_LearnIdx].NoteOn(Channel, MidiNote);
        if (!m_Out[m_LearnIdx].IsLearning())
        {
            ++m_LearnIdx;
            if (Size <= m_LearnIdx)
            {
                m_LearnIdx = -1; //stop learning
            }
            else
            {
                m_Out[m_LearnIdx].Learn(true);
            }
        }
    }
}

void Mono2Handler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t /*Velocity*/)
{
    //
    // Serial.print("M2 Off ");
    // Serial.print(Channel, HEX);
    // Serial.print(" ");
    // Serial.print(MidiNote, HEX);
    // //
    // Serial.print(" L ");
    // Serial.println(m_LearnIdx);
    //

    if (m_LearnIdx == -1)
    {
        // normal operation
        for (int idx = 0; idx < Size; ++idx)
        {
            m_Out[idx].NoteOff(Channel, MidiNote);
        }
    }
}

bool Mono2Handler::IsLearning() const
{
    return m_LearnIdx != -1;
}

void Mono2Handler::Learn(bool learn)
{
    if (learn)
    {
        if (m_LearnIdx == -1)
        {
            m_LearnIdx = 0;
            m_Out[m_LearnIdx].Learn(true);
        }
    }
    else if (m_LearnIdx != 1)
    {
        m_Out[m_LearnIdx].Learn(false);
        m_LearnIdx = -1;
    }
}

void Mono2Handler::updateUI(Midi8UI *ui)
{
    for (int idx = 0; idx < Size; ++idx)
    {
        m_Out[idx].updateUI(ui);
    }

    if (ui->learnBtn.IsFalling())
    {
        ui->printToggleLearn('M', '2');

        //toggle learn mode on/off
        Learn(ui->learnMode.Get() == Midi8UI::Learn1);
    }
    else if (ui->learnMode.Get() != Midi8UI::Learn2)
    {
        ui->learnMode.Set(IsLearning() ? Midi8UI::Learn1 : Midi8UI::NoLearn);
    }
}

void Mono2Handler::saveParams(int offset)
{
    int off = offset;
    EEPROM.update(off++, 'M');
    EEPROM.update(off++, '2');
    for (int idx = 0; idx < Size; ++idx)
    {
        m_Out[idx].saveParams(off);
        off += m_Out[idx].paramSize();
    }
}

int Mono2Handler::paramSize() const
{
    return 2 + Size * m_Out[0].paramSize();
}

void Mono2Handler::loadParams(int offset)
{
    int off = offset;
    if ('M' == EEPROM.read(off++) && '2' == EEPROM.read(off++))
    {
        for (int idx = 0; idx < Size; ++idx)
        {
            m_Out[idx].loadParams(off);
            off += m_Out[idx].paramSize();
        }
    }
}
