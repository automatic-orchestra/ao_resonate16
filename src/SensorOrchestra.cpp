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
#include "CFOSynthesizer.h"
#include "CFOMidi.h"

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
}


void SensorOrchestra::setupMotor(MotorProxy* pMotor) {
    mMotor = pMotor;
}


void SensorOrchestra::setupSensor(SensorProxy* pSensor) {
    mSensor = pSensor;
}


MotorProxy* SensorOrchestra::getMotor() {
    return mMotor;
}


SensorProxy* SensorOrchestra::getSensor() {
    return mSensor;
}


void SensorOrchestra::start() {
    Orchestra::start();

    if(mMotor == nullptr){
        Serial.println("(SO) -> start() - ERROR: call setupMotor() before calling start()");
        return;
    }

    if(mSensor == nullptr){
        Serial.println("(SO) -> start() - ERROR: call setupSensor() before calling start()");
        return;
    }

    // send out MIDI clock start which synchronizes the motors
    if(isKlockMeister()) {
        Serial.println("(SO) -> start()");
        Midi.sendStart();
    }

    // DO NOT CHANGE THE INITAL MOVEMENT HERE!!!
    // Set it in the getMovement() method when passing in the secodn parameter to movement null.
    sendChangeMovement(Movement::MOVEMENT_NULL);
}


void SensorOrchestra::update() {
    Orchestra::update();
    mMotor->update();
    mSensor->update();
}


void SensorOrchestra::onClockStart() {
    Serial.println("(SO) -> onClockStart()");
    // sync motors
    mMotor->setZeroPosition();
    // call super function for default behavior
    Orchestra::onClockStart();
}