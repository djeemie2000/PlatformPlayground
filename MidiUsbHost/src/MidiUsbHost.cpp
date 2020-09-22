#include "MidiUsbHost.h"

MidiUsbHost::MidiUsbHost()
    : m_Usb(),
      m_Midi(&m_Usb),
      m_UsbInitSucceeded(false),
      m_pid(0),
      m_vid(0),
      m_RecieveSize(0)
{
}

bool MidiUsbHost::begin()
{
    m_RecieveSize = 0;
    m_pid = 0;
    m_vid = 0;
    if (m_Usb.Init() == -1)
    {
        m_UsbInitSucceeded = false;
    }
    else
    {
        m_UsbInitSucceeded = true;
    }
    return m_UsbInitSucceeded;
}

void MidiUsbHost::Update()
{
    m_RecieveSize = 0;
    m_Usb.Task();
    if (m_Midi)
    {
        MIDI_checkConnected();
        MIDI_poll();
    }
}

void MidiUsbHost::MIDI_checkConnected()
{
    if (m_Midi.idVendor() != m_vid || m_Midi.idProduct() != m_pid)
    {
        // connected device changed!
        m_vid = m_Midi.idVendor();
        m_pid = m_Midi.idProduct();
        // sprintf(buf, "VID:%04X, PID:%04X", vid, pid);
        // Serial.println(buf);
    }
}

void MidiUsbHost::MIDI_poll()
{
    m_RecieveSize = m_Midi.RecvData(m_MidiBuffer, true);
    // if (0 < size)
    // {
    //   for (uint16_t i = 1; i <= size; i++)
    //   {
    //     //Serial.print(bufMidi[i], HEX);
    //     //Serial.print(" ");
    //   }
    //   // Serial.println();
    // }
}