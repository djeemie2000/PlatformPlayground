#include "summingquantizer.h"
#include "mcp48xxdac.h"

SummingQuantizer::SummingQuantizer()
: m_CvPin1(-1)
, m_CvPin2(-1)
, m_OutChannel(-1)
{
}

void SummingQuantizer::begin(int cvPin1, int cvPin2, int outChannel)
{
    m_CvPin1 = cvPin1;
    m_CvPin2 = cvPin2;
    m_OutChannel = outChannel;
}

void SummingQuantizer::update()
{
    int cvValue1 = analogRead(m_CvPin1);
    int cvValue2 = analogRead(m_CvPin2);
    int cvSum = cvValue1 + cvValue2;
    // assuming 1 = 1 mV, 
    // quantize per 1000/12 mV
    int midiNote = cvSum * 12 / 1000;
    // quantized output is max 4.096 V = 4096 mV
    // => max midiNote is 49 corresponding to 4083 mV
    if(49<midiNote)
    {
        midiNote = 49;
    }
    int cvQuantized = midiNote * 1000 / 12;
    // mcp4822 output
    mcp48xxdac::SetOutput(cvQuantized, m_OutChannel == 0 ?  mcp48xxdac::Channel_A : mcp48xxdac::Channel_B, mcp48xxdac::Gain_x2, 0);
}