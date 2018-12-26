#include "SerialTestReporter.h"
#include "Test.h"

namespace mbedunit
{

SerialTestReporter::SerialTestReporter(Serial& pc) 
: pc_(pc)
, numTestsRun(0)
, numTestsFailed(0)
, numAsserts(0)
, numAssertsFailed(0)
{}

void SerialTestReporter::OnStartTests()
{
    pc_.printf("starting unit tests ...\r\n");
}

void SerialTestReporter::OnStartTest(Test* test)
{
    pc_.printf("test # %d %s (file %s line %d) started\r\n", numTestsRun, test->name_, test->file_, test->line_);
}

void SerialTestReporter::OnEndTest(Test* test)
{
    pc_.printf("test %d %s (file %s line %d) ended\r\n", numTestsRun, test->name_, test->file_, test->line_);
    ++numTestsRun;
}

void SerialTestReporter::OnAssert(bool success, Test* test, int line, const char* expression)
{
    ++numAsserts;
    if(success)
    {
        pc_.printf("assertion %s (file %s line %d) succeeded\r\n", expression, test->file_, line);
    }
    else
    {
        ++numAssertsFailed;
        pc_.printf("assertion %s (file %s line %d) failed!", expression, test->file_, line);
    }
}

int SerialTestReporter::OnEndTests()
{
    if(0==numAssertsFailed)
    {
        pc_.printf("unit tests succeeded : %d tests %d assertions \r\n", numTestsRun, numAsserts);
    }
    else
    {
        pc_.printf("unit tests failed : %d out of %d tests %d out of %d assertions \r\n", numTestsFailed, numTestsRun, numAssertsFailed, numAsserts);
    }

    return 0==numAssertsFailed?0:-1;
}

}
