#include <Arduino.h>
#include "MidiUsbHost.h"
#include "MidiParser.h"
#include "MidiOut.h"

const int ledPin = PD2;
const int debugPin = PD3;
bool debugMode;
MidiUsbHost midiUsbHost;
MidiParser midiParserUsb;
MidiParser midiParserSerial;
MidiOut midiOut;

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
  else
  {
    Serial.begin(31250);
    midiOut.begin(&Serial);
  }

  delay(1000);

  if (debugMode)
  {
    Serial.println("USB init...");
  }

  midiUsbHost.begin();

  if (debugMode)
  {
    if (midiUsbHost.InitSucceeded())
    {
      Serial.println("USB init succeeded!");
    }
    else
    {
      Serial.println("USB init failed!");
    }
  }
  delay(2000);
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (midiUsbHost.InitSucceeded())
  {
    midiUsbHost.Update();

    // led on if connected, off if not connected
    // + flicker while reciveing bytes
    if (midiUsbHost.DeviceConnected())
    {
      if (0 < midiUsbHost.RecieveSize())
      {
        digitalWrite(ledPin, LOW);
      }
      else
      {
        digitalWrite(ledPin, HIGH);
      }
    }
    else
    {
      digitalWrite(ledPin, LOW);
    }

    if (!debugMode)
    {
      // read usb midi in -> parser -> midi out
      if (midiUsbHost.DeviceConnected() && 0 < midiUsbHost.RecieveSize())
      {
        for (int idx = 0; idx < midiUsbHost.RecieveSize(); ++idx)
        {
          midiParserUsb.Parse(midiUsbHost.RecieveByte(idx), midiOut);
        }
      }

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
        midiParserSerial.Parse(byte, midiOut);
        --numBytesIn;
      }
    }

    if (debugMode)
    {
      if (midiUsbHost.DeviceConnected() && 0 < midiUsbHost.RecieveSize())
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
