#include "WebHandlers.h"

const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

void WebHandlers::setupHandlers() {
//  auto fp = std::bind(&WebHandlers::handleRoot, this);
  
  webServer.on("/", std::bind(&WebHandlers::handleRoot, this));

  webServer.on("/update", HTTP_GET, std::bind(&WebHandlers::handleUpdateIndex, this));

  webServer.on("/update", HTTP_POST, std::bind(&WebHandlers::handleUpdateUpload1, this), std::bind(&WebHandlers::handleUpdateUpload2, this));
  
//
//  webServer.on("/inline", [](){
//    this->webServer.send(200, "text/plain", "this works as well inline");
//  });
    
//    server.on(
//      "/update", HTTP_POST, 
//      [](){
//        server.sendHeader("Connection", "close");
//        server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
//        ESP.restart();
//      }
//     ,[](){
//      HTTPUpload& upload = server.upload();
//      if(upload.status == UPLOAD_FILE_START){
//        Serial.setDebugOutput(true);
//        WiFiUDP::stopAll();
//        Serial.printf("Update: %s\n", upload.filename.c_str());
//        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
//        if(!Update.begin(maxSketchSpace)){//start with max available size
//          Update.printError(Serial);
//        }
//      } else if(upload.status == UPLOAD_FILE_WRITE){
//        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
//          Update.printError(Serial);
//        }
//      } else if(upload.status == UPLOAD_FILE_END){
//        if(Update.end(true)){ //true to set the size to the current progress
//          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
//        } else {
//          Update.printError(Serial);
//        }
//        Serial.setDebugOutput(false);
//      }
//      yield();
//    }
//    );


  webServer.onNotFound(std::bind(&WebHandlers::handleNotFound, this));  
}

void WebHandlers::handleRoot() {
  webServer.send(200, "text/plain", "hello from esp8266 root!");
}

void WebHandlers::handleUpdateIndex() {
  webServer.sendHeader("Connection", "close");
  webServer.send(200, "text/html", serverIndex);
}

void WebHandlers::handleUpdateUpload1() {
  webServer.sendHeader("Connection", "close");
  webServer.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
  ESP.restart();
}

void WebHandlers::handleUpdateUpload2() {
  HTTPUpload& upload = webServer.upload();
  
  if(upload.status == UPLOAD_FILE_START) {
    Serial.setDebugOutput(true);
    WiFiUDP::stopAll();
    Serial.printf("Update: %s\n", upload.filename.c_str());
    
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    
    if(!Update.begin(maxSketchSpace)){//start with max available size
      Update.printError(Serial);
    }
  } else if(upload.status == UPLOAD_FILE_WRITE) {
    if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
      Update.printError(Serial);
    }
  } else if(upload.status == UPLOAD_FILE_END) {
    if(Update.end(true)){ //true to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
    Serial.setDebugOutput(false);
  }
  yield();
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
