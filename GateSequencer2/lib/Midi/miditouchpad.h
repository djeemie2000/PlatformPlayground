#pragma once 

#include <Arduino.h>

struct MidiVoiceMessage; 

class MidiTouchPad
{
public:
    static const int Capacity = 16;

    MidiTouchPad();

    void Begin();
    void ConfigureNote(uint8_t channel, uint8_t midiNote, int idx);
    void ConfigureController(uint8_t channel, uint8_t controllerNumber, int idx);

    void Update();
    void Update(MidiVoiceMessage& message);
    
    int GetNumPads() const;
    bool Get(int Pad) const;
    bool IsPushed(int Pad) const;
    bool IsClicked(int Pad) const;
    bool IsReleased(int Pad) const;

private:
    uint16_t m_TouchState;// bit = 1 => touched, bit is 0 => not touched
    uint16_t m_PrevTouchState;
    // configuration:
    uint8_t m_IsNote[Capacity]; 
    uint8_t m_Channel[Capacity];
    uint8_t m_MidiNote[Capacity];//or Controller number
};
