#include <stdio.h>
#include "CppUTest/TestHarness.h"

extern "C"
{
#include "word_clock.h"
}

TEST_GROUP(WordClock)
{
};

TEST(WordClock, FirstTest)
{
    CHECK_EQUAL(leds_per_word[IT], 1);

    bool *words = time_to_words(1, 2);
    bool expected_words[] = {true, true, false, false};
    for (int i = 0; i < TOTAL_WORDS; i++) {
        CHECK_EQUAL(words[i], expected_words[i]);
    }
}
