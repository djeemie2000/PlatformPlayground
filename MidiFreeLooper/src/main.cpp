#include <Arduino.h>
#include "orderedlist.h"
#include "testorderedlist.h"

#include "midivoicemessage.h"
#include "midinoteparser.h"

#include "devboard.h"
#include "midiout.h"

MidiVoiceMessage message;
MidiNoteParser midiNoteParser;
DevBoard devBoard;
MidiOut midiOut;

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(31250);//115200);
  devBoard.Begin();
  midiOut.Begin(&devBoard.serialMidi);
  devBoard.serialDebug.println("MidiFreeLooper v0.2...");
}

void DoTests(HardwareSerial& serial)
{
  TestListAddInOrder(serial);
  delay(1000);

  TestListAddResetHeadAdd(serial);
  delay(1000);
  
  TestListRead(serial);
  delay(1000);

  TestListReadWrite(serial);
  delay(1000);
  
  delay(20000);

}

// TODO poc Midi buffer byte read -> first byte thru, then parse note events + midi clock/start/stop
// TODO poc leds out (reset pulse, recording/armed/..., play/mute, empty yes/no)
// TODO poc fixed length e.g. 24ppq*8, fixed recording on, fixed play 
// TODO poc play/mute
// TODO poc clear
// TODO poc recording off/armed/on
// TODO poc reset (?)


void loop() {
  // put your main code here, to run repeatedly:

//  DoTests(devBoard.serialDebug);

  const int maxNumBytes = 6;
  int numBytes = 0;
  while (devBoard.serialMidi.available() && numBytes++ < maxNumBytes)
  {
    // midi in
    uint8_t byte = devBoard.serialMidi.read();
    // midi Thru
    devBoard.serialMidi.write(byte);

    // TODO midi start stop
    if(byte == 0xF8)
    {
      // on midi clock
    }
    else
    {  
      if (midiNoteParser.Parse(byte, message))
      {
        //TODO
      }
    }
  
  }

}


