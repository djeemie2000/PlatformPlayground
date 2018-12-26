#pragma once

namespace mbedunit
{

//start of macro
#define MBEDUNIT_TEST(Name)                                                   \
   class Test ## Name : public mbedunit::Test                                            \
   {                                                                                     \
   public:                                                                               \
      Test ## Name() : Test(#Name, __FILE__, __LINE__) {\
            mbedunit::TestRunner::RegisterTest(this);  \
      } \
      void RunImpl(mbedunit::TestReporter* reporter);                                   \
   } static test ## Name ## Instance;                                                    \
   void Test ## Name::RunImpl(mbedunit::TestReporter* reporter) 
//end of macro

//start of macro
#define MBEDUNIT_EXPECT(expression) \
    reporter->OnAssert((expression), this, __LINE__, #expression);
//end of macro

}
