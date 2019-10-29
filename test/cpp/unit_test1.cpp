// #include "some_program.hpp"
#include "gtest/gtest.h"

namespace
{
// TEST(SayHelloTest, Hello) {
//     EXPECT_EQ(SayHello(), "Hello World!");
// }
} // namespace

// some notes from deps/googletest/googletest/samples/sample1_unittest.cc
//
// TEST has two parameters: the test case name and the test name.
// After using the macro, you should define your test logic between a
// pair of braces.  You can use a bunch of macros to indicate the
// success or failure of a test.  EXPECT_TRUE and EXPECT_EQ are
// examples of such macros.  For a complete list, see gtest.h.
//
// <TechnicalDetails>
//
// In Google Test, tests are grouped into test cases.  This is how we
// keep test code organized.  You should put logically related tests
// into the same test case.
//
// The test case name and the test name should both be valid C++
// identifiers.  And you should not use underscore (_) in the names.
//
// Google Test guarantees that each test you define is run exactly
// once, but it makes no guarantee on the order the tests are
// executed.  Therefore, you should write your tests in such a way
// that their results don't depend on their order.
//
// </TechnicalDetails>
//
// <TechnicalDetails>
//
// EXPECT_EQ(expected, actual) is the same as
//
//   EXPECT_TRUE((expected) == (actual))
//
// except that it will print both the expected value and the actual
// value when the assertion fails.  This is very helpful for
// debugging.  Therefore in this case EXPECT_EQ is preferred.
//
// On the other hand, EXPECT_TRUE accepts any Boolean expression,
// and is thus more general.
//
// </TechnicalDetails>
