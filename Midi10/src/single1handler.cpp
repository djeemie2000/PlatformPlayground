#include "single1handler.h"
#include "midi10ui.h"
#include "cvfunctions.h"
#include "EEPROM.h"

Single1Handler::Single1Handler()
    : m_LearnIdx(-1)
{
    for (int idx = 0; idx < Size; ++idx)
    {
        m_Channel[idx] = 0xFF;
        m_MidiNote[idx] = 0xFF;
        m_Gate[idx] = 0;
    }
}

void Single1Handler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t /*Velocity*/)
{
    //
    // Serial.print("S1 On  ");
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
            if (m_Channel[idx] == Channel && m_MidiNote[idx] == MidiNote)
            {
                m_Gate[idx] = 1;
            }
        }
    }
    else
    {
        //learning
        m_Channel[m_LearnIdx] = Channel;
        m_MidiNote[m_LearnIdx] = MidiNote;
        ++m_LearnIdx;
        if (Size <= m_LearnIdx)
        {
            m_LearnIdx = -1; //stop learning
        }
    }
}

void Single1Handler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t /*Velocity*/)
{
    //
    // Serial.print("S1 Off ");
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
            if (m_Channel[idx] == Channel && m_MidiNote[idx] == MidiNote)
            {
                m_Gate[idx] = 0;
            }
        }
    }
}

bool Single1Handler::IsLearning() const
{
    return m_LearnIdx != -1;
}

void Single1Handler::updateUI(Midi10UI *ui)
{
    // gate 8  9 10 11
    // led 12 13 14 15
    for (int idx = 0; idx < Size; ++idx)
    {
        if (idx == m_LearnIdx)
        {
            ui->ledsOut.set(8 + idx, LedOutBank::Off);    //gate
            ui->ledsOut.set(12 + idx, LedOutBank::Blink); //led
        }
        else if (m_Gate[idx])
        {
            ui->ledsOut.set(8 + idx, LedOutBank::On);  //gate
            ui->ledsOut.set(12 + idx, LedOutBank::On); //led
        }
        else
        {
            ui->ledsOut.set(8 + idx, LedOutBank::Off);  //gate
            ui->ledsOut.set(12 + idx, LedOutBank::Off); //led
        }
    }

    if (ui->learnBtn.IsFalling())
    {
        //Serial.println("Toggle learn!");
        //toggle learn mode on/off
        if (m_LearnIdx == -1)
        {
            m_LearnIdx = 0;
        }
        else
        {
            m_LearnIdx = -1;
        }
    }
}

void Single1Handler::saveParams(int offset)
{
    int off = offset;
    EEPROM.update(off++, 'S');
    EEPROM.update(off++, '1');
    for (int idx = 0; idx < Size; ++idx)
    {
        EEPROM.update(off++, m_Channel[idx]);
        EEPROM.update(off++, m_MidiNote[idx]);
    }
}

int Single1Handler::paramSize() const
{
    // hack for compatibility with Midi8!!
    // return 2 + Size * 2;
    return 2 + 8 * 2;
}

void Single1Handler::loadParams(int offset)
{
    int off = offset;
    if ('S' == EEPROM.read(off++) && '1' == EEPROM.read(off++))
    {
        for (int idx = 0; idx < Size; ++idx)
        {
            m_Channel[idx] = EEPROM.read(off++);
            m_MidiNote[idx] = EEPROM.read(off++);
        }
    }
}