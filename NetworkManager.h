#include <Arduino.h>
#include <ESP8266WiFi.h> //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <WiFiUdp.h>
#include <ESP8266WiFi.h> //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h> //Local DNS Server used for redirecting all requests to the configuration portal
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include "WebHandlers.h"

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

class NetworkManager {
  protected:
    int lastNtpUpdate;
    unsigned int localPort;      // local port to listen for UDP packets
    const int UDP_TIMEOUT = 2000;    // timeout in miliseconds to wait for an UDP packet to arrive

    IPAddress timeServerIP; // time.nist.gov NTP server address
    const char* ntpServerName = "time.nist.gov";

    byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

    // A UDP instance to let us send and receive packets over UDP
    WiFiUDP udp;

    ESP8266WebServer webServer;
    WebHandlers webHandlers;
    int reqCount = 0;                // number of requests received
    
    boolean connected = false;

  public:
    NetworkManager() : lastNtpUpdate(-1), localPort(2390), webServer(80), webHandlers(webServer) {
      
    }

    void setupWebServer();
    void runWebServer();

    void setupMDNS();
    
    void configureAndConnectToWifi();
    void setupNTP();
    unsigned long getNTPTime();
    
  private:
    unsigned long receiveNTPTime(int timeout);
    unsigned long sendNTPpacket(IPAddress& address);
};
