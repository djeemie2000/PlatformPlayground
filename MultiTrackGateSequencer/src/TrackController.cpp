#include "TrackController.h"
#include "CommonState.h"
#include "BitWise.h"
#include "DigitalOutMatrix.h"

TrackController::TrackController(GateHandler& handler, int trackIdx, DigitalOutMatrix& ledMatrix)
 : m_TrackBtn()
 , m_AllTrackBtn()
 , m_Track(32)
 , m_Player(handler, m_Track)
 , m_TrackIdx(trackIdx)
 , m_LedMatrix(ledMatrix)
{
    m_GateOut.SetDuration(0.5f);
}

void TrackController::SetStep(int step)
{
    m_Track.SetStep(step);
    if(step<m_LedMatrix.NumCols())
    {
        m_LedMatrix.Set(m_TrackIdx, step);
    }
}

void TrackController::ClearStep(int step)
{
    m_Track.ClearStep(step);
    if(step<m_LedMatrix.NumCols())
    {
        m_LedMatrix.Clear(m_TrackIdx, step);
    }
}

void TrackController::Tick(const CommonState& commonState, int btn, int allBtn)
{
    m_TrackBtn.Tick(btn);
    m_AllTrackBtn.Tick(allBtn);

    if(commonState.resetStepPressed)
    {
        // trigger reset step on player
        m_Player.ResetStep();
    }

    m_GateOut.Tick(commonState.clockCntr, commonState.clockPeriod);
    if(m_TrackBtn.IsRising() || m_AllTrackBtn.IsRising())
    {
        if(commonState.mutePressed)
        {
            //toggle mute
            m_Track.Mute(!m_Track.IsMuted());
        } 
        else if(commonState.setPressed)
        {
            // in play mode : depending on closest to current or next step, 
                // set current step (and play) 
                // set next step (do not play, wait until step)
            // in step mode : 
                // always set current step (and play)
            if(commonState.clockCntr<commonState.clockPeriod/2)
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
        else if(commonState.clearPressed)
        {
            //clear current step
            ClearStep(m_Player.GetCurrentStep());
            // stop note as well
            m_Player.PlayOff(); 
        }
        else
        {
            // 'live' play note on
            m_Player.PlayOn();
        }
    }
    else if(m_TrackBtn.IsFalling() || m_AllTrackBtn.IsFalling())
    {
        //play note off (if note is on)
        m_Player.PlayOff();
    }
    else if((m_TrackBtn.Get() || m_AllTrackBtn.Get()) && commonState.clearPressed)
    {
        //clear current step
        ClearStep(m_Player.GetCurrentStep());
        // stop note as well
        m_Player.PlayOff();   
    }


    // 
    if(m_GateOut.IsRising())//TODO clock rising in case of gate length 1??
    {
        //clear previous step if needed
        if(!m_Track.GetStep(m_Player.GetCurrentStep()))
        {        
            m_LedMatrix.Clear(m_TrackIdx, m_Player.GetCurrentStep());
        }

        // step on
        m_Player.StepOn();
        // display : invert current step to indicate current step is playing
        // if muted: no change 
        if(!m_Track.IsMuted())
        {
            if(m_Track.GetStep(m_Player.GetCurrentStep()))
            {
                m_LedMatrix.Clear(m_TrackIdx, m_Player.GetCurrentStep());
            }
            else
            {
                m_LedMatrix.Set(m_TrackIdx, m_Player.GetCurrentStep());
            }
        }
    }
    else if(m_GateOut.IsFalling())
    {
        // step off
        m_Player.StepOff();
        // display: if set, non inverted current step 
        //          if clear, show during whole step
        if(m_Track.GetStep(m_Player.GetCurrentStep()))
        {
            m_LedMatrix.Set(m_TrackIdx, m_Player.GetCurrentStep());
        }
    }
}

void TrackController::SetPattern(uint32_t pattern, int length)
{
    // TODO 
    // first, set pattern/length on m_Track
    // then, apply on led matrix
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
    m_Track.SetNumSteps(length);
}
