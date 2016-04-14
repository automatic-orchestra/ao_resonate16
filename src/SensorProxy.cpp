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
#include "SensorMessages.h"

#define SP_DEBUG true


SensorProxy::SensorProxy(uint8_t pPin) : mPin(pPin) {
}


SensorProxy::~SensorProxy() {
  delete[] mBuffer;
}


void SensorProxy::update() {
  // uint16_t pinValue = analogRead(mPin);
  // mNormalizer->push(pinValue);
  // if(mNormalizer->hasNew()) {
  //   //TODO write value into buffer
  //   if (mValueCallback != NULL) {
  //       (*mValueCallback)(mNormalizer->get());
  //   }
  // }
  if(mBuffering) {
    unsigned long t = micros();
    if(t - mLastBufferTime >= BUFFER_CAPTURE_TIME_OFFSET) {
      captureBufferValue();
      mLastBufferTime = t;
    } 
  }
}


void SensorProxy::startBuffering() {
  #if SP_DEBUG
    Serial.println("(SP) -> startBuffering()");
  #endif
  mLastBufferTime = micros();
  captureBufferValue();  
  mBuffering = true;
}


void SensorProxy::setMessageCallback(void (*pCallback)(uint8_t, uint16_t)) {
  mCallback = pCallback;
}


void SensorProxy::sendMessage(uint8_t pMessage, uint16_t pValue) {
  if(mCallback) {
    (* mCallback)(pMessage, pValue);
  }
}


void SensorProxy::captureBufferValue() {
  if(mBufferIndex == BUFFER_SIZE) {
    #if SP_DEBUG
      Serial.println("(SP) -> captureBufferValue(): Buffer is full.");
    #endif
    sendMessage(SensorMessages::BUFFER_FULL);
    mBuffering = false;
  } else {
    uint16_t pinValue = analogRead(mPin);
    mBuffer[mBufferIndex] = pinValue;
    mapSensorToNote();
    #if SP_DEBUG
      Serial.print("(SP) -> captureBufferValue(): wrote value to buffer: ");
      Serial.println(mBuffer[mBufferIndex]);
      Serial.print("(SP) -> mapSensorToNote(): mapped value to note: ");
      Serial.println(mNotes[mBufferIndex]);
    #endif
    mBufferIndex++;
  }  
}

void SensorProxy::mapSensorToNote() {
  mNotes[mBufferIndex] = map(mBuffer[mBufferIndex],0,1023,36,95);
}






