#include "CppUTest/CommandLineTestRunner.h"

int main(int ac, const char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, const_cast<char**>(av));
}
