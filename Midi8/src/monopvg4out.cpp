#include "monopvg4out.h"
#include "cvfunctions.h"
#include "midi8ui.h"

MonoPVG4Out::MonoPVG4Out()
: m_Idx(0)
, m_Ui(0)
, m_Learn(false)
, m_Channel(0xFF)
, m_Stack() 
{}

void MonoPVG4Out::Begin(int idx, Midi8UI* ui)
{
  m_Idx = idx;
  m_Ui = ui;
  m_Learn = false;//??
  m_Channel = 0xFF;
  m_Stack.Clear();
}

void MonoPVG4Out::NoteOn(uint8_t channel, uint8_t midiNote, uint8_t velocity)
{
    if(m_Learn)
    {
        m_Stack.Clear();
        m_Channel = channel;
    }
    if(m_Channel == channel)
    {
        m_Stack.NoteOn(midiNote, velocity);
    }
}

void MonoPVG4Out::NoteOff(uint8_t channel, uint8_t midiNote, uint8_t velocity)
{
    if(m_Channel == channel)
    {
        m_Stack.NoteOff(midiNote);
    }
}

void MonoPVG4Out::updateUI(Midi8UI* ui)
{
    if(m_Learn)
    {
        LedOut(ui->ledsOut, m_Idx, LedOutBank::Blink);
        LedOut(ui->ledsOut, m_Idx+1, LedOutBank::Blink);
        LedOut(ui->ledsOut, m_Idx+4, LedOutBank::Blink); 
        LedOut(ui->ledsOut, m_Idx+5, LedOutBank::Blink);

        GateOut(ui->gatesOut, m_Idx, 0);
        GateOut(ui->gatesOut, m_Idx+1, 0);
    }
    else
    {
        // outputs
        int gate = 0<m_Stack.Size() ? 1:0;
        GateOut(ui->gatesOut, m_Idx, gate);
        GateOut(ui->gatesOut, m_Idx+1, gate);
        if(gate)
        {
            PitchOut(ui->cvOut, m_Idx, m_Stack.Note(0));
            VelocityOut(ui->cvOut, m_Idx+1, m_Stack.Velocity(0));
        }
        LedOut(ui->ledsOut, m_Idx, gate);
        LedOut(ui->ledsOut, m_Idx+1, gate);
        LedOut(ui->ledsOut, m_Idx+4, gate); 
        LedOut(ui->ledsOut, m_Idx+5, gate);
    }
}

void MonoPVG4Out::Learn(bool learn)
{
    m_Learn = learn;
    //TODO start blinking the leds!!!!!! 
    // how?????????????
}

bool MonoPVG4Out::IsLearning() const
{
    return m_Learn;
}