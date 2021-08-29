#pragma once
#include <Arduino.h>

class MidiLooperClock
{
public:
    //TODO duty cycle ? now 50%
    MidiLooperClock();

    void Tick(int bpmUnmapped);

    void Tick();
    void SetBPM(int bpm);
    int GetBPM() const;

    int Get() const;

private:
    int m_ClockPeriod;
    int m_ClockCounter;
    int m_BPM;
};
