#pragma once

#include "NetworkManager.h"
#include "Sound.h"

#include <DS1307RTC.h>
 
#include <TimeLib.h>
#include <Timezone.h>

//United Kingdom (London, Belfast)
TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};        //British Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};         //Standard Time
Timezone localTimeZone(BST, GMT);

#define LED_DATA_PIN    6
#define LED_TYPE  NEOPIXEL
#define COLOR_ORDER GRB
#define NUM_LEDS    60

#define LIGHT_SENSOR_PIN  D0

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


StaticHandClockFace clockFace1("Static hands - RG and no second hand", CRGB::Green, CRGB::Red, CRGB::Black);
StaticHandClockFace clockFace2("Static hands - RGB", CRGB::Green, CRGB::Red, CRGB::Blue);
PaletteTickClockFace clockFace3("Palette Tick Hands - RG, Blue to Green sweep", secondPalette, CRGB::Green, CRGB::Red);
PalettePerHandClockFace clockFace4("A palette per hand - Blue to Green sweep", hourPalette, minutePalette, secondPalette);

const int clocksFaceListLength = 4;
ClockFace* clocksFaceList[clocksFaceListLength] = {
  &clockFace1,
  &clockFace2,
  &clockFace3,
  &clockFace4
};

ClockState clockState(-1, 1, clocksFaceList, clocksFaceListLength);


#define BRIGHTNESS          30
#define FRAMES_PER_SECOND  120

#define NTP_RESYNC_FREQUENCY 300
#define RTC_RESYNC_FREQUENCY 30

NetworkManager networkManager(clockState);


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

  wav_setup();

  int chimesListLength = 0;

  Dir dir = SPIFFS.openDir("/sounds");
  while (dir.next()) {
//    Serial.println(dir.fileName());
//    clockState.addSoundFile("X");
    chimesListLength++;
  }

  String chimesList[chimesListLength];

  dir = SPIFFS.openDir("/sounds");
  int i = 0;
  while (dir.next()) {
    chimesList[i++] = dir.fileName();
//    String x = String(i + 1);
//    chimesList[i++] = x;
  }

//  for (uint8_t i = 0; i < chimesListLength; i++)
//  {
//    Serial.println(chimesList[i]);
//  }

  clockState.addChimeFiles(chimesList, chimesListLength);
  
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
//    if(isNightMode) {
//      Serial.println("Night Mode");
//    } else {
//      Serial.println("Not night Mode");
//    }
    
    time_t utc = now();
    time_t ukTime = localTimeZone.toLocal(utc);
  
    // Generate a pattern
  //  rainbowWithGlitter();

//    printTime(ukTime);
//    Serial.println(currentClockFace->getDescription());

    ClockFace *currentClockFace = clocksFaceList[clockState.getCurrentClockFaceNumber()];
    displayTimeOnLedRing(ukTime, currentClockFace);
  //  addGlitter(10);
    
    // send the 'leds' array out to the actual LED strip
    FastLED.show();
    // insert a delay to keep the framerate modest
//    FastLED.delay(1000/FRAMES_PER_SECOND); 
  }
  
  networkManager.handleServerClients();

//  EVERY_N_SECONDS( 30 ) {
//    wav_startPlayingFile("/sounds/cuckoo_clock2_16w.wav");
//  }

  wav_loop();
}


bool checkNightMode() {
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

void displayTimeOnLedRing(time_t timeNow, ClockFace *clockFace) 
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



  clockFace->setHandPositions(hour_led, minute_led, second_led);
  
  for(int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = clockFace->getLedColour(i);
  }
  
  clockFace->incrementIndex();
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

