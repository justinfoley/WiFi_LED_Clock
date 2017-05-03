#pragma once

#include "NetworkManager.h"
#include "ColourGenerators.h"

#include <DS1307RTC.h>
 
#include <TimeLib.h>
#include <Timezone.h>

//#define FASTLED_INTERRUPT_RETRY_COUNT 0
//#define FASTLED_ALLOW_INTERRUPTS 0
//#include "FastLED.h"

//FASTLED_USING_NAMESPACE

//United Kingdom (London, Belfast)
TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};        //British Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};         //Standard Time
Timezone localTimeZone(BST, GMT);

#define LED_DATA_PIN    6
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
//#define NUM_LEDS    16
#define NUM_LEDS    60

//CRGB leds[NUM_LEDS];
CRGBArray<NUM_LEDS> leds;

DEFINE_GRADIENT_PALETTE( retro2_16_gp ) {
    0, 188,135,  1,
  255,  46,  7,  1
};

DEFINE_GRADIENT_PALETTE( es_pinksplash_08_gp ) {
    0, 126, 11,255,
  127, 197,  1, 22,
  175, 210,157,172,
  221, 157,  3,112,
  255, 157,  3,112
};

DEFINE_GRADIENT_PALETTE( Coral_reef_gp ) {
    0,  40,199,197,
   50,  10,152,155,
   96,   1,111,120,
   96,  43,127,162,
  139,  10, 73,111,
  255,   1, 34, 71
};

DEFINE_GRADIENT_PALETTE( heatmap_gp ) {
  0,     0,  0,  0,   //black
128,   255,  0,  0,   //red
224,   255,255,  0,   //bright yellow
255,   255,255,255 }; //full white

DEFINE_GRADIENT_PALETTE( blue_green ) {
  0,     0,   0, 255, // blue
 60,     0, 255, 0,    // green
 255,   255,255,255
};

CRGBPalette16 hourPalette = es_pinksplash_08_gp;
CRGBPalette16 minutePalette = retro2_16_gp;
CRGBPalette16 secondPalette = blue_green;

//BasicClockColours clockColours(secondPalette);

PalettePerHandClockColours clockColours(hourPalette, minutePalette, secondPalette);

#define BRIGHTNESS          30
#define FRAMES_PER_SECOND  120

#define NTP_RESYNC_FREQUENCY 300000
#define RTC_RESYNC_FREQUENCY 30

NetworkManager networkManager;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, 3000);

//  setupGradientPalette();

  networkManager.configureAndConnectToWifi();
  networkManager.setupNTP();

  networkManager.setupOTAUpdateServer();
  networkManager.setupWebServer();
  networkManager.setupMDNS();
  
  time_t rtcTime = RTC.get(); 
  if (rtcTime) {
//      unsigned long epochGMT = networkManager.getNTPTime();
//  
//      if(epochGMT != -1) {
//        RTC.set(epochGMT);
//        Serial.println("NTP is setting the system time");
//        printTime(epochGMT);
//      }
    
    Serial.print("RTC already set rtcTime - ");
    Serial.println(rtcTime);
  } else {
    if(RTC.chipPresent()) {
      Serial.println("Have RTC present but unset");
      unsigned long epochGMT = networkManager.getNTPTime();
  
      if(epochGMT != -1) {
        RTC.set(epochGMT);
        Serial.println("NTP is setting the system time");
        printTime(epochGMT);
      }
    }
    else {
      Serial.println("No RTC.");
    }
  }

  if(timeStatus()!= timeSet) 
    Serial.println("TimeLib not synced yet");
  else
    Serial.println("TimeLib synced");  

  setSyncInterval(RTC_RESYNC_FREQUENCY);
  setSyncProvider(timeSync);   // the function to get the time from the RTC
}

time_t timeSync() // called periodically by Time library to syncronise itself
{   
  time_t rtcTime = RTC.get();
  Serial.print("rtcTime - ");
  Serial.println(rtcTime);
  Serial.println("RTC is setting the system time"); 
  return rtcTime;
}

void loop()
{
  networkManager.handleServerClients();

  EVERY_N_MILLISECONDS( NTP_RESYNC_FREQUENCY ) {
    unsigned long epochGMT = networkManager.getNTPTime();

    if(epochGMT != -1) {
      RTC.set(epochGMT);
      Serial.println("NTP is setting the system time");
      printTime(epochGMT);
    }
    
  }

//  time_t rtcTime = RTC.get();
//
//  Serial.print("RTC Epoch time = ");
//  Serial.println(rtcTime);
//  
//  printTime(rtcTime);
//  displayTimeOnLedRing(rtcTime);

  EVERY_N_MILLISECONDS( 500 ) {
    time_t utc = now();
    time_t ukTime = localTimeZone.toLocal(utc);
  
    // Generate a pattern
  //  rainbowWithGlitter();

//    printTime(ukTime);
    displayTimeOnLedRing(ukTime);
  //  addGlitter(10);
    
    // send the 'leds' array out to the actual LED strip
    FastLED.show();
    // insert a delay to keep the framerate modest
//    FastLED.delay(1000/FRAMES_PER_SECOND); 
  }
  
//  showNTPTime();

//  displayTimeOnLedRing(epoch);
//  addGlitter(10);
  
  // send the 'leds' array out to the actual LED strip
//  FastLED.show();
  // insert a delay to keep the framerate modest
//  FastLED.delay(1000/FRAMES_PER_SECOND); 
  
  // wait ten seconds before asking for the time again
//  delay(10000);
}


void printTime(time_t epoch) {
  if(epoch != -1) {
    // print Unix time:
    Serial.print("     Unix time = ");
    Serial.println(epoch);
  
    time_t ukTime = localTimeZone.toLocal(epoch);
    epoch = ukTime;  
  
    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if (((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second
  }
}

//void setupGradientPalette()
//{
////    // 'black out' all 16 palette entries...
////    fill_solid( currentPalette, 16, CRGB::Black);
////    currentPalette[0] = CRGB::DarkBlue;
////    currentPalette[1] = CRGB::Green;
////    currentPalette[2] = CRGB::DarkBlue;
//    currentPalette[0] = CRGB::DarkBlue;
//    currentPalette[255] = CRGB::Green;
//}

void displayTimeOnLedRing(time_t timeNow) 
{
  int second_led = map(second(timeNow), 0, 59, 0, NUM_LEDS - 1);
  int minute_led = map(minute(timeNow), 0, 59, 0, NUM_LEDS - 1);
  int hour_led = (mod8(hour(timeNow), 12) * 5) + (minute_led / 15);
  
//  map(mod8(hour(timeNow), 12), 0, 11, 0, NUM_LEDS - 1);


  // Do a better mapping to the partial number moving further up hour as minute moves
  // hour = 60 / 12 = five dots per hour
  // minute = 60 / 60 = 1 dot per min. Seconds same.

  // 0 to 11 * 5  40 45 50 55
  // minute / 12 = add on

  Serial.print(" ");
  Serial.print(hour_led);
  Serial.print(" ");
  Serial.print(minute_led);
  Serial.print(" ");
  Serial.println(second_led);

  clockColours.setHandPositions(hour_led, minute_led, second_led);
  
  for(int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = clockColours.getLedColour(i);
  }
  
  clockColours.incrementIndex();
}

/*
void displayTimeOnLedRing(time_t timeNow) 
{
  fill_solid(leds, 16, CRGB::Black);
//  int second_led = 16 * (second() / 60);
//
//  int map_twelth_to_leds_scale = 255 * (float(12) / NUM_LEDS);
//  int map_sixtieth_to_leds_scale = 255 * (float(NUM_LEDS) / 60);

  int hour_led = map(mod8(hour(timeNow), 12), 0, 11, 0, 15);
  int minute_led = map(minute(timeNow), 0, 59, 0, 15);
  int second_led = map(second(timeNow), 0, 59, 0, 15);

//
//  int hour_led = scale8(hour_int, map_twelth_to_leds_scale);
//  int minute_led = scale8(minute(), map_sixtieth_to_leds_scale);
//  int second_led = scale8(second(), map_sixtieth_to_leds_scale);

//  Serial.print(map_twelth_to_leds_scale);
//  Serial.print(" ");
//  Serial.print(map_sixtieth_to_leds_scale);
//  Serial.print(" ");
//  Serial.print(hour_led);
//  Serial.print(" ");
//  Serial.print(minute_led);
//  Serial.print(" ");
//  Serial.println(second_led);

  int rotation_factor = 4;
  hour_led = addmod8(hour_led, rotation_factor, NUM_LEDS);
  minute_led = addmod8(minute_led, rotation_factor, NUM_LEDS);
//  second_led = addmod8(second_led, rotation_factor, NUM_LEDS);

  Serial.print(" ");
  Serial.print(hour_led);
  Serial.print(" ");
  Serial.print(minute_led);
  Serial.print(" ");
  Serial.println(second_led);

  leds[hour_led] += CRGB::Purple;
  leds[minute_led] += CRGB::Red;

//  for(int i = rotation_factor; i <= second_led; ++i) {
//    leds[i] += CRGB::Green;
//  } // 4 -> 15?

  for(int i = 0; i < NUM_LEDS - rotation_factor; ++i) {
    if(i <= second_led) {
      int rotated = i + rotation_factor;
      if(rotated != minute_led && rotated != hour_led) {
        leds[rotated] += ColorFromPalette(currentPalette, i, 255, LINEARBLEND);
      }
    }
  }

  for(int i = NUM_LEDS - rotation_factor; i <= second_led; ++i) {
    int rotated = i + rotation_factor;
    if(rotated != minute_led && rotated != hour_led) {
      leds[rotated - NUM_LEDS] += ColorFromPalette(currentPalette, i, 255, LINEARBLEND);
    }
  }
  

//  for(int i = rotation_factor; i <= NUM_LEDS; ++i) {
//    if(i =< second_led) {
//      leds[i] += CRGB::Green;
//    }
//  }
  
  
//  for(int i = 0; i <= rotation_factor && second_led <= ; ++i) {
//    leds[i] += CRGB::Green;
//  }

//  for(int i = 0; i <= second_led; ++i) {
////  for(CRGB & pixel : leds) {
////    if(i > rotation_factor && i <= second_led || rotation_factor > second_led) {
//      Serial.println((i + rotation_factor) % NUM_LEDS);
//      leds[(i + rotation_factor) % NUM_LEDS] += CRGB::Green;
////      ColorFromPalette(currentPalette, 2, BRIGHTNESS, LINEARBLEND);
////      
////    }
//  } // 4 -> 15?  3
  
  // FastLED's built-in rainbow generator
//  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}
*/


/*
 * Colour modes pure colours - blue seconds, red minute, green hour
 * 
 * 
 * 
*/

