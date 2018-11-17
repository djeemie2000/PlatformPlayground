#include "GateTrack.h"

GateTrackPlayer::GateTrackPlayer(MidiHandler& handler, int NumSteps)
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

void GateTrackPlayer::PlayOn()
{
    // retrigger when playing during step note on
    if(m_NoteOn)
    {
        m_Handler.NoteOff(m_Channel, m_MidiNote, 0x7F);
    }
    m_Handler.NoteOn(m_Channel, m_MidiNote, 0x7f);
    m_NoteOn = true;
}
    
void GateTrackPlayer::PlayOff()
{
    if(m_NoteOn)
    {
        m_Handler.NoteOff(m_Channel, m_MidiNote, 0x7F);
        m_NoteOn = false;
    }
}

int GateTrackPlayer::AdvanceStep(int step) const
{
    ++step;
    if(m_NumSteps<=step)
    {
        step = 0;
    }
    return step;
}


void GateTrackPlayer::StepOn()
{
    //advance step
    m_CurrentStep = AdvanceStep(m_CurrentStep);

    uint32_t CurrentNoteMask = 1<<m_CurrentStep;
    if(m_Pattern & CurrentNoteMask)
    {
        if(!m_Muted)
        {
            PlayOn();//other function??            
        }
    }
}

void GateTrackPlayer::StepOff()
{
    PlayOff();//will check if a note was on, if not, do nothing
}

void GateTrackPlayer::Mute(bool mute)
{
    m_Muted = mute;
}

bool GateTrackPlayer::IsMuted() const
{
    return m_Muted;
}

void GateTrackPlayer::SetStep(int step)
{
    uint32_t noteMask = 1<<step;
    m_Pattern |= noteMask;
}

void GateTrackPlayer::ClearStep(int step)
{
    uint32_t noteMask = 1<<step;
    m_Pattern &= (~noteMask);
}

bool GateTrackPlayer::GetStep(int step) const
{
//    uint32_t bit = step;//TODO check boundaries
    return (m_Pattern>>step)&0x01;
}

int GateTrackPlayer::GetCurrentStep() const
{
    return m_CurrentStep;
}

int GateTrackPlayer::GetNextStep() const
{
    return AdvanceStep(m_CurrentStep);
}

void GateTrackPlayer::Learn(uint8_t MidiNote, uint8_t Channel)
{
    if(MidiNote != m_MidiNote || Channel != m_Channel)
    {
        PlayOff();//checks if a note was on
        m_MidiNote = MidiNote;
        m_Channel = Channel;
    }
}
