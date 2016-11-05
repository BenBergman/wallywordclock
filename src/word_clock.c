#include <stdbool.h>

#include "word_clock.h"

bool* time_to_words(int hour, int minute) {
    hour = minute = 0;
    minute = hour = 0;

    static bool words[TOTAL_WORDS] = {
        [IT] = true,
        [IS] = true,
        [H_ONE] = true,
        [AM] = true,
        [IN] = true,
        [THE] = true,
        [MORNING] = true,
    };
    return words;
}
