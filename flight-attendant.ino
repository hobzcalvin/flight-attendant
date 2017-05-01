#include <FastLED.h>



// drugInteractions rows/columns are interpreted in this order,
// mapped to the numbered pins.
// DON'T CHANGE THE DRUG ORDER; change the pin number as needed.
byte drugPinOrder[] = {
  18,  // 0:  LSD
  19,  // 1:  Mushrooms
  1,   // 2:  DMT
  13,  // 3:  Mescaline
  5,   // 4:  DOx
  3,   // 5:  NBOMes
  0,   // 6:  2C-x
  2,   // 7:  2C-T-x
  23,  // 8:  5-MeO-xxT
  20,  // 9:  Cannabis
  10,  // 10: Ketamine
  9,   // 11: MXE
  15,  // 12: DXM
  24,  // 13: Nitrous
  14,  // 14: Amphetamines
  25,  // 15: MDMA
  8,   // 16: Cocaine
  11,  // 17: Caffeine
  22,  // 18: Alcohol
  7,   // 19: GHB/GBL
  21,  // 20: Opioids
  4,   // 21: Tramadol
  6,   // 22: Benzodiazepines
  12,   // 23: MAOIs
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
#define DATA_PIN    17
#define CHIPSET     WS2811
#define COLOR_ORDER RGB
#define LED_SETTINGS CHIPSET, DATA_PIN, COLOR_ORDER

#define FPS 100
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

#define DEBUG false

void setup() {
  if (DEBUG) {
    Serial.begin(9600);
    Serial.println("HELLO WORLD!");
  }

  pinMode(LED_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  FastLED.addLeds<LED_SETTINGS>(leds, NUM_LEDS).setCorrection(Typical8mmPixel);
  FastLED.setBrightness(255);

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

  FastLED.setTemperature(Tungsten40W);

  for (int i = 0; i < NUM_DRUGS; i++) {
    pinMode(drugPinOrder[i], INPUT_PULLUP);
  }
}

void loop() {
  boolean anyPressed = false;
  boolean drugsPressed[NUM_DRUGS];
  byte risk = SELF;
  boolean decrease = false;
  boolean synergy = false;
  
  if (DEBUG) Serial.print("p: ");
  for (int i = 0; i < NUM_DRUGS; i++) {
    drugsPressed[i] = !digitalRead(drugPinOrder[i]);
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
  
  FastLED.delay(1000/FPS);
}
