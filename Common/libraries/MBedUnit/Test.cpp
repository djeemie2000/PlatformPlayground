#include "Test.h"

namespace mbedunit
{

Test::Test(const char* name, const char* file, int line) 
: name_(name)
, file_(file)
, line_(line)
, next(0)
{}

}
