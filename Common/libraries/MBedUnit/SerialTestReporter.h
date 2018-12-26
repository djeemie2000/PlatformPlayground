#pragma once

#include <mbed.h>
#include "TestReporter.h"

namespace mbedunit
{

class SerialTestReporter : public TestReporter
{
public: 
    SerialTestReporter(Serial& pc);
    void OnStartTests();
    void OnStartTest(Test* test);
    void OnEndTest(Test* test);
    void OnAssert(bool success, Test* test, int line, const char* expression);
    int OnEndTests();

private:
    Serial& pc_;
    int numTestsRun;
    int numTestsFailed;
    int numAsserts;
    int numAssertsFailed;
};

}
