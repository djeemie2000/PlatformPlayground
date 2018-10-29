#include "GateTrack.h"

GateTrack::GateTrack(MidiHandler& handler, int NumSteps)
 : m_NumSteps(NumSteps<=32 ? NumSteps : 32)
 , m_Handler(handler)
 , m_Channel(4)
 , m_MidiNote(0x30)
 , m_NoteOn(false)
 , m_Muted(false)
 , m_CurrentStep(m_NumSteps-1)
 , m_Pattern(0x00)
{
}

void GateTrack::PlayOn()
{
    // retrigger when playing during step note on
    if(m_NoteOn)
    {
        m_Handler.NoteOff(m_Channel, m_MidiNote, 0x7F);
    }
    m_Handler.NoteOn(m_Channel, m_MidiNote, 0x7f);
    m_NoteOn = true;
}
    
void GateTrack::PlayOff()
{
    if(m_NoteOn)
    {
        m_Handler.NoteOff(m_Channel, m_MidiNote, 0x7F);
        m_NoteOn = false;
    }
}

void GateTrack::StepOn()
{
    //advance step
    ++m_CurrentStep;
    if(m_NumSteps<=m_CurrentStep)
    {
        m_CurrentStep = 0;
    }
    uint32_t CurrentNoteMask = 1<<m_CurrentStep;
    if(m_Pattern & CurrentNoteMask)
    {
        PlayOn();//other function??
    }
}

void GateTrack::StepOff()
{
    PlayOff();//will check if a note was on, if not, do nothing
}

void GateTrack::Mute(bool mute)
{
    m_Muted = mute;
}

bool GateTrack::IsMuted() const
{
    return m_Muted;
}

void GateTrack::SetCurrentStep()
{
    uint32_t CurrentNoteMask = 1<<m_CurrentStep;
    m_Pattern |= CurrentNoteMask;
}
    
void GateTrack::ClearCurrentStep()
{
    uint32_t CurrentNoteMask = 1<<m_CurrentStep;
    m_Pattern &= (~CurrentNoteMask);
}

int GateTrack::GetCurrentStep() const
{
    return m_CurrentStep;
}

uint32_t GateTrack::GetPattern() const
{
    return m_Pattern;
}

void GateTrack::Learn(uint8_t MidiNote, uint8_t Channel)
{
    if(MidiNote != m_MidiNote || Channel != m_Channel)
    {
        PlayOff();//checks if a note was on
        m_MidiNote = MidiNote;
        m_Channel = Channel;
    }
}
