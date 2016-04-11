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


MotorProxy::MotorProxy(uint8_t pDirectionPin, uint8_t pStepPin) {
  mMotor = AccelStepper(AccelStepper::FULL2WIRE, pStepPin, pDirectionPin);
  // The desired maximum speed in steps per second. Must be > 0.
  mMotor.setMaxSpeed(1000);
  // Constant speed in steps per second. Positive is clockwise.
  // Speeds of more than 1000 steps per second are unreliable.
  mMotor.setSpeed(-500);  

  /// Resets the current position of the motor, so that wherever the motor
  /// happens to be right now is considered to be the new 0 position. Useful
  /// for setting a zero position on a stepper after an initial hardware
  /// positioning move.
  // setCurrentPosition(long position); 
}


MotorProxy::~MotorProxy() {

}


void MotorProxy::setZeroPosition() {
    /// Resets the current position of the motor, so that wherever the motor
    /// happens to be right now is considered to be the new 0 position. Useful
    /// for setting a zero position on a stepper after an initial hardware
    /// positioning move.
    mMotor.setCurrentPosition(0);
}


void MotorProxy::update() {
  mMotor.run();
}