#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"

class ClockColours {
  protected:
    int hour_led;
    int minute_led;
    int second_led;

  public:
    ClockColours() {
    }

    void setHandPositions(int h_led, int m_led, int s_led) {
      hour_led = h_led;
      minute_led = m_led;
      second_led = s_led;
    }
    
    virtual CRGB getLedColour(int ledIndex) = 0;
};

class BasicClockColours: public ClockColours {
  protected:
    CRGBPalette16& currentPalette;

  public:
    BasicClockColours(CRGBPalette16& palette): currentPalette(palette) {
    }
  
    CRGB getLedColour(int ledIndex);
};

class PalettePerHandClockColours: public ClockColours {
  protected:
    int index = 0;
    CRGBPalette16& hourPalette;
    CRGBPalette16& minutePalette;
    CRGBPalette16& secondPalette;
  
  public:
    PalettePerHandClockColours(CRGBPalette16& hPalette, CRGBPalette16& mPalette, CRGBPalette16& sPalette): 
      hourPalette(hPalette), minutePalette(mPalette), secondPalette(sPalette) {
    }

    void incrementIndex() {
      index = (index + 4) % 255;
    }
  
    CRGB getLedColour(int ledIndex);
};
