#include "TestRunner.h"
#include "Test.h"
#include "TestReporter.h"

namespace mbedunit
{
Test* TestRunner::testHead = 0;
Test* TestRunner::testTail = 0;

void TestRunner::RegisterTest(Test* test)
{
    if(!testHead)
    {
        testHead = test;
        testTail = test;
    }
    testTail->next = test;
    testTail = test;
}

int TestRunner::RunAllTests(TestReporter* reporter)
{
    reporter->OnStartTests();
    Test* test = testHead;
    while(test)
    {
        reporter->OnStartTest(test);
        test->RunImpl(reporter);
        reporter->OnEndTest(test);
        test = test->next;
    }
    return reporter->OnEndTests();
}

}
