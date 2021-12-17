#include <Arduino.h>
#include <Preferences.h>

class MidiLooperEvent;
class SDStorage;

class MidiLooperStorage
{
public:
    MidiLooperStorage();

    bool Begin(SDStorage* storage);

    bool Open(uint8_t slot);
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
    void SetPath(uint8_t bank, uint8_t slot, uint8_t track, const char* id);

    Preferences m_Preferences;
    char m_Lut[16];
    char m_Key[7];//2 char for slot, 2 char for track, 2 char for id, terminating zero
    char m_Path[17];// / + 2 char for bank, slot, track + 2 char for id + terminating zero

    SDStorage* m_Storage;
    uint8_t m_Bank;
    uint8_t m_Slot;
};
