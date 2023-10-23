#include <Arduino.h>

template<int N, class ValueType>
class OrderedList;

template<int N,  class ValueType>
void PrintList(HardwareSerial& serial, OrderedList<N, ValueType>& list);

template<int N,  class ValueType>
void AddToList(HardwareSerial& serial, OrderedList<N, ValueType>& list, int key, char value);

void TestListAddInOrder(HardwareSerial& serial);
void TestListAddResetHeadAdd(HardwareSerial& serial);
void TestListRead(HardwareSerial& serial);
void TestListReadWrite(HardwareSerial& serial);
