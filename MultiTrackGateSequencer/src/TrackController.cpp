#include "TrackController.h"
#include "CommonState.h"
#include "BitWise.h"

TrackController::TrackController(PinName btnPin, MidiHandler& midiHandler, uint8_t MidiNote, uint8_t MidiChannel)
 : m_TrackBtn(btnPin)
 , m_Player(midiHandler)
 , m_MidiChannel(MidiChannel)
{
    m_Player.Learn(MidiNote, MidiChannel);
}

void TrackController::Tick(CommonState& commonState)
{
    m_TrackBtn.Read();
    if(m_TrackBtn.IsRising())
    {
        if(commonState.mutePressed)
        {
            //toggle mute
            m_Player.Mute(!m_Player.IsMuted());
        } 
        else if(commonState.setPressed)
        {
            //set current Step
            m_Player.SetCurrentStep();
            //play note as well
            m_Player.PlayOn();
        } 
        else if(commonState.clearPressed)
        {
            //clear current step
            m_Player.ClearCurrentStep();
        }
        else if(commonState.learnMode)
        {
            //learn ~ pot analog in
            uint8_t midiNote = 32+commonState.learnValue*64;
            // if(127<=midiNote)
            // {
            //     midiNote = 127;
            // }
            m_Player.Learn(midiNote, m_MidiChannel);
        }
        else
        {
            //play note on
            m_Player.PlayOn();
        }
    }
    else if(m_TrackBtn.IsFalling())
    {
        //play note off (if note is on)
        m_Player.PlayOff();
    }

    // 
    if(commonState.clockIsRising)
    {
        // step on
        m_Player.StepOn();
    }
    else if(commonState.clockIsFalling)
    {
        // step off
        m_Player.StepOff();
    }
}

uint32_t TrackController::GetDisplayPattern() const
{
    uint32_t pattern = m_Player.GetPattern();
    if(!m_Player.IsMuted())
    {
        //invert or clear current step bit to indicate current step
        BitClear(pattern, m_Player.GetCurrentStep());
    }
    // muted => no inversion to indicate current step 
    // alternative? muted => empty pattern?
    return pattern;
}

uint32_t TrackController::GetPattern() const
{
    return m_Player.GetPattern();
}

void TrackController::SetPattern(uint32_t pattern)
{
    m_Player.SetPattern(pattern);
}
