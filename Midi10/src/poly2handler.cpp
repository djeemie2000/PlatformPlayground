#include "poly2handler.h"
#include "midi10ui.h"
#include "cvfunctions.h"
#include "EEPROM.h"

Poly2Handler::Poly2Handler()
    : m_LearnIdx(-1)
{
    m_Channel = 0xFF;
    m_BaseNote = 0x0;
    for (int idx = 0; idx < Size; ++idx)
    {
        m_MidiNote[idx] = 0xFF;
    }
}

void Poly2Handler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    //
    // Serial.print("P2 On  ");
    // Serial.print(Channel, HEX);
    // Serial.print(" ");
    // Serial.print(MidiNote, HEX);
    // //
    // Serial.print(" L ");
    // Serial.println(m_LearnIdx);
    //

    if (m_LearnIdx == -1)
    {
        // normal operation
        if (m_Channel == Channel)
        {
            bool handled = false;
            for (int idx = 0; !handled && idx < Size; ++idx)
            {
                if (m_MidiNote[idx] == 0xFF)
                {
                    m_MidiNote[idx] = MidiNote;
                    handled = true;
                }
            }
        }
    }
    else
    {
        //learning
        m_Channel = Channel;
        m_BaseNote = MidiNote;
        for (int idx = 0; idx < Size; ++idx)
        {
            m_MidiNote[idx] = 0xFF;
        }
        m_LearnIdx = -1; //stop learning
    }
}

void Poly2Handler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t /*Velocity*/)
{
    //
    // Serial.print("P2 Off ");
    // Serial.print(Channel, HEX);
    // Serial.print(" ");
    // Serial.print(MidiNote, HEX);
    // //
    // Serial.print(" L ");
    // Serial.println(m_LearnIdx);
    //

    if (m_LearnIdx == -1)
    {
        // normal operation
        if (m_Channel == Channel)
        {
            for (int idx = 0; idx < Size; ++idx)
            {
                if (m_MidiNote[idx] == MidiNote)
                {
                    m_MidiNote[idx] = 0xFF;
                }
            }
        }
    }
}

bool Poly2Handler::IsLearning() const
{
    return m_LearnIdx != -1;
}

void Poly2Handler::updateUI(Midi10UI *ui)
{
    // gates : 0 1 2 3
    // leds  : 4 5 6 7
    // pitch : 0 1 2 3
    for (int idx = 0; idx < Size; ++idx)
    {
        if (m_LearnIdx != -1)
        {
            ui->ledsOut.set(idx, LedOutBank::Off);       // gate
            ui->ledsOut.set(idx + 4, LedOutBank::Blink); // led
            ui->cvOut.set(idx, 0);                       // pitch
        }
        else if (m_MidiNote[idx] != 0xFF)
        {
            // note is on
            ui->ledsOut.set(idx, LedOutBank::On);                  //gate
            ui->ledsOut.set(idx + 4, LedOutBank::On);              //led
            PitchOut(ui->cvOut, idx, m_MidiNote[idx], m_BaseNote); //pitch
        }
        else
        {
            // note is off
            ui->ledsOut.set(idx, LedOutBank::Off);     //gate
            ui->ledsOut.set(idx + 4, LedOutBank::Off); //led
            // leave midi note / pitch unchanged
        }
    }

    if (ui->learnBtn.IsFalling())
    {
        //Serial.println("Toggle learn!");
        //toggle learn mode on/off
        if (m_LearnIdx == -1)
        {
            m_LearnIdx = 0;
        }
        else
        {
            m_LearnIdx = -1;
        }
    }
}

void Poly2Handler::saveParams(int offset)
{
    int off = offset;
    EEPROM.update(off++, 'P');
    EEPROM.update(off++, '2');
    EEPROM.update(off++, m_Channel);
    EEPROM.update(off++, m_BaseNote);
}

int Poly2Handler::paramSize() const
{
    return 4;
}

void Poly2Handler::loadParams(int offset)
{
    int off = offset;
    if ('P' == EEPROM.read(off++) && '2' == EEPROM.read(off++))
    {
        m_Channel = EEPROM.read(off++);
        m_BaseNote = EEPROM.read(off++);
    }
}
