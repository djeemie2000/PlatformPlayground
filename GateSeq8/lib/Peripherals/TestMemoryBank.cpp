#include "TestMemoryBank.h"
#include "MemoryBank.h"
#include "../IO/SerialOut.h"

void TestMemoryBank(MemoryBank& memBank, SerialOut& debugSerial, int bank)
{
    int bankSize = memBank.BankSize();
    uint8_t* data = new uint8_t[bankSize];

    //
    for(int idx = 0; idx<bankSize; ++idx)
    {
        data[idx] = idx & 0xFF;
    }
    int numWrite = memBank.WriteBank(bank, data, bankSize);
    memset(data, 0x00, bankSize);
    int numRead = memBank.ReadBank(bank, data, bankSize);
    
    int numEquals = 0;
    for(int idx = 0; idx<bankSize; ++idx)
    {
        if(data[idx] == idx)
        {
            ++numEquals;
        }
    }

    delete[] data;

    bool ok = (numWrite==bankSize) && (numRead==bankSize) && (numEquals==bankSize);

    debugSerial.printf("Test memoryBank : numWrite=%d numRead=%d numEquals=%d => ok=%d", numWrite, numRead, numEquals, ok?1:0);
}
