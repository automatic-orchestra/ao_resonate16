/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 PodSensor.h
 Copyright (c) 2016 Automatic Orchestra

 See the COPYRIGHT file at the top-level directory of this distribution and at:
 https://github.com/automatic-orchestra/ao_resonate16/blob/master/COPYRIGHT.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 This file is part of Automatic Orchestra. It is subject to the license terms
 in the LICENSE file found in the top-level directory of this distribution and at:
 https://github.com/automatic-orchestra/ao_resonate16/blob/master/LICENSE.

 No part of Automatic Orchestra, including this file, may be copied, modified,
 propagated, or distributed except according to the terms contained in the
 LICENSE file.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#ifndef PODSENSOR_H
#define PODSENSOR_H

#include <Arduino.h>
#include "Pod.h"
#include "SensorOrchestra.h"
#include "PodState.h"


class PodSensor : public Pod
{
public:
  PodSensor(Orchestra* pParent);
  ~PodSensor();
  
  void onClockBeatChange(unsigned long beat);
  void onMotorMessage(uint8_t pMessage, uint16_t pValue);
  void onSensorMessage(uint8_t pMessage, uint16_t pValue);

  void updateMeisterNoteIndex();
  void goToNote(uint8_t pAddRotations = 0);
  uint16_t getNextIndexToFollow();
  void playNote(uint16_t pNote);

  // uint8_t currentTuning = -1; //current note being tuned. -1 = intro. 0, 1, 2, 3, 4 refer to the bellow array
  // unsigned int tuneNotes[6] = {45, 57, 69, 81, 93}; //hard coded for now
  
  // unsigned int meisterOrder[5] = {3,4,2,5,1}; //hard coding the iteration of Meisters, relate to above
  // unsigned int accelPattern[5] = {500,2000,3500,5000,6000}; //increasing acceleration of "seeking" notes, related to above
  // unsigned int maxSpeedPattern[5] = {1000,3000,5000,7000,8500}; //increasing max speed of "seeking" notes, related to above
  // bool tuneFlags[5] = {false,false,false,false,false}; //turned ON when the first motion to that note has finished
  // unsigned int pulseTimings[7] = {450,1050,1550,1950,2250,2450,2550}; //lookup table for main timming triggers. first is intro, rest is related to the notes, last to the ending.
  
  uint16_t pMeisterNoteIndex;
  uint16_t mNoteToFollowIndex;
  
  // unsigned long pRealPosition = 0;

  bool mIsMeister = false; // meister flag
  bool mLastDrone = false; // flag for last fading out

private:
  SensorOrchestra* getConcreteParent();

  static const uint16_t BUFFER_SIZE = 800; // see NOTES.md for details
  uint16_t mNotesBuffer[BUFFER_SIZE] = {};
  unsigned long mPositionsBuffer[BUFFER_SIZE] = {};

  uint8_t mTuneRange = 0; //both up and down
  int mLastIndex = 0;

  uint8_t mPulseDelay = 0;
  uint64_t mPulseCount = 0;
  uint16_t bufferCounter = 0;
  // unsigned int midiChannel;
  uint16_t mLastNote = 0;
  bool mPlayNotesInSyncToMovement = false;

  // new state handling - to be tested
  void updateState();
  static const uint8_t STATE_COUNT = 7;
  PodState* mStates[STATE_COUNT];
  PodState* mCurrentState = nullptr;
  uint8_t mStateIndex = 0;
  // state related functions
  void updateMeister();


  void fadeOutVolume();
  void fadeInVolume();
};


#endif //PODSENSOR_H