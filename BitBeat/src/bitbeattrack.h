#pragma once
#include <Arduino.h>

class DigitalOutBank;
class LedOutBank;

class BitBeatTrack
{
public:
    static const int MaxNumSteps = 64;
    
    BitBeatTrack();

    void Play(bool clockRising, bool clockFalling, bool resetRising, bool resetClicked, 
                bool btn50On, bool btnTrackClicked);
    
    void StartRecording();
    void StopRecording();
    
    void Record(bool btn0Clicked, bool btn50Clicked, bool btn100Clicked);

    void DisplayPlay(LedOutBank& ledOut, int idx);
    void DisplayPlay(DigitalOutBank& gateOut, int idx);
    void DisplayPlayMute(LedOutBank& ledOut, int idx);
    void DisplayRecording(DigitalOutBank& gateOut, LedOutBank& ledOut, int idx);

    //TODO EEPROM : save upon StopRecording, loadupon start
    void SaveParams(int offset);
    void LoadParams(int offset);
    int ParamSize() const;

private:
    int GetBitOn(uint8_t step) const;
    int GetBitOff(uint8_t step) const;
    void SetBitOn(uint8_t step);
    void ClearBitOn(uint8_t step);
    void SetBitOff(uint8_t step);
    void ClearBitOff(uint8_t step);

    static const int ByteCapacity = 8;//8*8=64

    uint8_t m_NumSteps;
    uint8_t m_BitOn[ByteCapacity];
    uint8_t m_BitOff[ByteCapacity];

    //TODO state
    bool m_DoReset;
    uint8_t m_Step;
    bool m_PlayMute;// true = play false = mute
    bool m_GateOn;
};
