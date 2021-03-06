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

#define MIN_SENSOR 650
#define MAX_SENSOR 800
#define MIN_NOTE 33
#define MAX_NOTE 93

#define SP_DEBUG true
#define CHANNEL_DELAY_INCREMENT 15

PodSensor::PodSensor(Orchestra* pParent) : Pod(pParent) {
  // retrieve MIDI channel
  int midiChannel = getParent()->getChannel();

  // calculate delay in pulses
  mPulseDelay = midiChannel * CHANNEL_DELAY_INCREMENT;

  // apply jakob's magic crescendo preset
  uint8_t presetArray[] = {
    23, 0, 0, 0, 50, 1, 127, 0, 121, 3, 0, 64, 64, 127, 9, 40, 0, 2, 0, 0, 0, 64, 63, 127, 4, 0, 0, 0, 0, 0, 0, 64, 61, 127, 4, 12, 0, 1, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 127, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 127, 127, 127, 127, 0, 0, 0, 0, 0, 1, 127, 127, 0, 127
  };
  applyPresets(presetArray);

  #if SP_DEBUG
  Serial.printf("(PS) -> constructor(): midi channel is:  %i - pulse delay is: %i\n", midiChannel, mPulseDelay);  
  #endif

  // initialize states
  mStates[0] = new PodState("Introduction",    -1, 0,    3200,   0,         0, 450); // first state is not managed via AccelStepper acceleration
  mStates[1] = new PodState("1st Development", 3, 500,  1000,  MIN_NOTE,  50, 1050);
  mStates[2] = new PodState("2st Development", 4, 2000, 3000,  45,        40, 1550);
  mStates[3] = new PodState("3st Development", 2, 3500, 5000,  57,        30, 1950);
  mStates[4] = new PodState("4th Development", 5, 5000, 7000,  69,        20, 2250);
  mStates[5] = new PodState("5th Development", 1, 6000, 8500,  81,        10, 2450);
  mStates[6] = new PodState("Finale",          -1, 0,    10000, MAX_NOTE, 100, 2550);
  // mStates[1] = new PodState("Finale",          -1, 0,    10000, MAX_NOTE, 100, 700);
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
  #if SP_DEBUG
  if(!getConcreteParent()->getMotor()->isActive()) {
    Serial.printf("(PS) -> onClockBeatChange(): pulse count is: %i\n", mPulseCount);  
  }
  #endif

  if(mNoteVelocity == 0) {
    getConcreteParent()->getMotor()->mMotor.stop();
    Serial.println("APPLAUSE!!!!");
  }

  // update state - first thing to do before anything else
  updateState();

  // start initial motor movement
  MotorProxy* motor = getConcreteParent()->getMotor();
  if(mPulseDelay >= mPulseCount && !motor->isActive()) {
    // start acceleration phase
    motor->accelerateToSpeed(3200, 50, 1.25);
    motor->start();
  }

  //
  if(mLastDrone && mNoteDelay > -1) {
    if(mNoteDelay == 0) {
      mNoteVelocity--;
      playNote(mCurrentState->getTuningNote());
    }
    mNoteDelay--;
  } else if(!mMeisterStates.currentlyActive && mNoteDelay > -1) {
    if(mNoteDelay == 0) {
      goToNote();
    }
    mNoteDelay--;
  }

  // ---------------------------------------
  //  play the note based on motor position
  // ---------------------------------------
  // calculate relative postion of values in buffer
  unsigned long pos = getConcreteParent()->getMotor()->mMotor.currentPosition();
  unsigned long firstBufferPos = mPositionsBuffer[0];
  unsigned long relativePos = firstBufferPos + ((pos - firstBufferPos) % MotorProxy::FULL_REVOLUTION);
  // find matching index of position in buffer
  int index = -1;
  for(int i = 0; i < BUFFER_SIZE; i++) {
    // following index
    int j = (i + 1 == BUFFER_SIZE) ? 0 : i + 1;
    // check buffer values to determine index
    if(relativePos >= mPositionsBuffer[i] && relativePos < mPositionsBuffer[j]) {
      index = i;
      break;
    }
  }
  // play note from buffer
  if(mPlayNotesInSyncToMovement) {
    if(index != -1) {
      uint16_t note = mNotesBuffer[index];
      playNote(note);
    } else {
      #if SP_DEBUG
      Serial.println("(PS) -> onClockBeatChange(): No matching index found - which shouldn't happen :(");
      #endif
    }
  }

  // increment pulse count
  mPulseCount++;
}


void PodSensor::updateState() {
  if(mStateIndex >= STATE_COUNT) {
    return;
  }

  bool newState = false;

  // update the state index
  if(mCurrentState == nullptr) {
    // set initial state index
    mStateIndex = 0;
    // set inital state
    mCurrentState = *(mStates);
    // set flag
    newState = true;
  }

  // check if current state is done nd needs to change
  if(mPulseCount > mCurrentState->getDurationInPulses()) {
    // increment state index
    mStateIndex++;

    if(mStateIndex < STATE_COUNT) {
      // set new state
      mCurrentState = *(mStates + mStateIndex);
      // set flag
      newState = true;
    } else {
      mCurrentState = nullptr;
    }
  }

  if(newState) {
    // update meister states
    setMeisterStates();

    //
    if(mMeisterStates.wasActive && !mMeisterStates.currentlyActive) {
      goToNote();
    }

    if(mMeisterStates.wasActive && isLastState()) {
      goToNote(3); 
      mLastDrone = true;
    }
    // if(isLastState()) {
    //   Serial.println("DO IT!");
    //   goToNote(); 
    //   mLastDrone = true;
    // }

    #if SP_DEBUG
    Serial.print("\n(PS) -> updateState(): current state is: ");
    Serial.println(mCurrentState->getName());
    Serial.println("--------------------------------------------------------------------");
    Serial.printf("meister currently active: %i\n", mMeisterStates.currentlyActive);
    Serial.printf("meister was active: %i\n", mMeisterStates.wasActive);
    Serial.printf("meister is at note: %i\n", mMeisterStates.isAtNote);
    Serial.println();
    #endif
  }
}


void PodSensor::setMeisterStates() {
  // store previous activity
  mMeisterStates.wasActive = mMeisterStates.currentlyActive; 
  // set new activity
  mMeisterStates.currentlyActive = getParent()->getChannel() == mCurrentState->getMeisterID(); 
  // reset reached note
  mMeisterStates.isAtNote = false;
}


void PodSensor::goToNote(uint8_t pAddRotations)
{
  #if SP_DEBUG
  Serial.printf("(PS) -> goToNote(): current tuning note: %i\n", mCurrentState->getTuningNote());
  #endif

  // get next note and position index
  uint16_t nextIndex = getNextIndexToFollow();

  // convert relative position to absolute motor postion
  unsigned long relPosition = mPositionsBuffer[nextIndex] + MotorProxy::FULL_REVOLUTION * pAddRotations;
  unsigned long absPosition = getConcreteParent()->getMotor()->relativeToAbsolutePosition(relPosition);

  // move motor to new position
  getConcreteParent()->getMotor()->moveToPosition(
    absPosition,
    mCurrentState->getAcceleration(),
    mCurrentState->getMaxSpeed()
  );  
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
        if (mMeisterStates.currentlyActive)
        {
          #if SP_DEBUG
          Serial.println("(PS) -> onMotorMessage(): Decelerating Meister");
          #endif
          getConcreteParent()->getMotor()->decelerateToSpeed(50, 0.9);
        }
      break;

      case MotorMessages::DECELERATION_DONE:
        if (mMeisterStates.currentlyActive) {
          // updateMeisterNoteIndex();
        } else {
          #if SP_DEBUG
          Serial.println("(PS) -> onMotorMessage(): Waiting for pulse ");
          #endif
        }
        // enable plyaback based on motor movement
        mPlayNotesInSyncToMovement = true;
      break;

      case MotorMessages::TUNING_DONE:
        if(mLastDrone) {
          mNoteDelay = mCurrentState->getNoteDelay();
        } else {
          onTuningDone();
        }

      break;
  }
}


void PodSensor::onTuningDone() {
  if(isLastState()) {
    Serial.println("LAST DRONE");
    goToNote(3);
    mLastDrone = true;
  } else {
    if(mMeisterStates.currentlyActive) {
      if(!mMeisterStates.isAtNote) {
        goToNote();  
        mMeisterStates.isAtNote = true;
      }
    } else {
      mNoteDelay = mCurrentState->getNoteDelay();
    }
  }
}


bool PodSensor::isLastState() {
  return mStateIndex == STATE_COUNT - 1;
}


uint16_t PodSensor::getNextIndexToFollow() {
  // create an array to store possible new positions
  const uint8_t MAX_NEW_POSITIONS = 10;
  uint16_t newPositions[MAX_NEW_POSITIONS] = {};
  uint8_t newPositionCounter = 0;
  // start with random index in buffer
  int randomIndex = random(BUFFER_SIZE);
  uint16_t minPositionDistance = 500;
  // loop over all buffer entires and find matching notes
  for (int i = randomIndex; i < randomIndex + BUFFER_SIZE; i++) {
    // index wrapping to go over the whole buffer
    if(i >= BUFFER_SIZE) {
      i -= BUFFER_SIZE;
    }
    uint8_t tuneNote = mCurrentState->getTuningNote();
    // check new possible note is within the tuning range
    if(mNotesBuffer[i] >= tuneNote-mTuneRange && mNotesBuffer[i] <= tuneNote+mTuneRange) {
      // exclude current index and check if possible new position has a nice distance
      if (i != mLastIndex && abs(mPositionsBuffer[i] - mPositionsBuffer[mLastIndex]) > minPositionDistance) {
        // capture index of possible new position
        newPositions[newPositionCounter] = i;
        newPositionCounter++;
        // check if there are enough results already
        if(newPositionCounter >= MAX_NEW_POSITIONS) {
          break;
        }
      }
    }
  }
  // return new index for note and motor position
  if(newPositionCounter == 0) {
    // return the last index if no new index was found
    #if SP_DEBUG
    Serial.println("(PS) -> getNextIndexToFollow(): no new index was found. :(");
    #endif
    return mLastIndex;
  } else {
    // pick a random result out of found indecies
    uint16_t newPos = newPositions[random(MAX_NEW_POSITIONS)];
    Serial.printf("(PS) -> getNextIndexToFollow(): new index is %i:\n", newPos);
    return newPos;
  } 
}


void PodSensor::onSensorMessage(uint8_t pMessage, uint16_t pValue) {
  #if 0
    Serial.printf("(PS) -> onSensorMessage(): message: %i - value: %i\n", pMessage, pValue);
  #endif
  switch(pMessage) {
    case SensorMessages::BUFFER_FULL:
        //Check if it is the Meister to assemble constant turns
      if (mMeisterStates.currentlyActive)
      {
        //Is Meister, run more
        getConcreteParent()->getMotor()->turnAtSpeed(3200,1);
      }
      else
      {
        fadeOutVolume();
        getConcreteParent()->getMotor()->decelerateToSpeed(50, 0.9);
      }
      
      break;

      case SensorMessages::SENSOR_READING:
        mNotesBuffer[bufferCounter] = constrain(map(pValue, MIN_SENSOR, MAX_SENSOR, MIN_NOTE, MAX_NOTE), MIN_NOTE, MAX_NOTE);
        mPositionsBuffer[bufferCounter] = getConcreteParent()->getMotor()->mMotor.currentPosition();
        #if 0
          Serial.print("(PS) -> onSensorMessage(): index: ");
          Serial.print(bufferCounter);
          Serial.print(" pNote: ");
          Serial.print(mNotesBuffer[bufferCounter]);
          Serial.print(" mPositionsBuffer: ");
          Serial.println(mPositionsBuffer[bufferCounter]);
        #endif
        playNote(mNotesBuffer[bufferCounter]);
        bufferCounter++;
      break;
  }
}

void PodSensor::playNote(uint16_t pNote) {
  if(pNote != mLastNote) {
    // finally play the new note
    Serial.printf("(PS) -> playNote(): Note %i - Velocity: %i\n", pNote, mNoteVelocity);
    Music.noteOn(pNote, mNoteVelocity);
    mLastNote = pNote;
  }
}

void PodSensor::fadeOutVolume()
{
  #if SP_DEBUG
  Serial.println("(PS) -> PodSensor(): Fading Out");
  #endif
}

void PodSensor::fadeInVolume()
{
  #if SP_DEBUG
  Serial.println("(PS) -> PodSensor(): Fading In");
  #endif
}
