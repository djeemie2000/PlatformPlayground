#include "GateMidiHandler.h"

GateMidiHandler::GateMidiHandler(DigitalOut& gateOut) 
: m_GateOut(gateOut)
//, m_State()
, m_NoteOnCount(0)
{}

void GateMidiHandler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    ++m_NoteOnCount;
    m_GateOut = 0<m_NoteOnCount?1:0;
//    m_State.NoteOn(MidiNote, Velocity);
//    m_GateOut = 0<m_State.NumPressed()?1:0;
}
  
void GateMidiHandler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    if(0<m_NoteOnCount)
    {
        --m_NoteOnCount;
    }
    m_GateOut = 0<m_NoteOnCount?1:0;
//    m_State.NoteOff(MidiNote, Velocity);
//    m_GateOut = 0<m_State.NumPressed()?1:0;
}
