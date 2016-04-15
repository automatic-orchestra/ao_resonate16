/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 MovementSensor.cpp
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

#include "MovementSensor.h"
#include "PodSensor.h"


MovementSensor::MovementSensor(Orchestra* pParent, int pNextMovement) : Movement(pParent, pNextMovement) {
  mPod = new PodSensor(pParent);
}


MovementSensor::~MovementSensor() {
  delete mPod;
  mPod = nullptr;
}


Pod* MovementSensor::getPod() {
  return mPod;
}


PodSensor* MovementSensor::getConcretePod() {
  // upcast from base class Pod to concrete class PodSensor
  // http://www.cplusplus.com/doc/tutorial/typecasting/
  return static_cast<PodSensor*>(mPod);
}


String MovementSensor::getName() {
  return "MovementSensor (MS)";
}


int MovementSensor::isFinished() {
    if(parent()->isKlockMeister()) {
        return false ? getNextMovement() : MOVEMENT_DO_NOT_CHANGE;
    } else {
        return MOVEMENT_DO_NOT_CHANGE;
    }
}


void MovementSensor::onClockBeatChange(unsigned long beat) {
  Movement::onClockBeatChange(beat);
}


void MovementSensor::onMotorMessage(uint8_t pMessage, uint16_t pValue) {
  getConcretePod()->onMotorMessage(pMessage, pValue);
}


void MovementSensor::onSensorMessage(uint8_t pMessage, uint16_t pValue) {
  getConcretePod()->onSensorMessage(pMessage, pValue);
}


