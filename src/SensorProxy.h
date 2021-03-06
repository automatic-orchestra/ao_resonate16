/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 SensorProxy.h
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

#ifndef SENSORPROXY_H
#define SENSORPROXY_H
#define FILTERSAMPLES   7   //Has to be an ODD number

#include <Arduino.h>
#include "Normalizer.h"


class SensorProxy
{
public:
  SensorProxy(uint8_t pPin);
  ~SensorProxy();  
  void setMessageCallback(void (*pCallback)(uint8_t, uint16_t));
  void update();
  void startBuffering();

private:
  static const uint16_t BUFFER_SIZE = 800; // see NOTES.md for details
  static const uint16_t BUFFER_CAPTURE_TIME_OFFSET = 10000; // see NOTES.md for details

  void (*mCallback)(uint8_t, uint16_t) = NULL;
  void sendMessage(uint8_t pMessage, uint16_t pValue = 0);
  void captureBufferValue();
  int digitalSmooth(int rawIn, int *sensSmoothArray);

  uint8_t mPin;
  int mBuffer[BUFFER_SIZE] = {};
  int mBufferSmoothing [FILTERSAMPLES];

  bool mBuffering = false;
  unsigned long mLastBufferTime = 0;
  uint16_t mBufferIndex = 0;
};


#endif //SENSORPROXY_H