#include "WebHandlers.h"

const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

void WebHandlers::setupHandlers() {
  SPIFFS.begin();
//  webServer.on("/", std::bind(&WebHandlers::handleRoot, this));
  webServer.onNotFound(std::bind(&WebHandlers::handleNotFound, this));
  
  webServer.serveStatic("/index.htm", SPIFFS, "/index.htm");
  webServer.serveStatic("/fonts", SPIFFS, "/fonts", "max-age=86400");
  webServer.serveStatic("/js", SPIFFS, "/js");
  webServer.serveStatic("/css", SPIFFS, "/css", "max-age=86400");
  webServer.serveStatic("/images", SPIFFS, "/images", "max-age=86400");
  webServer.serveStatic("/sounds", SPIFFS, "/sounds", "max-age=86400");
  webServer.serveStatic("/", SPIFFS, "/index.htm");

  webServer.on("/face", HTTP_POST, std::bind(&WebHandlers::setCurrentClockFace, this));
  webServer.on("/chime", HTTP_POST, std::bind(&WebHandlers::setCurrentClockChime, this));
  webServer.on("/testChime", HTTP_POST, std::bind(&WebHandlers::testCurrentClockChime, this));
  webServer.on("/state", HTTP_GET, std::bind(&WebHandlers::getClockState, this));
}

void WebHandlers::handleRoot() {
  webServer.send(200, "text/plain", "hello from esp8266 root!");
}

void WebHandlers::getClockState() {  
  webServer.send(200, "application/json", clockState.getClockStateJSON());
}

void WebHandlers::setCurrentClockFace() {
  String value = webServer.arg("value");
  Serial.print("Clock face choice: ");
  Serial.println(value);
  clockState.setCurrentClockFaceNumber(value.toInt());
  webServer.send(200, "text/plain", "New clock chosen");
}

void WebHandlers::setCurrentClockChime() {
  String value = webServer.arg("value");
  Serial.print("Clock chime choice: ");
  Serial.println(value);
  clockState.setCurrentClockChimeNumber(value.toInt());
  webServer.send(200, "text/plain", "New chime chosen");
}

void WebHandlers::testCurrentClockChime() {
  Serial.print("Testing current chime");
  clockState.testCurrentClockChime();
  webServer.send(200, "text/plain", "Testing chime");
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
