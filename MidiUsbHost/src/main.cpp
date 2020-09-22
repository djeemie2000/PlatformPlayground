#include <Arduino.h>
#include "MidiUsbHost.h"

const int ledPin = PD2;
const int debugPin = PD3;
bool debugMode;
MidiUsbHost midiUsbHost;

void setup()
{
  // put your setup code here, to run once:

  // TODO led blink fast => usb init failed
  // TODO blink led slow => wait for pid/vid >0
  // TODO led on => active

  // TODO upon startup check din (pullup) ->
  //    LOW = debug mode => Serial debug print 115200
  //    HIGH = normal mode => midi 31250
  //vid = 0;
  //pid = 0;

  pinMode(ledPin, OUTPUT);
  //default no debug mode
  pinMode(debugPin, INPUT_PULLUP);
  debugMode = (digitalRead(debugPin) == LOW);

  if (debugMode)
  {
    Serial.begin(115200);
    Serial.println("MidiUsbHost v0.1...");
  }

  midiUsbHost.begin();

  if (debugMode)
  {
    if (midiUsbHost.InitSucceeded())
    {
      Serial.println("Usb init succeeded!");
    }
    else
    {
      Serial.println("Usb init failed!");
    }
  }
  delay(200);
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (midiUsbHost.InitSucceeded())
  {
    midiUsbHost.Update(); //TODO parser
    //TODO poll midi Serial In -> parser

    // led on if connected, off if not connected
    if (midiUsbHost.DeviceConnected())
    {
      digitalWrite(ledPin, HIGH);
    }
    else
    {
      digitalWrite(ledPin, LOW);
    }

    if (debugMode && midiUsbHost.DeviceConnected() && 0 < midiUsbHost.RecieveSize())
    {
      //print bytes if any
      Serial.print("pid ");
      Serial.print(midiUsbHost.Pid(), HEX);
      Serial.print("vid ");
      Serial.print(midiUsbHost.Vid(), HEX);
      Serial.print(" :  ");
      for (int idx = 0; idx < midiUsbHost.RecieveSize(); ++idx)
      {
        Serial.print(midiUsbHost.RecieveByte(idx), HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
  }
  else
  {
    // USB init failed => blink led fast
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}
