/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 PodState.h
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

#ifndef PODSTATE_H
#define PODSTATE_H

#include <Arduino.h>


class PodState
{
public:
  PodState(
    String pName,
    uint8_t pMeisterID,
    uint16_t pAcceleration,
    uint16_t pMaxSpeed,
    uint8_t pTuningNote,
    uint16_t pDurationInPulses
  );
  ~PodState();
  String getName();
  uint8_t getMeisterID();
  uint16_t getAcceleration();
  uint16_t getMaxSpeed();
  uint8_t getTuningNote();
  uint16_t getDurationInPulses();
private:
  String mName;
  uint8_t mMeisterID;
  uint16_t mAcceleration;
  uint16_t mMaxSpeed;
  uint8_t mTuningNote;
  uint16_t mDurationInPulses;
};


#endif //PODSTATE_H