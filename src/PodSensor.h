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


class PodSensor : public Pod
{
public:
  PodSensor(Orchestra* pParent);
  ~PodSensor();
  SensorOrchestra* getConcreteParent();
  void onClockBeatChange(unsigned long beat);
  void onMotorMessage(uint8_t pMessage, uint16_t pValue);
  void onSensorMessage(uint8_t pMessage, uint16_t pValue);
  void updateNoteToFollowIndex();
  void updateMeisterNoteIndex();
  void updateRealPosition();
  void fadeOutVolume();
  void fadeInVolume();
  void pGoToNote();

  uint8_t currentTuning = -1; //current note being tuned. -1 = intro. 0, 1, 2, 3, 4 refer to the bellow array
  int tuneNotes[6] = {45, 57, 69, 81, 93}; //hard coded for now
  uint8_t tuneRange = 1; //both up and down
  int meisterOrder[6] = {3,4,2,5,1}; //hard coding the iteration of Meisters, relate to above
  int accelPattern[6] = {500,800,1500,3000,4500}; //increasing acceleration of "seeking" notes, related to above
  int maxSpeedPattern[6] = {1000,1500,3000,4500,7000}; //increasing max speed of "seeking" notes, related to above
  int pulseTimings[7] = {450,1050,1550,1950,2250,2450,2550}; //lookup table for main timming triggers. first is intro, rest is related to the notes, last to the ending.
  int lastIndex = 0;
  uint16_t pMeisterNoteIndex;
  uint16_t pNoteToFollowIndex;
  static const uint16_t BUFFER_SIZE = 1600; // see NOTES.md for details
  uint16_t pNotes[BUFFER_SIZE] = {};
  unsigned long pPositions[BUFFER_SIZE] = {};
  unsigned long pRealPosition = 0;

  bool pIsMeister = false; // meister flag

private:
  uint8_t mPulseDelay = 0;
  uint64_t mPulseCount = 0;
  uint16_t bufferCounter = 0;
  int midiChannel;
};


#endif //PODSENSOR_H