/* 
 * TimeSerialDateStrings.pde
 * example code illustrating Time library date strings
 *
 * This sketch adds date string functionality to TimeSerial sketch
 * Also shows how to handle different messages
 *
 * A message starting with a time header sets the time
 * A Processing example sketch to automatically send the messages is inclided in the download
 * On Linux, you can use "date +T%s\n > /dev/ttyACM0" (UTC time zone)
 *
 * A message starting with a format header sets the date format

 * send: Fs\n for short date format
 * send: Fl\n for long date format 
 */ 
 
#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>
#include <EEPROM.h>


// single character message tags
#define TIME_HEADER   'T'   // Header tag for serial time sync message
#define FORMAT_HEADER 'F'   // Header tag indicating a date format message
#define FORMAT_SHORT  's'   // short month and day strings
#define FORMAT_LONG   'l'   // (lower case l) long month and day strings

#define TIME_REQUEST  7     // ASCII bell character requests a time sync message 

static boolean isLongFormat = true;





#include <Encoder.h>

Encoder myEnc(2, 3);
#define BUTTON_PIN 4

int buttonState = HIGH;
int lastButtonState = HIGH;

unsigned long lastDebounceTime = -1;
unsigned long debounceDelay = 50;

typedef enum Mode {
    DISPLAY_MODE,
    SET_TIME,
    SET_HUE,
    SET_SATURATION,
    SET_BRIGHTNESS,
    TOTAL_MODES,
} Mode;

Mode mode = DISPLAY_MODE;





#include "FastLED.h"

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define LED_DATA_PIN    6
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    122
CRGBArray<NUM_LEDS> leds;

#define BRIGHTNESS          255
#define FRAMES_PER_SECOND  120

#define FADE_RATE 1 // Smaller is slower

uint8_t hue = 100;
uint8_t saturation = 100;
uint8_t value = 150;





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
    DOT,
    TOTAL_WORDS
};


int led_map[NUM_LEDS] = {
    EVENING,EVENING,EVENING,EVENING,EVENING,
    HAPPY,HAPPY,HAPPY,HAPPY,
    BIRTHDAY,BIRTHDAY,BIRTHDAY,BIRTHDAY,BIRTHDAY,
    DOT,
    AFTERNOON,AFTERNOON,AFTERNOON,AFTERNOON,AFTERNOON,AFTERNOON,AFTERNOON,
    SLEEP,SLEEP,SLEEP,
    MORNING,MORNING,MORNING,MORNING,MORNING,MORNING,
    OCLOCK,OCLOCK,OCLOCK,OCLOCK,OCLOCK,
    AM,AM,AM,
    PM,PM,PM,
    IN,IN,
    THE,THE,THE,
    MIDNIGHT,MIDNIGHT,MIDNIGHT,MIDNIGHT,MIDNIGHT,MIDNIGHT,MIDNIGHT,
    NOON,NOON,NOON,NOON,
    H_ELEVEN,H_ELEVEN,H_ELEVEN,H_ELEVEN,H_ELEVEN,
    H_SEVEN,H_SEVEN,H_SEVEN,H_SEVEN,
    GO,GO,
    TO2,
    H_EIGHT,H_EIGHT,H_EIGHT,
    H_NINE,H_NINE,
    H_TEN,H_TEN,H_TEN,
    H_SIX,H_SIX,H_SIX,
    H_FIVE,H_FIVE,
    H_FOUR,H_FOUR,H_FOUR,
    H_THREE,H_THREE,H_THREE,H_THREE,
    H_TWO,H_TWO,H_TWO,
    TWENTY,TWENTY,TWENTY,TWENTY,TWENTY,
    FIVE,FIVE,
    PAST,PAST,PAST,
    TO,
    H_ONE,H_ONE,H_ONE,
    HALF,HALF,HALF,HALF,
    QUARTER,QUARTER,QUARTER,QUARTER,QUARTER,
    TEN,TEN,
    IS,IS,
    IT,IT
};

typedef struct {
    int *map;
    int size;
} led_map_t;







bool current_leds[NUM_LEDS];

void setup()  {
  delay(1000); // Power-up safety delay (copied from FastLED examples)

  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only
  setSyncProvider(RTC.get);  //set function to call when sync required

  if (timeStatus() != timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }

  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH);

  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // TODO: should this correction be changed when we have the final strip?
  FastLED.setBrightness(BRIGHTNESS);
  set_max_power_in_volts_and_milliamps(5, 450);
  set_max_power_indicator_LED(13);

  Serial.println("T1479235073");
  Serial.println("T1483306200");
  Serial.println("Waiting for sync message");

  int address = 0;
  EEPROM.get(address, hue);
  address += sizeof(hue);
  EEPROM.get(address, saturation);
  address += sizeof(saturation);
  EEPROM.get(address, value);
}

long lastDisplayTime = -100000;
long lastLedTime = -100000;
time_t lastActivity = 0;

void loop(){    
  if (Serial.available() > 1) { // wait for at least two characters
    char c = Serial.read();
    if( c == TIME_HEADER) {
      processSyncMessage();
    }
    else if( c== FORMAT_HEADER) {
      processFormatMessage();
    }
  }
  if (timeStatus()!= timeNotSet) {
      if (millis() - lastDisplayTime > 1000) {
          digitalClockDisplay();  
          lastDisplayTime = millis();
      }
  }

  readModeButton();
  handleRotation();

  if (now() - lastActivity > 30) {
      mode = DISPLAY_MODE;

      int address = 0;
      EEPROM.put(address, hue);
      address += sizeof(hue);
      EEPROM.put(address, saturation);
      address += sizeof(saturation);
      EEPROM.put(address, value);
  }

  if (millis() - lastLedTime > 1000/FRAMES_PER_SECOND) {
      for (int i = 0; i < NUM_LEDS; i++) {
          if (led_map[i] == DOT) {
	      unsigned long time = millis();
	      int pos = (time / 5) % 512;
	      if (pos >= 256) {
	          pos = 511 - pos;
	      }
              switch (mode) {
                  case DISPLAY_MODE:
                      leds[i] = 0x000000;
                      break;
                  case SET_TIME:
                      leds[i] = (second() % 2) * 0xFFFFFF;
                      break;
                  case SET_HUE:
                      leds[i].setHSV((millis() / 8) % 256, 255, 255);
                      break;
                  case SET_SATURATION:
                      leds[i].setHSV(255, pos, 255);
                      break;
                  case SET_BRIGHTNESS:
                      leds[i].setHSV(255, 0, pos);
                      break;
              }
	  } else {
              if (current_leds[i]) {
                  uint8_t h = hue;
                  if (led_map[i] == HAPPY || led_map[i] == BIRTHDAY) {
                      h = h + 85;
                  } else if (led_map[i] == GO || led_map[i] == TO2 || led_map[i] == SLEEP) {
                      h = h + 170;
                  }
                  leds[i] = fadeTowardColor(leds[i], CHSV(h, saturation, value), FADE_RATE);
              } else {
                  leds[i].fadeToBlackBy(FADE_RATE);
              }
          }
      }
      FastLED.show();
      lastLedTime = millis();
  }
}




void readModeButton()
{
    int btn_reading = digitalRead(BUTTON_PIN);

    if (btn_reading != lastButtonState) {
        lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (btn_reading != buttonState) {
            buttonState = btn_reading;

            if (buttonState == HIGH) {
	        lastActivity = now();
                mode = (Mode)((mode + 1) % TOTAL_MODES);
                switch (mode) {
                    case DISPLAY_MODE:
                        Serial.println("Display mode");
                        break;
                    case SET_TIME:
                        Serial.println("Set time");
                        break;
                    case SET_HUE:
                        Serial.println("Set hue");
                        break;
                    case SET_SATURATION:
                        Serial.println("Set saturation");
                        break;
                    case SET_BRIGHTNESS:
                        Serial.println("Set brightness");
                        break;
                }
            }
        }
    }
    lastButtonState = btn_reading;
}


long oldPosition = 0;
long change = 0;

void handleRotation()
{
    long newPosition = myEnc.read();
    if (newPosition != oldPosition) {
        lastActivity = now();
        change += (newPosition - oldPosition);
        while (change >= 4 || change <= -4) {
            int step = change / abs(change);
            int old_day = day();
            switch (mode) {
                case DISPLAY_MODE:
                    // Ignore rotation in display mode
                    break;
                case SET_TIME:
                    lastDisplayTime = 0;
                    adjustTime(step * SECS_PER_MIN);
                    if (old_day < day()) {
                        adjustTime(-SECS_PER_DAY);
                    } else if (old_day > day()) {
                        adjustTime(SECS_PER_DAY);
                    }

		    RTC.set(now());
                    lastActivity = now();
                    // TODO: don't let time roll over into next day
                    break;
                case SET_HUE:
                    hue += step;
                    Serial.print("New hue: ");
                    Serial.println(hue);
                    break;
                case SET_SATURATION:
                    if (step > 0) {
                        saturation = qadd8(saturation, step);
                    } else {
                        saturation = qsub8(saturation, -1 * step);
                    }
                    Serial.print("New saturation: ");
                    Serial.println(saturation);
                    break;
                case SET_BRIGHTNESS:
                    if (step > 0) {
                        value = qadd8(value, step);
                    } else {
                        value = qsub8(value, -1 * step);
                    }
                    Serial.print("New value: ");
                    Serial.println(value);
                    break;
            }

            if (change >= 4) {
                change -= 4;
            } else {
                change += 4;
            }
        }
        oldPosition = newPosition;
    }
}








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
        if (hour_word == NOON) {
            hour_word = H_ONE;
        } else {
            hour_word += 1;
        }
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







void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  if(isLongFormat)
    Serial.print(dayStr(weekday()));
  else  
   Serial.print(dayShortStr(weekday()));
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  if(isLongFormat)
     Serial.print(monthStr(month()));
  else
     Serial.print(monthShortStr(month()));
  Serial.print(" ");
  Serial.print(year()); 
  Serial.print("  -  "); 



  bool words[TOTAL_WORDS] = { false };
  time_to_words(words, hour(), minute());

  if (month() == 8 && day() == 26 || ((month() < 10 || month() == 10 && day() <= 9) && year() == 2017)) {
    words[HAPPY] = words[BIRTHDAY] = true;
  }

  if (hour() < 7) {
    words[GO] = words[TO2] = words[SLEEP] = true;
  }

  if (words[IT]) { Serial.print("It "); }
  if (words[IS]) { Serial.print("is "); }
  if (words[TWENTY]) { Serial.print("twenty "); }
  if (words[FIVE]) { Serial.print("five "); }
  if (words[TEN]) { Serial.print("ten "); }
  if (words[QUARTER]) { Serial.print("quarter "); }
  if (words[HALF]) { Serial.print("half "); }
  if (words[PAST]) { Serial.print("past "); }
  if (words[TO]) { Serial.print("to "); }
  if (words[H_ONE]) { Serial.print("one "); }
  if (words[H_TWO]) { Serial.print("two "); }
  if (words[H_THREE]) { Serial.print("three "); }
  if (words[H_FOUR]) { Serial.print("four "); }
  if (words[H_FIVE]) { Serial.print("five "); }
  if (words[H_SIX]) { Serial.print("six "); }
  if (words[H_SEVEN]) { Serial.print("seven "); }
  if (words[H_EIGHT]) { Serial.print("eight "); }
  if (words[H_NINE]) { Serial.print("nine "); }
  if (words[H_TEN]) { Serial.print("ten "); }
  if (words[H_ELEVEN]) { Serial.print("eleven "); }
  if (words[NOON]) { Serial.print("noon "); }
  if (words[MIDNIGHT]) { Serial.print("midnight "); }
  if (words[OCLOCK]) { Serial.print("o'clock "); }
  if (words[AM]) { Serial.print("am "); }
  if (words[PM]) { Serial.print("pm "); }
  if (words[IN]) { Serial.print("in "); }
  if (words[THE]) { Serial.print("the "); }
  if (words[MORNING]) { Serial.print("morning "); }
  if (words[AFTERNOON]) { Serial.print("afternoon "); }
  if (words[EVENING]) { Serial.print("evening "); }
  if (words[HAPPY]) { Serial.print("happy "); }
  if (words[BIRTHDAY]) { Serial.print("birthday "); }
  if (words[GO]) { Serial.print("go "); }
  if (words[TO2]) { Serial.print("to "); }
  if (words[SLEEP]) { Serial.print("sleep"); }
  if (words[DOT]) { Serial.print("."); }
  Serial.println("");

  led_map_t my_map;
  my_map.map = led_map;
  my_map.size = sizeof(led_map) / sizeof(led_map[0]);

  words_to_leds(my_map, current_leds, words);
}

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void  processFormatMessage() {
   char c = Serial.read();
   if( c == FORMAT_LONG){
      isLongFormat = true;
      Serial.println(F("Setting long format"));
   }
   else if( c == FORMAT_SHORT) {
      isLongFormat = false;   
      Serial.println(F("Setting short format"));
   }
}

void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 - paul, perhaps we define in time.h?

   pctime = Serial.parseInt();
   if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
     RTC.set(pctime);
     setTime(pctime); // Sync Arduino clock to the time received on the serial port
   }
}

time_t requestSync() {
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}








// Helper function that blends one uint8_t toward another by a given amount
void nblendU8TowardU8( uint8_t& cur, const uint8_t target, uint8_t amount)
{
  if( cur == target) return;
  
  if( cur < target ) {
    uint8_t delta = target - cur;
    delta = scale8_video( delta, amount);
    cur += delta;
  } else {
    uint8_t delta = cur - target;
    delta = scale8_video( delta, amount);
    cur -= delta;
  }
}

// Blend one CRGB color toward another CRGB color by a given amount.
// Blending is linear, and done in the RGB color space.
// This function modifies 'cur' in place.
CRGB fadeTowardColor( CRGB& cur, const CRGB& target, uint8_t amount)
{
  nblendU8TowardU8( cur.red,   target.red,   amount);
  nblendU8TowardU8( cur.green, target.green, amount);
  nblendU8TowardU8( cur.blue,  target.blue,  amount);
  return cur;
}

// Fade an entire array of CRGBs toward a given background color by a given amount
// This function modifies the pixel array in place.
void fadeTowardColor( CRGB* L, uint16_t N, const CRGB& bgColor, uint8_t fadeAmount)
{
  for( uint16_t i = 0; i < N; i++) {
    fadeTowardColor( L[i], bgColor, fadeAmount);
  }
}
