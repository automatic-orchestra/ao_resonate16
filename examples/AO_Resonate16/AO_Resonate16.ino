/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 AO_Resonate16.ino
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
#include "MotorProxy.h"
#include "SensorOrchestra.h"

#define SENSOR_PIN A0
#define MOTOR_DIR_PIN 3
#define MOTOR_STEP_PIN 4
#define LED_PIN 13 // original value: 5

SensorProxy* mSensor;
MotorProxy* mMotor;
SensorOrchestra* mOrchestra;


void setup() {
  Serial.begin(9600);
  delay(500);
  // setup pins
  pinMode(SENSOR_PIN, INPUT_PULLDOWN);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  pinMode(MOTOR_STEP_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  // setup sensor
  mSensor = new SensorProxy(SENSOR_PIN);
  mSensor->setValueCallback(&onSensorValue);
  // setup motor
  mMotor = new MotorProxy(MOTOR_DIR_PIN, MOTOR_STEP_PIN);
  // setup orchestra
  mOrchestra = new SensorOrchestra();
}


void loop() {
  mSensor->update();
}


void onSensorValue(uint16_t pValue) {
  //Serial.printf("sensor value: %i\n", pValue);
}