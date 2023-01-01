#include "midi2clock.h"
#include "gateoutbank.h"
#include "ledout.h"
#include "EEPROM.h"

Midi2Clock::Midi2Clock()
    : m_Gates(0)
    , m_ClockIsRunning(true)
    , m_DoReset(true)

{
    for (int cntr = 0; cntr < NumCounters; ++cntr)
    {
        m_TicksOn[cntr] = 1;
        m_TicksPeriod[cntr] = 2;
        m_TicksCounter[cntr] = 0x00;
    }
}

void Midi2Clock::Begin(GateOutBank<Midi2Clock::NumGates>* gates, LedOut* ledOut)
{
    m_Gates = gates;
    m_LedOut = ledOut;

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
    m_TicksOn[3] = 12;
    m_TicksPeriod[3] = 24;

    for (int cntr = 0; cntr < NumCounters; ++cntr)
    {
        m_TicksCounter[cntr] = 0x00;
    }

    for(int gate = 0; gate<NumGates; ++gate)
    {
        m_Gates->GateOff(gate);
    }

    // status led on
    m_LedOut->LedOn();
}

void Midi2Clock::OnMessage(uint8_t byte)
{
    if(byte == 0xFA)
    {
        // midi start => reset pulses on        
        m_ClockIsRunning = true;
        m_DoReset = true;

        m_Gates->GateOn(0);
        m_Gates->GateOn(1);
    }
    else if(byte == 0xFC)
    {
        // midi stop
        m_ClockIsRunning = false;
        // all (clock + reset) outputs low
        for(int gate = 0; gate<NumGates; ++gate)
        {        
             m_Gates->GateOff(0);
        }

        m_LedOut->LedOn();//always on when clock is not running
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

                m_Gates->GateOff(0);
                m_Gates->GateOff(1);
            }
            else 
            {
                for (int cntr = 0; cntr < NumCounters; ++cntr)
                {
                    ++m_TicksCounter[cntr];
                    if(m_TicksPeriod[cntr]<=m_TicksCounter[cntr])
                    {
                        m_TicksCounter[cntr] = 0;
                    }
                }
            }

            if(m_TicksCounter[0]<m_TicksOn[0])
            {
                m_Gates->GateOn(2);
                m_Gates->GateOn(3);
            }
            else
            {
                m_Gates->GateOff(2);
                m_Gates->GateOff(3);
            }


            if(m_TicksCounter[1]<m_TicksOn[1])
            {
                m_Gates->GateOn(4);
                m_Gates->GateOn(5);
            }
            else
            {
                m_Gates->GateOff(4);
                m_Gates->GateOff(5);
            }

            if(m_TicksCounter[2]<m_TicksOn[2])
            {
                m_Gates->GateOn(6);
                m_Gates->GateOn(7);
            }
            else
            {
                m_Gates->GateOff(6);
                m_Gates->GateOff(7);
            }

            if(m_TicksCounter[3]<m_TicksOn[3])
            {
                m_LedOut->LedOn();
            }
            else
            {
                m_LedOut->LedOff();
            }
        }
    }
}

bool Midi2Clock::ClockIsRunning() const
{
    return m_ClockIsRunning;
}


void Midi2Clock::PrintState()
{
    //TODO print counters
}


void ToggleClockIsRunning(Midi2Clock& midi2Clock)
{
    if(midi2Clock.ClockIsRunning())
    {
        // midi stop
        midi2Clock.OnMessage(0xFC);
    }
    else    
    {
        // midi start
        midi2Clock.OnMessage(0xFA);
    }
}

void Midi2Clock::saveParams(int offset)
{
    int off = offset;
    EEPROM.update(off++, 'C');
    EEPROM.update(off++, 'L');
}

int Midi2Clock::paramSize() const
{
    return 2;
}

void Midi2Clock::loadParams(int offset)
{
    int off = offset;
    if ('C' == EEPROM.read(off++) && 'L' == EEPROM.read(off++))
    {
        //TODO
    }
}