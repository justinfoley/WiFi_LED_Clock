#include "ColourGenerators.h"
#include <Arduino.h>

CRGB BasicClockColours::getLedColour(int ledIndex) {
  if(ledIndex == hour_led) {
    return CRGB::Green;
  }
  else if(ledIndex == minute_led) {
    return CRGB::Red;
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
