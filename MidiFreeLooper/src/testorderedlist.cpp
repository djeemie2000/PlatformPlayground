#include "testorderedlist.h"
#include <orderedlist.h>

template<int N, class ValueType>
void PrintList(HardwareSerial& serial, OrderedList<N, char>& list)
{
  serial.print("Size ");
  serial.println(list.Size());
  
  list.ResetHead();
  while(list.MoveHeadToNext())
  {
    serial.print(list.HeadKey());
    serial.print(' ');
    serial.println(list.HeadValue());
  }
}

void TestListAddInOrder(HardwareSerial& serial)
{
  serial.println("TestListAddInOrder");

  OrderedList<8, char> list('0');

  list.Add(12, 'a');
  list.Add(12, 'b');
  list.Add(16, 'c');
  list.Add(14, 'n');
  list.Add(17, 'd');
  list.Add(2, 'n');
  list.Add(17, 'e');
  list.Add(18, 'f');
  list.Add(18, 'g');
  list.Add(17, 'n');
  list.Add(18, 'h');
  list.Add(18, 'i');// >= capacity 8

  PrintList(serial, list);
}

void TestListAddResetHeadAdd(HardwareSerial& serial)
{
  serial.println("TestListAddResetHeadAdd");

  OrderedList<8, char> list('0');

  list.Add(12, 'a');
  list.Add(12, 'b');
  list.ResetHead();
  list.Add(9, 'c');
  list.Add(11, 'd');
  list.ResetHead();
  list.Add(1, 'e');
  list.Add(2, 'f');

  list.Add(17, 'n');

  PrintList(Serial, list);
}

void TestListRead(HardwareSerial& serial)
{
  serial.println("TestListRead");

  OrderedList<8, char> list('0');

  list.Add(12, 'a');
  list.Add(12, 'b');
  list.Add(16, 'c');
  list.Add(17, 'd');
  list.Add(17, 'e');
  list.Add(18, 'f');
  list.Add(18, 'g');
  list.Add(20, 'h');

  PrintList(Serial, list);

  for(int repeat = 0; repeat<2; ++repeat)
  {
    for(int key = 0; key<24; ++key)
    {
      serial.print("Key ");
      serial.println(key);
      // handle if begin is not at key ??
      if(0==key)
      {
        list.ResetHead();
      }

      while(list.MoveHeadToNextIfNextKeyEquals(key))
      {
        serial.println(list.HeadValue());
      }
    }
  }
}

template<int N, class ValueType>
void AddToList(HardwareSerial& serial, OrderedList<N, ValueType>& list, int key, char value)
{
  bool added = list.Add(key, value);
  serial.print("add ");
  serial.print(value);
  serial.print(" at ");
  serial.print(key);
  serial.print(" -> ");
  serial.println(added);
}

void TestListReadWrite(HardwareSerial& serial)
{
  serial.println("TestListReadWrite");

  OrderedList<8, char> list('0');
  int writeCntr = 0;
  char writeValue = 'a';

  for(int repeat = 0; repeat<4; ++repeat)
  {
    serial.print("Repeat ");
    serial.println(repeat);

    // do this if you want to use the same keys on each repeat:
    writeCntr = 0;

    for(int key = 0; key<24; ++key)
    {
      serial.print("Key ");
      serial.println(key);
      // handle if begin is not at key ?
      if(0==key)
      {
        list.ResetHead();
      }

      while(list.MoveHeadToNextIfNextKeyEquals(key))
      {
        serial.println(list.HeadValue());
      }

      ++writeCntr;
      if(8==writeCntr)
      {
        AddToList(serial, list, key, writeValue);
        writeValue+=1;
      }
      else if(13==writeCntr)
      {
        AddToList(serial, list, key, writeValue);
        writeValue+=1;
        writeCntr = -1;
      }
    }

    PrintList(serial, list);
  }
}
