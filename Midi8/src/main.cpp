#include <Arduino.h>
#include <SPI.h>

#include "midi8ui.h"
#include "mode1handler.h"
#include "mode4handler.h"
#include "MidiParser.h"

Midi8UI midi8UI;
MidiParser midiParserSerial;
Mode1Handler mode1Handler;
Mode4Handler mode4Handler;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(31250);//115200);
  Serial.println("Midi8 v0.1...");

  midi8UI.begin();
}

// void ledPattern(uint8_t pattern)
// {
//   for(int idx = 0; idx<8; ++idx)
//   {
//     uint8_t curr = (pattern>>idx) & 0x01;
//     digitalWrite(LED_BUILTIN, curr);
//     delay(200);
//   }
// }

void testUi()
{
  testAnalogOutBank(midi8UI.cvOut, 1);
  testDigitalOutBank(midi8UI.gatesOut, 1);
  testLedOutBank(midi8UI.ledsOut,1);
}

void handleMidi(MidiHandler& midiHandler)
{
  // read serial midi in -> parser -> midi out
    // limit # bytes for performance issues
    int numBytesIn = Serial.available();
    const int maxNumBytesIn = 3;
    if (maxNumBytesIn < numBytesIn)
    {
      numBytesIn = maxNumBytesIn;
    }
    while (0 < numBytesIn)
    {
      uint8_t byte = Serial.read();
      //Serial.println(byte,HEX);
      midiParserSerial.Parse(byte, midiHandler);
      --numBytesIn;
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  testUi();

  while (true)
  {
    midi8UI.update();
    if(midi8UI.mode == 1)
    {
      handleMidi(mode1Handler);
      mode1Handler.updateUI(&midi8UI);
    }
    else if(midi8UI.mode==4)
    {
      handleMidi(mode4Handler);
      mode4Handler.updateUI(&midi8UI);
    }
  }

}
