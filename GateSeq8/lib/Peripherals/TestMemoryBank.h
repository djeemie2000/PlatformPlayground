#pragma once

class MemoryBank;
class SerialOut;

void TestMemoryBank(MemoryBank& memBank, SerialOut& debugSerial, int bank);

void PrintMemoryBank(MemoryBank& memBank, SerialOut& debugSerial, int bank, int size, int offset);
