/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 SensorOrchestra.cpp
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

#include "SensorOrchestra.h"
#include "Arrangement.h"
#include "MacAddress.h"
#include "CFOMidi.h"
#include "Movement.h"
#include "MovementSensor.h"

#define MAC_ADDRESS_COUNT 7


SensorOrchestra::SensorOrchestra() : Orchestra() {
    // create array of mac addresses
    String macAddresses[] = {
        #include "inc/MacAddresses.inc"
    };
    // initialize arrangement with mac addresses
    Arrangement::getInstance().init(macAddresses, MAC_ADDRESS_COUNT);
    // set midi channel and klock meister based on mac address
    setupDeviceParameters(MacAddress::get());
}


SensorOrchestra::~SensorOrchestra() {
    delete mMotor;
    mMotor = nullptr;
    delete mSensor;
    mSensor = nullptr;
    delete mClock;
    mClock = nullptr;
}


void SensorOrchestra::setMotor(MotorProxy* pMotor) {
    mMotor = pMotor;
}


void SensorOrchestra::setSensor(SensorProxy* pSensor) {
    mSensor = pSensor;
}

void SensorOrchestra::setClock(PulseClock* pClock) {
    mClock = pClock;
}


MotorProxy* SensorOrchestra::getMotor() {
    return mMotor;
}


SensorProxy* SensorOrchestra::getSensor() {
    return mSensor;
}


PulseClock* SensorOrchestra::getClock() {
    return mClock;
}


void SensorOrchestra::start() {
    Orchestra::start();

    if(mMotor == nullptr){
        Serial.println("(SO) -> start() - ERROR: call setMotor() before calling start()");
        return;
    }

    if(mSensor == nullptr){
        Serial.println("(SO) -> start() - ERROR: call setSensor() before calling start()");
        return;
    }

    if(mClock == nullptr){
        Serial.println("(SO) -> start() - ERROR: call setClock() before calling start()");
        return;
    }

    // send out MIDI clock start which synchronizes the motors
    if(isKlockMeister()) {
        Serial.println("(SO) -> start()");
        Midi.sendStart();
    }
}


void SensorOrchestra::update() {
    Orchestra::update();
    mMotor->update();
    mSensor->update();
    if(mClock->update()){
        // Serial.printf("(SO) -> update(): clock tick: %i\n", mClock->getCount());
        if(mCurrentMovement){
            mCurrentMovement->onClockBeatChange(mClock->getCount());
        }
    }
}

MovementSensor* SensorOrchestra::getCurrentMovement() {
    // upcast from base class Movement to concrete class MovementSensor
    // http://www.cplusplus.com/doc/tutorial/typecasting/
    return static_cast<MovementSensor*>(mCurrentMovement);
}


void SensorOrchestra::onClockStart() {
    Serial.println("(SO) -> onClockStart()");
    // sync motors
    mMotor->setZeroPosition();
    // call super function for default behavior
    Orchestra::onClockStart();
    // pass on MIDI start message
    if(!isKlockMeister()){
        Midi.sendStart();
    } else {
        startFirstMovement();
    }
}



void SensorOrchestra::changeMovement(int pMovementID) {
    mClock->reset();
    Orchestra::changeMovement(pMovementID);
}


void SensorOrchestra::onMotorMessage(uint8_t pMessage, uint16_t pValue) {
    if(mCurrentMovement) {
        getCurrentMovement()->onMotorMessage(pMessage, pValue);
    }
}


void SensorOrchestra::onSensorMessage(uint8_t pMessage, uint16_t pValue) {
    if(mCurrentMovement) {
        getCurrentMovement()->onSensorMessage(pMessage, pValue);
    }   
}


void SensorOrchestra::startFirstMovement() {
    // DO NOT CHANGE THE INITAL MOVEMENT HERE!!!
    // Set it in the getMovement() method when passing in the secodn parameter to movement null.
    sendChangeMovement(Movement::MOVEMENT_NULL);
}