#pragma once
#define FASTLED_INTERRUPT_RETRY_COUNT 0
//#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"

class ClockFace {
  protected:
    int hour_led;
    int minute_led;
    int second_led;
    String description;

  public:
    ClockFace(String description_) : description(description_) {
    }

    void setHandPositions(int h_led, int m_led, int s_led) {
      hour_led = h_led;
      minute_led = m_led;
      second_led = s_led;
    }

    String getDescription() {
      return description;
    }

    virtual void incrementIndex() = 0;
    
    virtual CRGB getLedColour(int ledIndex) = 0;
};

class StaticHandClockFace: public ClockFace {
  protected:
    CRGB::HTMLColorCode secondColor;
    CRGB::HTMLColorCode hourColor;
    CRGB::HTMLColorCode minuteColor;

  public:
    StaticHandClockFace(String description, CRGB::HTMLColorCode hColor, CRGB::HTMLColorCode mColor, CRGB::HTMLColorCode sColor): 
      ClockFace(description), hourColor(hColor), minuteColor(mColor), secondColor(sColor) {
    }

    void incrementIndex() {
    }
  
    CRGB getLedColour(int ledIndex);
};

class PaletteTickClockFace: public ClockFace {
  protected:
    CRGBPalette16& currentPalette;
    CRGB::HTMLColorCode hourColor;
    CRGB::HTMLColorCode minuteColor;

  public:
    PaletteTickClockFace(String description, CRGBPalette16& palette, CRGB::HTMLColorCode hColor, CRGB::HTMLColorCode mColor): 
      ClockFace(description), currentPalette(palette), hourColor(hColor), minuteColor(mColor) {
    }

    void incrementIndex() {
    }
  
    CRGB getLedColour(int ledIndex);
};

class PalettePerHandClockFace: public ClockFace {
  protected:
    int index = 0;
    CRGBPalette16& hourPalette;
    CRGBPalette16& minutePalette;
    CRGBPalette16& secondPalette;
  
  public:
    PalettePerHandClockFace(String description, CRGBPalette16& hPalette, CRGBPalette16& mPalette, CRGBPalette16& sPalette): 
      ClockFace(description), hourPalette(hPalette), minutePalette(mPalette), secondPalette(sPalette) {
    }

    void incrementIndex() {
      index = (index + 2) % 255;
    }
  
    CRGB getLedColour(int ledIndex);
};
