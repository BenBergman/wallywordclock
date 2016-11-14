#ifndef WORD_CLOCK_H
#define WORD_CLOCK_H

#include <stdint.h>

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

typedef struct {
    int *map;
    int size;
} led_map_t;

void time_to_words(bool *words, int hour, int minute);
void words_to_leds(led_map_t led_map, bool *leds, bool *words);
void interpolate_frame_at_time(uint8_t leds[][3], bool *old_points, bool *new_points, int frame, uint8_t colour[3]);

#endif // WORD_CLOCK_H
