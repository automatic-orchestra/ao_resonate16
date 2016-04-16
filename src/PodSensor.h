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

struct Meister {
  bool currentlyActive;
  bool wasActive;
  bool isAtNote;
};


class PodSensor : public Pod
{
public:
  PodSensor(Orchestra* pParent);
  ~PodSensor();
  
  void onClockBeatChange(unsigned long beat);
  void onMotorMessage(uint8_t pMessage, uint16_t pValue);
  void onSensorMessage(uint8_t pMessage, uint16_t pValue);

private:
  SensorOrchestra* getConcreteParent();
  void goToNote(uint8_t pAddRotations = 0);
  void playNote(uint16_t pNote);
  uint16_t getNextIndexToFollow();

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
  static const uint8_t STATE_COUNT = 7; //2
  PodState* mStates[STATE_COUNT];
  PodState* mCurrentState = nullptr;
  uint8_t mStateIndex = 0;
  // state related functions
  void setMeisterStates();

  void onTuningDone();
  bool isLastState();

  void fadeOutVolume();
  void fadeInVolume();

  Meister mMeisterStates = {false, false, false};

  bool mLastDrone = false; // flag for last fading out

  int8_t mNoteDelay = -1;

  uint8_t mNoteVelocity = 127;

};


#endif //PODSENSOR_H