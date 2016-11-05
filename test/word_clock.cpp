#include <stdio.h>
#include "CppUTest/TestHarness.h"

extern "C"
{
#include "word_clock.h"
}

TEST_GROUP(WordClock)
{
};

void words_list_equal(bool *words, bool *expected_words) {
    for (int i = 0; i < TOTAL_WORDS; i++) {
        CHECK_EQUAL(words[i], expected_words[i]);
    }
}

TEST(WordClock, FirstTest)
{
    bool *words = time_to_words(1, 2);
    bool expected_words[TOTAL_WORDS] = { false };
    expected_words[IT] = true;
    expected_words[IS] = true;
    expected_words[H_ONE] = true;
    expected_words[AM] = true;
    expected_words[IN] = true;
    expected_words[THE] = true;
    expected_words[MORNING] = true;
    words_list_equal(words, expected_words);
}
