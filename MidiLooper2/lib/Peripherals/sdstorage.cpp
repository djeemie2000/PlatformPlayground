#include "sdstorage.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

SDStorage::SDStorage()
 : m_IsOpen(false)
 {}

 bool SDStorage::Open(int CSPin)
 {
    m_IsOpen = false;
    if(SD.begin(CSPin))
    {
        uint8_t cardType = SD.cardType();
        m_IsOpen = (cardType != CARD_NONE);
    }
    return m_IsOpen;
 }

bool SDStorage::IsOpen() const
{
    return m_IsOpen;
}

bool SDStorage::Write(const char* path, uint8_t value)
{
    File file = SD.open(path, FILE_WRITE);
    if(file)
    {
        file.write(value);
        file.close();
        return true;
    }
    return false;
}

bool SDStorage::Write(const char* path, uint8_t* values, int size)
{
    File file = SD.open(path, FILE_WRITE);
    if(file)
    {
        file.write(values, size);
        file.close();
        return true;
    }
    return false;
}

bool SDStorage::Read(const char* path, uint8_t& value)
{
    bool succeeded = false;
    File file = SD.open(path, FILE_READ);
    if(file)
    {
        if(file.available())
        {
            value = file.read();//?? error => -1
            succeeded = true;
        }
        //else error
        file.close();
    }
    return succeeded;
}

bool SDStorage::Read(const char* path, uint8_t* values, int capacity, int& size)
{
    bool succeeded = false;
    File file = SD.open(path, FILE_READ);
    if(file)
    {
        size = file.readBytes((char*)values, capacity);
        succeeded = true;
        file.close();
    }
    return succeeded;
}

bool SDStorage::Remove(const char* path)
{
    return SD.remove(path);
}

void SDStorage::PrintState(HardwareSerial& serial)
{

  uint8_t cardType = SD.cardType();
  serial.print("SD Card Type: ");
  if(cardType == CARD_MMC)
  {
    serial.println("MMC");
  } 
  else if(cardType == CARD_SD)
  {
    serial.println("SDSC");
  } 
  else if(cardType == CARD_SDHC)
  {
    serial.println("SDHC");
  } 
  else if(cardType== CARD_NONE)
  {
    serial.println("NONE");
  }
  else
  {
    serial.println("UNKNOWN");
  }

    if(IsOpen())
    {
        uint64_t cardSize = SD.cardSize() / (1024 * 1024);
        serial.printf("SD Card Size: %llu MB\n", cardSize);

        serial.printf("Total space: %llu bytes\n", SD.totalBytes());
        serial.printf("Used space: %llu bytes\n", SD.usedBytes());
    }
}

void SDStorage::PrintListDir(const char* path, HardwareSerial& serial)
{
    if(IsOpen())
    {
        serial.printf("Listing directory: %s\n", path);

        File root = SD.open(path);
        if(!root)
        {
            serial.println("Failed to open directory");
            return;
        }

        if(!root.isDirectory())
        {
            serial.println("Not a directory");
            return;
        }

        File file = root.openNextFile();
        while(file)
        {
            if(file.isDirectory())
            {
                serial.print("  DIR : ");
                serial.println(file.name());
            } 
            else
            {
                serial.print("  FILE: ");
                serial.print(file.name());
                serial.print("  SIZE: ");
                serial.println(file.size());
            }
            file = root.openNextFile();
        }
    }
}