#pragma once
#include <Arduino.h>
#include "step5board.h"

struct Step5App
{
    int m_Step;
    int m_Length;
    bool m_ResetFlag;
    //TODO cvclock on gate outputs 

    Step5App() 
        : m_Step(0)
        , m_Length(5)
        , m_ResetFlag(false)
    {}

    void Update(Step5Board& board)
    {
        board.lengthIn.Read();
        board.clockResetIn.Update();

        int prevLength = m_Length;
        m_Length = 10*board.lengthIn.Get()>>10;

        bool clockRising = board.clockResetIn.IsClicked(0);
        bool resetRising = board.clockResetIn.IsClicked(1) || board.resetBtnIn.IsClicked(0); 
        int prevStep = m_Step;

        if(resetRising)
        {
            m_ResetFlag = true;
            // TODO m_Step=0 already here?
        }

        if(clockRising)
        {
            // advance step
            if(m_ResetFlag)
            {
                m_Step = 0;
                m_ResetFlag = false;
            }
            else 
            {
                ++m_Step;
                if(m_Length<=m_Step)
                {
                    m_Step = 0;
                }            
            }
        }

        if(prevLength<=5)
        {
            board.stepOut.Clear(prevStep);
            board.stepOut.Clear(prevStep+5);
        }
        else
        {
            board.stepOut.Clear(prevStep);
        }

        if(m_Length<=5)
        {
            board.stepOut.Set(m_Step);
            board.stepOut.Set(m_Step+5);

            // TODO or the other way around?
            board.selectGateOut.Set(0);
            board.selectGateOut.Clear(1);
        }
        else 
        {
            // all step outs low exceptcurrent step
            board.stepOut.Set(m_Step);

            // select depeding on step<=5 or not
            if(m_Step<=5)
            {
                board.selectGateOut.Set(0);
                board.selectGateOut.Set(1);
            }
            else 
            {
                board.selectGateOut.Clear(0);
                board.selectGateOut.Clear(1);
            }
        }

        // read bus CV AFTER setting current step/select
        // gate out length depending on bus CV
        board.bus1In.Read();
        board.bus2In.Read();

        int bus1Cv = board.bus1In.Get();
        int bus2Cv = board.bus2In.Get();
        // TODO gate out ~CVClock
        // fornow, gate out ~
        if(bus1Cv<512)
        {
            board.selectGateOut.Clear(2);
        }
        else 
        {
            board.selectGateOut.Set(2);
        }

        if(bus2Cv<512)
        {
            board.selectGateOut.Clear(3);
        }
        else 
        {
            board.selectGateOut.Set(3);
        }

        bool clockHigh = board.clockResetIn.Get(0);
        if(clockHigh)
        {
            board.LedOn();
        }
        else
        {
            board.LedOff();
        }
    }


};



