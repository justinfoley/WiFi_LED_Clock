#pragma once
#define FASTLED_INTERRUPT_RETRY_COUNT 0
//#define FASTLED_ALLOW_INTERRUPTS 0
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

    virtual void incrementIndex() = 0;
    
    virtual CRGB getLedColour(int ledIndex) = 0;
};

class StaticHandClockColours: public ClockColours {
  protected:
    CRGB::HTMLColorCode secondColor;
    CRGB::HTMLColorCode hourColor;
    CRGB::HTMLColorCode minuteColor;

  public:
    StaticHandClockColours(CRGB::HTMLColorCode hColor, CRGB::HTMLColorCode mColor, CRGB::HTMLColorCode sColor): 
      hourColor(hColor), minuteColor(mColor), secondColor(sColor) {
    }

    void incrementIndex() {
    }
  
    CRGB getLedColour(int ledIndex);
};

class PaletteTickClockColours: public ClockColours {
  protected:
    CRGBPalette16& currentPalette;
    CRGB::HTMLColorCode hourColor;
    CRGB::HTMLColorCode minuteColor;

  public:
    PaletteTickClockColours(CRGBPalette16& palette, CRGB::HTMLColorCode hColor, CRGB::HTMLColorCode mColor): 
      currentPalette(palette), hourColor(hColor), minuteColor(mColor) {
    }

    void incrementIndex() {
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
      index = (index + 2) % 255;
    }
  
    CRGB getLedColour(int ledIndex);
};
