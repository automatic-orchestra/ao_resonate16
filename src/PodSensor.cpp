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
  // check if is the first Meister (lookup table,)
  // if (midiChannel == meisterOrder[0])
  // {
  //   mIsMeister = true;
  // }
  // else
  // {
  //   mIsMeister = false;
  // }
  // apply jakob's magic crescendo preset
  uint8_t presetArray[] = {
    23, 0, 0, 0, 50, 1, 127, 0, 121, 3, 0, 64, 64, 127, 9, 40, 0, 2, 0, 0, 0, 64, 63, 127, 4, 0, 0, 0, 0, 0, 0, 64, 61, 127, 4, 12, 0, 1, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 127, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 127, 127, 127, 127, 0, 0, 0, 0, 0, 1, 127, 127, 0, 127
  };
  applyPresets(presetArray);

  #if SP_DEBUG
  Serial.printf("(PS) -> constructor(): midi channel is:  %i - meister flag: %i - pulse delay is: %i\n", midiChannel, mIsMeister, mPulseDelay);  
  #endif

  // initialize states
  mStates[0] = new PodState("Introduction",    0, 0,    3200,   0,  450); // first state is not managed via AccelStepper acceleration
  mStates[1] = new PodState("1st Development", 3, 500,  1000,  45, 1050);
  mStates[2] = new PodState("2st Development", 4, 2000, 3000,  57, 1550);
  mStates[3] = new PodState("3st Development", 2, 3500, 5000,  69, 1950);
  mStates[4] = new PodState("4th Development", 5, 5000, 7000,  81, 2250);
  mStates[5] = new PodState("5th Development", 1, 6000, 8500,  93, 2450);
  mStates[6] = new PodState("Finale",          0, 0,    10000,  0, 2550);
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

  // update state - first thing to do before anything else
  updateState();

  // update meister flag
  updateMeister();





  // start motor movement after delay has elapsed
  MotorProxy* motor = getConcreteParent()->getMotor();
  if(mPulseCount >= mPulseDelay && !motor->isActive()) {
    // start acceleration phase
    motor->accelerateToSpeed(3200, 50, 1.25);
    motor->start();
  }

  //Introduction to Development Transition Check

  // if (mPulseCount == pulseTimings[0])
  // {
  //   //Update Current Tuning Tone and Meister Flag
  //   // currentTuning = 0;
  //   // if (midiChannel != meisterOrder[currentTuning])
  //   // {
  //   //   mIsMeister = false;
  //   // }
  //   // else
  //   // {
  //   //   mIsMeister = true;
  //   // }

  //   if (!mIsMeister)
  //   {
  //     #if SP_DEBUG
  //       Serial.printf("(PS) -> onClockBeatChange(): currentTuning: %i ", currentTuning);
  //       Serial.println();
  //     #endif
  //       goToNote();
  //   }
  //   else
  //   {
  //     //Hold the note
  //   }
  // }

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



  
  // //Development part
  
  // if (mPulseCount == pulseTimings[0])
  // {
  //   #if SP_DEBUG
  //   Serial.println();
  //   Serial.println("(PS) -> onClockBeatChange(): change Tuning to 1");
  //   Serial.println();
  //   #endif
  //   currentTuning = 1;
  //   if (mIsMeister && !tuneFlags[1])
  //   {
  //     tuneFlags[1] = true;
  //     goToNote();
  //   }
  // }

  // if (mPulseCount == pulseTimings[1])
  // {
  //   #if SP_DEBUG
  //   Serial.println();
  //   Serial.println("(PS) -> onClockBeatChange(): change Tuning to 2");
  //   Serial.println();
  //   #endif
  //   currentTuning = 2;
  //   if (mIsMeister && !tuneFlags[2])
  //   {
  //     tuneFlags[2] = true;
  //     goToNote();
  //   }
  // }

  // if (mPulseCount == pulseTimings[2])
  // {
  //   #if SP_DEBUG
  //   Serial.println();
  //   Serial.println("(PS) -> onClockBeatChange(): change Tuning to 3");
  //   Serial.println();
  //   #endif
  //   currentTuning = 3;
  //   if (mIsMeister && !tuneFlags[3])
  //   {
  //     tuneFlags[3] = true;
  //     goToNote();
  //   }
  // }

  // if (mPulseCount == pulseTimings[3])
  // {
  //   #if SP_DEBUG
  //   Serial.println();
  //   Serial.println("(PS) -> onClockBeatChange(): change Tuning to 4");
  //   Serial.println();
  //   #endif
  //   currentTuning = 4;
  //   if (mIsMeister && !tuneFlags[4])
  //   {
  //     tuneFlags[4] = true;
  //     goToNote();
  //   }
  // }

  // if (mPulseCount == pulseTimings[4])
  // {
  //   #if SP_DEBUG
  //   Serial.println();
  //   Serial.println("(PS) -> onClockBeatChange(): change Tuning to 5");
  //   Serial.println();
  //   #endif
  //   currentTuning = 5;
  //   if (mIsMeister && !tuneFlags[5])
  //   {
  //     tuneFlags[5] = true;
  //     goToNote();
  //   }
  // }

  // //Final Part

  // if (mPulseCount == pulseTimings[5])
  // {
  //   #if SP_DEBUG
  //   Serial.println();
  //   Serial.println("(PS) -> onClockBeatChange(): change Tuning to 6");
  //   Serial.println();
  //   #endif
  //   currentTuning = 6;
  //   if (mIsMeister && !tuneFlags[6])
  //   {
  //     tuneFlags[6] = true;
  //     goToNote();
  //   }
  // }

  // if (midiChannel != meisterOrder[currentTuning])
  // {
  //   mIsMeister = false;
  // }
  // else
  // {
  //   mIsMeister = true;
  // }

  // increment pulse count
  mPulseCount++;
}


void PodSensor::updateState() {
  if(mStateIndex > STATE_COUNT) {
    return;
  }

  bool newState = false;

  // update the state index
  if(mCurrentState == nullptr) {
    // set initial state index
    mStateIndex = 0;
    // set inital state
    mCurrentState = *(mStates + mStateIndex);
    // set debug flag
    newState = true;
  }

  // check if current state is done nd needs to change
  if(mCurrentState->getDurationInPulses() >= mPulseCount) {
    // increment state index
    mStateIndex++;

    if(mStateIndex < STATE_COUNT) {
      // set new state
      mCurrentState = *(mStates + mStateIndex);
      // set debug flag
      newState = true;
    } else {
      mCurrentState = nullptr;
    }
  }

  if(newState) {
    #if SP_DEBUG
    Serial.print("\n(PS) -> updateState(): current state is: ");
    Serial.println(mCurrentState->getName());
    Serial.println();
    #endif
  }
}


void PodSensor::updateMeister() {
  if (getParent()->getChannel() == mCurrentState->getMeisterID()) {
    mIsMeister = true;
  } else {
    mIsMeister = false;
  }
}


void PodSensor::goToNote(uint8_t pAddRotations)
{
  #if SP_DEBUG
  Serial.printf("(PS) -> goToNote(): current tuning note: %i\n", mCurrentState->getTuningNote());
  #endif

  // get next note and position index
  mNoteToFollowIndex = getNextIndexToFollow();

  // convert relative position to absolute motor postion
  unsigned long relPosition = mPositionsBuffer[mNoteToFollowIndex] + MotorProxy::FULL_REVOLUTION * pAddRotations;
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
        if (mIsMeister)
        {
          #if SP_DEBUG
          Serial.println("(PS) -> onMotorMessage(): Decelerating Meister");
          #endif
          getConcreteParent()->getMotor()->decelerateToSpeed(50, 0.9);
        }
      break;

      case MotorMessages::DECELERATION_DONE:
        if (mIsMeister) {
          updateMeisterNoteIndex();
        } else {
          #if SP_DEBUG
          Serial.println("(PS) -> onMotorMessage(): Waiting for pulse ");
          #endif
        }
        // enable plyaback based on motor movement
        mPlayNotesInSyncToMovement = true;
      break;

      case MotorMessages::TUNING_DONE:

        // if (!mIsMeister)
        // {
        //   if (currentTuning < 6)
        //   {
        //     //THE PROBLEMATIC ONE
        //     goToNote();
        //   }
        //   else
        //   {
        //     if (tuneFlags[6])
        //     {
        //       fadeOutVolume();
        //     }
        //     else
        //     {
        //       // go to last note with three additional turns
        //       goToNote(3);
        //       // set last drone flag
        //       mLastDrone = true;
        //     }
        //   }
        // }
        // tuneFlags[currentTuning] = true;
        // if (tuneFlags[6])
        // {
        //  getConcreteParent()->getMotor()->mMotor.stop();
        // }

      break;
  }
}


void PodSensor::updateMeisterNoteIndex()
{
  // //Faking to be the current Note for Tunning
  // bool safecheck = false;
  // for (int i = 0; i < BUFFER_SIZE; i++)
  // {
  //   if (mNotesBuffer[i] >= tuneNotes[0]-mTuneRange && mNotesBuffer[i] <= tuneNotes[0]+mTuneRange )
  //   {
  //     pMeisterNoteIndex = i;
  //     safecheck = true;
  //   }
  // }
  // if (!safecheck)
  // {
  //   //Note found, choose a random index to assign the note.
  //   pMeisterNoteIndex = random(BUFFER_SIZE-1);
  //   mNotesBuffer[pMeisterNoteIndex] = tuneNotes[0];
  // }

  // #if SP_DEBUG
  // Serial.printf("(PS) -> updateMeisterNoteIndex(): meisterNoteIndex %i meisterNote %i",pMeisterNoteIndex,mNotesBuffer[pMeisterNoteIndex]);
  // #endif
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
      if (mIsMeister)
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
        mNotesBuffer[bufferCounter] = map(pValue,0,1023,24,71);//map(pValue,0,1023,36,95);
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
    // Serial.printf("(PS) -> onClockBeatChange(): Note %i\n",note);
    //TODO add correct velocity
    Music.noteOn(pNote, 127);
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
