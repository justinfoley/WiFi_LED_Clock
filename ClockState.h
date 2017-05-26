#include "ClockFaces.h"
class ClockState {
  protected:
    int lastNtpUpdate;
    int currentFaceNumber;
    ClockFace** faceList;
    int faceListLength;

  public:
    ClockState(int ntpUpdate, int faceNumber, ClockFace** faceList_, int faceListLength_) : 
      lastNtpUpdate(ntpUpdate), currentFaceNumber(faceNumber), faceList(faceList_), faceListLength(faceListLength_) {
    }

    void setNtpUpdate(int datetime);

    int getFaceListLength() {
      return faceListLength;
    }

    String getClockFaceNamesJSON() {
      String json = "[";
      for (uint8_t i = 0; i < faceListLength; i++)
      {
        json += "\"" + faceList[i]->getDescription() + "\"";
        if (i < faceListLength - 1)
          json += ",";
      }
      json += "]";
      return json;
    }

    int getCurrentClockFaceNumber() {
      return currentFaceNumber;
    }

    void setCurrentClockFaceNumber(int number) {
      currentFaceNumber = number;
    }

    void printState() {
      Serial.println(faceList[1]->getLedColour(0));
    }
};
