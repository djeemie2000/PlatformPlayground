#include "MidiLooperStorage.h"
#include "ArduinoNvs.h"
#include "MidiLooperEvent.h"

MidiLooperStorage::MidiLooperStorage()
{}

bool MidiLooperStorage::SaveMidiChannel(uint8_t slot, uint8_t track, uint8_t midiChannel)
{
    SetKey(slot, track, "Ch");
    return NVS.setInt(m_Key, midiChannel);
}
    
bool MidiLooperStorage::LoadMidiChannel(uint8_t slot, uint8_t track, uint8_t& midiChannel)
{
    SetKey(slot, track, "Ch");
    return NVS.getInt(m_Key, midiChannel);
}

bool MidiLooperStorage::SavePlayMute(uint8_t slot, uint8_t track, bool playMute)
{
    SetKey(slot, track, "Pl");
    return NVS.setBool(m_Key, playMute);
}
    
bool MidiLooperStorage::LoadPlayMute(uint8_t slot, uint8_t track, bool& playMute)
{
    SetKey(slot, track, "Pl");
    return NVS.getBool(m_Key, playMute);
}

bool MidiLooperStorage::SaveEvents(uint8_t slot, uint8_t track, MidiLooperEvent* events, int numEvents)
{
    SetKey(slot, track, "Ev");
    return NVS.setBlob(m_Key, (uint8_t*)events, numEvents*sizeof(MidiLooperEvent));
}

bool MidiLooperStorage::LoadEvents(uint8_t slot, uint8_t track, MidiLooperEvent* events, int capacity, int& numEvents)
{
    SetKey(slot, track, "Ev");
    size_t size = 0;
    bool ok = NVS.getBlob(m_Key, (uint8_t*)events, capacity*sizeof(MidiLooperEvent), size);
    numEvents = ok ? size/sizeof(MidiLooperEvent) : 0;
    return ok;
}
