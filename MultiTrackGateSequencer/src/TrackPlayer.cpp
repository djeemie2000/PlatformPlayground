#include "TrackPlayer.h"


Track::Track(int NumSteps)
{
    m_Track.m_Pattern = 0x00000000;
    m_Track.m_NumSteps = NumSteps<=32 ? NumSteps : 32;
    m_Track.m_Muted = false;
    m_Track.m_GateDuration = 0.5f;
}

void Track::Mute(bool mute)
{
    m_Track.m_Muted = mute;
}

bool Track::IsMuted() const
{
    return m_Track.m_Muted;
}

void Track::SetStep(int step)
{
    uint32_t noteMask = 1<<step;
    m_Track.m_Pattern |= noteMask;
}

void Track::ClearStep(int step)
{
    uint32_t noteMask = 1<<step;
    m_Track.m_Pattern &= (~noteMask);

}

bool Track::GetStep(int step) const
{
//    uint32_t bit = step;//TODO check boundaries
    return (m_Track.m_Pattern>>step)&0x01;
}

void Track::SetNumSteps(int numSteps)
{
    m_Track.m_NumSteps = numSteps;
}

int Track::GetNumSteps() const
{
    return m_Track.m_NumSteps;
}



TrackPlayer::TrackPlayer(GateHandler& handler, Track& track)
 : m_Handler(handler)
 , m_Track(track)
 , m_NoteOn(false)
// , m_Muted(false)
 , m_CurrentStep(m_Track.GetNumSteps()-1)
 , m_ResetStep(true)
// , m_Pattern(0x00)
{
}

void TrackPlayer::PlayOn()
{
    // retrigger when playing during step note on
    if(m_NoteOn)
    {
        m_Handler.Off();//?????????
    }
    m_Handler.On();
    m_NoteOn = true;
}
    
void TrackPlayer::PlayOff()
{
    if(m_NoteOn)
    {
        m_Handler.Off();
        m_NoteOn = false;
    }
}

int TrackPlayer::AdvanceStep(int step) const
{
    ++step;
    if(m_Track.GetNumSteps()<=step)
    {
        step = 0;
    }
    return step;
}


void TrackPlayer::StepOn()
{
    //advance step
    m_CurrentStep = m_ResetStep ? 0 : AdvanceStep(m_CurrentStep);
    m_ResetStep = false;

    //uint32_t CurrentNoteMask = 1<<m_CurrentStep;
    //if((m_Pattern & CurrentNoteMask) && !m_Muted)
    if(!m_Track.IsMuted() && m_Track.GetStep(m_CurrentStep))
    {
        PlayOn();//other function??            
    }
    else 
    {
        PlayOff();//checks for note on
    }
}

void TrackPlayer::StepOff()
{
    PlayOff();//will check if a note was on, if not, do nothing
}

// void TrackPlayer::Mute(bool mute)
// {
//     m_Muted = mute;
// }

// bool TrackPlayer::IsMuted() const
// {
//     return m_Muted;
// }

// void TrackPlayer::SetStep(int step)
// {
//     uint32_t noteMask = 1<<step;
//     m_Pattern |= noteMask;
// }

// void TrackPlayer::ClearStep(int step)
// {
//     uint32_t noteMask = 1<<step;
//     m_Pattern &= (~noteMask);
// }

// bool TrackPlayer::GetStep(int step) const
// {
// //    uint32_t bit = step;//TODO check boundaries
//     return (m_Pattern>>step)&0x01;
// }

// void TrackPlayer::SetNumSteps(int numSteps)
// {
//     m_NumSteps = numSteps;
// }

void TrackPlayer::ResetStep()
{
    m_ResetStep = true;
}

int TrackPlayer::GetCurrentStep() const
{
    return m_CurrentStep;
}

int TrackPlayer::GetNextStep() const
{
    return AdvanceStep(m_CurrentStep);
}
