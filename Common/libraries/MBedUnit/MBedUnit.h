#pragma once

#include "TestReporter.h"
#include "TestRunner.h"
#include "SerialTestReporter.h"
#include "Test.h"
#include "TestMacros.h"

// EXAMPLE UNIT TESTS...
namespace mbedunit
{

MBEDUNIT_TEST(test2)
{
    int bs = 2;
    MBEDUNIT_EXPECT(bs == 2);
    MBEDUNIT_EXPECT(bs == 3);
}

MBEDUNIT_TEST(test3)
{
    int bs = 2;
    MBEDUNIT_EXPECT(bs == 3);
    MBEDUNIT_EXPECT(bs != 3);
}

}//namespace mbedunit

int testMain(Serial& pc)
{
    mbedunit::SerialTestReporter reporter(pc);
    int result = mbedunit::TestRunner::RunAllTests(&reporter);
    return result;
}


