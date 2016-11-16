#include <stdbool.h>
#include <stdio.h>

#include "word_clock.h"


void time_to_words(bool *words, int hour, int minute)
{
    int hour_word = H_ONE + ((hour - 1) % 12);
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
    } else if (minute >= 20 && minute < 25) {
        words[TWENTY] = true;
        words[PAST] = true;
    } else if (minute >= 25 && minute < 30) {
        words[TWENTY] = true;
        words[FIVE] = true;
        words[PAST] = true;
    } else if (minute >= 30 && minute < 35) {
        words[HALF] = true;
        words[PAST] = true;
    } else if (minute >= 35 && minute < 40) {
        words[TWENTY] = true;
        words[FIVE] = true;
        words[TO] = true;
    } else if (minute >= 40 && minute < 45) {
        words[TWENTY] = true;
        words[TO] = true;
    } else if (minute >= 45 && minute < 50) {
        words[QUARTER] = true;
        words[TO] = true;
    } else if (minute >= 50 && minute < 55) {
        words[TEN] = true;
        words[TO] = true;
    } else if (minute >= 55 && minute < 60) {
        words[FIVE] = true;
        words[TO] = true;
    }
    if (words[TO]) {
        hour_word += 1;
    }
    if (hour_word == NOON && hour != 23) {
        words[NOON] = true;
        words[OCLOCK] = true;
    } else if (hour_word == NOON || (words[PAST] && hour == 0)) {
        words[MIDNIGHT] = true;
        words[OCLOCK] = true;
    } else {
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
}


void words_to_leds(led_map_t led_map, bool *leds, bool *words)
{
    for (int i = 0; i < led_map.size; i++) {
        leds[i] = words[led_map.map[i]];
    }
}


long scale(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void interpolate_frame_at_time(uint8_t leds[][3], bool *old_points, bool *new_points, int frame, uint8_t colour[3])
{
    uint8_t in_colour[] = {
        (uint8_t)scale(colour[0], 0, 255, 0, frame),
        (uint8_t)scale(colour[1], 0, 255, 0, frame),
        (uint8_t)scale(colour[2], 0, 255, 0, frame)
    };
    uint8_t out_colour[] = {
        (uint8_t)scale(colour[0], 0, 255, 0, 255-frame),
        (uint8_t)scale(colour[1], 0, 255, 0, 255-frame),
        (uint8_t)scale(colour[2], 0, 255, 0, 255-frame)
    };
    for (int i = 0; i < 4; i++) { // TODO 4?!
        if (old_points[i] && new_points[i]) {
            for (int rgb = 0; rgb < 3; rgb++) {
                leds[i][rgb] = colour[rgb];
            }
        } else if (old_points[i] && !new_points[i]) {
            for (int rgb = 0; rgb < 3; rgb++) {
                leds[i][rgb] = out_colour[rgb];
            }
        } else if (new_points[i] && !old_points[i]) {
            for (int rgb = 0; rgb < 3; rgb++) {
                leds[i][rgb] = in_colour[rgb];
            }
        }
    }
}
