#pragma once
#include <Arduino.h>
#include "step5board.h"
#include "cvclock.h"

#define FAKECLOCK 1

struct Step5App
{
    int m_Step;
    int m_Length;
    bool m_ResetFlag;
    // TODO cvclock on gate outputs
    CVClock clockOutBus1;
    CVClock clockOutBus2;

    Step5App()
        : m_Step(0), m_Length(5), m_ResetFlag(false), clockOutBus1(), clockOutBus2()
    {
    }

    void Begin()
    {
        clockOutBus1.Begin();
        clockOutBus2.Begin();
    }

    void Update(Step5Board &board)
    {
        board.lengthIn.Read();
        board.clockResetIn.Update();

        int prevLength = m_Length;
        m_Length = 1 + (10 * board.lengthIn.Get() >> 10);

        bool clockRising = board.clockResetIn.IsClicked(0);
        bool clockHigh = board.clockResetIn.Get(0);
        bool resetRising = board.clockResetIn.IsClicked(1) || board.resetBtnIn.IsClicked(0);
        int prevStep = m_Step;

#ifdef FAKECLOCK
        board.fakeClock.Tick();
        clockRising = board.fakeClock.IsClicked();
        clockHigh = board.fakeClock.Get();
#endif

        if (resetRising)
        {
            m_ResetFlag = true;
            // TODO m_Step=0 already here?
        }

        if (clockRising)
        {
            // advance step
            if (m_ResetFlag)
            {
                m_Step = 0;
                m_ResetFlag = false;
            }
            else
            {
                ++m_Step;
                if (m_Length <= m_Step)
                {
                    m_Step = 0;
                }
            }
        }

        // only clear/update prev upon change
        if (clockRising) // prevLength!=m_Length || prevStep != m_Step)
        {
            if (prevLength <= 5)
            {
                board.stepOut.Clear(prevStep);
                board.stepOut.Clear(prevStep + 5);
            }
            else
            {
                board.stepOut.Clear(prevStep);
            }

            if (m_Length <= 5)
            {
                // bus 1 to cv out 1
                // bus 2 to cv out 2
                board.selectGateOut.Clear(0);
                board.selectGateOut.Set(1);

                board.stepOut.Set(m_Step);
                board.stepOut.Set(m_Step + 5);
            }
            else
            {
                // select depending on step<=5 or not
                if (m_Step < 5)
                {
                    // bus 1 to cv out 1 and to cv out 2
                    board.selectGateOut.Clear(0);
                    board.selectGateOut.Clear(1);
                }
                else
                {
                    // bus 2 to cv out 1 and to cv out 2
                    board.selectGateOut.Set(0);
                    board.selectGateOut.Set(1);
                }

                // all step outs low exceptcurrent step
                board.stepOut.Set(m_Step);
            }

            // need 'settling time' before reading CV?
            delay(8);
        }

        // read bus CV AFTER setting current step/select
        // gate out length depending on bus CV
        board.bus1In.Read();
        board.bus2In.Read();

        int bus1Cv = board.bus1In.Get();
        int bus2Cv = board.bus2In.Get();

        // gate out ~CVClock
        if (m_Length <= 5)
        {
            clockOutBus1.Update(clockHigh, bus1Cv);
            clockOutBus2.Update(clockHigh, bus2Cv);
        }
        else if (m_Step < 5)
        {
            clockOutBus1.Update(clockHigh, bus1Cv);
            clockOutBus2.Update(clockHigh, bus1Cv);
        }
        else
        {
            clockOutBus1.Update(clockHigh, bus2Cv);
            clockOutBus2.Update(clockHigh, bus2Cv);
        }

        if (clockOutBus1.Get())
        {
            board.selectGateOut.Set(2);
        }
        else
        {
            board.selectGateOut.Clear(2);
        }

        if (clockOutBus2.Get())
        {
            board.selectGateOut.Set(3);
        }
        else
        {
            board.selectGateOut.Clear(3);
        }

#ifdef DEBUGOUT
        // TODO serial out all member and local variables
        Serial.print(m_Length);
        Serial.print(' ');
        Serial.print(m_Step);
        Serial.print(' ');
        Serial.print(prevLength);
        Serial.print(' ');
        Serial.print(prevStep);
        Serial.print(' ');
        Serial.print(clockRising);
        Serial.print(' ');
        Serial.print(clockHigh);
        Serial.print(' ');
        Serial.print(bus1Cv);
        Serial.print(' ');
        Serial.print(bus2Cv);
        Serial.print(' ');
        Serial.print(clockOutBus1.Get());
        Serial.print(' ');
        Serial.print(clockOutBus2.Get());
        Serial.println();
#endif
    }
};
