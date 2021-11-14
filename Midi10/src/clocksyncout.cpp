#include "clocksyncout.h"
#include "cvfunctions.h"
#include "midi8ui.h"

ClockSyncHandler::ClockSyncHandler()
    : m_IdxClock(0), m_IdxClockLed(0), m_IdxReset(0), m_IdxResetLed(0), m_IdxClock2(0), m_IdxClock3(0), m_Cntr(0x00)
{
}

void ClockSyncHandler::Begin(int idxClock, int idxReset, int idxClockLed, int idxResetLed, int idxClock2, int idxClock3)
{
    m_IdxClock = idxClock;
    m_IdxClockLed = idxClockLed;
    m_IdxReset = idxReset;
    m_IdxResetLed = idxResetLed;

    m_IdxClock2 = idxClock2;
    m_IdxClock3 = idxClock3;
}

void ClockSyncHandler::MidiClock()
{
    //toggle
    --m_Cntr;
}

void ClockSyncHandler::MidiContinue()
{
    // trigger reset pulse
    m_Cntr = 0;
}

void ClockSyncHandler::updateUI(Midi10UI *ui)
{
    if (m_Cntr == 0xFF)
    {
        ui->clockDigitalOut.set(m_IdxReset, DigitalOutBank::On);
        ui->clockDigitalOut.set(m_IdxResetLed, DigitalOutBank::On);
    }
    else
    {
        ui->clockDigitalOut.set(m_IdxReset, DigitalOutBank::Off);
        ui->clockDigitalOut.set(m_IdxResetLed, DigitalOutBank::Off);
    }

    if (m_Cntr & 0x01)
    {
        ui->clockDigitalOut.set(m_IdxClock, DigitalOutBank::On);
        ui->clockDigitalOut.set(m_IdxClockLed, DigitalOutBank::On);
    }
    else
    {
        ui->clockDigitalOut.set(m_IdxClock, DigitalOutBank::Off);
        ui->clockDigitalOut.set(m_IdxClockLed, DigitalOutBank::Off);
    }

    if (m_Cntr & 0x02)
    {
        ui->clockDigitalOut.set(m_IdxClock2, DigitalOutBank::On);
    }
    else
    {
        ui->clockDigitalOut.set(m_IdxClock2, DigitalOutBank::Off);
    }

    if (m_Cntr & 0x04)
    {
        ui->clockDigitalOut.set(m_IdxClock3, DigitalOutBank::On);
    }
    else
    {
        ui->clockDigitalOut.set(m_IdxClock3, DigitalOutBank::Off);
    }

    // clock led off each 4th tick
    // if (m_Cntr & 0x07)
    // {
    // }
    // else
    // {
    // }
}
