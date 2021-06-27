#include "clocksyncout.h"
#include "cvfunctions.h"
#include "midi8ui.h"

ClockSyncHandler::ClockSyncHandler()
    : m_IdxClock(0), m_IdxReset(0), m_Cntr(0x00)
{
}

void ClockSyncHandler::Begin(int idxClock, int idxReset, int idxClockLed, int idxResetLed)
{
    m_IdxClock = idxClock;
    m_IdxClockLed = idxClockLed;
    m_IdxReset = idxReset;
    m_IdxResetLed = idxResetLed;
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
    }
    else
    {
        ui->clockDigitalOut.set(m_IdxClock, DigitalOutBank::Off);
    }
    // clock led each 4th tick
    if (m_Cntr & 0x07)
    {
        ui->clockDigitalOut.set(m_IdxClockLed, DigitalOutBank::On);
    }
    else
    {
        ui->clockDigitalOut.set(m_IdxClockLed, DigitalOutBank::Off);
    }
}
