#include "TrackController.h"
#include "CommonState.h"
#include "DigitalOutMatrix.h"
#include "GSData.h"
#include "MemController.h"

TrackController::TrackController(GateHandler& handler, int trackIdx, GSTrack* track)
 : m_TrackBtn()
 , m_Track(track)
 , m_Player(handler, m_Track)
 , m_TrackIdx(trackIdx)
{
    SetTrack(track);
}

void TrackController::Tick(const CommonState& commonState, int trackBtn, MemController& memController)
{
    if(commonState.resetStepPressed)
    {
        // trigger reset step on player
        m_Player.ResetStep();
    }

    m_GateOut.SetDuration(m_Track.m_Track->m_GateDuration);
    m_GateOut.Tick(commonState.clockCntr, commonState.clockPeriod);

    m_TrackBtn.Tick(trackBtn);

    if(m_TrackBtn.IsRising())
    {
        if(commonState.selectPatternPressed)
        {
            memController.SelectPattern(m_TrackIdx);
        }
        else if(commonState.selectBankPressed)
        {
            memController.SelectBank(m_TrackIdx);
        }
        else if(commonState.savePatternsPressed)
        {
            //ignore
        }
        else if(commonState.mutePressed)
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
                m_Track.SetStep(m_Player.GetCurrentStep());
                //play note as well
                m_Player.PlayOn();
            }
            else 
            {
                m_Track.SetStep(m_Player.GetNextStep());
                // do not play (yet)
            } 
        } 
        else if(commonState.clearPressed)
        {
            //clear current step
            m_Track.ClearStep(m_Player.GetCurrentStep());//ClearStep();

            // stop note as well
            m_Player.PlayOff(); 
        }
        else
        {
            // 'live' play note on
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
        m_Track.ClearStep(m_Player.GetCurrentStep());
        // stop note as well
        m_Player.PlayOff();   
    }


    // 
    //TODO clock rising in case of gate length 1??
    if(m_GateOut.IsRising() || commonState.forceAdvanceOn)
    {
        // step on
        m_Player.StepOn();
    }
    else if(m_GateOut.IsFalling() || commonState.forceAdvanceOff)
    {
        // step off
        m_Player.StepOff();
    }
}

void TrackController::SetTrack(GSTrack* track)
{
    m_Track.m_Track = track;
}

void TrackController::Update(DigitalOutMatrix& ledMatrix)
{
    for(int step = 0; step<32 && step<ledMatrix.NumCols(); ++step)
    {
        if(m_Track.GetStep(step))
        {
            ledMatrix.Set(m_TrackIdx, step);
        }
        else
        {
            ledMatrix.Clear(m_TrackIdx, step);
        }
    }
    // if track not muted => invert current step
    if(!m_Track.IsMuted())
    {
        if(m_Track.GetStep(m_Player.GetCurrentStep()))
        {
            ledMatrix.Clear(m_TrackIdx, m_Player.GetCurrentStep());
        }
        else
        {
            ledMatrix.Set(m_TrackIdx, m_Player.GetCurrentStep());
        }        
    }
}
