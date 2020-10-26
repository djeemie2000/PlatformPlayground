#include "monopg2out.h"
#include "cvfunctions.h"
#include "midi8ui.h"

MonoPG2Out::MonoPG2Out()
: m_Idx(0)
, m_Learn(false)
, m_Channel(0xFF)
, m_Stack() 
{}

void MonoPG2Out::Begin(int idx)
{
  m_Idx = idx;
  m_Learn = false;//??
  m_Channel = 0xFF;
  m_Stack.Clear();
}

void MonoPG2Out::NoteOn(uint8_t channel, uint8_t midiNote)
{
    if(m_Learn)
    {
        m_Stack.Clear();
        m_Channel = channel;
        m_Learn = false;
    }
    if(m_Channel == channel)
    {
        m_Stack.NoteOn(midiNote);
    }
}

void MonoPG2Out::NoteOff(uint8_t channel, uint8_t midiNote)
{
    if(m_Channel == channel)
    {
        m_Stack.NoteOff(midiNote);
    }
}

void MonoPG2Out::updateUI(Midi8UI* ui)
{
    if(m_Learn)
    {
        LedOut(ui->ledsOut, m_Idx, LedOutBank::Blink);
        LedOut(ui->ledsOut, m_Idx+4, LedOutBank::Blink); 

        GateOut(ui->gatesOut, m_Idx, 0);
    }
    else
    {
        // outputs
        int gate = 0<m_Stack.Size() ? 1:0;
        GateOut(ui->gatesOut, m_Idx, gate);
        if(gate)
        {
            PitchOut(ui->cvOut, m_Idx, m_Stack.Note(0));
        }
        LedOut(ui->ledsOut, m_Idx, gate);
        LedOut(ui->ledsOut, m_Idx+4, gate); 
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