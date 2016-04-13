/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 MotorProxy.cpp
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

#include "MotorProxy.h"

#define MP_DEBUG false


MotorProxy::MotorProxy(uint8_t pDirectionPin, uint8_t pStepPin) {
  mMotor = AccelStepper(AccelStepper::FULL2WIRE, pStepPin, pDirectionPin);
  setDefaultSpeed(FULL_REVOLUTION);
}


MotorProxy::~MotorProxy() {

}


void MotorProxy::setZeroPosition() {
  Serial.println("(MP) -> setZeroPosition()");
  // Resets the current position of the motor, so that wherever the motor
  // happens to be right now is considered to be the new 0 position. Useful
  // for setting a zero position on a stepper after an initial hardware
  // positioning move. Has the side effect of setting the current motor speed to 0.
  mMotor.setCurrentPosition(0);
  // set speed to actual speed again
  mMotor.setSpeed(mDefaultSpeed);
}


void MotorProxy::update() {
  if(mActive) {
    if(mTargetMode) {
      mMotor.run();
      if(!mMotor.isRunning()) {
        mTargetMode = false;
        // setSpeed(mDefaultSpeed);
      }
    } else {
      if(mAccelSpeed && mMotor.speed() == mMotor.maxSpeed()) {
        mAccelSpeed = false;
        mAccelRate = 1.0;
      }
      mMotor.runSpeed();  
      if(mAccelSpeed) {
        unsigned long t = millis();
        if(t - mAccelTime > 200) {
          mMotor.setSpeed(mMotor.speed() * mAccelRate); 
          Serial.print("(MP) -> update(): speed: ");
          Serial.println(mMotor.speed());
          mAccelTime = t;
        }
      }
    }
  }
}


void MotorProxy::start() {
  if(!mActive) {
    Serial.println("(MP) -> start()");
    mActive = true;  
  }
}


void MotorProxy::setDefaultSpeed(uint16_t pSpeed) {
  mDefaultSpeed = pSpeed;
  setSpeed(pSpeed);
}


void MotorProxy::moveToTarget(uint16_t pPosition, float pAcceleration) {
  mMotor.setAcceleration(pAcceleration);
  mMotor.moveTo(pPosition);
  mTargetMode = true;
}


void MotorProxy::accelerateToSpeed(uint16_t pMaxSpeed, uint16_t pStartSpeed, float pAccelRate) {
  mMotor.setMaxSpeed(pMaxSpeed);
  mMotor.setSpeed(pStartSpeed);
  Serial.print("(MP) -> accelerateToSpeed(): speed: ");
  Serial.println(mMotor.speed());
  mAccelRate = pAccelRate;
  mAccelStart = millis();
  mAccelSpeed = true;
}


bool MotorProxy::isActive() {
  return mActive;
}


bool MotorProxy::isInTargetMode() {
  return mTargetMode;
}


bool MotorProxy::isInSpeedMode() {
  return !mTargetMode;
}


void MotorProxy::setSpeed(uint16_t pSpeed) {
  // The desired maximum speed in steps per second. Must be > 0.
  mMotor.setMaxSpeed(pSpeed);
  // Constant speed in steps per second. Positive is clockwise.
  // Speeds of more than 1000 steps per second are unreliable.
  mMotor.setSpeed(pSpeed);
}
