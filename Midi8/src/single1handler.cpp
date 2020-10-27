#include "single1handler.h"
#include "midi8ui.h"
#include "cvfunctions.h"

Single1Handler::Single1Handler()
 : m_LearnIdx(-1)
{
    for(int idx = 0; idx<Size; ++idx)
    {
        m_Channel[idx] = 0xFF;
        m_MidiNote[idx] = 0xFF;
        m_Gate[idx] = 0;
    }
}

void Single1Handler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t /*Velocity*/)
{
    //
    Serial.print("S1 On  ");
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
            if(m_Channel[idx] == Channel && m_MidiNote[idx]==MidiNote)
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
        if(Size<=m_LearnIdx)
        {
            m_LearnIdx = -1;//stop learning
        }
    }
}
    
void Single1Handler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t /*Velocity*/)
{
    //
    Serial.print("S1 Off ");
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
            if(m_Channel[idx] == Channel && m_MidiNote[idx]==MidiNote)
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

void Single1Handler::updateUI(Midi8UI* ui)
{
    for(int idx = 0; idx<4; ++idx)
    {
        if(idx==m_LearnIdx)
        {
            ui->ledsOut.set(idx, LedOutBank::Blink);
            ui->gatesOut.set(idx, 0);
        }
        else if(m_Gate[idx])
        {
            ui->ledsOut.set(idx, LedOutBank::On);
            ui->gatesOut.set(idx, 1);
        }
        else
        {
            ui->ledsOut.set(idx, LedOutBank::Off);
            ui->gatesOut.set(idx, 0);
        }
    }
    for(int idx = 4; idx<8; ++idx)
    {
        if(idx==m_LearnIdx)
        {
            ui->ledsOut.set(idx, LedOutBank::Blink);
            GateOut(ui->cvOut, idx-4, 0);
        }
        else if(m_Gate[idx])
        {
            ui->ledsOut.set(idx, LedOutBank::On);
            GateOut(ui->cvOut, idx-4, 1);
        }
        else
        {
            ui->ledsOut.set(idx, LedOutBank::Off);
            GateOut(ui->cvOut, idx-4, 0);
        }
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
