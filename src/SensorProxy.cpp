/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 SensorProxy.cpp
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

#include "SensorProxy.h"


SensorProxy::SensorProxy(uint8_t pPin) : mPin(pPin) {
  mBuffer = new uint16_t[BUFFER_SIZE];
  mNormalizer = new Normalizer();
}


SensorProxy::~SensorProxy() {
  delete mBuffer;
  delete mNormalizer;
}


void SensorProxy::update() {
  uint16_t pinValue = analogRead(mPin);
  mNormalizer->push(pinValue);
  if(mNormalizer->hasNew()) {
    //TODO write value into buffer
    if (mValueCallback != NULL) {
        (*mValueCallback)(mNormalizer->get());
    }
  }
}


void SensorProxy::setValueCallback(void (*pValueCallback)(uint16_t)) {
  mValueCallback = pValueCallback;
}
