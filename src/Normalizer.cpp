/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Normalizer.cpp
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

#include "Normalizer.h"


Normalizer::Normalizer() {
  mSteps = 0;
  mAccum = 0;
  mValue = 0;
  mNew = false;
}


Normalizer::~Normalizer() {
}


void Normalizer::push(uint16_t pValue) {
  mAccum += pValue;
  if(++mSteps == MAX_STEPS) {
    mNew = true;
    mValue = mAccum >> 2; // quick divide by 4
    mAccum = 0;
    mSteps = 0;
  }
}


uint16_t Normalizer::get() {
  mNew = false;
  return mValue;
}


bool Normalizer::hasNew() {
  return mNew;
}
