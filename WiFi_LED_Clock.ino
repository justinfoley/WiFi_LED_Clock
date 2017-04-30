#include "NetworkManager.h"

#include <DS1307RTC.h>
 
#include <TimeLib.h>
#include <Timezone.h>

#include "FastLED.h"

FASTLED_USING_NAMESPACE

//United Kingdom (London, Belfast)
TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};        //British Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};         //Standard Time
Timezone localTimeZone(BST, GMT);

#define LED_DATA_PIN    6
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    16

//CRGB leds[NUM_LEDS];
CRGBArray<NUM_LEDS> leds;

CRGBPalette16 currentPalette;

#define BRIGHTNESS          50
#define FRAMES_PER_SECOND  120

NetworkManager networkManager;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, 3000);

  setupGradientPalette();
  
  networkManager.configureAndConnectToWifi();
  networkManager.setupNTP();

  networkManager.setupOTAUpdateServer();
  networkManager.setupWebServer();
  networkManager.setupMDNS();
}

void loop()
{
  networkManager.handleServerClients();
  
  showNTPTime();

//  displayTimeOnLedRing(epoch);
//  addGlitter(10);
  
  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
//  FastLED.delay(1000/FRAMES_PER_SECOND); 
  
  // wait ten seconds before asking for the time again
//  delay(10000);
}

void showNTPTime() {
  unsigned long epoch = networkManager.getNTPTime();

  Serial.print("  Raw NTP time = "); 
  Serial.println(epoch);

  RTC.set(epoch);

  time_t rtcTime = RTC.get();

  Serial.print("RTC Epoch time = ");
  Serial.println(rtcTime);
  
  printTime(rtcTime);
  displayTimeOnLedRing(rtcTime);
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

void setupGradientPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    currentPalette[0] = CRGB::DarkBlue;
    currentPalette[1] = CRGB::Green;
}

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
