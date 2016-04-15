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
#include "MotorMessages.h"

#define MP_DEBUG true


MotorProxy::MotorProxy(uint8_t pDirectionPin, uint8_t pStepPin) {
  mMotor = AccelStepper(AccelStepper::FULL2WIRE, pStepPin, pDirectionPin);
  setDefaultSpeed(FULL_REVOLUTION);
}


MotorProxy::~MotorProxy() {
}


void MotorProxy::setZeroPosition() {
  #if MP_DEBUG
  Serial.println("(MP) -> setZeroPosition()");
  #endif
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
    /*
    if (mMotor.currentPosition() >= 25600)
    {
      // update positioning
      uint16_t currentSpeed = mMotor.speed();
      mMotor.setCurrentPosition(0);
      mMotor.setSpeed(currentSpeed);
    }
    */

    // check if acceleration is done
    if(mIsAccelerating && mMotor.speed() == mMotor.maxSpeed()) {
      mIsAccelerating = false;
      mAccelRate = 1.0;
      sendCallback(MotorMessages::ACCELERATION_DONE);
    }
    // check if deceleration is done
    if(mIsDecelerating && (int) mMotor.speed() <= mDecelEndSpeed) {
      mMotor.setSpeed(0);
      mIsDecelerating = false;
      mDecelRate = 1.0;
      sendCallback(MotorMessages::DECELERATION_DONE);
    }
    // calculate new acceleration motor speed
    if(mIsAccelerating) {
      unsigned long t = millis();
      if(t - mTime > 200) {
        mMotor.setSpeed(mMotor.speed() * mAccelRate); 
        #if MP_DEBUG
        Serial.print("(MP) -> update(): speed: ");
        Serial.println(mMotor.speed());
        #endif
        mTime = t;
      }
    }
    // calculate deceleration motor speed
    if(mIsDecelerating) {
      unsigned long t = millis();
      if(t - mTime > 200) {
        mMotor.setSpeed(mMotor.speed() * mDecelRate); 
        #if MP_DEBUG
        Serial.print("(MP) -> update(): speed: ");
        Serial.println(mMotor.speed());
        #endif
        mTime = t;
      } 
    }
    // check turns in turnAtSpeed currentTuning
    if (mIsTurning) {
      if (mMotor.currentPosition() >= mTurnEnd)
      {
        #if MP_DEBUG
        Serial.println("(MP) -> update(): motor turns completed! ");
        #endif
        mIsTurning = false;
        sendCallback(MotorMessages::TURNS_DONE);

      }
    }
    // check if is moving to position (accelerated, tuning)
    if (mIsMovingToPosition)
    {
      if(mMotor.distanceToGo() == 0)
      {
        mIsMovingToPosition = false;
        Serial.println("(MP) -> update(): reached note, begin again");
        sendCallback(MotorMessages::TUNING_DONE);
      }
    }

    // update motor
    if (!mIsMovingToPosition)
    {
      mMotor.runSpeed();
    }
    else
    {
      mMotor.run();  
    }
  }
}

void MotorProxy::moveToPosition(unsigned long pos, uint16_t acceleration, uint16_t maxspeed)
{
  mMotor.setMaxSpeed(12000);
  mMotor.setAcceleration(acceleration);
  mMotor.moveTo(pos);  
  mIsMovingToPosition = true;
}

void MotorProxy::start() {
  if(!mActive) {
    #if MP_DEBUG
    Serial.println("(MP) -> start()");
    #endif
    mActive = true;  
  }
}


void MotorProxy::setDefaultSpeed(uint16_t pSpeed) {
  mDefaultSpeed = pSpeed;
  setSpeed(pSpeed);
}


void MotorProxy::setMessageCallback(void (*pCallback)(uint8_t, uint16_t)) {
  mCallback = pCallback;
}


void MotorProxy::accelerateToSpeed(uint16_t pMaxSpeed, uint16_t pStartSpeed, float pRate) {
  mMotor.setMaxSpeed(pMaxSpeed);
  mMotor.setSpeed(pStartSpeed);
  #if MP_DEBUG
  Serial.print("(MP) -> accelerateToSpeed(): speed: ");
  #endif
  Serial.println(mMotor.speed());
  mAccelRate = pRate;
  mTime = millis();
  mIsAccelerating = true;
}

void MotorProxy::turnAtSpeed(uint16_t speed,uint8_t turns) {
  mMotor.setSpeed(speed);
  mIsTurning = true;
  mTurnStart = mMotor.currentPosition();
  mTurnEnd = mTurnStart + (25600*turns);
  #if MP_DEBUG
  Serial.print("(MP) -> turnAtFullSpeed(): turns: ");
  Serial.print(turns);
  Serial.print(" startPosition: ");
  Serial.print(mTurnStart);
  Serial.print(" endPosition: ");
  Serial.println(mTurnEnd);
  #endif
}


void MotorProxy::decelerateToSpeed(uint16_t pEndSpeed, float pRate) {
  mDecelEndSpeed = pEndSpeed;
  mDecelRate = pRate;
  mMotor.setSpeed(mMotor.speed() * mDecelRate); 
  mTime = millis();
  mIsDecelerating = true;
}


bool MotorProxy::isActive() {
  return mActive;
}


void MotorProxy::setSpeed(uint16_t pSpeed) {
  // The desired maximum speed in steps per second. Must be > 0.
  mMotor.setMaxSpeed(pSpeed);
  // Constant speed in steps per second. Positive is clockwise.
  // Speeds of more than 1000 steps per second are unreliable.
  mMotor.setSpeed(pSpeed);
}


void MotorProxy::sendCallback(uint8_t pMessage, uint16_t pValue) {
  if (mCallback != NULL) {
    (*mCallback)(pMessage, pValue);
  }
}


