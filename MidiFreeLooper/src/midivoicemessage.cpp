#include "midivoicemessage.h"

uint8_t Command(MidiVoiceMessage& message)
{
    return message.StatusByte & 0xF0;
}

uint8_t Channel(MidiVoiceMessage& message)
{
    return message.StatusByte & 0x0F;
}

uint8_t MidiNote(MidiVoiceMessage& message)
{
    return message.Param1 & 0x7F;
}

uint8_t Velocity(MidiVoiceMessage& message)
{
    return message.Param2 & 0x7F;
}

bool IsNoteOn(MidiVoiceMessage& message)
{
    // exclude note off 0x90
    return 0x90 == Command(message) 
    && 0x00 != Velocity(message);
}

bool IsNoteOff(MidiVoiceMessage& message)
{
    // note off 0x80 or note on with zero velocity
    return (0x80 == Command(message)) 
    || (0x90 == Command(message) && 0x00 == Velocity(message));
}

bool IsController(MidiVoiceMessage& message)
{
     return (0xB0 == Command(message));
}

void PrintVoiceMessage(MidiVoiceMessage &message, HardwareSerial& serialDebug)
{
  serialDebug.print(message.StatusByte, HEX);
  serialDebug.print(' ');
  serialDebug.print(message.Param1, HEX);
  serialDebug.print(' ');
  serialDebug.println(message.Param2, HEX);
}
