#pragma once

namespace mbedunit
{

class Test;
class TestReporter;

class TestRunner
{
public:
    static int RunAllTests(TestReporter* reporter);
    static void RegisterTest(Test* test);

private:
    TestRunner(){}

    static Test* testHead;
    static Test* testTail;
};

}
