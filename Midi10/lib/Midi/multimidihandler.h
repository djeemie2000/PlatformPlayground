#pragma once

#include <Arduino.h>
#include "MidiHandler.h"

class MultiMidiHandler : public MidiHandler
{
public:
    MultiMidiHandler()
    {
        for (int idx = 0; idx < Capacity; ++idx)
        {
            m_Handler[idx] = 0;
        }
    }

    void Begin(MidiHandler *h0 = 0, MidiHandler *h1 = 0, MidiHandler *h2 = 0)
    {
        m_Handler[0] = h0;
        m_Handler[1] = h1;
        m_Handler[2] = h2;
    }

    // voice commands
    void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
    {
        for (int idx = 0; idx < Capacity; ++idx)
        {
            if (m_Handler[idx])
            {
                m_Handler[idx]->NoteOn(Channel, MidiNote, Velocity);
            }
        }
    }
    void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
    {
        for (int idx = 0; idx < Capacity; ++idx)
        {
            if (m_Handler[idx])
            {
                m_Handler[idx]->NoteOff(Channel, MidiNote, Velocity);
            }
        }
    }
    // void AfterTouch(uint8_t Channel, uint8_t MidiNote, uint8_t Pressure)
    // {
    //     for (int idx = 0; idx < Capacity; ++idx)
    //     {
    //         if (m_Handler[idx])
    //         {
    //             m_Handler[idx]->AfterTouch(Channel, MidiNote, Pressure);
    //         }
    //     }
    // }
    void ContinuousController(uint8_t Channel, uint8_t Controller,
                              uint8_t Value)
    {
        for (int idx = 0; idx < Capacity; ++idx)
        {
            if (m_Handler[idx])
            {
                m_Handler[idx]->ContinuousController(Channel, Controller, Value);
            }
        }
    }
    // void ProgramChange(uint8_t Channel, uint8_t Program)
    // {
    //     for (int idx = 0; idx < Capacity; ++idx)
    //     {
    //         if (m_Handler[idx])
    //         {
    //             m_Handler[idx]->ProgramChange(Channel, Program);
    //         }
    //     }
    // }
    // void ChannelPressure(uint8_t Channel, uint8_t Pressure)
    // {
    //     for (int idx = 0; idx < Capacity; ++idx)
    //     {
    //         if (m_Handler[idx])
    //         {
    //             m_Handler[idx]->ChannelPressure(Channel, Pressure);
    //         }
    //     }
    // }
    void PitchWheel(uint8_t Channel, int Bend)
    {
        for (int idx = 0; idx < Capacity; ++idx)
        {
            if (m_Handler[idx])
            {
                m_Handler[idx]->PitchWheel(Channel, Bend);
            }
        }
    } // centered = 0 ! range +/- 8192 ( 2^13 )

    // system common
    // void QuarterFrameMessage(uint8_t TimeCode)
    // {
    //     for (int idx = 0; idx < Capacity; ++idx)
    //     {
    //         if (m_Handler[idx])
    //         {
    //             m_Handler[idx]->QuarterFrameMessage(TimeCode);
    //         }
    //     }
    // }
    // void SongPositionPointer(int MidiBeat)
    // {
    //     for (int idx = 0; idx < Capacity; ++idx)
    //     {
    //         if (m_Handler[idx])
    //         {
    //             m_Handler[idx]->SongPositionPointer(MidiBeat);
    //         }
    //     }
    // }
    // void SongSelect(uint8_t SongNumber)
    // {
    //     for (int idx = 0; idx < Capacity; ++idx)
    //     {
    //         if (m_Handler[idx])
    //         {
    //             m_Handler[idx]->SongSelect(SongNumber);
    //         }
    //     }
    // }
    // void TuneRequest()
    // {
    //     for (int idx = 0; idx < Capacity; ++idx)
    //     {
    //         if (m_Handler[idx])
    //         {
    //             m_Handler[idx]->TuneRequest();
    //         }
    //     }
    // }

    // system realtime
    void MidiClock()
    {
        for (int idx = 0; idx < Capacity; ++idx)
        {
            if (m_Handler[idx])
            {
                m_Handler[idx]->MidiClock();
            }
        }
    }
    void MidiStart()
    {
        for (int idx = 0; idx < Capacity; ++idx)
        {
            if (m_Handler[idx])
            {
                m_Handler[idx]->MidiStart();
            }
        }
    }
    void MidiContinue()
    {
        for (int idx = 0; idx < Capacity; ++idx)
        {
            if (m_Handler[idx])
            {
                m_Handler[idx]->MidiContinue();
            }
        }
    }
    void MidiStop()
    {
        for (int idx = 0; idx < Capacity; ++idx)
        {
            if (m_Handler[idx])
            {
                m_Handler[idx]->MidiStop();
            }
        }
    }
    // void ActiveSense()
    // {
    //     for (int idx = 0; idx < Capacity; ++idx)
    //     {
    //         if (m_Handler[idx])
    //         {
    //             m_Handler[idx]->ActiveSense();
    //         }
    //     }
    // }
    // void SystemReset()
    // {
    //     for (int idx = 0; idx < Capacity; ++idx)
    //     {
    //         if (m_Handler[idx])
    //         {
    //             m_Handler[idx]->SystemReset();
    //         }
    //     }
    // }

    // system exclusive (sysex)

private:
    static const int Capacity = 3;
    MidiHandler *m_Handler[Capacity];
};
