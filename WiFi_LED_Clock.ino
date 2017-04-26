  // put your setup code here, to run once:
#include <ESP8266WiFi.h> //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h> //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <WiFiUdp.h>

#include <DS1307RTC.h>
 
#include <TimeLib.h>
#include <Timezone.h>

#include "FastLED.h"

FASTLED_USING_NAMESPACE

//United Kingdom (London, Belfast)
TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};        //British Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};         //Standard Time
Timezone localTimeZone(BST, GMT);


const int JANUARY_FIRST_2000 = 946684800;

unsigned int localPort = 2390;      // local port to listen for UDP packets
const int UDP_TIMEOUT = 2000;    // timeout in miliseconds to wait for an UDP packet to arrive

/* Don't hardwire the IP address or we won't get the benefits of the pool.
 *  Lookup the IP address for the host name instead */
//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

WiFiServer server(80);
int reqCount = 0;                // number of requests received

boolean connected = false;


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


void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, 3000);

  setupGradientPalette();
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
//  wifiManager.resetSettings();

  wifiManager.setAPStaticIPConfig(IPAddress(10,0,0,1), IPAddress(10,0,0,1), IPAddress(255,255,255,0));

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect("wifi-clock", "password")) {  // , "ifiwifiw"
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());

  server.begin();
}


void loop()
{
  runServer();
  
  showNTPTime();

//  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
//  unsigned long epoch = receiveNTPTime(UDP_TIMEOUT);
//  printTime(epoch);

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
  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP); 

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  unsigned long epoch = receiveNTPTime(UDP_TIMEOUT);

  Serial.print("  Raw NTP time = "); 
  Serial.println(epoch);

  RTC.set(epoch);

  time_t rtcTime = RTC.get();

  Serial.print("RTC Epoch time = ");
  Serial.println(rtcTime);
  
  printTime(rtcTime);
  displayTimeOnLedRing(rtcTime);
}
//
//  Raw NTP time = 1493235541
//RTC Epoch time = 1671896785
//     Unix time = 1671896785


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

void runServer() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      connected = true;
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          Serial.println("Sending response");

//          delay(200);
          
          // send a standard http response header
          // use \r\n instead of many println statements to speedup data send
          client.println(F(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"  // the connection will be closed after completion of the response
            "Refresh: 20\r\n"        // refresh the page automatically every 20 sec
            "\r\n"));
          client.print("<!DOCTYPE HTML>\r\n");
          client.print("<html>\r\n");
          client.print("<h1>Hello World!</h1>\r\n");
          client.print("Requests received: ");
          client.print(++reqCount);
          client.print("<br>\r\n");
          client.print("Analog input A0: ");
//          client.print(analogRead(0));
          client.print("<br>\r\n");
          client.print("</html>\r\n");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(10);

    // close the connection:
    client.stop();
    connected = false;
    Serial.println("Client disconnected");
  }
}

unsigned long receiveNTPTime(int timeout) {
  // wait to see if a reply is available
  delay(1000);
  
  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
  }
  else {
    Serial.print("packet received, length=");
    Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    return epoch;
  }
  
  return -1;
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
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
