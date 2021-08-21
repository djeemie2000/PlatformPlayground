#include <Arduino.h>

class MidiLooperEvent;

class MidiLooperStorage
{
public:
    MidiLooperStorage();

    void Begin();

    bool SaveMidiChannel(uint8_t slot, uint8_t track, uint8_t midiChannel);
    bool LoadMidiChannel(uint8_t slot, uint8_t track, uint8_t& midiChannel);
    bool SavePlayMute(uint8_t slot, uint8_t track, bool playMute);
    bool LoadPlayMute(uint8_t slot, uint8_t track, bool& playMute);
    bool SaveEvents(uint8_t slot, uint8_t track, MidiLooperEvent* events, int numEvents);
    bool LoadEvents(uint8_t slot, uint8_t track, MidiLooperEvent* events, int capacity, int& numEvents);
private:
    void SetKey(uint8_t slot, uint8_t track, const char* id);

    char m_Lut[16];
    char m_Key[7];//2 char for slot, 2 char for track, 2 char for id, terminating zero
};
