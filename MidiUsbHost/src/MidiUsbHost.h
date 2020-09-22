#pragma once

#include <Arduino.h>

#include "usbh_midi.h"
#include "usbhub.h"

#include <SPI.h>

class MidiUsbHost
{
public:
    MidiUsbHost();

    bool begin();
    bool InitSucceeded() const { return m_UsbInitSucceeded; }

    void Update();

    bool DeviceConnected() const { return m_pid > 0 && m_vid > 0; }
    uint16_t Pid() const { return m_pid; }
    uint16_t Vid() const { return m_vid; }

    int RecieveSize() const { return m_RecieveSize; }
    uint8_t RecieveByte(int idx) const { return m_MidiBuffer[1 + idx]; }

private:
    // Poll USB MIDI Controller
    void MIDI_checkConnected();
    void MIDI_poll();

    USB m_Usb;
    //USBHub Hub(&Usb);
    USBH_MIDI m_Midi;
    bool m_UsbInitSucceeded;

    uint16_t m_pid;
    uint16_t m_vid;

    uint8_t m_MidiBuffer[64];
    int m_RecieveSize;
};
