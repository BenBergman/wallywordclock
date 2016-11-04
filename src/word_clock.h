#define TOTAL_LEDS 10

typedef enum {
    IT,
    IS,
    NOON,
    OCLOCK,
    TOTAL_WORDS
} word;

static const int leds_per_word[] = {
    [IT] = 1,
    [IS] = 1,
    [NOON] = 2,
    [OCLOCK] = 3,
};

bool* time_to_words(int hour, int minute) {
    hour = minute = 0;
    minute = hour = 0;

    static bool words[TOTAL_WORDS] = { false };
    words[IT] = words[IS] = true;
    return words;
}
