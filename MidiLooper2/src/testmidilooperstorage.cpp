#include "testmidilooperstorage.h"
#include "sdstorage.h"
#include "MidiLooperStorage.h"
#include "MidiLooperEvent.h"

void PrintResult(HardwareSerial& serialDebug, const char* message, uint8_t value, bool ok)
{
    serialDebug.print(message);
    serialDebug.print(" 0x");
    serialDebug.print(value, HEX);
    serialDebug.print(" -> ");
    serialDebug.println(ok);
}

void PrintResult(HardwareSerial& serialDebug, const char* message, bool ok)
{
    serialDebug.print(message);
    serialDebug.print(" -> ");
    serialDebug.println(ok);
}

void PrintResult(HardwareSerial& serialDebug, const char* message, uint8_t expected, uint8_t actual)
{
    serialDebug.print(message);
    serialDebug.print(" -> expected ");
    serialDebug.print(expected, HEX);
    serialDebug.print(" and was ");
    serialDebug.print(actual, HEX);
    serialDebug.print(" -> ");
    serialDebug.println(expected == actual);
}

void PrintResultEqualsInt(HardwareSerial& serialDebug, const char* message, int expected, int actual)
{
    serialDebug.print(message);
    serialDebug.print(" -> expected ");
    serialDebug.print(expected);
    serialDebug.print(" and was ");
    serialDebug.print(actual);
    serialDebug.print(" -> ");
    serialDebug.println(expected == actual);
}


void TestMidiChannel(uint8_t track, MidiLooperStorage& storage, HardwareSerial& serialDebug)
{
    uint8_t channel = 0x0A;
    bool ok = storage.SaveMidiChannel(track, channel);
    PrintResult(serialDebug, "write midi channel", channel, ok);

    uint8_t channelRead = 0;
    ok = storage.LoadMidiChannel(track, channelRead);
    PrintResult(serialDebug, "reading midi channel", channelRead, ok);

    PrintResult(serialDebug, "read and write channel equal", channel, channelRead);
}

void TestPlayMute(uint8_t track, MidiLooperStorage& storage, HardwareSerial& serialDebug)
{
    bool playMute = true;
    bool ok = storage.SavePlayMute(track, playMute);
    PrintResult(serialDebug, "write play/mute", playMute?1:0, ok);

    bool playMuteRead = false;
    ok = storage.LoadPlayMute(track, playMuteRead);
    PrintResult(serialDebug, "reading play/mute", playMuteRead?1:0, ok);

    PrintResult(serialDebug, "read and write play/mute equal", playMute, playMuteRead);
}

void TestEvents(uint8_t track, MidiLooperStorage& storage, HardwareSerial& serialDebug)
{
    MidiLooperEvent events[3];
    events[0].m_Step = 0x02;
    events[0].m_MidiNote = 0x37;
    events[0].m_Velocity = 0x45;
    events[0].m_Layer = 0x01;
    events[1].m_Step = 0x03;
    events[1].m_MidiNote = 0x25;
    events[1].m_Velocity = 0x67;
    events[1].m_Layer = 0x02;
    events[2].m_Step = 0x04;
    events[2].m_MidiNote = 0x57;
    events[2].m_Velocity = 0x23;
    events[2].m_Layer = 0x03;
    
    bool ok = storage.SaveEvents(track, events, 3);
    PrintResult(serialDebug, "write events", 3, ok);

    int Size =0;
    MidiLooperEvent eventsRead[4];
    ok = storage.LoadEvents(track, eventsRead, 4, Size);
    PrintResult(serialDebug, "reading events", Size, ok);

    PrintResultEqualsInt(serialDebug, "read and write size equal", 3, Size);
    PrintResult(serialDebug, "read and write event 0 step", events[0].m_Step, eventsRead[0].m_Step);
    PrintResult(serialDebug, "read and write event 0 note", events[0].m_MidiNote, eventsRead[0].m_MidiNote);
    PrintResult(serialDebug, "read and write event 0 velo", events[0].m_Velocity, eventsRead[0].m_Velocity);
    PrintResult(serialDebug, "read and write event 0 layr", events[0].m_Layer, eventsRead[0].m_Layer);
    PrintResult(serialDebug, "read and write event 1 step", events[1].m_Step, eventsRead[1].m_Step);
    PrintResult(serialDebug, "read and write event 1 note", events[1].m_MidiNote, eventsRead[1].m_MidiNote);
    PrintResult(serialDebug, "read and write event 1 velo", events[1].m_Velocity, eventsRead[1].m_Velocity);
    PrintResult(serialDebug, "read and write event 1 layr", events[1].m_Layer, eventsRead[1].m_Layer);
    PrintResult(serialDebug, "read and write event 2 step", events[2].m_Step, eventsRead[2].m_Step);
    PrintResult(serialDebug, "read and write event 2 note", events[2].m_MidiNote, eventsRead[2].m_MidiNote);
    PrintResult(serialDebug, "read and write event 2 velo", events[2].m_Velocity, eventsRead[2].m_Velocity);
    PrintResult(serialDebug, "read and write event 2 layr", events[2].m_Layer, eventsRead[2].m_Layer);
}


void TestMidiLooperStorage(MidiLooperStorage& storage, SDStorage& sdStorage, HardwareSerial& serialDebug)
{
    serialDebug.println("testing midi looper storage...");

    sdStorage.PrintListDir("/", serialDebug);

    // open unused slot, for >=2 tracks : write + read + check equals for ch/playmute/events(empty)/events(not empty), close
    uint8_t bank = 0x00;
    uint8_t unusedSlot = 0x0F;
    bool ok = storage.Open(bank, unusedSlot);
    PrintResult(serialDebug, "opening unused slot", unusedSlot, ok);

    {
        uint8_t track = 20;// 0x14 = track 20
        PrintResult(serialDebug, "testing for track", track, true);
        
        TestMidiChannel(track, storage, serialDebug);
        TestPlayMute(track, storage, serialDebug);
        TestEvents(track, storage, serialDebug);
    }

    storage.Close();
    PrintResult(serialDebug, "closing unused slot", unusedSlot, true);
    //TODO how to test current slot/bank????
    
    // SD list dir
    sdStorage.PrintListDir("/", serialDebug);
    // remove files used for testing????
    serialDebug.println("removing files");
    sdStorage.Remove("/00-0F-14-Ch.bin");
    sdStorage.Remove("/00-0F-14-Pl.bin");
    sdStorage.Remove("/00-0F-14-Ev.bin");

    // open + read non existing slot
    // SD list dir
    // 
    // TODO remove files used for testing????

    serialDebug.println("testing midi looper storage done");
}