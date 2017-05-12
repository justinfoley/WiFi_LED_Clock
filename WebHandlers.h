#pragma once
#include <FS.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

class WebHandlers {
  protected:
    ESP8266WebServer& webServer;

    void handleRoot();
    void handleNotFound();
    void handleClockChoice();

  public:
    WebHandlers(ESP8266WebServer& webServer_): webServer(webServer_)  {
    }

    void setupHandlers();
};
