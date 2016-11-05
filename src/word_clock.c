#include <stdbool.h>

#include "word_clock.h"

void time_to_words(bool *words, int hour, int minute) {
    int hour_word = H_ONE -1 + hour % 12;
    int period = hour < 12 ? AM : PM;

    words[IT] = true;
    words[IS] = true;
    if (minute >= 5 && minute < 10) {
        words[FIVE] = true;
        words[PAST] = true;
    } else if (minute >= 10 && minute < 15) {
        words[TEN] = true;
        words[PAST] = true;
    } else if (minute >= 15 && minute < 20) {
        words[QUARTER] = true;
        words[PAST] = true;
    }
    words[hour_word] = true;
    words[period] = true;
    words[IN] = true;
    words[THE] = true;
    if (hour >= 0 && hour < 12) {
        words[MORNING] = true;
    } else if (hour >= 12 && hour < 18) {
        words[AFTERNOON] = true;
    } else if (hour >= 18) {
        words[EVENING] = true;
    }
}
