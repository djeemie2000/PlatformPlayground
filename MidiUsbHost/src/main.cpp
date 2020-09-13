#include <Arduino.h>
#include "usbh_midi.h"
#include "usbhub.h"

#include <SPI.h>

const int ledPin = PD2;

USB Usb;
//USBHub Hub(&Usb);
USBH_MIDI Midi(&Usb);

uint16_t pid, vid;

void setup()
{
  // put your setup code here, to run once:
  vid = 0;
  pid = 0;

  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);

  Serial.println("MidiUsbHost v0.1...");

  if (Usb.Init() == -1)
  {
    Serial.println("Usb init failed!");
    while (1)
      ; //halt
  }     //if (Usb.Init() == -1...
  Serial.println("Usb init succeeded!");

  delay(200);
}

void testBlinkLed()
{
  for (int repeat = 0; repeat < 2; ++repeat)
  {
    digitalWrite(ledPin, HIGH);
    delay(800);
    digitalWrite(ledPin, LOW);
    delay(800);
  }

  for (int repeat = 0; repeat < 8; ++repeat)
  {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
}

// Poll USB MIDI Controler and send to serial MIDI
void MIDI_poll()
{
  char buf[20];
  uint8_t bufMidi[64];
  //  uint16_t rcvd;

  if (Midi.idVendor() != vid || Midi.idProduct() != pid)
  {
    vid = Midi.idVendor();
    pid = Midi.idProduct();
    sprintf(buf, "VID:%04X, PID:%04X", vid, pid);
    Serial.println(buf);
  }
  uint8_t size = Midi.RecvData(bufMidi, true);
  if (0 < size)
  {
    for (uint16_t i = 1; i <= size; i++)
    {
      Serial.print(bufMidi[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  // if (Midi.RecvData(&rcvd, bufMidi) == 0)
  // {
  //   uint32_t time = (uint32_t)millis();
  //   sprintf(buf, "%04X%04X: ", (uint16_t)(time >> 16), (uint16_t)(time & 0xFFFF)); // Split variable to prevent warnings on the ESP8266 platform
  //   Serial.print(buf);
  //   Serial.print(rcvd);
  //   Serial.print(':');
  //   for (uint16_t i = 0; i < rcvd; i++)
  //   {
  //     sprintf(buf, " %02X", bufMidi[i]);
  //     Serial.print(buf);
  //   }
  //   Serial.println("");
  // }
}

void loop()
{
  // put your main code here, to run repeatedly:
  testBlinkLed();

  while (true)
  {
    Usb.Task();
    if (Midi)
    {
      MIDI_poll();
    }
  }
}
