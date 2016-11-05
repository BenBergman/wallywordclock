#define TOTAL_LEDS 10

typedef enum {
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
    TOTAL_WORDS
} word;

/*
static const int leds_per_word[] = {
    [IT] = 1,
    [IS] = 1,
    [NOON] = 2,
    [OCLOCK] = 3,
};
*/

bool* time_to_words(int hour, int minute);
