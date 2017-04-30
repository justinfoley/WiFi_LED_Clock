#include "WebHandlers.h"

const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

void WebHandlers::setupHandlers() {
  webServer.on("/", std::bind(&WebHandlers::handleRoot, this));
  webServer.onNotFound(std::bind(&WebHandlers::handleNotFound, this));  
}

void WebHandlers::handleRoot() {
  webServer.send(200, "text/plain", "hello from esp8266 root!");
}

void WebHandlers::handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  for (uint8_t i=0; i<webServer.args(); i++){
    message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  }
  webServer.send(404, "text/plain", message);
}
