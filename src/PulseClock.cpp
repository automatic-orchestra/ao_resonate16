/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 PulseClock.cpp
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

#include "PulseClock.h"

#define MAX_PULSES 200


PulseClock::PulseClock(unsigned long pPulseInMs) : mTimeDiff(pPulseInMs * 1000) {
}


PulseClock::~PulseClock() {
}


void PulseClock::start() {
  reset();
}


void PulseClock::reset() {
  // Serial.println("(PC) -> reset()");
  mTimeNow = 0;
  mTimeLast = 0;
  // set to max pulses so first pulse is zero.
  mPulseCount = MAX_PULSES;
}


bool PulseClock::update() {
  bool res = false;
  // time in mircoseconds: 1000 milliseconds
  mTimeNow = micros();
  if (mTimeNow - mTimeLast >= mTimeDiff) {
    if (++mPulseCount >= MAX_PULSES) {
      mPulseCount = 0;
    }
    mTimeLast = mTimeNow;
    res = true;
  }
  return res;
}


uint8_t PulseClock::getCount() {
  return mPulseCount;
}