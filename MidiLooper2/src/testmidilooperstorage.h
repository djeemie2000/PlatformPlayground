#pragma once
#include <Arduino.h>

class SDStorage;
class MidiLooperStorage;

void TestMidiLooperStorage(MidiLooperStorage& storage, SDStorage& sdStorage, HardwareSerial& serialDebug);
