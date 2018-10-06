#include "GateMidiHandler.h"

GateMidiHandler::GateMidiHandler(DigitalOut& gateOut) 
: m_gateOut(gateOut)
, m_State()
{}

void GateMidiHandler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    m_State.NoteOn(MidiNote, Velocity);
    m_gateOut = 0<m_State.NumPressed()?1:0;
}
  
void GateMidiHandler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    m_State.NoteOff(MidiNote, Velocity);
    m_gateOut = 0<m_State.NumPressed()?1:0;
}
