#include <Arduino.h>

class OrderedList;

void PrintList(OrderedList& list);
void AddToList(OrderedList& list, int key, char value);

void TestListAddInOrder();
void TestListAddResetHeadAdd();
void TestListRead();
void TestListReadWrite();
