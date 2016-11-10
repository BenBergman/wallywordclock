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
    bool words[TOTAL_WORDS] = { false };
    time_to_words(words, 1, 2);

    bool expected_words[TOTAL_WORDS] = { false };
    expected_words[IT] = true;
    expected_words[IS] = true;
    expected_words[H_ONE] = true;
    expected_words[AM] = true;
    expected_words[IN] = true;
    expected_words[THE] = true;
    expected_words[MORNING] = true;

    MEMCMP_EQUAL(expected_words, words, sizeof(expected_words));
}

TEST(WordClock, AfternoonWithMinutes)
{
    bool words[TOTAL_WORDS] = { false };
    time_to_words(words, 15, 15);

    bool expected_words[TOTAL_WORDS] = { false };
    expected_words[IT] = true;
    expected_words[IS] = true;
    expected_words[QUARTER] = true;
    expected_words[PAST] = true;
    expected_words[H_THREE] = true;
    expected_words[PM] = true;
    expected_words[IN] = true;
    expected_words[THE] = true;
    expected_words[AFTERNOON] = true;

    MEMCMP_EQUAL(expected_words, words, sizeof(expected_words));
}

TEST(WordClock, TwentyFiveToNineInTheEvening)
{
    bool words[TOTAL_WORDS] = { false };
    time_to_words(words, 20, 35);

    bool expected_words[TOTAL_WORDS] = { false };
    expected_words[IT] = true;
    expected_words[IS] = true;
    expected_words[TWENTY] = true;
    expected_words[FIVE] = true;
    expected_words[TO] = true;
    expected_words[H_NINE] = true;
    expected_words[PM] = true;
    expected_words[IN] = true;
    expected_words[THE] = true;
    expected_words[EVENING] = true;

    MEMCMP_EQUAL(expected_words, words, sizeof(expected_words));
}

TEST(WordClock, NoonOclock)
{
    bool words[TOTAL_WORDS] = { false };
    time_to_words(words, 12, 0);

    bool expected_words[TOTAL_WORDS] = { false };
    expected_words[IT] = true;
    expected_words[IS] = true;
    expected_words[NOON] = true;
    expected_words[OCLOCK] = true;

    MEMCMP_EQUAL(expected_words, words, sizeof(expected_words));
}

TEST(WordClock, TenToNoonOclock)
{
    bool words[TOTAL_WORDS] = { false };
    time_to_words(words, 11, 53);

    bool expected_words[TOTAL_WORDS] = { false };
    expected_words[IT] = true;
    expected_words[IS] = true;
    expected_words[TEN] = true;
    expected_words[TO] = true;
    expected_words[NOON] = true;
    expected_words[OCLOCK] = true;

    MEMCMP_EQUAL(expected_words, words, sizeof(expected_words));
}

TEST(WordClock, FivePastMidnight)
{
    bool words[TOTAL_WORDS] = { false };
    time_to_words(words, 0, 7);

    bool expected_words[TOTAL_WORDS] = { false };
    expected_words[IT] = true;
    expected_words[IS] = true;
    expected_words[FIVE] = true;
    expected_words[PAST] = true;
    expected_words[MIDNIGHT] = true;
    expected_words[OCLOCK] = true;

    MEMCMP_EQUAL(expected_words, words, sizeof(expected_words));
}

TEST(WordClock, QuarterToMidnight)
{
    bool words[TOTAL_WORDS] = { false };
    time_to_words(words, 23, 49);

    bool expected_words[TOTAL_WORDS] = { false };
    expected_words[IT] = true;
    expected_words[IS] = true;
    expected_words[QUARTER] = true;
    expected_words[TO] = true;
    expected_words[MIDNIGHT] = true;
    expected_words[OCLOCK] = true;

    MEMCMP_EQUAL(expected_words, words, sizeof(expected_words));
}





TEST(WordClock, BasicWordsToLedsConversion)
{
    bool words[TOTAL_WORDS] = { false };
    words[IT] = true;
    words[IS] = true;
    words[QUARTER] = true;
    words[TO] = true;
    words[MIDNIGHT] = true;
    words[OCLOCK] = true;

    int master_led_map[] = {
        IT, IS,
        FIVE, TEN, QUARTER, TWENTY, HALF,
        PAST, TO,
        H_ONE, H_TWO, H_THREE, H_FOUR, H_FIVE, H_SIX, H_SEVEN, H_EIGHT, H_NINE, H_TEN, H_ELEVEN,
        NOON, MIDNIGHT,
        OCLOCK,
        AM, PM,
        IN, THE,
        MORNING, AFTERNOON, EVENING,
        HAPPY, BIRTHDAY,
        GO, TO2, SLEEP,
    };

    led_map_t my_map;
    my_map.map = master_led_map;
    my_map.size = sizeof(master_led_map) / sizeof(master_led_map[0]);

    bool *leds = new bool[my_map.size];
    //leds = { false };
    words_to_leds(my_map, leds, words);

    bool *expected_leds = new bool[my_map.size];
    //expected_leds = { false };
    expected_leds[0] = expected_leds[1] = true;
    expected_leds[4] = true;
    expected_leds[8] = true;
    expected_leds[21] = true;
    expected_leds[22] = true;

    MEMCMP_EQUAL(expected_leds, leds, sizeof(expected_leds));

    delete[] leds;
    delete[] expected_leds;
}
