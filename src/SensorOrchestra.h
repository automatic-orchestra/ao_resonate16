/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 SensorOrchestra.h
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

#ifndef SENSORORCHESTRA_H
#define SENSORORCHESTRA_H

#include <Arduino.h>
#include "Orchestra.h"
#include "SensorProxy.h"
#include "MotorProxy.h"
#include "Playlist.h"


class SensorOrchestra : public Orchestra
{
public:
  SensorOrchestra();
  ~SensorOrchestra();
  void setupMotor(MotorProxy* pMotor);
  void setupSensor(SensorProxy* pSensor);
  MotorProxy* getMotor();
  SensorProxy* getSensor();
  void start();
  void update();
  void onClockStart();
private:
  MotorProxy* mMotor = nullptr;
  SensorProxy* mSensor = nullptr;
};


#endif //SENSORORCHESTRA_H