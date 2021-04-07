#include "single2handler.h"
#include "midi8ui.h"
#include "cvfunctions.h"
#include "EEPROM.h"

Single2Handler::Single2Handler()
 : m_LearnIdx(-1)
{
    for(int idx = 0; idx<Size; ++idx)
    {
        m_Channel[idx] = 0xFF;
        m_MidiNote[idx] = 0xFF;
        m_Gate[idx] = 0;
        m_Velocity[idx] = 0x00;
    }
}

void Single2Handler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    //
    Serial.print("S2 On  ");
    Serial.print(Channel, HEX);
    Serial.print(" ");
    Serial.print(MidiNote, HEX);
    //
    Serial.print(" L ");
    Serial.println(m_LearnIdx);
    //

    if(m_LearnIdx == -1)
    {
        // normal operation
        for(int idx = 0; idx<Size; ++idx)
        {
            if(m_Channel[idx] == Channel && m_MidiNote[idx]==MidiNote)
            {
                m_Gate[idx] = 1;
                m_Velocity[idx] = Velocity;
            }
        }
    }
    else
    {
        //learning
        m_Channel[m_LearnIdx] = Channel;
        m_MidiNote[m_LearnIdx] = MidiNote;
        ++m_LearnIdx;
        if(Size<=m_LearnIdx)
        {
            m_LearnIdx = -1;//stop learning
        }
    }
}
    
void Single2Handler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t /*Velocity*/)
{
    //
    Serial.print("S2 Off ");
    Serial.print(Channel, HEX);
    Serial.print(" ");
    Serial.print(MidiNote, HEX);
    //
    Serial.print(" L ");
    Serial.println(m_LearnIdx);
    //

    if(m_LearnIdx == -1)
    {
        // normal operation
        for(int idx = 0; idx<Size; ++idx)
        {
            if(m_Channel[idx] == Channel && m_MidiNote[idx]==MidiNote)
            {
                m_Gate[idx] = 0;
                // velocity unchanged
            }
        }
    }
}

bool Single2Handler::IsLearning() const
 {
     return m_LearnIdx != -1;
 }

void Single2Handler::updateUI(Midi8UI* ui)
{
    for(int idx = 0; idx<Size; ++idx)
    {
        if(idx==m_LearnIdx)
        {
            ui->ledsOut.set(idx, LedOutBank::Blink);
            ui->ledsOut.set(idx+4, LedOutBank::Blink);
            ui->gatesOut.set(idx, 0);
            ui->cvOut.set(idx, 0);
        }
        else if(m_Gate[idx])
        {
            ui->ledsOut.set(idx, LedOutBank::On);
            ui->ledsOut.set(idx+4, LedOutBank::On);
            ui->gatesOut.set(idx, 1);
            VelocityOut(ui->cvOut, idx, m_Velocity[idx]);
        }
        else
        {
            ui->ledsOut.set(idx, LedOutBank::Off);
            ui->ledsOut.set(idx+4, LedOutBank::Off);
            ui->gatesOut.set(idx, 0);
            // velocity unchanged
        }
    }

    if(ui->learnBtn.IsFalling())
    {
        Serial.println("Toggle learn!");
        //toggle learn mode on/off
        if(m_LearnIdx ==-1)
        {
            m_LearnIdx = 0;
        }
        else
        {
            m_LearnIdx = -1;
        }
    }
}

void Single2Handler::saveParams(int offset)
{
    int off = offset;
    EEPROM.update(off++, 'S');
    EEPROM.update(off++,'2');
    for(int idx = 0; idx<Size; ++idx)
    {
        EEPROM.update(off++, m_Channel[idx]);
        EEPROM.update(off++, m_MidiNote[idx]);
    }
}

int Single2Handler::paramSize() const
{
    return 2+Size*2;
}

void Single2Handler::loadParams(int offset)
{
    int off = offset;
    if('S' == EEPROM.read(off++) && '2' == EEPROM.read(off++))
    {
        for(int idx = 0; idx<Size; ++idx)
        {
            m_Channel[idx] = EEPROM.read(off++);
            m_MidiNote[idx] = EEPROM.read(off++);
        }
    }
}
