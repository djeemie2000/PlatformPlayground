#include "midi2clock.h"
#include "gateoutbank.h"
#include "ledout.h"
#include "EEPROM.h"

Midi2Clock::Midi2Clock()
    : m_Gates(0)
    , m_LedOut(0)
    , m_ClockIsRunning(true)
    , m_DoReset(true)

{
}

void Midi2Clock::Begin(GateOutBank<Midi2Clock::NumGates>* gates, LedOut* ledOut)
{
    m_Gates = gates;
    m_LedOut = ledOut;

    m_ClockPattern[0] = 0x01000000;//reset -> only on bit 24

    m_ClockPattern[1] = 0x00000FFF;// 2PPQ 6/12
    m_ClockPattern[2] = 0x001C71C7;// 4PPQ 3/6
    m_ClockPattern[3] = 0x00249249;// 8PPQ 1/3
    
    m_ClockPattern[4] = m_ClockPattern[0];//reset -> only on bit 24

    m_ClockPattern[5] = 0x000F0F0F;// 6PPQ 4/8
    m_ClockPattern[6] = 0x00333333;// 6PPQ 2/4
    m_ClockPattern[7] = 0x00555555;// 12PPQ 1/2

    m_Cntr24PPQ = 24;//reset
    m_ClockIsRunning = true;
    m_DoReset = true;

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
        m_Cntr24PPQ = 24;//reset bit

        for(int idx=0; idx<NumGates;++idx)
        {
            if(bitRead(m_ClockPattern[idx], m_Cntr24PPQ))
            {
                m_Gates->GateOn(idx);
            }
            else
            {
                m_Gates->GateOff(idx);
            }
        }
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
    else if(byte == 0xFB)
    {
        // midi continue => ignored or threated as midi start?
        m_ClockIsRunning = true;
        m_LedOut->LedOn();//always on??
    }
    else if(byte == 0XF8)
    {
        // midi clock => update counters
        // ignore if clock is not running
        if(m_ClockIsRunning)
        {
            ++m_Cntr24PPQ;
            if(24<=m_Cntr24PPQ)
            {
                m_Cntr24PPQ = 0;
            }

            if(m_DoReset)
            {
                // if was reset => reset pulses off, reset all counters
                m_DoReset = false;
            }

            // gate out
            for(int idx=0; idx<NumGates;++idx)
            {
                if(bitRead(m_ClockPattern[idx], m_Cntr24PPQ))
                {
                    m_Gates->GateOn(idx);
                }
                else
                {
                    m_Gates->GateOff(idx);
                }
            }

            m_LedOut->LedOn();//always on??
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
    Serial.println(m_Cntr24PPQ, DEC);
    for(int idx = 0; idx<NumCounters;++idx)
    {
        Serial.print(m_ClockPattern[idx], HEX);
        Serial.print(' ');
        Serial.println(bitRead(m_ClockPattern[idx], m_Cntr24PPQ)?1:0, DEC);
    }
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