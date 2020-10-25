#include "mode2handler.h"
#include "midi8ui.h"
#include "cvfunctions.h"

Mode2Handler::Mode2Handler()
 : m_LearnIdx(-1)
{
    for(int idx = 0; idx<Size; ++idx)
    {
        int column = idx;
        m_Out[idx].Begin(column);
    }
}

void Mode2Handler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t /*Velocity*/)
{
    //
    Serial.print("M2 On  ");
    Serial.print(Channel, HEX);
    Serial.print(" ");
    Serial.print(MidiNote, HEX);
    //
    Serial.print(" L ");
    Serial.println(m_LearnIdx);
    //

    if(m_LearnIdx == -1)
    {
        // normal operation
        for(int idx = 0; idx<Size; ++idx)
        {
            m_Out[idx].NoteOn(Channel, MidiNote);
        }
    }
    else
    {
        //learning
        m_Out[m_LearnIdx].Learn(true);
        m_Out[m_LearnIdx].NoteOn(Channel, MidiNote);
        if(!m_Out[m_LearnIdx].IsLearning())
        {
            ++m_LearnIdx;
            if(Size<=m_LearnIdx)
            {
                m_LearnIdx = -1;//stop learning
            }
        }
    }
}
    
void Mode2Handler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t /*Velocity*/)
{
    //
    Serial.print("M2 Off ");
    Serial.print(Channel, HEX);
    Serial.print(" ");
    Serial.print(MidiNote, HEX);
    //
    Serial.print(" L ");
    Serial.println(m_LearnIdx);
    //

    if(m_LearnIdx == -1)
    {
        // normal operation
        for(int idx = 0; idx<Size; ++idx)
        {
            m_Out[idx].NoteOff(Channel, MidiNote);
        }
    }
}

void Mode2Handler::updateUI(Midi8UI* ui)
{
    if(ui->mode != 2)
    {
        //Serial.println("Not Mode4");
        return;
    }

    for(int idx = 0; idx<Size; ++idx)
    {
        m_Out[idx].updateUI(ui);
    }

    if(ui->learnBtn.IsFalling())
    {
        Serial.println("Toggle learn!");
        //toggle learn mode on/off
        if(m_LearnIdx ==-1)
        {
            m_LearnIdx = 0;
        }
        else
        {
            m_LearnIdx = -1;
        }
    }
}
