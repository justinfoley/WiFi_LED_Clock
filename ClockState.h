#include "ColourGenerators.h"
class ClockState2 {
  protected:
    int lastNtpUpdate;
    int currentColourScheme;
    ClockColours** clocksColourList;
    int clocksColourListLength;

public:
    ClockState2(int ntpUpdate, int colourScheme, ClockColours** colourList, int colourListLength) : 
      lastNtpUpdate(ntpUpdate), currentColourScheme(colourScheme), clocksColourList(colourList), clocksColourListLength(colourListLength) {
    }

    void setNtpUpdate(int datetime);

    void printState() {
      Serial.println(clocksColourList[1]->getLedColour(0));
    }
};
