#include "poly4handler.h"
#include "midi8ui.h"
#include "cvfunctions.h"
#include "EEPROM.h"
#include "clocksyncout.h"

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

void Poly4Handler::begin(ClockSyncOut *clockSyncOut)
{
    m_ClockSyncOut = clockSyncOut;
}

void Poly4Handler::MidiClock()
{
    m_ClockSyncOut->MidiClock();
}

void Poly4Handler::MidiContinue()
{
    m_ClockSyncOut->MidiContinue();
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

void Poly4Handler::updateUI(Midi8UI *ui)
{
    for (int idx = 0; idx < Size; ++idx)
    {
        int column = 2 * idx;
        if (m_LearnIdx != -1)
        {
            //TODO all leds will blink same as in poly2
            ui->ledsOut.set(column, LedOutBank::Blink);
            //ui->ledsOut.set(column+1, LedOutBank::Blink);
            ui->ledsOut.set(column + 4, LedOutBank::Blink);
            ui->ledsOut.set(column + 5, LedOutBank::Blink);
            ui->gatesOut.set(column, 0);
            //ui->gatesOut.set(column+1, 0);
            ui->cvOut.set(column, 0);
            ui->cvOut.set(column + 1, 0);
        }
        else if (m_MidiNote[idx] != 0xFF)
        {
            // note is on
            ui->ledsOut.set(column, LedOutBank::On);
            //ui->ledsOut.set(column+1, LedOutBank::On);
            ui->ledsOut.set(column + 4, LedOutBank::On);
            ui->ledsOut.set(column + 5, LedOutBank::On);
            ui->gatesOut.set(column, 1);
            //ui->gatesOut.set(column+1, 1);
            PitchOut(ui->cvOut, column, m_MidiNote[idx], m_BaseNote);
            VelocityOut(ui->cvOut, column + 1, m_Velocity[idx]);
        }
        else
        {
            // note is off
            ui->ledsOut.set(column, LedOutBank::Off);
            //ui->ledsOut.set(column+1, LedOutBank::Off);
            ui->ledsOut.set(column + 4, LedOutBank::Off);
            ui->ledsOut.set(column + 5, LedOutBank::Off);
            ui->gatesOut.set(column, 0);
            //ui->gatesOut.set(column+1, 0);
            // leave midi note, velocity unchanged
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
