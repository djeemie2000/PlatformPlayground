#pragma once
#include <Arduino.h>

class MidiVoiceMessage;
class GateOutBank;
class LedOut;

class Midi2Gate
{
public:
    static const int NumGates = 8;

    Midi2Gate();

    void Begin(GateOutBank* gates, LedOut* ledOut);

    void OnMessage(MidiVoiceMessage &message);
    void ToggleLearning();
    bool IsLearning() const;

    void saveParams(int offset);
    int paramSize() const;
    void loadParams(int offset);

    void PrintState();

private:
    GateOutBank* m_Gates;
    LedOut* m_LedOut;
    uint8_t m_Channel[NumGates];
    uint8_t m_MidiNote[NumGates];
    uint8_t m_Gate[NumGates];
    int m_LearnIndex;
};
