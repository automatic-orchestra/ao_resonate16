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

#define SO_DEBUG true
#define MAC_ADDRESS_COUNT 7


SensorOrchestra::SensorOrchestra() {
    // create array of mac addresses
    String macAddresses[] = {
        #include "inc/MacAddresses.inc"
    };
    // initialize arrangement with mac addresses
    Arrangement::getInstance().init(macAddresses, MAC_ADDRESS_COUNT);
    // set midi channel and klock meister based on mac address
    setupDeviceParameters(MacAddress::get());
    // print device parameters
#if SO_DEBUG
    Serial.println("Sensor Orchestra (SO)");
    Serial.println("-------------------------------------------");
    Serial.print("(SO) Mac Address: ");
    Serial.println(mMacAddress);
    Serial.printf("(SO) MIDI Channel: %i\n", mMidiChannel);
    Serial.printf("(SO) Klock Meister: %i\n", mKlockMeister);
    Serial.println("-------------------------------------------");
#endif
}


SensorOrchestra::~SensorOrchestra(){
}