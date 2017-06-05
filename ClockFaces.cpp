#include "ClockFaces.h"
#include <Arduino.h>

CRGB StaticHandClockFace::getLedColour(int ledIndex) {
  if(ledIndex == hour_led) {
    return hourColor;
  }
  else if(ledIndex == minute_led) {
    if(index % 15 != 0) {
      return minuteColor;
    } else {
      return highLightLed(minuteColor);
    }
  }
  else if(ledIndex == second_led) {
    return secondColor;
  } else {
    return CRGB::Black;
  }
}

CRGB PaletteTickClockFace::getLedColour(int ledIndex) {
  if(ledIndex == hour_led) {
    return hourColor;
  }
  else if(ledIndex == minute_led) {
    if(index % 15 != 0) {
      return minuteColor;
    } else {
      return highLightLed(minuteColor);
    }
  }
  else if(ledIndex <= second_led) {
    return ColorFromPalette(currentPalette, ledIndex, 255, LINEARBLEND);
  } else {
    return CRGB::Black;
  }
}

CRGB PalettePerHandClockFace::getLedColour(int ledIndex) {
  CRGB color = CRGB::Black;
  if(ledIndex == hour_led) {
    color += ColorFromPalette(hourPalette, index, 255, LINEARBLEND);
  }
  else if(ledIndex == minute_led) {
    color += ColorFromPalette(minutePalette, index, 255, LINEARBLEND);
    if(index % 15 == 0) {
      color = highLightLed(color);
    }
  }
  else if(ledIndex <= second_led) {
    color += ColorFromPalette(secondPalette, ledIndex, 255, LINEARBLEND);
  }
  return color;
}

