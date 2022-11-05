#include "midi2clock.h"

Midi2Clock::Midi2Clock()
    : m_ClockIsRunning(true)
    , m_DoReset(true)
    , m_StatusLed(-1)
{
    for (int gate = 0; gate < NumGates; ++gate)
    {
        m_OutputPin[gate] = -1;
    }

    for (int cntr = 0; cntr < NumCounters; ++cntr)
    {
        m_TicksOn[cntr] = 1;
        m_TicksPeriod[cntr] = 2;
        m_TicksCounter[cntr] = 0x00;
    }
}

void Midi2Clock::Begin(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3,
                      uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7,
                      uint8_t statusLed)
{
    m_OutputPin[0] = pin0;
    m_OutputPin[1] = pin1;
    m_OutputPin[2] = pin2;
    m_OutputPin[3] = pin3;

    m_OutputPin[4] = pin4;
    m_OutputPin[5] = pin5;
    m_OutputPin[6] = pin6;
    m_OutputPin[7] = pin7;

    m_StatusLed = statusLed;

    // counter 0 -> 3/6 -> 4PPQ
    m_TicksOn[0] = 3;
    m_TicksPeriod[0] = 6;
    // counter 1 -> 2/4 -> 6PPQ
    m_TicksOn[1] = 2;
    m_TicksPeriod[1] = 4;
    // counter 2 -> 1/2 -> 12PPQ
    m_TicksOn[2] = 1;
    m_TicksPeriod[2] = 2;
    // counter 3 -> 12/24 -> 1PPQ
    m_TicksOn[0] = 12;
    m_TicksPeriod[0] = 24;

    for (int cntr = 0; cntr < NumCounters; ++cntr)
    {
        m_TicksCounter[cntr] = 0x00;
    }

    for(int gate = 0; gate<NumGates; ++gate)
    {
        if(0<=m_OutputPin[gate])
        {
            pinMode(m_OutputPin[gate], OUTPUT);
        }
        digitalWrite(m_OutputPin[gate], LOW);
    }

    pinMode(m_StatusLed, OUTPUT);
    digitalWrite(m_StatusLed, HIGH);
}

void Midi2Clock::OnMessage(uint8_t byte)
{
    if(byte == 0xFA)
    {
        // midi start => reset pulses on        
        m_ClockIsRunning = true;
        m_DoReset = true;

        digitalWrite(m_OutputPin[0], HIGH);
        digitalWrite(m_OutputPin[1], HIGH);
    }
    else if(byte == 0xFC)
    {
        // midi stop
        m_ClockIsRunning = false;
        // all (clock + reset) outputs low
        for(int gate = 0; gate<NumGates; ++gate)
        {        
            digitalWrite(m_OutputPin[gate], LOW);
        }
        digitalWrite(m_StatusLed, HIGH);//always on when clock is not running
    }
    // midi continue => ignored or threated as midi start?
    else if(byte == 0XF8)
    {
        // midi clock => update counters
        // ignore if clock is not running
        if(m_ClockIsRunning)
        {
            if(m_DoReset)
            {
                // if was reset => reset pulses off, reset all counters
                m_DoReset = false;

                for (int cntr = 0; cntr < NumCounters; ++cntr)
                {
                    m_TicksCounter[cntr] = 0x00;
                }

                digitalWrite(m_OutputPin[0], LOW);
                digitalWrite(m_OutputPin[1], LOW);
            }
            else 
            {
                for (int cntr = 0; cntr < NumCounters; ++cntr)
                {
                    ++m_TicksCounter[cntr];
                    if(m_TicksCounter[cntr]<=m_TicksPeriod[cntr])
                    {
                        m_TicksCounter[cntr] = 0;
                    }
                }
            }

            digitalWrite(m_OutputPin[2], m_TicksCounter[0]<m_TicksPeriod[0] ? HIGH : LOW);
            digitalWrite(m_OutputPin[3], m_TicksCounter[0]<m_TicksPeriod[0] ? HIGH : LOW);

            digitalWrite(m_OutputPin[4], m_TicksCounter[1]<m_TicksPeriod[1] ? HIGH : LOW);
            digitalWrite(m_OutputPin[5], m_TicksCounter[1]<m_TicksPeriod[1] ? HIGH : LOW);

            digitalWrite(m_OutputPin[6], m_TicksCounter[2]<m_TicksPeriod[2] ? HIGH : LOW);
            digitalWrite(m_OutputPin[7], m_TicksCounter[2]<m_TicksPeriod[2] ? HIGH : LOW);

            digitalWrite(m_StatusLed, m_TicksCounter[3]<m_TicksPeriod[3] ? HIGH : LOW);
        }
    }
}

void Midi2Clock::PrintState()
{
    //TODO print counters
}
