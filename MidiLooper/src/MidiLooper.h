#include <Arduino.h>

#include "MidiHandler.h"
#include "MidiOut.h"
#include "MidiLooperTicker.h"
#include "MidiLooperTrack.h"
#include "MidiMetronome.h"

class MidiLooper : public MidiHandler
{
public:
    MidiLooper();

    void begin(HardwareSerial *midiSerial);

    void onTick(int clock, int reset);

    void NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity);
    void NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity);
    void MidiStart();
    void MidiStop();
    void allNotesOff();

    void printState(HardwareSerial &serial);
 
    MidiOut m_MidiOut;
    MidiLooperTicker m_Ticker;
    static const int NumTracks = 8;
    MidiLooperTrack m_Track[NumTracks];
    MidiMetronome m_Metronome;
};
