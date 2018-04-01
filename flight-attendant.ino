#include <FastLED.h>
#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>



// drugInteractions rows/columns are interpreted in this order,
// mapped to the numbered pins.
// DON'T CHANGE THE DRUG ORDER; change the pin number as needed.
byte drugPinOrder[] = {
  18,  // 0:  LSD
  19,  // 1:  Mushrooms
  1,   // 2:  DMT
  26,  // 3:  Mescaline
  5,   // 4:  DOx
  3,   // 5:  NBOMes
  0,   // 6:  2C-x
  2,   // 7:  2C-T-x
  23,  // 8:  5-MeO-xxT
  20,  // 9:  Cannabis
  10,  // 10: Ketamine
  9,   // 11: MXE
  15,  // 12: DXM
  25,  // 13: Nitrous
  14,  // 14: Amphetamines
  24,  // 15: MDMA
  8,   // 16: Cocaine
  11,  // 17: Caffeine
  22,  // 18: Alcohol
  7,   // 19: GHB/GBL
  21,  // 20: Opioids
  4,   // 21: Tramadol
  6,   // 22: Benzodiazepines
  12,  // 23: MAOIs
  16   // 24: SSRIs
};

// Interaction codes
enum Interaction {
  SELF         = 0, // Represents the spot where a drug intersects itself on the chart
  SYNERGY,  // = 1, Low Risk & Synergy
  LOW_RISK, // = 2, Low Risk & No Synergy
  DECREASE, // = 3, Low Risk & Decrease
  CAUTION,  // = 4,
  UNSAFE,   // = 5,
  DANGER,   // = 6, Dangerous
  NUM_INTERACTIONS
};

#define NUM_DRUGS (sizeof(drugPinOrder) / sizeof(byte))
byte drugInteractions[NUM_DRUGS][NUM_DRUGS] {
/*
   Drug order here must be consistent with drugPinOrder above;
   this is a representation of the TripSit drug interaction chart.
   NOTE: Actually only one diagonal half of this is needed!
 
                 m     m              5
                 u     e              -  c  k                    c           t
                 s     s              M  a  e        n        c  a  a  G  o  r
                 h     c     N     2  e  n  t        i        o  f  l  H  p  a  b
                 r     a     B     C  O  n  a        t        c  f  c  B  i  m  e  M  S
                 o     l     O  2  -  -  a  m        r  m  M  a  e  o  /  o  a  n  A  S
              L  o  D  i  D  M  C  T  x  b  i  M  D  o  e  D  i  i  h  G  i  d  z  O  R
              S  m  M  n  O  e  -  -  x  i  n  X  X  u  t  M  n  n  o  B  d  o  o  I  I
              D  s  T  e  x  s  x  x  T  s  e  E  M  s  h  A  e  e  l  L  s  l  s  s  s
      LSD */{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 4, 1, 4, 2, 3, 3, 2, 5, 3, 3, 3 },/* LSD
mushrooms */{ 1, 0, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 4, 1, 4, 2, 3, 3, 2, 5, 3, 1, 3 },/* mushrooms
      DMT */{ 1, 1, 0, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 4, 1, 4, 2, 3, 3, 2, 5, 3, 1, 3 },/* DMT
mescaline */{ 1, 1, 1, 0, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 4, 1, 4, 2, 3, 3, 2, 5, 3, 4, 3 },/* mescaline
      DOx */{ 1, 1, 1, 4, 0, 4, 4, 4, 4, 4, 1, 4, 5, 1, 5, 4, 5, 4, 3, 3, 2, 5, 3, 4, 3 },/* DOx
   NBOMes */{ 1, 1, 1, 4, 4, 0, 4, 4, 4, 4, 1, 4, 5, 1, 5, 4, 5, 4, 3, 3, 2, 5, 3, 4, 3 },/* NBOMes
     2C-x */{ 1, 1, 1, 4, 4, 4, 0, 4, 4, 4, 1, 1, 1, 1, 4, 1, 4, 2, 3, 3, 2, 5, 3, 4, 3 },/* 2C-x
   2C-T-x */{ 1, 1, 1, 4, 4, 4, 4, 0, 4, 4, 1, 4, 5, 1, 5, 4, 5, 2, 3, 3, 2, 5, 3, 4, 3 },/* 2C-T-x
5-MeO-xxT */{ 1, 1, 1, 4, 4, 4, 4, 4, 0, 4, 1, 1, 5, 1, 5, 4, 5, 2, 3, 3, 2, 5, 3, 6, 3 },/* 5-MeO-xxT
 cannabis */{ 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 1, 1, 1, 1, 4, 1, 4, 2, 1, 1, 1, 1, 3, 1, 2 },/* cannabis
 ketamine */{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 2, 1, 4, 1, 4, 2, 6, 6, 6, 6, 4, 4, 2 },/* ketamine
      MXE */{ 1, 1, 1, 1, 4, 4, 1, 4, 1, 1, 1, 0, 2, 1, 4, 4, 4, 2, 6, 6, 6, 6, 4, 5, 4 },/* MXE
      DXM */{ 1, 1, 1, 1, 5, 5, 1, 5, 5, 1, 2, 2, 0, 1, 5, 6, 5, 2, 6, 6, 6, 6, 4, 6, 6 },/* DXM
  nitrous */{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 2, 4, 4, 4, 4, 3, 2, 2 },/* nitrous
     meth */{ 4, 4, 4, 4, 5, 5, 4, 5, 5, 4, 4, 4, 5, 1, 0, 1, 4, 4, 4, 4, 4, 6, 3, 6, 2 },/* meth
     MDMA */{ 1, 1, 1, 1, 4, 4, 1, 4, 4, 1, 1, 4, 6, 1, 1, 0, 4, 4, 4, 4, 2, 6, 3, 6, 3 },/* MDMA
  cocaine */{ 4, 4, 4, 4, 5, 5, 4, 5, 5, 4, 4, 4, 5, 1, 4, 4, 0, 4, 5, 4, 6, 6, 3, 6, 2 },/* cocaine
 caffeine */{ 2, 2, 2, 2, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 0, 2, 2, 2, 2, 3, 2, 2 },/* caffeine
  alcohol */{ 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 6, 6, 6, 4, 4, 4, 5, 2, 0, 6, 6, 6, 6, 5, 4 },/* alcohol
  GHB/GBL */{ 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 6, 6, 6, 4, 4, 4, 4, 2, 6, 0, 6, 6, 6, 1, 2 },/* GHB/GBL
  opioids */{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 6, 6, 6, 4, 4, 2, 6, 2, 6, 6, 0, 6, 6, 4, 2 },/* opioids
 tramadol */{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 6, 6, 6, 4, 6, 6, 6, 2, 6, 6, 6, 0, 6, 6, 6 },/* tramadol
   benzos */{ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 3, 3, 3, 3, 3, 6, 6, 6, 6, 0, 1, 2 },/* benzos
    MAOIs */{ 3, 1, 1, 4, 4, 4, 4, 4, 6, 1, 4, 5, 6, 2, 6, 6, 6, 2, 5, 1, 4, 6, 1, 0, 6 },/* MAOIs
    SSRIs */{ 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 4, 6, 2, 2, 3, 2, 2, 4, 2, 2, 6, 2, 6, 0 } /* SSRIs */
}; // UP=1, OK=2, DN=3, !=4, <3=5, X=6

#define LED_PIN     13
#define DATA_PIN    38
#define CHIPSET     WS2811
#define COLOR_ORDER RGB
#define LED_SETTINGS CHIPSET, DATA_PIN, COLOR_ORDER

#define NUM_LEDS 6
CRGB leds[NUM_LEDS];

CRGB indicators[7] = {
  0x000F00,
  0x2F00FF,
  0x00FF00,
  0x0000FF,
  0xFF7F00,
  0xFF2F00,
  0xFF0000
};

// SD Card Stuff
const int chipSelect = BUILTIN_SDCARD;
// Directory will be created on SD card if necessary
#define DATA_DIR "flightat"
// Bump this version if we ever start adding new columns, etc.
#define CSV_VERSION 1
String filename;
// millis() of last log, used to determine time between logs (button changes)
long lastLogMillis;

#define DEBUG false

void setup() {
  setSyncProvider(getTeensy3Time);
  
  Serial.begin(9600);
  Serial.print("Hello. The time is ");
  printClock(&Serial);
  Serial.println();

  if (SD.begin(chipSelect)) {
    filename = DATA_DIR;
    filename += '/';
    filename += year();
    filename += zeroPad(month(), false);
    filename += zeroPad(day(), false);
    filename += ".csv";
    Serial.print("SD card initialized: ");
    Serial.println(filename);
    
    if (!SD.mkdir(DATA_DIR)) {
      Serial.print("Failed to mkdir ");
      Serial.println(DATA_DIR);
    }

    if (!SD.exists(filename.c_str())) {
      // File doesn't exist yet; before writing normal data,
      // write a CSV header describing the columns.
      writeDataHeader();
    }
    
    // Set zero so we know we've never had a log before
    lastLogMillis = 0;
  } else {
    Serial.println("SD card initialize failed / not present");
  }

  randomSeed(analogRead(drugPinOrder[0]));

  pinMode(LED_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  FastLED.addLeds<LED_SETTINGS>(leds, NUM_LEDS).setCorrection(Typical8mmPixel);
  FastLED.setBrightness(42);

  fill_solid(leds, NUM_LEDS, 0xFF0000);
  digitalWrite(LED_PIN, HIGH);
  FastLED.show();
  delay(500);
  fill_solid(leds, NUM_LEDS, 0x00FF00);
  digitalWrite(LED_PIN, LOW);
  FastLED.show();
  delay(500);
  fill_solid(leds, NUM_LEDS, 0x0000FF);
  digitalWrite(LED_PIN, HIGH);
  FastLED.show();
  delay(500);
  fill_solid(leds, NUM_LEDS, 0);
  digitalWrite(LED_PIN, LOW);
  FastLED.show();
  delay(500);

  for (int i = 0; i < NUM_DRUGS; i++) {
    pinMode(drugPinOrder[i], INPUT_PULLUP);
  }
}

// Start with all drugs pressed so the first real reading is different.
// This ensures that the first real reading is always logged.
boolean drugsPressed[NUM_DRUGS] = { true };

void loop() {
  boolean anyPressed = false;
  boolean pressChanged = false;
  byte risk = SELF;
  boolean decrease = false;
  boolean synergy = false;
  
  if (DEBUG) Serial.print("p: ");
  for (int i = 0; i < NUM_DRUGS; i++) {
    boolean pressed = !digitalRead(drugPinOrder[i]);
    if (drugsPressed[i] != pressed) {
      // This button's state changed.
      // Update drugsPressed and note pressChanged=true
      // so we log the state of the buttons.
      drugsPressed[i] = pressed;
      pressChanged = true;
    }
    if (drugsPressed[i]) {
      anyPressed = true;
      if (DEBUG) {
        Serial.print(i);
        Serial.print(' ');        
      }
      for (int j = 0; j < i; j++) {
        if (drugsPressed[j]) {
          Interaction action = (Interaction)drugInteractions[i][j];
          if (action == SYNERGY) {
            synergy = true;
          } else if (action == DECREASE) {
            decrease = true;
          }
          if (action > risk) {
            risk = action;
          }
        }
      }
    }
  }
  if (DEBUG) {
    if (anyPressed) {
      Serial.print("; ");
      Serial.print(risk);
      Serial.print("-");
      Serial.print(synergy);
      Serial.print("-");
      Serial.println(decrease);
    } else {
      Serial.println('.');
    }
  }

  if (anyPressed) {
    fill_solid(leds, NUM_LEDS, 0);
    if (risk >= CAUTION) {
      leds[NUM_LEDS - risk] = indicators[risk];
    } else {
      leds[NUM_LEDS - LOW_RISK] = risk == SELF ? indicators[SELF] : indicators[LOW_RISK];
      if (synergy) {
        leds[NUM_LEDS - SYNERGY] = indicators[SYNERGY];
      }
      if (decrease) {
        leds[NUM_LEDS - DECREASE] = indicators[DECREASE];
      }
    }
  } else {
    fill_rainbow(leds, NUM_LEDS, (uint8_t)(millis() >> 3), 40);
  }
  digitalWrite(LED_PIN, anyPressed);

  // SD card activity slows things down, so don't delay() here.
  FastLED.show();

  if (pressChanged) {
    // The button state changed since last loop(). Log the current state.
    File datafile = SD.open(filename.c_str(), FILE_WRITE);
    if (datafile) {
      datafile.print(CSV_VERSION);
      datafile.print(',');
      printClock(&datafile);
      datafile.print(',');
      datafile.print(lastLogMillis ? millis() - lastLogMillis : 0);
      lastLogMillis = millis();
      for (int i = 0; i < NUM_DRUGS; i++) {
        datafile.print(',');
        datafile.print(drugsPressed[i]);
      }
      datafile.println();
      datafile.close();
    } else if (DEBUG) {
      Serial.print("Failed to open file: ");
      Serial.println(filename);
    }
  }
}

void writeDataHeader() {
  File datafile = SD.open(filename.c_str(), FILE_WRITE);
  if (datafile) {
    datafile.println(F(
      "CSV Version,Date,Time,ms since last,"
      "LSD,Mushrooms,DMT,Mescaline,DOx,"
      "NBOMes,2C-x,2C-T-x,5-MeO-xxT,Cannabis,"
      "Ketamine,MXE,DXM,Nitrous,Amphetamines,"
      "MDMA,Cocaine,Caffeine,Alcohol,GHB/GBL,"
      "Opioids,Tramadol,Benzodiazepines,MAOIs,SSRIs"));
    datafile.close();
    Serial.println("Wrote data header");
  } else {
    Serial.println("Failed to write data header");
  }
}






// TIME!!!


time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

void printClock(Stream* stream) {
  // Print the current time to the given stream in the format:
  // YYYY-MM-DD,hh:mm:ss
  // Comma between date and time makes for easier CSV processing.
  stream->print(year());
  stream->print('-');
  stream->print(zeroPad(month(), false));
  stream->print('-');
  stream->print(zeroPad(day(), false));
  stream->print(',');
  stream->print(zeroPad(hour(), false));
  stream->print(':');
  stream->print(zeroPad(minute(), false));
  stream->print(':');
  stream->print(zeroPad(second(), false));
}

String zeroPad(int value, boolean thousand) {
  // Return a string representing the given value,
  // padding with zeroes as needed.
  // 4-character return if thousand is true, 2-character otherwise.
  String result = "";
  if (thousand && value < 1000) {
    result += '0';
  }
  if (thousand && value < 100) {
    result += '0';
  }
  if (value < 10) {
    result += '0';
  }
  result += value;
  return result;
}

