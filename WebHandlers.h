#pragma once
#include <FS.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include "ClockState.h"

class WebHandlers {
  protected:
    ESP8266WebServer& webServer;
    ClockState& clockState;

    void handleRoot();
    void handleNotFound();
    void handleClockChoice();
    void handleClockFace();

  public:
    WebHandlers(ESP8266WebServer& webServer_, ClockState& clockState_): webServer(webServer_), clockState(clockState_)  {
    }

    void setupHandlers();
};
