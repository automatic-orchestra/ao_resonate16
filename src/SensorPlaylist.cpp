/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 SensorPlaylist.cpp
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

#include "SensorPlaylist.h"
#include "Movement.h"
#include "MovementNull.h"
#include "MovementSensor.h"

#define MOVEMENT_COUNT 1
// IDs for movement con not be 0!!!
#define MOVEMENT_SENSOR 1


SensorPlaylist::SensorPlaylist() : Playlist() {
    populateMovementIDs();
}


SensorPlaylist::~SensorPlaylist() {
}


uint8_t SensorPlaylist::getNumberOfMovements() {
    return MOVEMENT_COUNT;
}


Movement* SensorPlaylist::createMovement(int pMovementID) {
    Serial.printf("(SP) -> createMovement(): with id %i\n", pMovementID);
    Movement* mMovement = 0;
    switch (pMovementID) {
        case Movement::MOVEMENT_NULL:
            mMovement = new MovementNull(parent(), mMovementIDs[0]);
            break;
        case MOVEMENT_SENSOR:
            mMovement = new MovementSensor(parent(), nextMovementID(pMovementID));
            break;
    }
    return mMovement;
}


void SensorPlaylist::populateMovementIDs() {
    mMovementIDs = new int[MOVEMENT_COUNT];
    mMovementIDs[0] = MOVEMENT_SENSOR;
}