#include "mono4handler.h"
#include "midi8ui.h"
#include "cvfunctions.h"
#include "EEPROM.h"

Mono4Handler::Mono4Handler()
    : m_LearnIdx(-1)
{
    for (int idx = 0; idx < Size; ++idx)
    {
        int column = 2 * idx;
        m_Out[idx].Begin(column);
    }
}

void Mono4Handler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    //
    // Serial.print("M4 On  ");
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
            m_Out[idx].NoteOn(Channel, MidiNote, Velocity);
        }
    }
    else
    {
        //learning
        m_Out[m_LearnIdx].NoteOn(Channel, MidiNote, Velocity);
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

void Mono4Handler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    //
    // Serial.print("M4 Off ");
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
            m_Out[idx].NoteOff(Channel, MidiNote, Velocity);
        }
    }
}

bool Mono4Handler::IsLearning() const
{
    return m_LearnIdx != -1;
}

void Mono4Handler::updateUI(Midi8UI *ui)
{
    for (int idx = 0; idx < Size; ++idx)
    {
        m_Out[idx].updateUI(ui);
    }

    if (ui->learnBtn.IsFalling())
    {
        ui->printToggleLearn('M', '4');

        //toggle learn mode on/off
        if (ui->learnMode.Get() == Midi8UI::Learn1)
        {
            if (m_LearnIdx == -1)
            {
                m_LearnIdx = 0;
                m_Out[m_LearnIdx].Learn(true);
            }
        }
        else
        {
            m_Out[m_LearnIdx].Learn(false);
            m_LearnIdx = -1;
        }
    }
    else if (ui->learnMode.Get() != Midi8UI::Learn2)
    {
        ui->learnMode.Set(IsLearning() ? Midi8UI::Learn1 : Midi8UI::NoLearn);
    }
}

void Mono4Handler::saveParams(int offset)
{
    int off = offset;
    EEPROM.update(off++, 'M');
    EEPROM.update(off++, '4');
    for (int idx = 0; idx < Size; ++idx)
    {
        m_Out[idx].saveParams(off);
        off += m_Out[idx].paramSize();
    }
}

int Mono4Handler::paramSize() const
{
    return 2 + Size * m_Out[0].paramSize();
}

void Mono4Handler::loadParams(int offset)
{
    int off = offset;
    if ('M' == EEPROM.read(off++) && '4' == EEPROM.read(off++))
    {
        for (int idx = 0; idx < Size; ++idx)
        {
            m_Out[idx].loadParams(off);
            off += m_Out[idx].paramSize();
        }
    }
}
