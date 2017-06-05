#pragma once

#include "ClockFaces.h"
#include "Vector.h"
//#include <stdlib.h>
//#include <ArduinoSTL.h>



class ClockState {
  protected:
    int lastNtpUpdate;
    int currentFaceNumber;
    int currentChimeNumber;
    ClockFace** faceList;
    int faceListLength;
    String* chimeList;
    int chimesListLength;

//    vector<String> getSoundFileList() {
//      vector<String> soundFiles;
//      
////      Dir dir = SPIFFS.openDir("/sounds");
////      while (dir.next()) {
////        soundFiles.push_back(String(dir.fileName()));
////      }
//
//      return soundFiles;
//    }

  public:
    ClockState(int ntpUpdate, int faceNumber);

    void addChimeFiles(String* chimes, int chimesListLength);
    void setNtpUpdate(int datetime);

    int getFaceListLength();

    String getClockStateJSON();

    int getCurrentClockFaceNumber();

    ClockFace* getCurrentClockFace();

    void setCurrentClockFaceNumber(int number);

    void setCurrentClockChimeNumber(int number);

    void testCurrentClockChime();

    void printState();
};


