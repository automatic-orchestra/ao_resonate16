/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 PodState.cpp
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

#include "PodState.h"


PodState::PodState(
  String pName, 
  uint8_t pMeisterID, 
  uint16_t pAcceleration, 
  uint16_t pMaxSpeed, 
  uint8_t pTuningNote, 
  uint8_t pNoteDelay, 
  uint16_t pDurationInPulses
) {
  mName = pName;
  mMeisterID = pMeisterID;
  mAcceleration = pAcceleration;
  mMaxSpeed = pMaxSpeed;
  mTuningNote = pTuningNote;
  mNoteDelay = pNoteDelay;
  mDurationInPulses = pDurationInPulses;
}


PodState::~PodState() {
}


String PodState::getName() {
  return mName;
}


uint8_t PodState::getMeisterID() {
  return mMeisterID;
}


uint16_t PodState::getAcceleration() {
  return mAcceleration;
}


uint16_t PodState::getMaxSpeed() {
  return mMaxSpeed;
}


uint8_t PodState::getTuningNote() {
  return mTuningNote;
}


uint8_t PodState::getNoteDelay() {
  return mNoteDelay;
}


uint16_t PodState::getDurationInPulses() {
  return mDurationInPulses;
}
