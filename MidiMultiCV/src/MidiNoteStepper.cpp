#include "MidiNoteStepper.h"

MidiNoteStepper::MidiNoteStepper(){}

void MidiNoteStepper::Tick(int Gate)
{
    m_PrevGate=m_Gate;
    m_Gate = Gate;
    if(!m_PrevGate && m_Gate)
    {
        Advance();
    }
}

void MidiNoteStepper::Advance()
{
    ++m_CurrStep;
    if(m_NumSteps<=m_CurrStep)
    {
        m_CurrStep = 0;
    }    
}

void MidiNoteStepper::SetBaseNote(uint8_t MidiNote)
{
    m_PlayBaseNote = MidiNote;
}

void MidiNoteStepper::Clear()
{
    m_NumSteps = 0;
    m_BaseNote = -1;
    m_PlayBaseNote = -1;
    m_CurrStep = 0;
}

void MidiNoteStepper::AddStep(uint8_t MidiNote, uint8_t Velocity)
{
    if(m_NumSteps<Capacity)
    {
        m_Step[m_NumSteps].Gate = true;
        m_Step[m_NumSteps].MidiNote = MidiNote;
        m_Step[m_NumSteps].Velocity= Velocity;
        ++m_NumSteps;

        if(-1==m_BaseNote)
        {
            m_BaseNote = MidiNote;
            m_PlayBaseNote = MidiNote;
        }
    }
}

void MidiNoteStepper::AddSkipStep()
{
    if(m_NumSteps<Capacity)
    {
        m_Step[m_NumSteps].Gate = false;
        if(0<m_NumSteps)
        {
            m_Step[m_NumSteps].MidiNote = m_Step[m_NumSteps-1].MidiNote;
            m_Step[m_NumSteps].Velocity= m_Step[m_NumSteps-1].Velocity;
        }
        ++m_NumSteps;
    }
}

int MidiNoteStepper::CurrStep() const
{
    return m_CurrStep;
}

int MidiNoteStepper::NumSteps() const
{
    return m_NumSteps;
}

bool MidiNoteStepper::Skip() const
{
    return !m_Step[m_CurrStep].Gate;
}

uint8_t MidiNoteStepper::Velocity() const
{
    return m_Step[m_CurrStep].Velocity;
}

uint8_t MidiNoteStepper::MidiNote() const
{
    // add offset (play vs record base note) and crop midinote
    int note = m_Step[m_CurrStep].MidiNote + m_PlayBaseNote-m_BaseNote;
    if(note<0)
    {
        note = 0;
    } else if(127<note)
    {
        note = 127;
    }
    return note;
}

bool MidiNoteStepper::Gate() const
{
    return m_NumSteps && m_Step[m_CurrStep].Gate && m_Gate;
}

MidiNoteStepper::EState MidiNoteStepper::CurrState() const
{
    if(0<m_NumSteps && !m_Step[m_CurrStep].Gate)
    {
        return Off;
    }
    if(m_PrevGate==m_Gate)
    {
        return m_Gate ? On : Off;
    }
    return m_Gate ? NoteOn : NoteOff;
}
