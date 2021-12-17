#include "MidiLooperStorage.h"
#include "MidiLooperEvent.h"

#include "sdstorage.h"

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

bool MidiLooperStorage::Open(uint8_t slot)
{
    //use bank/slot
    m_Bank = 0x00;
    m_Slot = slot;

    m_Preferences.begin("slot0", false);
    return true;
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
    // File file = SD.open(m_Path, FILE_WRITE);
    // if(file)
    // {
    //     file.write(midiChannel);
    //     file.close();
    //     return true;
    // }
    // return false;
    // SetKey(m_Slot, track, "Ch");
    // size_t result =  m_Preferences.putUChar(m_Key, midiChannel);
    // return 0<result;
}
    
bool MidiLooperStorage::LoadMidiChannel(uint8_t track, uint8_t& midiChannel)
{
    SetKey(m_Slot, track, "Ch");
    midiChannel = m_Preferences.getUChar(m_Key, midiChannel);
    return true;//Assume true
}

bool MidiLooperStorage::SavePlayMute(uint8_t track, bool playMute)
{
    SetPath(m_Bank, m_Slot, track, "Pl");
    return m_Storage->Write(m_Path, playMute);

    // File file = SD.open(m_Path, FILE_WRITE);
    // if(file)
    // {
    //     file.write(playMute);
    //     file.close();
    //     return true;
    // }
    // return false;
    // SetKey(m_Slot, track, "Pl");
    // size_t result = m_Preferences.putBool(m_Key, playMute);
    // return 0<result;
}
    
bool MidiLooperStorage::LoadPlayMute(uint8_t track, bool& playMute)
{
    SetKey(m_Slot, track, "Pl");
    playMute = m_Preferences.getBool(m_Key, playMute);
    return true;//Assume true
}

bool MidiLooperStorage::SaveEvents(uint8_t track, MidiLooperEvent* events, int numEvents)
{
    SetPath(m_Bank, m_Slot, track, "Ev");
    return m_Storage->Write(m_Path, (uint8_t*)events, numEvents*sizeof(MidiLooperEvent));

    // File file = SD.open(m_Path, FILE_WRITE);
    // if(file)
    // {
    //     file.write((uint8_t*)events, numEvents*sizeof(MidiLooperEvent));
    //     file.close();
    //     return true;
    // }
    // return false;

    // SetKey(m_Slot, track, "Ev");
    // size_t result = m_Preferences.putBytes(m_Key, events, numEvents*sizeof(MidiLooperEvent));
    // return result>0;
}

bool MidiLooperStorage::LoadEvents(uint8_t track, MidiLooperEvent* events, int capacity, int& numEvents)
{
    SetKey(m_Slot, track, "Ev");
    size_t result = m_Preferences.getBytes(m_Key, events, capacity*sizeof(MidiLooperEvent));
    numEvents = result/sizeof(MidiLooperEvent);
    return result>0;
}

bool MidiLooperStorage::LoadNumEvents(uint8_t track, int& numEvents)
{
    SetKey(m_Slot, track, "Ev");
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

    m_Storage->PrintState(serial);

  //
//   uint8_t cardType = SD.cardType();
//   serial.print("SD Card Type: ");
//     if(cardType == CARD_MMC){
//     serial.println("MMC");
//     } else if(cardType == CARD_SD){
//     serial.println("SDSC");
//     } else if(cardType == CARD_SDHC){
//     serial.println("SDHC");
//     } else {
//     serial.println("UNKNOWN");
//     }

//     uint64_t cardSize = SD.cardSize() / (1024 * 1024);
//     serial.printf("SD Card Size: %llu MB\n", cardSize);

//     serial.printf("Total space: %llu bytes\n", SD.totalBytes());
//     serial.printf("Used space: %llu bytes\n", SD.usedBytes());

}

// void MidiLooperStorage::EraseAll()
// {
//     NVS.eraseAll();
// }
