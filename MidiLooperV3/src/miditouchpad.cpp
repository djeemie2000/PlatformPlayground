#include "miditouchpad.h"
#include "midivoicemessage.h"

MidiTouchPad::MidiTouchPad()
    : m_TouchState(0)
    , m_PrevTouchState(0)
{
}

void MidiTouchPad::Begin()
{
    m_TouchState = 0;
    m_PrevTouchState = 0;

    for(int idx=0; idx<GetNumPads(); ++idx)
    {
        m_IsNote[idx] = 0; 
        m_Channel[idx] = 0xFF;//unused
        m_MidiNote[idx] = 0xFF;//unused
    }
}

void MidiTouchPad::ConfigureNote(uint8_t channel, uint8_t midiNote, int idx)
{
    m_IsNote[idx] = 1;
    m_Channel[idx] = channel;
    m_MidiNote[idx] = midiNote; 
}

void MidiTouchPad::ConfigureController(uint8_t channel, uint8_t controllerNumber, int idx)
{
    m_IsNote[idx] = 0;
    m_Channel[idx] = channel;
    m_MidiNote[idx] = controllerNumber; 
}

void MidiTouchPad::Update()
{
  m_PrevTouchState = m_TouchState;
  // m_TouchPad unchanged
}

void MidiTouchPad::Update(MidiVoiceMessage& message)
{
    m_PrevTouchState = m_TouchState;
    
    uint8_t channel = Channel(message);
    uint8_t midiNote = MidiNote(message);
    if(IsNoteOn(message))
    {
        // check channel and note wrt config 
        for(int idx=0; idx<GetNumPads(); ++idx)
        {
            if(m_IsNote[idx] 
            && channel == m_Channel[idx]
            && midiNote == m_MidiNote[idx])
            {
                bitSet(m_TouchState, idx);
            }
        }
    }
    else if(IsNoteOff(message))
    {
        // check channel and note wrt config 
        for(int idx=0; idx<GetNumPads(); ++idx)
        {
            if(m_IsNote[idx] 
            && channel == m_Channel[idx]
            && midiNote == m_MidiNote[idx])
            {
                bitClear(m_TouchState, idx);
            }
        }
    }
    else if(IsController(message))
    {
         // check channel and note wrt config 
        for(int idx=0; idx<GetNumPads(); ++idx)
        {
            if(!m_IsNote[idx] 
            && channel == m_Channel[idx]
            && midiNote == m_MidiNote[idx])
            {
                // CC => 
                //  value == 0 off 
                //  value  > 0 on
                bitWrite(m_TouchState, idx, message.Param2);
                //Serial.println(m_TouchState, BIN);
            }
        }
    }
}

int MidiTouchPad::GetNumPads() const
{
    return 20;
}

bool MidiTouchPad::Get(int Pad) const
{
    if(0<=Pad && Pad<GetNumPads())
    {
        return m_TouchState & (1ul<<Pad);
    }
    return false;
}

bool MidiTouchPad::IsPushed(int Pad) const
{
	if(0<=Pad && Pad<GetNumPads())
    {
        return m_TouchState & (1ul<<Pad);
    }
    return false;
}

bool MidiTouchPad::IsClicked(int Pad) const
{
    if(0<=Pad && Pad<GetNumPads())
    {
        return !(m_PrevTouchState & (1ul<<Pad)) && (m_TouchState & (1ul<<Pad));
    }
    return false;
}

bool MidiTouchPad::IsReleased(int Pad) const
{
    if(0<=Pad && Pad<GetNumPads())
    {
        return (m_PrevTouchState & (1ul<<Pad)) && !(m_TouchState & (1ul<<Pad));
    }
    return false;
}
