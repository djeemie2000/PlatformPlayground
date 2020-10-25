#include "mode4handler.h"
#include "midi8ui.h"
#include "cvfunctions.h"

Mode4Handler::Mode4Handler()
 : m_LearnIdx(-1)
{
    for(int idx = 0; idx<Size; ++idx)
    {
        int column = 2*idx;
        m_Out[idx].Begin(column);
    }
}

void Mode4Handler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    //
    Serial.print("M4 On  ");
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
            m_Out[idx].NoteOn(Channel, MidiNote, Velocity);
        }
    }
    else
    {
        //learning
        m_Out[m_LearnIdx].Learn(true);
        m_Out[m_LearnIdx].NoteOn(Channel, MidiNote, Velocity);
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
    
void Mode4Handler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    //
    Serial.print("M4 Off ");
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
            m_Out[idx].NoteOff(Channel, MidiNote, Velocity);
        }
    }
}

void Mode4Handler::updateUI(Midi8UI* ui)
{
    if(ui->mode != 4)
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
