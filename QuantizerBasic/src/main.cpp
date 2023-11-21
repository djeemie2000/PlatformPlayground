#include <Arduino.h>
#include "quantizerbasicboard.h"

#include "digitaloutbank.h"
#include "buttoninbank.h"
#include "rgled.h"
#include "pitchoutbank.h"

QuantizerBoard board;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  Serial.println("QuantizerBasic v0.1...");
  board.Begin();
}

void TestPitchIO()
{
  // write to pitch out1 -> read pitch in 2 -> write pitch in 2
  Serial.println("Test pitch IO...");

  int notes[] = {0, 2, 4, 5, 7, 9, 11};
  for(int octave = 0; octave<5; ++octave)
  {
    for(int idx = 0; idx<7; ++idx)
    {
      int pitch = 12*octave + notes[idx];
      board.pitchOut.PitchOut(0, 0, pitch);

      board.pitchIn2.Read();
      int pitchin= board.pitchIn2.Get();
      board.pitchOut.PitchOut(1, 0, pitchin);

      Serial.print(pitch);
      Serial.print(" -> ");
      Serial.println(pitchin);

      delay(500);
    }
  }

  Serial.println("Done");  
}

void DoPitchIO()
{
  board.pitchIn1.Read();
  board.pitchIn2.Read();
  board.pitchOut.PitchOut(0, 0, board.pitchIn1.Get());
  board.pitchOut.PitchOut(1, 0, board.pitchIn2.Get());

  uint8_t color1 = board.pitchIn1.Get()*4;
  ColorOut(board.led1Out, color1);

  uint8_t color2 = board.pitchIn2.Get()*4;
  ColorOut(board.led2Out, color2);

  // Serial.print(board.pitchIn1.Get());
  // Serial.print(' ');
  // Serial.println(board.pitchIn2.Get());
}

void loop() {
  // put your main code here, to run repeatedly:
//  TestDigitalOutBank(board.triggerOut, 4);
  //TestButtonIn(board.buttons, 30);
//  TestColorOut(board.led1Out, 1);
//  TestColorOut(board.led2Out, 1);
  //TestPitchOut2(board.pitchOut, 0);
  //TestPitchOut2(board.pitchOut, 1);
  //TestPitchIO();
  DoPitchIO();
}
