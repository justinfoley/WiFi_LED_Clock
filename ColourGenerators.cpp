#include "ColourGenerators.h"
#include <Arduino.h>

CRGB StaticHandClockColours::getLedColour(int ledIndex) {
  if(ledIndex == hour_led) {
    return hourColor;
  }
  else if(ledIndex == minute_led) {
    return minuteColor;
  }
  else if(ledIndex == second_led) {
      return secondColor;
  } else {
    return CRGB::Black;
  }
}

CRGB PaletteTickClockColours::getLedColour(int ledIndex) {
  if(ledIndex == hour_led) {
    return hourColor;
  }
  else if(ledIndex == minute_led) {
    return minuteColor;
  }
  else if(ledIndex <= second_led) {
      return ColorFromPalette(currentPalette, ledIndex, 255, LINEARBLEND);
  } else {
    return CRGB::Black;
  }
}

CRGB PalettePerHandClockColours::getLedColour(int ledIndex) {
//  Serial.println(index);
  if(ledIndex == hour_led) {
    return ColorFromPalette(hourPalette, index, 255, LINEARBLEND);
  }
  else if(ledIndex == minute_led) {
    return ColorFromPalette(minutePalette, index, 255, LINEARBLEND);
  }
  else if(ledIndex <= second_led) {
      return ColorFromPalette(secondPalette, ledIndex, 255, LINEARBLEND);
  } else {
    return CRGB::Black;
  }
}
