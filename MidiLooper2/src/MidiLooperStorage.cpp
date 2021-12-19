#include "MidiLooperStorage.h"
#include "sdstorage.h"
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

    m_Storage = nullptr;
    memset(m_Path, 0, 17);
    m_Bank = 0x00;
    m_Slot = 0x00;
}

bool MidiLooperStorage::Begin(SDStorage* storage)
{
    m_Storage = storage;
    return m_Storage->IsOpen();
}

bool MidiLooperStorage::Open(uint8_t bank, uint8_t slot)
{
    //use bank/slot
    m_Bank = bank;
    m_Slot = slot;

    return true;
}

void MidiLooperStorage::Close()
{
    //TODO (?) save current bank/slot
}

void MidiLooperStorage::SetPath(uint8_t bank, uint8_t slot, uint8_t track, const char* id)
{
    // path = "/bb-ss-tt-id.bin"
    m_Path[0] = '/';
    m_Path[1] = m_Lut[bank >> 4];
    m_Path[2] = m_Lut[bank & 0x0F];
    m_Path[3] = '-';
    m_Path[4] = m_Lut[slot >> 4];
    m_Path[5] = m_Lut[slot & 0x0F];
    m_Path[6] = '-';
    m_Path[7] = m_Lut[track >> 4];
    m_Path[8] = m_Lut[track & 0x0F];
    m_Path[9] = '-';
    m_Path[10] = id[0];
    m_Path[11] = id[1];
    m_Path[12] = '.';
    m_Path[13] = 'b';
    m_Path[14] = 'i';
    m_Path[15] = 'n';
    m_Path[16]= 0;//terminating zero
}


bool MidiLooperStorage::SaveMidiChannel(uint8_t track, uint8_t midiChannel)
{
    SetPath(m_Bank, m_Slot, track, "Ch");
    return m_Storage->Write(m_Path, midiChannel);
}
    
bool MidiLooperStorage::LoadMidiChannel(uint8_t track, uint8_t& midiChannel)
{
    SetPath(m_Bank, m_Slot, track, "Ch");
    return m_Storage->Read(m_Path, midiChannel);
}

bool MidiLooperStorage::SavePlayMute(uint8_t track, bool playMute)
{
    SetPath(m_Bank, m_Slot, track, "Pl");
    uint8_t pm = playMute ? 0x01 : 0x00;
    return m_Storage->Write(m_Path, pm);
}
    
bool MidiLooperStorage::LoadPlayMute(uint8_t track, bool& playMute)
{
    SetPath(m_Bank, m_Slot, track, "Pl");
    uint8_t pm = playMute ? 0x01 : 0x00;
    bool succeeded = m_Storage->Read(m_Path, pm);
    playMute = (pm!=0x00);
    return succeeded;
}

bool MidiLooperStorage::SaveEvents(uint8_t track, MidiLooperEvent* events, int numEvents)
{
    SetPath(m_Bank, m_Slot, track, "Ev");
    return m_Storage->Write(m_Path, (uint8_t*)events, numEvents*sizeof(MidiLooperEvent));
}

bool MidiLooperStorage::LoadEvents(uint8_t track, MidiLooperEvent* events, int capacity, int& numEvents)
{
    SetPath(m_Bank, m_Slot, track, "Ev");
    int result = 0;
    bool succeeded = m_Storage->Read(m_Path, (uint8_t*)events, capacity*sizeof(MidiLooperEvent), result);
    numEvents = result/sizeof(MidiLooperEvent);
    return succeeded;
}

bool MidiLooperStorage::LoadCurrentSlot(uint8_t& bank, uint8_t& slot)
{
    return  m_Storage->Read("/bank.bin", bank) 
        && m_Storage->Read("/slot.bin", slot);
}


bool MidiLooperStorage::SaveCurrentSlot()
{
    return  m_Storage->Write("/bank.bin", m_Bank) 
        && m_Storage->Write("/slot.bin", m_Slot);
}

void MidiLooperStorage::PrintStats(HardwareSerial& serial)
{
    serial.println("stats:");
    m_Storage->PrintState(serial);
}
