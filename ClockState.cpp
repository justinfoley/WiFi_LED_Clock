#include "ClockState.h"
#include "Sound.h"

DEFINE_GRADIENT_PALETTE( retro2_16_gp ) {
    0, 188,135,  1,
  255,  46,  7,  1
};

DEFINE_GRADIENT_PALETTE( es_pinksplash_08_gp ) {
    0, 126, 11,255,
  127, 197,  1, 22,
  175, 210,157,172,
  221, 157,  3,112,
  255, 157,  3,112
};

DEFINE_GRADIENT_PALETTE( Coral_reef_gp ) {
    0,  40,199,197,
   50,  10,152,155,
   96,   1,111,120,
   96,  43,127,162,
  139,  10, 73,111,
  255,   1, 34, 71
};

DEFINE_GRADIENT_PALETTE( heatmap_gp ) {
  0,     0,  0,  0,   //black
128,   255,  0,  0,   //red
224,   255,255,  0,   //bright yellow
255,   255,255,255 }; //full white

DEFINE_GRADIENT_PALETTE( blue_green ) {
  0,     0,   0, 255, // blue
 60,     0, 255, 0,    // green
 255,   255,255,255
};

CRGBPalette16 hourPalette = es_pinksplash_08_gp;
CRGBPalette16 minutePalette = retro2_16_gp;
CRGBPalette16 secondPalette = blue_green;


StaticHandClockFace redGreenStaticFace("Static hands - RG and no second hand", CRGB::Green, CRGB::Red, CRGB::Black);
StaticHandClockFace redGreenBlueStaticFace("Static hands - RGB", CRGB::Green, CRGB::Red, CRGB::Blue);
PaletteTickClockFace redYellowBlueSweepPaletteFace("Palette Tick Hands - RG, Blue to Green sweep", secondPalette, CRGB::Yellow, CRGB::Red);
PalettePerHandClockFace blueSweepOtherPaletteFace("A palette per hand - Blue to Green sweep", hourPalette, minutePalette, secondPalette);

const int clocksFaceListLength = 4;
ClockFace* clocksFaceList[clocksFaceListLength] = {
  &redGreenStaticFace,
  &redGreenBlueStaticFace,
  &redYellowBlueSweepPaletteFace,
  &blueSweepOtherPaletteFace
};

ClockState::ClockState(int ntpUpdate, int faceNumber) : 
  lastNtpUpdate(ntpUpdate), currentFaceNumber(faceNumber), faceList(clocksFaceList), faceListLength(clocksFaceListLength) {
//  soundFiles = new Vector<String>();
}

void ClockState::addChimeFiles(String* chimes, int chimesListLength_) {
  chimeList = new String[chimesListLength_];
  chimesListLength = chimesListLength_;
//      soundFiles.push_back(String(file));
//  soundFiles.PushBack("A");

  for (uint8_t i = 0; i < chimesListLength_; i++) {
    chimeList[i] = String(chimes[i]);
  }

  for (uint8_t i = 0; i < chimesListLength_; i++) {
    Serial.println(chimeList[i]);
  }
}

int ClockState::getFaceListLength() {
  return faceListLength;
}

String ClockState::getClockStateJSON() {
  String json = "{";

  json += "\"faces\":[";
  for (uint8_t i = 0; i < faceListLength; i++)
  {
    json += "\"" + faceList[i]->getDescription() + "\"";
    if (i < faceListLength - 1)
      json += ",";
  }
  json += "]";

  json += ", \"currentFace\":{";
  json += "\"index\":" + String(currentFaceNumber);
  json += ",\"description\":\"" + faceList[currentFaceNumber]->getDescription() + "\"}";

  json += ", \"chimes\":[";
  
//      \"Chime 1\", \"Chime 2\", \"Cuckoo\"]";

//      std::vector<String> soundFiles = getSoundFileList();
//      using Iter = std::vector<String>::const_iterator;
//      for (Iter it = soundFiles.begin(); it!=soundFiles.end(); ++it) {
//        json += "\"";
//        json += *it;
//        json += "\",";
//      }

//        vector<String> soundFiles = getSoundFileList();
    for( int i = 0; i < chimesListLength; i++) {
      json += "\"";
      json += chimeList[i];
      json += "\",";
    }

  json.remove(json.length()-1);
  
  json += "]";


  json += ", \"currentChime\":{";
  json += "\"index\":" + String(0);
  json += ",\"description\":\"Chime 1\"}";


  json += "}";
  return json;
}


int ClockState::getCurrentClockFaceNumber() {
  return currentFaceNumber;
}

ClockFace* ClockState::getCurrentClockFace() {
  return faceList[currentFaceNumber];
}

void ClockState::setCurrentClockFaceNumber(int number) {
  currentFaceNumber = number;
}

void ClockState::setCurrentClockChimeNumber(int number) {
  currentChimeNumber = number;
}

void ClockState::testCurrentClockChime() {
  Serial.print("Testing chime - ");
  Serial.println(chimeList[currentChimeNumber]);
  wav_startPlayingFile(chimeList[currentChimeNumber].c_str());
}

void ClockState::printState() {
  Serial.println(faceList[1]->getLedColour(0));
}

