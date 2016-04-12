/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 PulseClock.h
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

#ifndef PULSECLOCK_H
#define PULSECLOCK_H

#include <Arduino.h>


class PulseClock
{
public:
  PulseClock(unsigned long pPulseInMs);
  ~PulseClock();
  void start();
  void reset();
  bool update();
  uint8_t getCount();
private:
  uint8_t mPulseCount = 0;
  unsigned long mTimeDiff;
  unsigned long mTimeNow = 0;
  unsigned long mTimeLast = 0;
};

#endif //PULSECLOCK_H