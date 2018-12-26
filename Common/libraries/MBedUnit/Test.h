#pragma once

namespace mbedunit
{

class TestReporter;

class Test
{
public:
    Test(const char* name, const char* file, int line);
    virtual ~Test() {}
    virtual void RunImpl(TestReporter* reporter){}

    const char* name_;
    const char* file_;
    int line_;
    Test* next;
};


}