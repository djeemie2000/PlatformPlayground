#include <Arduino.h>
#include "orderedlist.h"


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("MidiFreeLooper v0.1...");
}

void PrintList(OrderedList& list)
{
  Serial.print("Size ");
  Serial.println(list.Size());
  
  list.ResetHead());
  while(list.MoveHeadToNext())
  {
    Serial.print(list.HeadKey());
    Serial.print(' ');
    Serial.println(list.HeadValue());
  }
}

void TestListAddInOrder()
{
  Serial.println("TestListAddInOrder");

  OrderedList list;

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

  PrintList(list);
}

void TestListAddResetHeadAdd()
{
  Serial.println("TestListAddResetHeadAdd");

  OrderedList list;

  list.Add(12, 'a');
  list.Add(12, 'b');
  list.ResetHead();
  list.Add(9, 'c');
  list.Add(11, 'd');
  list.ResetHead();
  list.Add(1, 'e');
  list.Add(2, 'f');

  list.Add(17, 'n');

  PrintList(list);
}

void TestListRead()
{
  Serial.println("TestListRead");

  OrderedList list;

  list.Add(12, 'a');
  list.Add(12, 'b');
  list.Add(16, 'c');
  list.Add(17, 'd');
  list.Add(17, 'e');
  list.Add(18, 'f');
  list.Add(18, 'g');
  list.Add(20, 'h');

  PrintList(list);

  for(int repeat = 0; repeat<2; ++repeat)
  {
    for(int key = 0; key<24; ++key)
    {
      Serial.print("Key ");
      Serial.println(key);
      // handle if begin is not at key ??
      if(0==key)
      {
        list.ResetHead();
      }

      while(list.MoveHeadToNextIfNextKeyEquals(key))
      {
        Serial.println(list.HeadValue());
      }
    }
  }
}

void AddToList(OrderedList& list, int key, char value)
{
  bool added = list.Add(key, value);
  Serial.print("add ");
  Serial.print(value);
  Serial.print(" at ");
  Serial.print(key);
  Serial.print(" -> ");
  Serial.println(added);
}

void TestListReadWrite()
{
  Serial.println("TestListReadWrite");

  OrderedList list;
  int writeCntr = 0;
  char writeValue = 'a';

  for(int repeat = 0; repeat<4; ++repeat)
  {
    Serial.print("Repeat ");
    Serial.println(repeat);

    // do this if you want to use the same keys on each repeat:
    writeCntr = 0;

    for(int key = 0; key<24; ++key)
    {
      Serial.print("Key ");
      Serial.println(key);
      // handle if begin is not at key ?
      if(0==key)
      {
        list.ResetHead();
      }

      while(list.MoveHeadToNextIfNextKeyEquals(key))
      {
        Serial.println(list.HeadValue());
      }

      ++writeCntr;
      if(8==writeCntr)
      {
        AddToList(list, key, writeValue);
        writeValue+=1;
      }
      else if(13==writeCntr)
      {
        AddToList(list, key, writeValue);
        writeValue+=1;
        writeCntr = -1;
      }
    }

    PrintList(list);
  }

}


void loop() {
  // put your main code here, to run repeatedly:
  TestListAddInOrder();
  delay(1000);

  TestListAddResetHeadAdd();
  delay(1000);
  
  TestListRead();
  delay(1000);

  TestListReadWrite();
  delay(1000);
  
  delay(20000);
}

