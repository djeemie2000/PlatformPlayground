#include "TrackController.h"
#include "CommonState.h"
#include "BitWise.h"
#include "DigitalOutMatrix.h"

TrackController::TrackController(MidiHandler& midiHandler, uint8_t MidiNote, uint8_t MidiChannel, int trackIdx, DigitalOutMatrix& ledMatrix)
 : m_TrackBtn()
 , m_Player(midiHandler)
 , m_MidiChannel(MidiChannel)
 , m_TrackIdx(trackIdx)
 , m_LedMatrix(ledMatrix)
{
    m_Player.Learn(MidiNote, MidiChannel);
}

void TrackController::SetStep(int step)
{
    if(step<m_LedMatrix.NumCols())
    {
        m_Player.SetStep(step);
        m_LedMatrix.Set(m_TrackIdx, step);
    }
}

void TrackController::ClearStep(int step)
{
    if(step<m_LedMatrix.NumCols())
    {
        m_Player.ClearStep(step);
        m_LedMatrix.Clear(m_TrackIdx, step);
    }
}

void TrackController::Tick(const CommonState& commonState, int btn)
{
    m_TrackBtn.Tick(btn);
    if(m_TrackBtn.IsRising())
    {
        if(commonState.mutePressed)
        {
            //toggle mute
            m_Player.Mute(!m_Player.IsMuted());
        } 
        else if(commonState.setPressed)
        {
            // in play mode : depending on closest to current or next step, 
                // set current step (and play) 
                // set next step (do not play, wait until step)
            // in step mode : 
                // always set current step (and play)
            if(commonState.playMode)
            {
                if(commonState.clockOn)
                {
                    //set current Step
                    SetStep(m_Player.GetCurrentStep());
                    //play note as well
                    m_Player.PlayOn();
                }
                else 
                {
                    SetStep(m_Player.GetNextStep());
                    // do not play (yet)
                }
            }
            else
            {
                //set current Step
                SetStep(m_Player.GetCurrentStep());
                //play note as well
                m_Player.PlayOn();
            }
        } 
        else if(commonState.clearPressed)
        {
            //clear current step
            ClearStep(m_Player.GetCurrentStep());
            // stop note as well
            m_Player.PlayOff(); 
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
            m_Player.PlayOn();
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
    else if(m_TrackBtn.Get() && commonState.clearPressed)
    {
        //clear current step
        ClearStep(m_Player.GetCurrentStep());
        // stop note as well
        m_Player.PlayOff();   
    }

    // 
    if(commonState.clockIsRising)
    {
        // step on
        m_Player.StepOn();
        // display : clear current step
        m_LedMatrix.Clear(m_TrackIdx, m_Player.GetCurrentStep());
    }
    else if(commonState.clockIsFalling)
    {
        // step off
        m_Player.StepOff();
        // display: set current step to step set/cleared
        if(m_Player.GetStep(m_Player.GetCurrentStep()))
        {
            m_LedMatrix.Set(m_TrackIdx, m_Player.GetCurrentStep());
        }// else: is already cleared
    }
}

void TrackController::SetPattern(uint32_t pattern)
{
    for(uint32_t step = 0; step<32u; ++step)
    {
        if(BitRead(pattern, step))
        {
            SetStep(step);
        }
        else
        {
            ClearStep(step);
        }
    }
}
