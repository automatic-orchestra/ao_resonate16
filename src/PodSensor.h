/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 PodSensor.h
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

#ifndef PODSENSOR_H
#define PODSENSOR_H

#include <Arduino.h>
#include "Pod.h"
#include "SensorOrchestra.h"


class PodSensor : public Pod
{
public:
  PodSensor(Orchestra* pParent);
  ~PodSensor();
  SensorOrchestra* getConcreteParent();
  void onClockBeatChange(unsigned long beat);
  void onMotorMessage(uint8_t pMessage, uint16_t pValue);
  void onSensorMessage(uint8_t pMessage, uint16_t pValue);

  static const uint16_t BUFFER_SIZE = 1600; // see NOTES.md for details
  uint16_t pNotes[BUFFER_SIZE] = {};
  unsigned long pPositions[BUFFER_SIZE] = {};

  bool pIsMeister = true;

private:
  uint8_t mPulseDelay = 0;
  uint64_t mPulseCount = 0;
  uint16_t bufferCounter = 0;
};


#endif //PODSENSOR_H