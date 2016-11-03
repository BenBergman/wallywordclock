#include <stdio.h>
#include "CppUTest/TestHarness.h"

extern "C"
{
#include "hello_world.h"
}

TEST_GROUP(HelloWorld)
{
};

TEST(HelloWorld, FirstTest)
{
    CHECK_EQUAL(true, true);
}
