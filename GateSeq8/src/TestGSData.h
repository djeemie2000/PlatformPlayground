#pragma once

#include <Arduino.h>
#include "GSData.h"
#include "SerialOut.h"

void PrintGSTrack(const GSTrack& track, SerialOut& debugSerial)
{
    unsigned int ptrn1 = track.m_Pattern>>16;
    unsigned int ptrn2 = track.m_Pattern & 0xFFFF;
    debugSerial.printf("id %d ver %d ptrn 0x%x%x l %d m %d gate %d",
                        track.m_ID, 
                        track.m_Version, 
                        ptrn1, ptrn2, 
                        track.m_NumSteps, 
                        track.m_Muted, 
                        int(track.m_GateDuration*256));
}

void PrintGSCommon(const GSCommon& common, SerialOut& debugSerial)
{
    debugSerial.printf("id %d ver %d bank %d ptrn %d",
                        common.m_Id, 
                        common.m_Version, 
                        common.m_SelectedBank, 
                        common.m_SelectedPattern);
}

