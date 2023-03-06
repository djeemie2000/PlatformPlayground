#pragma once
#include <Arduino.h>

template<int Size>
class GateOutBank;
class LedOut;

template<int NumGates>
class Midi2ClockTick
{
public:
    Midi2ClockTick();

    void Begin(GateOutBank<NumGates>* gates, uint8_t ticksOn, uint8_t ticksPeriod);

    void OnMessage(uint8_t byte);
    bool ClockIsRunning() const;

private:
    GateOutBank<NumGates>* m_Gates;

    uint8_t m_TicksOn;
    uint8_t m_TicksPeriod;
    uint8_t m_TicksCounter;

    bool m_ClockIsRunning;
    bool m_DoReset;
};

#include "midi2clock.h"
#include "gateoutbank.h"
#include "ledout.h"
#include "EEPROM.h"

template<int NumGates>
Midi2ClockTick<NumGates>::Midi2ClockTick()
    : m_Gates(0)
    , m_TicksOn(1)
    , m_TicksPeriod(2)
    , m_TicksCounter(0x00)
    , m_ClockIsRunning(true)
    , m_DoReset(true)
{
}

template<int NumGates>
void Midi2ClockTick<NumGates>::Begin(GateOutBank<NumGates>* gates, uint8_t ticksOn, uint8_t ticksPeriod)
{
    m_Gates = gates;

    // counter 0 -> 3/6 -> 4PPQ
    m_TicksOn = ticksOn;
    m_TicksPeriod = ticksPeriod;

    m_TicksCounter = 0x00;

    for(int gate = 0; gate<NumGates; ++gate)
    {
        m_Gates->GateOff(gate);
    }
}

template<int NumGates>
void Midi2ClockTick<NumGates>::OnMessage(uint8_t byte)
{
    if(byte == 0xFA)
    {
        // midi start => reset pulses on        
        m_ClockIsRunning = true;
        m_DoReset = true;
    }
    else if(byte == 0xFC)
    {
        // midi stop
        m_ClockIsRunning = false;
        // all (clock + reset) outputs low
        for(int gate = 0; gate<NumGates; ++gate)
        {        
            m_Gates->GateOff(gate);
        }
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

                m_TicksCounter = 0x00;                
            }
            else 
            {
                ++m_TicksCounter;
                if(m_TicksPeriod<=m_TicksCounter)
                {
                    m_TicksCounter = 0;
                }
            }

            if(m_TicksCounter<m_TicksOn)
            {
                for(int gate = 0; gate<NumGates; ++gate)
                {        
                    m_Gates->GateOn(gate);
                }
            }
            else
            {
                for(int gate = 0; gate<NumGates; ++gate)
                {        
                    m_Gates->GateOff(gate);
                }
            }            
        }
    }
}

template<int NumGates>
bool Midi2ClockTick<NumGates>::ClockIsRunning() const
{
    return m_ClockIsRunning;
}

