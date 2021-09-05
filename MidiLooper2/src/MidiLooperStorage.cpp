#include "MidiLooperStorage.h"
#include "ArduinoNvs.h"
#include "MidiLooperEvent.h"

MidiLooperStorage::MidiLooperStorage()
{
    m_Lut[0] = '0';
    m_Lut[1] = '1';
    m_Lut[2] = '2';
    m_Lut[3] = '3';
    m_Lut[4] = '4';
    m_Lut[5] = '5';
    m_Lut[6] = '6';
    m_Lut[7] = '7';
    m_Lut[8] = '8';
    m_Lut[9] = '9';
    m_Lut[10] = 'A';
    m_Lut[11] = 'B';
    m_Lut[12] = 'C';
    m_Lut[13] = 'D';
    m_Lut[14] = 'E';
    m_Lut[15] = 'F';
    memset(m_Key, 0, 7);
}

void MidiLooperStorage::Begin()
{
    NVS.begin();
}

void MidiLooperStorage::SetKey(uint8_t slot, uint8_t track, const char* id)
{
    m_Key[0] = m_Lut[slot >> 4];
    m_Key[1] = m_Lut[slot & 0x0F];
    m_Key[2] = m_Lut[track >> 4];
    m_Key[3] = m_Lut[track & 0x0F];
    m_Key[4] = id[0];
    m_Key[5] = id[1];
    m_Key[6]= 0;//terminating zero
}

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

bool MidiLooperStorage::LoadNumEvents(uint8_t slot, uint8_t track, int& numEvents)
{
    SetKey(slot, track, "Ev");
    size_t size = 0;
    bool ok = NVS.getBlobSize(m_Key, size);
    numEvents = ok ? size/sizeof(MidiLooperEvent) : 0;
    return ok;
}

void MidiLooperStorage::PrintStats(HardwareSerial& serialDebug)
{
    NVS.printStats(serialDebug);
}

void MidiLooperStorage::EraseAll()
{
    NVS.eraseAll();
}
