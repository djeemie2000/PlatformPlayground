#include "midilooperapp.h"

MidiLooperApp::MidiLooperApp()
    : learnIdx(-1), recordingIdx(-1), isPlaying(true), prevIsPlaying(false)
{
}

void MidiLooperApp::ReadMidiIn(HardwareSerial &serial)
{
    while (serial.available())
    {
        uint8_t byte = serial.read();
        // filter out realtime clock messages for midi thru
        // also for parsing??
        if (!(0xF8 <= byte && byte <= 0xFC))
        {
            // midi thru:
            serial.write(byte);

            // buffer for further processing
            serialBuffer.Write(byte);
        }
    }
}
