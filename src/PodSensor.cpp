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
  midiChannel = getParent()->getChannel();
  // calculate delay in pulses
  mPulseDelay = midiChannel * CHANNEL_DELAY_INCREMENT;
  // check if is the first Meister (lookup table,)
  if (midiChannel == meisterOrder[0])
  {
    pIsMeister = true;
  }
  else
  {
    pIsMeister = false;
  }
  // apply jakob's magic crescendo preset
  uint8_t presetArray[] = {
    23, 0, 0, 0, 50, 1, 127, 0, 121, 3, 0, 64, 64, 127, 9, 40, 0, 2, 0, 0, 0, 64, 63, 127, 4, 0, 0, 0, 0, 0, 0, 64, 61, 127, 4, 12, 0, 1, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 127, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 127, 127, 127, 127, 0, 0, 0, 0, 0, 1, 127, 127, 0, 127
  };
  applyPresets(presetArray);
#if SP_DEBUG
  Serial.printf("(PS) -> constructor(): midi channel is:  %i - meister flag: %i - pulse delay is: %i\n", midiChannel, pIsMeister, mPulseDelay);  
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

  // check pulse to alternate currentTuning

  if (mPulseCount == pulseTimings[0])
  {

    //Update Current Tuning Tone and Meister Flag
    currentTuning = 0;
    if (midiChannel != meisterOrder[currentTuning])
    {
      pIsMeister = false;
    }
    else
    {
      pIsMeister = true;
    }

    if (!pIsMeister)
    {
      #if SP_DEBUG
        Serial.printf("(PS) -> onClockBeatChange(): currentTuning: %i ", currentTuning);
        Serial.println();
      #endif
        pGoToNote();
    }
    else
    {
      //Hold the note

    }
  }

  // ---------------------------------------
  //  play the note based on motor position
  // ---------------------------------------
  // calculate relative postion of values in buffer
  unsigned long pos = getConcreteParent()->getMotor()->mMotor.currentPosition();
  unsigned long firstBufferPos = pPositions[0];
  unsigned long relativePos = firstBufferPos + ((pos - firstBufferPos) % MotorProxy::FULL_REVOLUTION);
  // find matching index of position in buffer
  int index = -1;
  for(int i = 0; i < BUFFER_SIZE; i++) {
    // following index
    int j = (i + 1 == BUFFER_SIZE) ? 0 : i + 1;
    // check buffer values to determine index
    if(relativePos >= pPositions[i] && relativePos < pPositions[j]) {
      index = 1;
      break;
    }
  }
  // play note from buffer
  if(index != -1) {
    uint16_t note = pNotes[index];
    if(note != mLastNote) {
      // finally play the new note
      Music.noteOn(note, 127);
      mLastNote = note;
    }
  } else {
#if SP_DEBUG
    Serial.println("(PS) -> onClockBeatChange(): No matching index found - which shouldn't happen :(");
#endif
  }
  

  // keep track of total pulse count
  mPulseCount++;
}

void PodSensor::pGoToNote()
{
  //Search for Note
  updateRealPosition();
  updateNoteToFollowIndex();
  getConcreteParent()->getMotor()->moveToPosition(pPositions[pNoteToFollowIndex]+25600, accelPattern[currentTuning],maxSpeedPattern[currentTuning]);
}


void PodSensor::onMotorMessage(uint8_t pMessage, uint16_t pValue) {
#if SP_DEBUG
  Serial.printf("(PS) -> onMotorMessage(): message: %i - value: %i\n", pMessage, pValue);
#endif
  switch(pMessage) {

    case MotorMessages::ACCELERATION_DONE:
      // start buffering phase
      getConcreteParent()->getSensor()->startBuffering();
      break;

      case MotorMessages::TURNS_DONE:
        if (pIsMeister)
        {
          #if SP_DEBUG
          Serial.println("(PS) -> onMotorMessage(): Decelerating Meister");
          #endif
          getConcreteParent()->getMotor()->decelerateToSpeed(50, 0.9);
        }
      break;

      case MotorMessages::DECELERATION_DONE:
        if (pIsMeister)
        {
          updateRealPosition();
          updateMeisterNoteIndex();
        }
        else
        {
          #if SP_DEBUG
          Serial.println("(PS) -> onMotorMessage(): Waiting for pulse ");
          #endif
        }
      break;

      case MotorMessages::TUNING_DONE:
        if (pIsMeister)
        {
          
        }
        else
        {
          #if SP_DEBUG
          Serial.println("(PS) -> onMotorMessage(): Go to Current Note");
          #endif
          pGoToNote();
        }
      break;
  }
}

void PodSensor::updateRealPosition()
{
  long mpos = getConcreteParent()->getMotor()->mMotor.currentPosition();
  long div = mpos/25600;
  pRealPosition = mpos -(div*25600);
  #if SP_DEBUG
  Serial.print("(PS) -> updateRealPosition(): pRealPosition ");
  Serial.println(pRealPosition);
  Serial.printf("(PS) -> updateRealPosition(): pulse: %i\n", mPulseCount);  
  #endif
}


void PodSensor::updateMeisterNoteIndex()
{
  //Faking to be the current Note for Tunning
  bool safecheck = false;
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    if (pNotes[i] >= tuneNotes[0]-tuneRange && pNotes[i] <= tuneNotes[0]+tuneRange )
    {
      pMeisterNoteIndex = i;
      safecheck = true;
    }
  }
  if (!safecheck)
  {
    //Note found, choose a random index to assign the note.
    pMeisterNoteIndex = random(BUFFER_SIZE-1);
    pNotes[pMeisterNoteIndex] = tuneNotes[0];
  }

  #if SP_DEBUG
  Serial.printf("(PS) -> updateMeisterNoteIndex(): meisterNoteIndex %i meisterNote %i",pMeisterNoteIndex,pNotes[pMeisterNoteIndex]);
  #endif
}

void PodSensor::updateNoteToFollowIndex()
{
  //Change the following for the received Meister Note sent by the Meister
  //For now, it is hardcoded to iterate to the notes: A1, A2, A3, A4, A5, A6, A7.

  bool safecheck = false;
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    if (pNotes[i] >= tuneNotes[currentTuning]-tuneRange && pNotes[i] <= tuneNotes[currentTuning]+tuneRange )
    {
      if (i != lastIndex)
      {
        if (abs(pPositions[lastIndex]-pPositions[i]) > 500)
        {
          pNoteToFollowIndex = i;
          safecheck = true;
          lastIndex = pNoteToFollowIndex;
          break;
        }
      }
    }
  }

  if (!safecheck)
  {
    //Note found, choose a random index to assign the note.
    while (safecheck == false)
    {
        pNoteToFollowIndex = random(BUFFER_SIZE-1);
        if (pNoteToFollowIndex != lastIndex)
        {
          if (abs(pPositions[lastIndex]-pPositions[pNoteToFollowIndex]) > 500)
          {
            pNotes[pNoteToFollowIndex] = tuneNotes[currentTuning];
            safecheck = true;
            lastIndex = pNoteToFollowIndex;
          }
        }
      }
  }

  #if SP_DEBUG
  Serial.printf("(PS) -> updateNoteToFollowIndex(): pNoteToFollowIndex %i podNote %i",pNoteToFollowIndex,pNotes[pNoteToFollowIndex] );
  #endif
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
        fadeOutVolume();
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

void PodSensor::fadeOutVolume()
{

}

void PodSensor::fadeInVolume()
{

}
