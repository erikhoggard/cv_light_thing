#include <Arduino.h>
#include <FastLED.h>
#include <cppQueue.h>

#define DATA_PIN 12
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS 265
CRGB leds[NUM_LEDS];

#define BRIGHTNESS 50
#define FRAMES_PER_SECOND 240
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0;                  // rotating "base color" used by many of the patterns
uint8_t colorValue = 0;
uint8_t prevColorValue = 0;

uint8_t in = 0;
uint8_t out;
uint8_t peek;
cppQueue qu(sizeof(in), 5, FIFO, true);

uint8_t colorOffset = 40;
uint8_t colorScaleRange = 2; // max is 3.64

// void nextPattern();
// void rainbowWithGlitter();
// void bpm();
// void rainbow();
// void confetti();
// void sinelon();
// void juggle();
void fillSolid();

// SMOOOOOOOOOOOOOOOOOOOOOOOOTHING STUFF

const int numReadings = 5;

int readings[numReadings]; // the readings from the analog input
int readIndex = 0;         // the index of the current reading
int total = 0;             // the running total
int average = 0;           // the average

int inputPin = A0;

// List of patterns to cycle through.  Each is defined as a separate function below.
// typedef void (*SimplePatternList[])();
// SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm};

void fillSolid(int color, int brightness)
{
  fill_solid(leds, NUM_LEDS, CHSV(color, 255, brightness));
}

// void nextPattern()
// {
//   // add one to the current pattern number, and wrap around at the end
//   gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
// }

// void rainbow()
// {
//   // FastLED's built-in rainbow generator
//   fill_rainbow(leds, NUM_LEDS, gHue, 7);
// }

// void addGlitter(fract8 chanceOfGlitter)
// {
//   if (random8() < chanceOfGlitter)
//   {
//     leds[random16(NUM_LEDS)] += CRGB::White;
//   }
// }

// void confetti()
// {
//   // random colored speckles that blink in and fade smoothly
//   fadeToBlackBy(leds, NUM_LEDS, 10);
//   int pos = random16(NUM_LEDS);
//   leds[pos] += CHSV(gHue + random8(64), 200, 255);
// }

// void sinelon()
// {
//   // a colored dot sweeping back and forth, with fading trails
//   fadeToBlackBy(leds, NUM_LEDS, 20);
//   int pos = beatsin16(13, 0, NUM_LEDS - 1);
//   leds[pos] += CHSV(gHue, 255, 192);
// }

// void bpm()
// {
//   // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
//   uint8_t BeatsPerMinute = 62;
//   CRGBPalette16 palette = PartyColors_p;
//   uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
//   for (int i = 0; i < NUM_LEDS; i++)
//   { // 9948
//     leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
//   }
// }

// void juggle()
// {
//   // eight colored dots, weaving in and out of sync with each other
//   fadeToBlackBy(leds, NUM_LEDS, 20);
//   byte dothue = 0;
//   for (int i = 0; i < 8; i++)
//   {
//     leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
//     dothue += 32;
//   }
// }
// void rainbowWithGlitter()
// {
//   // built-in FastLED rainbow, plus some random sparkly glitter
//   rainbow();
//   addGlitter(80);
// }

void setup()
{

  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  qu.push(&in);

  for (int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    readings[thisReading] = 0;
  }

  Serial.begin(4800);
}

void loop()
{

  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  int sensorValue = analogRead(A0);
  sensorValue = analogRead(A0);
  readings[readIndex] = sensorValue;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings)
  {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits
  Serial.println(average);
  // delay(1); // delay in between reads for stability

  int octaveRange = 70;
  // int sensorValue = analogRead(A0);
  // sensorValue = analogRead(A0);
  // colorValue = sensorValue % octaveRange;
  colorValue = average % octaveRange;
  if (colorValue < 5 || colorValue > 65)
  {
    colorValue = 70;
  }

  int maxVolume = 1023;
  int envelopeValue = analogRead(A1);
  envelopeValue = analogRead(A1);
  if (envelopeValue < 40)
  {
    envelopeValue = 0;
  }

  // int quTotal = 0;
  // int quTotalCounted = 0;
  // int quAvg;
  // int quMin = octaveRange, quMax = 0;

  // if (qu.getRemainingCount() == 0)
  // {
  //   qu.pop(&out);
  // }

  // for (int i = 0; i < qu.getCount(); i++)
  // {
  //   qu.peekIdx(&peek, i);
  //   quMin = peek < quMin ? peek : quMin;
  //   quMax = peek > quMax ? peek : quMax;
  // }

  // int quRange = quMax - quMin;
  // for (int i = 0; i < qu.getCount(); i++)
  // {
  //   qu.peekIdx(&peek, i);

  //   if ((quRange < octaveRange / 3) || (peek - (octaveRange / 3) > 0))
  //   {
  //     quTotal += peek;
  //     quTotalCounted++;
  //   }
  // }

  // qu.push(&colorValue);

  // quAvg = quTotal / quTotalCounted;

  // colorValue = quAvg;

  // out is now the oldest color. use to determine what newest should be.

  fillSolid((colorValue + colorOffset) * colorScaleRange, envelopeValue / 4);

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // EVERY_N_SECONDS(1) { Serial.println(colorValue); }
  // do some periodic updates
  // EVERY_N_MILLISECONDS(20) { gHue++; } // slowly cycle the "base color" through the rainbow
  // EVERY_N_SECONDS(10) { nextPattern(); } // change patterns periodically
}
