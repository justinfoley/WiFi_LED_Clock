#include "ColourGenerators.h"
class ClockState {
  protected:
    int lastNtpUpdate;
    int currentColourScheme;
    ClockColours** clocksColourList;
    int clocksColourListLength;

  public:
    ClockState(int ntpUpdate, int colourScheme, ClockColours** colourList, int colourListLength) : 
      lastNtpUpdate(ntpUpdate), currentColourScheme(colourScheme), clocksColourList(colourList), clocksColourListLength(colourListLength) {
    }

    void setNtpUpdate(int datetime);

    int getClocksColourListLength() {
      return clocksColourListLength;
    }

    String getClocksColourNamesJSON() {
      String json = "[";
      for (uint8_t i = 0; i < clocksColourListLength; i++)
      {
        json += "\"" + clocksColourList[i]->getDescription() + "\"";
        if (i < clocksColourListLength - 1)
          json += ",";
      }
      json += "]";
      return json;
    }

    int getCurrentColourScheme() {
      return currentColourScheme;
    }

    void setCurrentColourScheme(int number) {
      currentColourScheme = number;
    }

    void printState() {
      Serial.println(clocksColourList[1]->getLedColour(0));
    }
};
