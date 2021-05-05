#include "clocksyncout.h"
#include "cvfunctions.h"
#include "midi10ui.h"

ClockSyncOut::ClockSyncOut()
    : m_IdxClock(0), m_IdxReset(0), m_Cntr(0x00)
{
}

void ClockSyncOut::Begin(int idxClock, int idxReset)
{
    m_IdxClock = idxClock;
    m_IdxReset = idxReset;
}

void ClockSyncOut::MidiClock()
{
    //toggle
    --m_Cntr;
}

void ClockSyncOut::MidiContinue()
{
    // trigger reset pulse
    m_Cntr = 0;
}

void ClockSyncOut::updateUI(Midi10UI *ui)
{
    if (m_Cntr == 0xFF)
    {
        LedOut(ui->ledsOut, m_IdxReset, LedOutBank::On);
    }
    else
    {
        LedOut(ui->ledsOut, m_IdxReset, LedOutBank::Off);
    }

    if (m_Cntr & 0x01)
    {
        LedOut(ui->ledsOut, m_IdxClock, LedOutBank::On);
    }
    else
    {
        LedOut(ui->ledsOut, m_IdxClock, LedOutBank::Off);
    }
}
