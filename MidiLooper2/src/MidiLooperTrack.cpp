#include "MidiLooperTrack.h"

MidiLooperTrack::MidiLooperTrack()
    : m_MidiChannel(0), m_MidiLearn(true), m_Recording(false), m_Muted(false)
    , m_CurrLayer(0), m_CurrLayerSize(0)
    , m_NumEvents(0)
{
    for(int step = 0; step<StepCapacity; ++step)
    {
        m_StepSize[step] = 0x00;
        memset(m_StepEventIndex[step], 0x00, StepDepth);
    }
}

void MidiLooperTrack::onUndo(MidiOut &midiOut)
{
    // only undo last recorded 'layer'
    //  use current layer if something is recorded, previous layer if not
    uint8_t undoLayer = 0 < m_CurrLayerSize ? m_CurrLayer : m_CurrLayer - 1;
    while (0 < m_NumEvents && m_Events[m_NumEvents - 1].m_Layer == undoLayer)
    {
        --m_StepSize[m_Events[m_NumEvents - 1].m_Step];//remove from step
         --m_NumEvents;
    }
    m_CurrLayer = undoLayer;
    m_CurrLayerSize = 0;

    // all playing notes should be note off!! => use note stack
    AllNotesOff(midiOut);
}

void MidiLooperTrack::onToggleMuted(MidiOut &midiOut)
{
    m_Muted = !m_Muted;
    //TODO all notes off ~ steps??
    if (m_Muted)
    {
        // all playing notes should be note off!! => use note stack
        AllNotesOff(midiOut);
        //midiOut.allNotesOff(m_MidiChannel);
    }
}

void MidiLooperTrack::ToggleRecording()
{
    m_Recording = !m_Recording;
}

void MidiLooperTrack::StartRecording()
{
    m_Recording = true;
}

void MidiLooperTrack::StopRecording()
{
    m_Recording = false;
}

void MidiLooperTrack::ToggleMidiLearn()
{
    m_MidiLearn = !m_MidiLearn;
}

void MidiLooperTrack::StopMidiLearn()
{
    m_MidiLearn = false;
}

void MidiLooperTrack::onTick(const MidiLooperTicker &ticker, MidiOut &midiOut)
{
    if (ticker.clockIsRising())
    {
        //TODO if muted => all note off?
        //TODO if channel changed (midi learn) => all notes off on original channel?
        int step = ticker.Counter();

        if (0 == step)
        {
            if (0 < m_CurrLayerSize)
            {
                //something was recorded => new layer
                ++m_CurrLayer;
            }
            //reset current layer to zero
            m_CurrLayerSize = 0;
        }

        // iterate all items, if step matches current step, play it
        // muted => no note on, but still do all note off (?)
        if (!m_Muted)
        {
            int idx = 0;
            // do not play the stuff that was recorded in the current layer to prevent playing twice (live + playback)
            // -> this is now handled by the note on stack in the midi out
            while(idx<m_StepSize[step])
            {
                uint8_t eventIdx = m_StepEventIndex[step][idx];
                if (m_Events[eventIdx].IsNoteOn())
                {
                    midiOut.NoteOn(m_MidiChannel, m_Events[eventIdx].m_MidiNote, m_Events[eventIdx].m_Velocity);
                    m_NoteStack.NoteOn(m_Events[eventIdx].m_MidiNote);
                }
                else
                {
                    // use some dummy velocity but not zero!
                    midiOut.NoteOff(m_MidiChannel, m_Events[eventIdx].m_MidiNote, 0x01);
                    m_NoteStack.NoteOff(m_Events[eventIdx].m_MidiNote);
                }
                ++idx;
            }
        }
    }
}

void MidiLooperTrack::onNoteOn(const MidiLooperTicker &ticker, MidiOut &midiOut, uint8_t midiChannel, uint8_t midiNote, uint8_t velocity)
{
    // if midi learn changes channels, need to note off all open notes!
    //TODO midi learn also learns 2 cc sliders!
    if (m_MidiLearn)
    {
        // only notes of this track not all notes of this channel!!
        AllNotesOff(midiOut);//m_MidiChannel);
        m_MidiChannel = midiChannel;
        m_MidiLearn = false;
    }
    if (midiChannel == m_MidiChannel)
    {
        if (m_Recording && m_NumEvents < EventCapacity)
        {
            uint8_t step = ticker.recordingStep();
            uint8_t stepSize = m_StepSize[step];
            if(stepSize<StepDepth)
            {
                m_Events[m_NumEvents].SetNoteOn(step, midiNote, velocity, m_CurrLayer);
                m_StepEventIndex[step][stepSize] = m_NumEvents;

                ++m_NumEvents;
                ++m_CurrLayerSize;
                ++m_StepSize[step];
            }
        }
    }
}

void MidiLooperTrack::onNoteOff(const MidiLooperTicker &ticker, uint8_t midiChannel, uint8_t midiNote, uint8_t velocity)
{
    if (midiChannel == m_MidiChannel)
    {
        if (m_Recording && m_NumEvents < EventCapacity)        
        {
            uint8_t step = ticker.recordingStep();
            uint8_t stepSize = m_StepSize[step];
            if(stepSize<StepDepth)
            {
                m_Events[m_NumEvents].SetNoteOff(step, midiNote, velocity, m_CurrLayer);
                m_StepEventIndex[step][stepSize] = m_NumEvents;

                ++m_NumEvents;
                ++m_CurrLayerSize;
                ++m_StepSize[step];
            }
        }
    }
}

void MidiLooperTrack::AllNotesOff(MidiOut &midiOut)
{
    while (m_NoteStack.Size())
    {
        // fixed non zero velocity
        midiOut.NoteOff(m_MidiChannel, m_NoteStack.Pop(), 0x01);
    }
}

void MidiLooperTrack::printItems(HardwareSerial &serial)
{
    serial.print("items # ");
    serial.println(m_NumEvents);

    for (int idx = 0; idx < m_NumEvents; ++idx)
    {
        serial.println(m_Events[idx].m_Layer, HEX);
        serial.print(" ");
        serial.print(m_Events[idx].m_MidiNote, HEX);
        serial.print(" ");
        serial.print(m_Events[idx].m_Step);
        serial.print(" ");
        serial.println(m_Events[idx].m_Velocity, HEX);
    }
}

void MidiLooperTrack::printState(HardwareSerial &serial)
{
    serial.print("Ch");
    serial.print(m_MidiChannel, HEX);
    serial.print(" L");
    serial.print(m_MidiLearn ? 1 : 0);
    serial.print(" R");
    serial.print(m_Recording ? 1 : 0);
    serial.print(" P");
    serial.print(m_Muted ? 0 : 1);
    serial.print(" S");
    serial.print(m_NoteStack.Size());
    serial.print(" L");
    serial.print(m_CurrLayer);
    serial.print(" :");
    serial.print(m_CurrLayerSize);
    serial.print(" #");
    serial.println(m_NumEvents);
}