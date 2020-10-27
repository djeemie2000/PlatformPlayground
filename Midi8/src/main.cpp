#include <Arduino.h>
#include <SPI.h>

#include "midi8ui.h"
#include "single1handler.h"
#include "single2handler.h"
#include "mono2handler.h"
#include "mono4handler.h"
#include "poly2handler.h"
#include "poly4handler.h"
#include "MidiParser.h"

Midi8UI midi8UI;
MidiParser midiParserSerial;
Single1Handler single1Handler;
Single2Handler single2Handler;
Mono2Handler mono2Handler;
Mono4Handler mono4Handler;
Poly2Handler poly2Handler;
Poly4Handler poly4Handler;

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

  unsigned long debugCntr =0;
  while (true)
  {
    midi8UI.update();
    if(midi8UI.grouping == Midi8UI::Grouping1)
    {
      // always single mode!
      handleMidi(single1Handler);
      single1Handler.updateUI(&midi8UI);
    }
    else if(midi8UI.grouping==Midi8UI::Grouping2)
    {
      if(midi8UI.mode==Midi8UI::MonoMode)
      {
        handleMidi(mono2Handler);
        mono2Handler.updateUI(&midi8UI);
      }
      else if(midi8UI.mode==Midi8UI::PolyMode)
      {
        handleMidi(poly2Handler);
        poly2Handler.updateUI(&midi8UI);
      }
      else if(midi8UI.mode==Midi8UI::SingleMode)
      {
        handleMidi(single2Handler);
        single2Handler.updateUI(&midi8UI);
      }
    }
    else if(midi8UI.grouping==Midi8UI::Grouping4)
    {
      // only mono or poly
      if(midi8UI.mode==Midi8UI::MonoMode)
      {
        handleMidi(mono4Handler);
        mono4Handler.updateUI(&midi8UI);
      }
      else if(midi8UI.mode==Midi8UI::PolyMode)
      {
        handleMidi(poly4Handler);
        poly4Handler.updateUI(&midi8UI);
      }
    }

    ++debugCntr;
    if(2000<debugCntr)
    {
      Serial.print(debugCntr);
      Serial.print(" -> ");
      Serial.print(millis());

      Serial.print(" ");
      Serial.print(midi8UI.mode);
      Serial.println(midi8UI.grouping);

      debugCntr = 0;
    }
  }

}
