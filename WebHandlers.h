#pragma once
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

class WebHandlers {
  protected:
    ESP8266WebServer& webServer;

    void handleRoot();
    void handleNotFound();
    void handleUpdateIndex();
    void handleUpdateUpload1();
    void handleUpdateUpload2();

  public:
    WebHandlers(ESP8266WebServer& webServer_): webServer(webServer_)  {
    }

    void setupHandlers();
};
