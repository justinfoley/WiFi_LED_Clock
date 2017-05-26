#pragma once
#define FASTLED_INTERRUPT_RETRY_COUNT 0
//#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"

class ClockFace {
  protected:
    int hour_led;
    int minute_led;
    int second_led;

  public:
    ClockFace() {
    }

    void setHandPositions(int h_led, int m_led, int s_led) {
      hour_led = h_led;
      minute_led = m_led;
      second_led = s_led;
    }

    virtual void incrementIndex() = 0;
    
    virtual CRGB getLedColour(int ledIndex) = 0;
    virtual String getDescription() = 0;
};

class StaticHandClockFace: public ClockFace {
  protected:
    CRGB::HTMLColorCode secondColor;
    CRGB::HTMLColorCode hourColor;
    CRGB::HTMLColorCode minuteColor;

  public:
    StaticHandClockFace(CRGB::HTMLColorCode hColor, CRGB::HTMLColorCode mColor, CRGB::HTMLColorCode sColor): 
      hourColor(hColor), minuteColor(mColor), secondColor(sColor) {
    }

    void incrementIndex() {
    }
  
    CRGB getLedColour(int ledIndex);

    String getDescription() {
      return "Static Hands";
    }
};

class PaletteTickClockFace: public ClockFace {
  protected:
    CRGBPalette16& currentPalette;
    CRGB::HTMLColorCode hourColor;
    CRGB::HTMLColorCode minuteColor;

  public:
    PaletteTickClockFace(CRGBPalette16& palette, CRGB::HTMLColorCode hColor, CRGB::HTMLColorCode mColor): 
      currentPalette(palette), hourColor(hColor), minuteColor(mColor) {
    }

    void incrementIndex() {
    }
  
    CRGB getLedColour(int ledIndex);

    String getDescription() {
      return "Palette Tick Hands";
    }
};

class PalettePerHandClockFace: public ClockFace {
  protected:
    int index = 0;
    CRGBPalette16& hourPalette;
    CRGBPalette16& minutePalette;
    CRGBPalette16& secondPalette;
  
  public:
    PalettePerHandClockFace(CRGBPalette16& hPalette, CRGBPalette16& mPalette, CRGBPalette16& sPalette): 
      hourPalette(hPalette), minutePalette(mPalette), secondPalette(sPalette) {
    }

    void incrementIndex() {
      index = (index + 2) % 255;
    }
  
    CRGB getLedColour(int ledIndex);

    String getDescription() {
      return "A palette per hand";
    }
};
