#pragma once

namespace mbedunit
{

class Test;

class TestReporter
{
public:
    virtual ~TestReporter(){};

    virtual void OnStartTest(Test* test){}

    virtual void OnEndTest(Test* test){}

    virtual void OnAssert(bool success, Test* test, int line, const char* expression){}

    virtual void OnStartTests(){}

    virtual int OnEndTests(){return 0;}
};

}
