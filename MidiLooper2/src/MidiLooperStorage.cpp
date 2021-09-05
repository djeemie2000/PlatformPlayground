#include "MidiLooperStorage.h"
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

void MidiLooperStorage::Open(uint8_t slot)
{
    //TODO use slot
    m_Preferences.begin("slot0", false);
}

void MidiLooperStorage::Close()
{
    m_Preferences.end();
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

bool MidiLooperStorage::SaveMidiChannel(uint8_t track, uint8_t midiChannel)
{
    SetKey(0x00, track, "Ch");
    size_t result =  m_Preferences.putUChar(m_Key, midiChannel);
    return 0<result;
}
    
bool MidiLooperStorage::LoadMidiChannel(uint8_t track, uint8_t& midiChannel)
{
    SetKey(0x00, track, "Ch");
    midiChannel = m_Preferences.getUChar(m_Key, midiChannel);
    return true;//Assume true
}

bool MidiLooperStorage::SavePlayMute(uint8_t track, bool playMute)
{
    SetKey(0x00, track, "Pl");
    size_t result = m_Preferences.putBool(m_Key, playMute);
    return 0<result;
}
    
bool MidiLooperStorage::LoadPlayMute(uint8_t track, bool& playMute)
{
    SetKey(0x00, track, "Pl");
    playMute = m_Preferences.getBool(m_Key, playMute);
    return true;//Assume true
}

bool MidiLooperStorage::SaveEvents(uint8_t track, MidiLooperEvent* events, int numEvents)
{
    SetKey(0x00, track, "Ev");
    size_t result = m_Preferences.putBytes(m_Key, events, numEvents*sizeof(MidiLooperEvent));
    return result>0;
}

bool MidiLooperStorage::LoadEvents(uint8_t track, MidiLooperEvent* events, int capacity, int& numEvents)
{
    SetKey(0x00, track, "Ev");
    size_t result = m_Preferences.getBytes(m_Key, events, capacity*sizeof(MidiLooperEvent));
    numEvents = result/sizeof(MidiLooperEvent);
    return result>0;
}

bool MidiLooperStorage::LoadNumEvents(uint8_t track, int& numEvents)
{
    SetKey(0x00, track, "Ev");
    size_t result = m_Preferences.getBytesLength(m_Key);
    numEvents = result/sizeof(MidiLooperEvent);
    return result>0;
}

void MidiLooperStorage::PrintStats(HardwareSerial& serial)
{
    serial.println("stats:");
  size_t free = m_Preferences.freeEntries();
  serial.print("Free entries ");
  serial.println(free);
}

// void MidiLooperStorage::EraseAll()
// {
//     NVS.eraseAll();
// }
