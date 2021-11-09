#pragma once
#include <Arduino.h>

class SummingQuantizer
{
public:
    SummingQuantizer();

    void begin(int cvPin1, int cvPin2, int outChannel);
    void update();

private:
    int m_CvPin1;
    int m_CvPin2;
    int m_OutChannel;
};
