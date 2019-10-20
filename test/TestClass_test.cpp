#include <gtest/gtest.h>
#include <../include/TestClass.h>

TEST(TestClass, test_getA)
{
    TestClass test(1,2);
    ASSERT_EQ(test.getA(), 1);
}