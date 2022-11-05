#include <Arduino.h>

// TODO make more generic cfr configuration 
// has 2 x reset pulse output (pulse on upon midi start, pulse off upon first midi clock)
// has 2x 4PPPQ (1/16th) clock output
// has 2x 6PPQ clock output
// has 2x 12PPQ (high resolution) clock output
// status led will be 
//  continuously on when clock is not running
//  showing 1PPQ clock when clock is running
class Midi2Clock
{
public:
    static const int NumGates = 8;
    static const int NumCounters = 4;

    Midi2Clock();

    void Begin(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, 
                uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7,
                uint8_t statusLed);

    void OnMessage(uint8_t byte);
//    void OnTick(uint8_t counter); // only for blinking in learn mode?
//    void ToggleLearning();
//    bool IsLearning() const;

    void saveParams(int offset);
    int paramSize() const;
    void loadParams(int offset);

    void PrintState();

private:
    uint8_t m_OutputPin[NumGates];

    uint8_t m_TicksOn[NumCounters];
    uint8_t m_TicksPeriod[NumCounters];
    uint8_t m_TicksCounter[NumCounters];

    bool m_ClockIsRunning;
    bool m_DoReset;
    
    uint8_t m_StatusLed;
};