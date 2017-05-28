#include "ClockState.h"
#include "Sound.h"

ClockState::ClockState(int ntpUpdate, int faceNumber, ClockFace** faceList_, int faceListLength_) : 
  lastNtpUpdate(ntpUpdate), currentFaceNumber(faceNumber), faceList(faceList_), faceListLength(faceListLength_) {
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
