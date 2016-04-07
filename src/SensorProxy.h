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

#include <Arduino.h>
#include "Normalizer.h"


class SensorProxy
{
public:
  SensorProxy(uint8_t pPin);
  ~SensorProxy();  
  void update();
  void setValueCallback(void (*pValueCallback)(uint16_t));
private:
  static const uint16_t BUFFER_SIZE = 6400; // 200 steps and 1/32 microsteps
  uint16_t* mBuffer;
  Normalizer* mNormalizer;
  uint8_t mPin;
  void (*mValueCallback)(uint16_t) = NULL;
};


#endif //SENSORPROXY_H