#pragma once

//#include "vector.h"

#include "ClockState.h"
#include "NetworkManager.h"

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
//#define LED_TYPE    WS2811
//#define LED_TYPE  WS2812B
#define LED_TYPE  NEOPIXEL
#define COLOR_ORDER GRB
#define NUM_LEDS    60

#define LIGHT_SENSOR_PIN  D0

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

StaticHandClockColours clockColours1(CRGB::Green, CRGB::Red, CRGB::Black);
StaticHandClockColours clockColours2(CRGB::Green, CRGB::Red, CRGB::Blue);
PaletteTickClockColours clockColours3(secondPalette, CRGB::Green, CRGB::Red);
PalettePerHandClockColours clockColours4(hourPalette, minutePalette, secondPalette);

const int clocksColourListLength = 4;
ClockColours* clocksColourList[clocksColourListLength] = {
  &clockColours1,
  &clockColours2,
  &clockColours3,
  &clockColours4
};
//
//etl::vector<int, 10> v1;
//v1.push_back(1);
//v1.push_back(2);

//etl::vector<ClockColours*, 4> clocksColourList;
//clocksColourList.push_back(&clockColours1);
//clocksColourList.push_back(&clockColours2);
//clocksColourList.push_back(&clockColours3);
//clocksColourList.push_back(&clockColours4);

ClockState2 clockState(-1, 0, clocksColourList, clocksColourListLength);


#define BRIGHTNESS          30
#define FRAMES_PER_SECOND  120

#define NTP_RESYNC_FREQUENCY 300
#define RTC_RESYNC_FREQUENCY 30

NetworkManager networkManager;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  printInfo();

  pinMode(LIGHT_SENSOR_PIN, INPUT); 

  clockState.printState();

  FastLED.addLeds<LED_TYPE, LED_DATA_PIN>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, 2200);
  FastLED.setMaxRefreshRate(400);

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
  EVERY_N_SECONDS( NTP_RESYNC_FREQUENCY ) {
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

  EVERY_N_MILLISECONDS( 100 ) {
    bool isNightMode = checkNightMode();
    if(isNightMode) {
      Serial.println("Night Mode");
    } else {
      Serial.println("Not night Mode");
    }
    
    time_t utc = now();
    time_t ukTime = localTimeZone.toLocal(utc);
  
    // Generate a pattern
  //  rainbowWithGlitter();

//    printTime(ukTime);
    ClockColours *currentClockColours = clocksColourList[2];
    displayTimeOnLedRing(ukTime, currentClockColours);
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

  networkManager.handleServerClients();
}


bool checkNightMode() {
  Serial.println(digitalRead(LIGHT_SENSOR_PIN));
  if (digitalRead(LIGHT_SENSOR_PIN))
  {
    Serial.println("NIGHT TIME!!");
  } else {
    Serial.println("DAY TIME!!");
  }
  
  return digitalRead(LIGHT_SENSOR_PIN);
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

void displayTimeOnLedRing(time_t timeNow, ClockColours *clockColours) 
{
  int second_led = map(second(timeNow), 0, 59, 0, NUM_LEDS - 1);
//  int minute_led = map(minute(timeNow), 0, 59, 0, NUM_LEDS - 1);
  int minute_led = minute(timeNow);
  int hour_led = (mod8(hour(timeNow), 12) * 5) + (minute_led / 15);
  
//  map(mod8(hour(timeNow), 12), 0, 11, 0, NUM_LEDS - 1);


  // Do a better mapping to the partial number moving further up hour as minute moves
  // hour = 60 / 12 = five dots per hour
  // minute = 60 / 60 = 1 dot per min. Seconds same.

  // 0 to 11 * 5  40 45 50 55
  // minute / 12 = add on

//  Serial.print(" ");
//  Serial.print(hour_led);
//  Serial.print(" ");
//  Serial.print(minute_led);
//  Serial.print(" ");
//  Serial.println(second_led);



  clockColours->setHandPositions(hour_led, minute_led, second_led);
  
  for(int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = clockColours->getLedColour(i);
  }
  
  clockColours->incrementIndex();
}

void printInfo() {
  Serial.println();
  Serial.print( F("Heap: ") ); Serial.println(system_get_free_heap_size());
  Serial.print( F("Boot Vers: ") ); Serial.println(system_get_boot_version());
  Serial.print( F("CPU: ") ); Serial.println(system_get_cpu_freq());
  Serial.print( F("SDK: ") ); Serial.println(system_get_sdk_version());
  Serial.print( F("Chip ID: ") ); Serial.println(system_get_chip_id());
  Serial.print( F("Flash ID: ") ); Serial.println(spi_flash_get_id());
  Serial.print( F("Flash Size: ") ); Serial.println(ESP.getFlashChipRealSize());
  Serial.print( F("Vcc: ") ); Serial.println(ESP.getVcc());
  Serial.println();
}


/*
 * Colour modes pure colours - blue seconds, red minute, green hour
 * 
 * 
 * 
*/

