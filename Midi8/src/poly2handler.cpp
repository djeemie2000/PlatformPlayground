#include "poly2handler.h"
#include "midi8ui.h"
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

void Poly2Handler::Learn(bool learn)
{
    if (learn)
    {
        if (m_LearnIdx == -1)
        {
            m_LearnIdx = 0;
        }
    }
    else //if(m_LearnIdx != -1)
    {
        m_LearnIdx = -1;
    }
}

void Poly2Handler::updateUI(Midi8UI *ui)
{
    for (int idx = 0; idx < Size; ++idx)
    {
        if (m_LearnIdx != -1)
        {
            //TODO all will blink same as in poly4
            ui->ledsOut.set(idx, LedOutBank::Blink);
            ui->gatesOut.set(idx, 0);
            ui->cvOut.set(idx, 0);
        }
        else if (m_MidiNote[idx] != 0xFF)
        {
            // note is on
            ui->ledsOut.set(idx, LedOutBank::On);
            ui->gatesOut.set(idx, 1);
            PitchOut(ui->cvOut, idx, m_MidiNote[idx], m_BaseNote);
        }
        else
        {
            // note is off
            ui->ledsOut.set(idx, LedOutBank::Off);
            ui->gatesOut.set(idx, 0);
            // leave midi note unchanged
        }
    }

    if (ui->learnBtn.IsFalling())
    {
        ui->printToggleLearn('P', '2');

        //toggle learn mode on/off
        Learn(ui->learnMode.Get() == Midi8UI::Learn1);
    }
    else if (ui->learnMode.Get() != Midi8UI::Learn2)
    {
        ui->learnMode.Set(IsLearning() ? Midi8UI::Learn1 : Midi8UI::NoLearn);
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
