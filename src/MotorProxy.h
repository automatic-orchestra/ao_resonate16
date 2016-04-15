/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 MotorProxy.h
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

#ifndef MOTORPROXY_H
#define MOTORPROXY_H

#include <Arduino.h>
#include "AccelStepper.h"


class MotorProxy
{
public:
  MotorProxy(uint8_t pDirectionPin, uint8_t pStepPin);
  ~MotorProxy();
  static const uint16_t FULL_REVOLUTION = 25600;
  void setZeroPosition();
  void update();
  void start();
  void setDefaultSpeed(uint16_t pSpeed);
  void setMessageCallback(void (*pCallback)(uint8_t, uint16_t));
  void accelerateToSpeed(uint16_t pMaxSpeed, uint16_t pStartSpeed, float pAccelRate);
  void decelerateToSpeed(uint16_t pEndSpeed, float pAccelRate);
  void turnAtSpeed(uint16_t speed,uint8_t turns);
  bool isActive();
  AccelStepper mMotor;
  
private:
  void setSpeed(uint16_t pSpeed);
  void sendCallback(uint8_t pMessage, uint16_t pValue = 0);
  uint16_t mDefaultSpeed = 0;
  bool mActive = false;
  bool mIsAccelerating = false;
  bool mIsDecelerating = false;
  bool mIsTurning = false;
  float mAccelRate = 1.0;
  float mDecelRate = 1.0;
  uint16_t mDecelEndSpeed = 0;
  unsigned long mAccelStart;
  unsigned long mTime;
  unsigned long mTurnStart;
  unsigned long mTurnEnd;
  void (*mCallback)(uint8_t, uint16_t) = NULL;
};


#endif //MOTORPROXY_H