#include "poly4handler.h"
#include "midi8ui.h"
#include "cvfunctions.h"
#include "EEPROM.h"

Poly4Handler::Poly4Handler()
    : m_LearnIdx(-1)
{
    m_Channel = 0xFF;
    m_BaseNote = 0x00;
    for (int idx = 0; idx < Size; ++idx)
    {
        m_MidiNote[idx] = 0xFF;
        m_Velocity[idx] = 0xFF;
    }
}

void Poly4Handler::NoteOn(uint8_t Channel, uint8_t MidiNote, uint8_t Velocity)
{
    //
    // Serial.print("P4 On  ");
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
                    m_Velocity[idx] = Velocity;
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

void Poly4Handler::NoteOff(uint8_t Channel, uint8_t MidiNote, uint8_t /*Velocity*/)
{
    //
    // Serial.print("P4 Off ");
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
                    m_Velocity[idx] = 0xFF; //not really necessary
                }
            }
        }
    }
}

bool Poly4Handler::IsLearning() const
{
    return m_LearnIdx != -1;
}

void Poly4Handler::Learn(bool learn)
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

void Poly4Handler::updateUI(Midi10UI *ui)
{
    // gates : 01 23
    // leds  : 89 1011
    // pitch : 0 2
    // velocity : 1 3
    for (int idx = 0; idx < Size; ++idx)
    {
        int column = 2 * idx;
        if (m_LearnIdx != -1)
        {
            ui->gateDigitalOut.set(column, LedOutBank::Off);       //gate
            ui->gateDigitalOut.set(column + 1, LedOutBank::Off);   //gate
            ui->gateDigitalOut.set(column + 8, LedOutBank::Blink); //led
            ui->gateDigitalOut.set(column + 9, LedOutBank::Blink); //led
            ui->cvOut.set(column, 0);                              //pitch
            ui->cvOut.set(column + 1, 0);                          //velocity
        }
        else if (m_MidiNote[idx] != 0xFF)
        {
            // note is on
            ui->gateDigitalOut.set(column, LedOutBank::On);           //gate
            ui->gateDigitalOut.set(column + 1, LedOutBank::On);       //gate
            ui->gateDigitalOut.set(column + 8, LedOutBank::On);       //led
            ui->gateDigitalOut.set(column + 9, LedOutBank::On);       //led
            PitchOut(ui->cvOut, column, m_MidiNote[idx], m_BaseNote); //pitch
            VelocityOut(ui->cvOut, column + 1, m_Velocity[idx]);      //velocity
        }
        else
        {
            // note is off
            ui->gateDigitalOut.set(column, LedOutBank::Off);     //gate
            ui->gateDigitalOut.set(column + 1, LedOutBank::Off); //gate
            ui->gateDigitalOut.set(column + 8, LedOutBank::Off); //led
            ui->gateDigitalOut.set(column + 9, LedOutBank::Off); //led
            // leave midi note/pitch, velocity unchanged
        }
    }
}

void Poly4Handler::saveParams(int offset)
{
    int off = offset;
    EEPROM.update(off++, 'P');
    EEPROM.update(off++, '4');
    EEPROM.update(off++, m_Channel);
    EEPROM.update(off++, m_BaseNote);
}

int Poly4Handler::paramSize() const
{
    return 4;
}

void Poly4Handler::loadParams(int offset)
{
    int off = offset;
    if ('P' == EEPROM.read(off++) && '4' == EEPROM.read(off++))
    {
        m_Channel = EEPROM.read(off++);
        m_BaseNote = EEPROM.read(off++);
    }
}
