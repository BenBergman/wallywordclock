#include <stdbool.h>

#include "word_clock.h"

bool* time_to_words(int hour, int minute) {
    hour = minute = 0;
    minute = hour = 0;

    static bool words[TOTAL_WORDS] = { false };
    words[IT] = words[IS] = true;
    return words;
}
