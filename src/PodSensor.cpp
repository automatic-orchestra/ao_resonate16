/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 PodSensor.cpp
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

#include "PodSensor.h"
#include "MotorProxy.h"
#include "CFOSynthesizer.h"
#include "MotorMessages.h"
#include "SensorMessages.h"

#define SP_DEBUG true
#define CHANNEL_DELAY_INCREMENT 15


PodSensor::PodSensor(Orchestra* pParent) : Pod(pParent) {
  // retrieve MIDI channel
  int midiChannel = getParent()->getChannel();
  // calculate delay in pulses
  mPulseDelay = midiChannel * CHANNEL_DELAY_INCREMENT;
#if SP_DEBUG
  Serial.printf("(PS) -> constructor(): pulse delay is: %i\n", mPulseDelay);  
#endif
}


PodSensor::~PodSensor() {
}


SensorOrchestra* PodSensor::getConcreteParent() {
  // upcast from base class Orchestra to concrete class SensorOrchestra
  // to gain access to motor and sensor proxy instances.
  // http://www.cplusplus.com/doc/tutorial/typecasting/
  return static_cast<SensorOrchestra*>(getParent());
}


void PodSensor::onClockBeatChange(unsigned long beat) {
  // check initial pulse at zero...
#if SP_DEBUG
  if(!getConcreteParent()->getMotor()->isActive()) {
    #if SP_DEBUG
    Serial.printf("(PS) -> onClockBeatChange(): pulse count is: %i\n", mPulseCount);  
    #endif
  }
#endif
  // start motor movement after delay has elapsed
  MotorProxy* motor = getConcreteParent()->getMotor();
  if(mPulseCount >= mPulseDelay && !motor->isActive()) {
    // start acceleration phase
    motor->accelerateToSpeed(1600, 50, 1.25);
    motor->start();
  }
  // therefore increment afterwards
  mPulseCount++;
}


void PodSensor::onMotorMessage(uint8_t pMessage, uint16_t pValue) {
#if SP_DEBUG
  Serial.printf("(PS) -> onMotorMessage(): message: %i - value: %i", pMessage, pValue);
#endif
  switch(pMessage) {

    case MotorMessages::ACCELERATION_DONE:
      // start buffering phase
      getConcreteParent()->getSensor()->startBuffering();
      break;

      case MotorMessages::TURNS_DONE:
        if (pIsMeister)
        {
          //getConcreteParent()->getMotor()->decelerateToPosition(50, 0.9);
        }
      break;
  }
}


void PodSensor::onSensorMessage(uint8_t pMessage, uint16_t pValue) {
  #if SP_DEBUG
    Serial.printf("(PS) -> onSensorMessage(): message: %i - value: %i", pMessage, pValue);
  #endif
  switch(pMessage) {
    case SensorMessages::BUFFER_FULL:
        //Check if it is the Meister to assemble constant turns
      if (pIsMeister)
      {
        //Is Meister, run more
        getConcreteParent()->getMotor()->turnAtSpeed(1600,1);
      }
      else
      {
        //-> INCLUDE TURN THE VOLUME OF THE SYNTH DOWN
        getConcreteParent()->getMotor()->decelerateToSpeed(50, 0.9);
      }
      
      break;

      case SensorMessages::SENSOR_READING:
        pNotes[bufferCounter] = map(pValue,0,1023,36,95);
        pPositions[bufferCounter] = getConcreteParent()->getMotor()->mMotor.currentPosition();
        #if SP_DEBUG
          Serial.print("(PS) -> onSensorMessage(): index: ");
          Serial.print(bufferCounter);
          Serial.print(" pNote: ");
          Serial.print(pNotes[bufferCounter]);
          Serial.print(" pPositions: ");
          Serial.println(pPositions[bufferCounter]);
        #endif
        bufferCounter++;
      break;
  }
}



