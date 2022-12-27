#include "midiloopertrack.h"
#include "midiout.h"

MidiLooperTrack::MidiLooperTrack()
 : m_Channel(0xFF)//no channel
 , m_Size(0)
 , m_PlayMute(true)
{}

bool MidiLooperTrack::AllNotesOff(HardwareSerial& serial)
{
    //TODO optimize!!
    for(uint8_t midiNote=0; midiNote<128; ++midiNote)
    {
        if(m_MidiNoteMask.IsNoteOn(midiNote))
        {
            NoteOff(serial, m_MidiNoteMask, m_Channel, midiNote, 0x00);
        }
    }

    return true;
}

bool MidiLooperTrack::LearnNoteOn(uint8_t channel, HardwareSerial& serial)
{
    //TODO all note of for active notes on current channel
    if(channel != m_Channel)
    {
        AllNotesOff(serial);
    }

    // update channel
    m_Channel = channel;

    //return true if learned, false if not
    return true;
}

bool MidiLooperTrack::RecordNoteOn(int step, uint8_t channel, uint8_t midinote, uint8_t velocity)
{
    if(m_Channel == channel)
    {
        auto item = m_Items[m_Size];
        item.midiNote = midinote;
        item.velocity = velocity;
        item.stepOn = step;
        item.stepOff = -1;//indicates this item souldnot be played! 
        ++m_Size;
        // do not add to item refs yet, wait for note off
        // => handle stop recording before corresponding note off is handled
    }
    return true;
}

bool MidiLooperTrack::RecordNoteOff(int step, uint8_t channel, uint8_t midinote, uint8_t velocity)
{
    if(m_Channel == channel)
    {
        // find corresponding item
        for(int idx=m_Size; idx>=0; --idx)
        {
            if(midinote == m_Items[idx].midiNote)
            {
                auto item = m_Items[idx];
                item.stepOff = step;
                // add to item refs
                m_ItemRefs[item.stepOn].AddReference(idx);
                m_ItemRefs[item.stepOff].AddReference(idx);
            }
        }
    }

    return true;
}

bool MidiLooperTrack::Undo(HardwareSerial& serial)
{
    // remove last item 
    if(0<m_Size)
    {
        --m_Size;
        // remove from item refs
        // carefull: incomplete step is not added to refs
        if(0<=m_Items[m_Size].stepOff)
        {
            m_ItemRefs[m_Items[m_Size].stepOn].RemoveReference();
            m_ItemRefs[m_Items[m_Size].stepOff].RemoveReference();        
        }
    }
    // TODO note off if removed item note on is active!!!!!

    return true;
}

bool MidiLooperTrack::PlayClockOn(int step, HardwareSerial& serial)
{
    // play -> mute => all active notes off (once) -> cfr SetPlayMute()
    // when muted => do nothing

    if(!m_PlayMute)
    {
        return false;
    }

    auto itemRefs = m_ItemRefs[step];
    for(int idx = 0; idx<itemRefs.size; ++idx)
    {
        auto item = m_Items[itemRefs.refs[idx]];
        // check if item is complete to be sure
        if(0<=item.stepOff)
        {
            // special case (stepOn == stepOff) => note on upon clockOn and note off upon clockOff           
            // normal case => noteon/off upon clock on
            if(step == item.stepOn)
            {
                NoteOn(serial, m_MidiNoteMask, m_Channel, item.midiNote, item.velocity);
            }
            else if(step == item.stepOff)
            {
                NoteOff(serial, m_MidiNoteMask, m_Channel, item.midiNote, item.velocity);
            }
        }
    }

    return true;
}

bool MidiLooperTrack::PlayClockOff(int step, HardwareSerial& serial)
{
    // play -> mute => all active notes off (once) -> cfr SetPlayMute()
    // when muted => do nothing

    if(!m_PlayMute)
    {
        return false;
    }

    auto itemRefs = m_ItemRefs[step];
    for(int idx = 0; idx<itemRefs.size; ++idx)
    {
        auto item = m_Items[itemRefs.refs[idx]];
        // check if item is complete to be sure
        // special case (stepOn == stepOff) => note on upon clockOn and note off upon clockOff
        if(0<=item.stepOff && step == item.stepOff && item.stepOff == item.stepOn)
        {
            NoteOff(serial, m_MidiNoteMask, m_Channel, item.midiNote, item.velocity);
        }
    }

    return true;
}

bool MidiLooperTrack::SetPlayMute(bool playMute, HardwareSerial& serial)
{
    if(m_PlayMute != playMute)
    {
        if(m_PlayMute)
        {
            AllNotesOff(serial);
        }
        m_PlayMute = playMute;
        return true;
    }
    return false;
}
