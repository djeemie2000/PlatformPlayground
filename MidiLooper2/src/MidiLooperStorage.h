#include <Arduino.h>
#include <Preferences.h>

class MidiLooperEvent;

class MidiLooperStorage
{
public:
    MidiLooperStorage();

    void Open(uint8_t slot);

    bool SaveMidiChannel(uint8_t track, uint8_t midiChannel);
    bool LoadMidiChannel(uint8_t track, uint8_t& midiChannel);
    bool SavePlayMute(uint8_t track, bool playMute);
    bool LoadPlayMute(uint8_t track, bool& playMute);
    bool SaveEvents(uint8_t track, MidiLooperEvent* events, int numEvents);
    bool LoadEvents(uint8_t track, MidiLooperEvent* events, int capacity, int& numEvents);
    bool LoadNumEvents(uint8_t track, int& numEvents);

    void Close();

    void PrintStats(HardwareSerial& serialDebug);
//    void EraseAll();

private:
    void SetKey(uint8_t slot, uint8_t track, const char* id);

    Preferences m_Preferences;
    char m_Lut[16];
    char m_Key[7];//2 char for slot, 2 char for track, 2 char for id, terminating zero
};
