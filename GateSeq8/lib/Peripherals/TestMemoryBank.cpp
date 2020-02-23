#include "TestMemoryBank.h"
#include "MemoryBank.h"
#include "../IO/SerialOut.h"

void TestMemoryBank(MemoryBank& memBank, SerialOut& debugSerial, int bank)
{
    debugSerial.printf("Test memory bank %d ...", bank);

    int bankSize = memBank.BankSize();
    uint8_t* data = new uint8_t[bankSize];

    //
    for(int idx = 0; idx<bankSize; ++idx)
    {
        data[idx] = idx & 0xFF;
    }
    int numWrite = memBank.WriteBank(bank, data, bankSize, 0);
    memset(data, 0x00, bankSize);
    int numRead = memBank.ReadBank(bank, data, bankSize, 0);
    
    int numEquals = 0;
    for(int idx = 0; idx<numRead; ++idx)
    {
        if(data[idx] == idx)
        {
            ++numEquals;
        }
        else
        {
            debugSerial.printf("expected 0x%x actual 0x%x\r\n", idx, (int)data[idx]);
        }
        
    }

    delete[] data;

    bool ok = (numWrite==bankSize) && (numRead==bankSize) && (numEquals==bankSize);

    debugSerial.printf("Test memoryBank : numWrite=%d numRead=%d numEquals=%d => ok=%d", numWrite, numRead, numEquals, ok?1:0);
}

void PrintMemoryBank(MemoryBank& memBank, SerialOut& debugSerial, int bank, int size, int offset)
{
    debugSerial.printf("Print memory bank %d size %d offset %d...", bank, size, offset);

    uint8_t* data = new uint8_t[size];
    
    int numRead = memBank.ReadBank(bank, data, size, offset);
    debugSerial.printf("read %d bytes\r\n", numRead);
    for(int idx = 0; idx<numRead; ++idx)
    {
        debugSerial.printf("(%d : 0x%x)", idx, (int)data[idx]);
    }
    debugSerial.println();
    
    delete[]data;
    debugSerial.println();
}
