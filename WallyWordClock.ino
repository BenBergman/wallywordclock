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

int mode = 0;





#include "FastLED.h"

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define LED_DATA_PIN    13
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    64
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

uint8_t hue = 100;
uint8_t saturation = 255;
uint8_t value = 255;





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
};







void setup()  {
  Serial.begin(9600);
  while (!Serial) ; // Needed for Leonardo only
  setSyncProvider( requestSync);  //set function to call when sync required

  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH);

  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // TODO: should this correction be changed when we have the final strip?
  FastLED.setBrightness(BRIGHTNESS);

  Serial.println("T1479235073");
  Serial.println("Waiting for sync message");
}

unsigned long lastDisplayTime = 0;
unsigned long lastLedTime = 0;

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
      if (millis() - lastDisplayTime > 10000) {
          digitalClockDisplay();  
          lastDisplayTime = millis();
      }
  }

  readModeButton();
  handleRotation();

  leds[0] = CHSV(hue, saturation, value);
  if (millis() - lastLedTime > 1000/FRAMES_PER_SECOND) {
      FastLED.show();
      lastLedTime = millis();
  }
}





enum {
    DISPLAY_MODE,
    SET_TIME,
    SET_HUE,
    SET_SATURATION,
    SET_BRIGHTNESS,
    TOTAL_MODES,
};

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
                mode = (mode + 1) % TOTAL_MODES;
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
                    // TODO: store time in RTC
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
  Serial.println(".");
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
     setTime(pctime); // Sync Arduino clock to the time received on the serial port
   }
}

time_t requestSync() {
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}
