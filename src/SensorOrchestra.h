/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 SensorOrchestra.h
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

#ifndef SENSORORCHESTRA_H
#define SENSORORCHESTRA_H

#include <Arduino.h>
#include "Orchestra.h"
#include "MotorProxy.h"
#include "SensorProxy.h"
#include "PulseClock.h"

class MovementSensor;


class SensorOrchestra : public Orchestra
{
public:
  SensorOrchestra();
  ~SensorOrchestra();
  void setMotor(MotorProxy* pMotor);
  void setSensor(SensorProxy* pSensor);
  void setClock(PulseClock* pClock);
  MotorProxy* getMotor();
  SensorProxy* getSensor();
  PulseClock* getClock();
  void start();
  void update();
  // sensor specific functions
  MovementSensor* getCurrentMovement();
  // MIDI message methods
  void onClockStart();
  // movement handling
  void changeMovement(int pMovementID);
  // sensor handling
  void onMotorMessage(uint8_t pMessage, uint16_t pValue);
  void onSensorMessage(uint8_t pMessage, uint16_t pValue);
private:
  MotorProxy* mMotor = nullptr;
  SensorProxy* mSensor = nullptr;
  PulseClock* mClock = nullptr;
  void startFirstMovement();
};


#endif //SENSORORCHESTRA_H