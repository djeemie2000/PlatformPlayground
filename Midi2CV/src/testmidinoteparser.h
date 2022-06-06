#pragma once
#include <Arduino.h>
#include "midinoteparser.h"

void ExpectEquals(bool expected, bool actual, const char* description)
{
    if(expected != actual)
    {
        Serial.print(description);
        Serial.print(" : expected ");
        Serial.print(expected);
        Serial.print(" but was ");
        Serial.println(actual);
    }
}

void ExpectEquals(uint8_t expected, uint8_t actual, const char* description)
{
    if(expected != actual)
    {
        Serial.print(description);
        Serial.print(" : expected ");
        Serial.print(expected, HEX);
        Serial.print(" but was ");
        Serial.println(actual, HEX);
    }
}

void TestNoteOnOff()
{
    MidiNoteParser parser;

    MidiVoiceMessage message;
    ExpectEquals(false, parser.Parse(0x90, message), "TestNoteOnOff");
    ExpectEquals(false, parser.Parse(0x2A, message), "TestNoteOnOff");
    ExpectEquals(true, parser.Parse(0x7B, message), "TestNoteOnOff");
    ExpectEquals(0x90, message.StatusByte, "TestNoteOnOff");
    ExpectEquals(0x2A, message.Param1, "TestNoteOnOff");
    ExpectEquals(0x7B, message.Param2, "TestNoteOnOff");

    ExpectEquals(false, parser.Parse(0x80, message), "TestNoteOnOff");
    ExpectEquals(false, parser.Parse(0x16, message), "TestNoteOnOff");
    ExpectEquals(true, parser.Parse(0x7B, message), "TestNoteOnOff");
    ExpectEquals(0x80, message.StatusByte, "TestNoteOnOff");
    ExpectEquals(0x16, message.Param1, "TestNoteOnOff");
    ExpectEquals(0x7B, message.Param2, "TestNoteOnOff");
}

void TestNoteOnOffSystemRealtime()
{
    MidiNoteParser parser;

    MidiVoiceMessage message;
    ExpectEquals(false, parser.Parse(0xFF, message), "TestNoteOnOffSystemRealtime");
    ExpectEquals(false, parser.Parse(0x90, message), "TestNoteOnOffSystemRealtime");
    ExpectEquals(false, parser.Parse(0xF8, message), "TestNoteOnOffSystemRealtime");
    ExpectEquals(false, parser.Parse(0x2A, message), "TestNoteOnOffSystemRealtime");
    ExpectEquals(false, parser.Parse(0xF9, message), "TestNoteOnOffSystemRealtime");
    ExpectEquals(true, parser.Parse(0x7B, message), "TestNoteOnOffSystemRealtime");
    ExpectEquals(0x90, message.StatusByte, "TestNoteOnOffSystemRealtime");
    ExpectEquals(0x2A, message.Param1, "TestNoteOnOffSystemRealtime");
    ExpectEquals(0x7B, message.Param2, "TestNoteOnOffSystemRealtime");
    ExpectEquals(false, parser.Parse(0xFA, message), "TestNoteOnOffSystemRealtime");

    ExpectEquals(false, parser.Parse(0x80, message), "TestNoteOnOffSystemRealtime");
    ExpectEquals(false, parser.Parse(0xFB, message), "TestNoteOnOffSystemRealtime");
    ExpectEquals(false, parser.Parse(0xFE, message), "TestNoteOnOffSystemRealtime");
    ExpectEquals(false, parser.Parse(0x16, message), "TestNoteOnOffSystemRealtime");
    ExpectEquals(false, parser.Parse(0xFC, message), "TestNoteOnOffSystemRealtime");
    ExpectEquals(true, parser.Parse(0x7B, message), "TestNoteOnOffSystemRealtime");
    ExpectEquals(0x80, message.StatusByte, "TestNoteOnOffSystemRealtime");
    ExpectEquals(0x16, message.Param1, "TestNoteOnOffSystemRealtime");
    ExpectEquals(0x7B, message.Param2, "TestNoteOnOffSystemRealtime");
    ExpectEquals(false, parser.Parse(0xFD, message), "TestNoteOnOffSystemRealtime");

}

void TestNoteOnOnRunningStatus()
{
    MidiNoteParser parser;

    MidiVoiceMessage message;
    ExpectEquals(false, parser.Parse(0x91, message), "TestNoteOnOnRunningStatus");
    ExpectEquals(false, parser.Parse(0x2A, message), "TestNoteOnOnRunningStatus");
    ExpectEquals(true, parser.Parse(0x7B, message), "TestNoteOnOnRunningStatus");
    ExpectEquals(0x91, message.StatusByte, "TestNoteOnOnRunningStatus");
    ExpectEquals(0x2A, message.Param1, "TestNoteOnOnRunningStatus");
    ExpectEquals(0x7B, message.Param2, "TestNoteOnOnRunningStatus");

    // no repeat of 0x90
    ExpectEquals(false, parser.Parse(0x16, message), "TestNoteOnOnRunningStatus");
    ExpectEquals(true, parser.Parse(0x7B, message), "TestNoteOnOnRunningStatus");
    ExpectEquals(0x91, message.StatusByte, "TestNoteOnOnRunningStatus");
    ExpectEquals(0x16, message.Param1, "TestNoteOnOnRunningStatus");
    ExpectEquals(0x7B, message.Param2, "TestNoteOnOnRunningStatus");
}

void TestNoteOnOffInsideSysex()
{
    MidiNoteParser parser;

    MidiVoiceMessage message;
    //start sysex
    ExpectEquals(false, parser.Parse(0xF0, message), "TestNoteOnOffInsideSysex");
    
    ExpectEquals(false, parser.Parse(0x90, message), "TestNoteOnOffInsideSysex");
    ExpectEquals(false, parser.Parse(0x2A, message), "TestNoteOnOffInsideSysex");
    ExpectEquals(false, parser.Parse(0x7B, message), "TestNoteOnOffInsideSysex");

    ExpectEquals(false, parser.Parse(0x80, message), "TestNoteOnOffInsideSysex");
    ExpectEquals(false, parser.Parse(0x16, message), "TestNoteOnOffInsideSysex");
    ExpectEquals(false, parser.Parse(0x7B, message), "TestNoteOnOffInsideSysex");

    //end sysex
    ExpectEquals(false, parser.Parse(0xF7, message), "TestNoteOnOffInsideSysex");    

    //after sysex
    ExpectEquals(false, parser.Parse(0x90, message), "TestNoteOnOffAfterSysex");
    ExpectEquals(false, parser.Parse(0x2A, message), "TestNoteOnOffAfterSysex");
    ExpectEquals(true, parser.Parse(0x7B, message), "TestNoteOnOffAfterSysex");
    ExpectEquals(0x90, message.StatusByte, "TestNoteOnOffAfterSysex");
    ExpectEquals(0x2A, message.Param1, "TestNoteOnOffAfterSysex");
    ExpectEquals(0x7B, message.Param2, "TestNoteOnOffAfterSysex");
}

void TestNoteOnOffSystemCommon()
{
    MidiNoteParser parser;

    MidiVoiceMessage message;
    ExpectEquals(false, parser.Parse(0x90, message), "TestNoteOnOffSystemCommon");
    ExpectEquals(false, parser.Parse(0x2A, message), "TestNoteOnOffSystemCommon");
    // sys common => affects status byte
    ExpectEquals(false, parser.Parse(0xF3, message), "TestNoteOnOffSystemCommon");
    ExpectEquals(false, parser.Parse(0x7B, message), "TestNoteOnOffSystemCommon");

    ExpectEquals(false, parser.Parse(0x90, message), "TestNoteOnOffSystemCommon");
    // sys common => affects status byte
    ExpectEquals(false, parser.Parse(0xF2, message), "TestNoteOnOffSystemCommon");
    ExpectEquals(false, parser.Parse(0x2A, message), "TestNoteOnOffSystemCommon");
    ExpectEquals(false, parser.Parse(0x7B, message), "TestNoteOnOffSystemCommon");

    ExpectEquals(false, parser.Parse(0x94, message), "TestNoteOnOffSystemCommon");
    // sys common => affects status byte
    ExpectEquals(false, parser.Parse(0xF2, message), "TestNoteOnOffSystemCommon");
    ExpectEquals(false, parser.Parse(0x2A, message), "TestNoteOnOffSystemCommon");
    ExpectEquals(false, parser.Parse(0x7B, message), "TestNoteOnOffSystemCommon");

    ExpectEquals(false, parser.Parse(0xFD, message), "TestNoteOnOffSystemCommon");
}

void TestNoteOnOffIncomplete()
{
    // note on/off with param1 then statusbyte (not param2) => ignore that param1
    MidiNoteParser parser;

    MidiVoiceMessage message;
    ExpectEquals(false, parser.Parse(0x90, message), "TestNoteOnOffIncomplete");
    ExpectEquals(false, parser.Parse(0x2A, message), "TestNoteOnOffIncomplete");

    ExpectEquals(false, parser.Parse(0x90, message), "TestNoteOnOffIncomplete");
    ExpectEquals(false, parser.Parse(0x3C, message), "TestNoteOnOffIncomplete");
    ExpectEquals(true, parser.Parse(0x7B, message), "TestNoteOnOffIncomplete");
    ExpectEquals(0x90, message.StatusByte, "TestNoteOnOffIncomplete");
    ExpectEquals(0x3C, message.Param1, "TestNoteOnOffIncomplete");
    ExpectEquals(0x7B, message.Param2, "TestNoteOnOffIncomplete");

    ExpectEquals(false, parser.Parse(0x80, message), "TestNoteOnOffIncomplete");
    ExpectEquals(false, parser.Parse(0x16, message), "TestNoteOnOffIncomplete");


    ExpectEquals(false, parser.Parse(0x80, message), "TestNoteOnOffIncomplete");
    ExpectEquals(false, parser.Parse(0x27, message), "TestNoteOnOffIncomplete");
    ExpectEquals(true, parser.Parse(0x67, message), "TestNoteOnOffIncomplete");
    ExpectEquals(0x80, message.StatusByte, "TestNoteOnOffIncomplete");
    ExpectEquals(0x27, message.Param1, "TestNoteOnOffIncomplete");
    ExpectEquals(0x67, message.Param2, "TestNoteOnOffIncomplete");
}


//TODO
//  incomplete 

void TestNoteOnOffOtherVoiceMessages()
{
    // note on/off vs other voice messages
    // complete messages
    // 1 byte / 2 byte voice messages

    MidiNoteParser parser;

    MidiVoiceMessage message;

    // channel pressure 1 param byte
    ExpectEquals(false, parser.Parse(0xDA, message), "TestNoteOnOffOtherVoiceMessages");
    ExpectEquals(false, parser.Parse(0x19, message), "TestNoteOnOffOtherVoiceMessages");

    ExpectEquals(false, parser.Parse(0x90, message), "TestNoteOnOffOtherVoiceMessages");
    ExpectEquals(false, parser.Parse(0x2A, message), "TestNoteOnOffOtherVoiceMessages");
    ExpectEquals(true, parser.Parse(0x7B, message), "TestNoteOnOffOtherVoiceMessages");
    ExpectEquals(0x90, message.StatusByte, "TestNoteOnOffOtherVoiceMessages");
    ExpectEquals(0x2A, message.Param1, "TestNoteOnOffOtherVoiceMessages");
    ExpectEquals(0x7B, message.Param2, "TestNoteOnOffOtherVoiceMessages");

    // program change 1 param byte
    ExpectEquals(false, parser.Parse(0xC3, message), "TestNoteOnOffOtherVoiceMessages");
    ExpectEquals(false, parser.Parse(0x2A, message), "TestNoteOnOffOtherVoiceMessages");
    
    // midi CC 2 param bytes
    ExpectEquals(false, parser.Parse(0xB3, message), "TestNoteOnOffOtherVoiceMessages");
    ExpectEquals(false, parser.Parse(0x2A, message), "TestNoteOnOffOtherVoiceMessages");
    ExpectEquals(false, parser.Parse(0x47, message), "TestNoteOnOffOtherVoiceMessages");
    
    ExpectEquals(false, parser.Parse(0x80, message), "TestNoteOnOffOtherVoiceMessages");
    ExpectEquals(false, parser.Parse(0x16, message), "TestNoteOnOffOtherVoiceMessages");
    ExpectEquals(true, parser.Parse(0x7B, message), "TestNoteOnOffOtherVoiceMessages");
    ExpectEquals(0x80, message.StatusByte, "TestNoteOnOffOtherVoiceMessages");
    ExpectEquals(0x16, message.Param1, "TestNoteOnOffOtherVoiceMessages");
    ExpectEquals(0x7B, message.Param2, "TestNoteOnOffOtherVoiceMessages");
}


void TestAll()
{
    Serial.println("testmidinoteparser begin");
    TestNoteOnOff();
    TestNoteOnOnRunningStatus();
    TestNoteOnOffSystemRealtime();
    TestNoteOnOffSystemCommon();
    TestNoteOnOffInsideSysex();
    TestNoteOnOffIncomplete();
    TestNoteOnOffOtherVoiceMessages();
    Serial.println("testmidinoteparser done");
}

