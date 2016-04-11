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
#include "CFOMidi.h"
#include "CFOSynthesizer.h"

#define SENSOR_PIN A0
#define MOTOR_DIR_PIN 3
#define MOTOR_STEP_PIN 4
#define LED_PIN 13 // original value: 5
#define INIT_DELAY 1000

SensorProxy* mSensor;
MotorProxy* mMotor;
SensorOrchestra* mOrchestra;

// Forward declaration of callback functions
void onMidiNoteOn(byte channel, byte note, byte velocity);
void onMidiNoteOff(byte channel, byte note, byte velocity);
void onMidiControlChange(byte channel, byte control, byte value);
void onMidiClockTick();
void onMidiClockStart();


void setup() {
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

  // Initialize synth engine
  Music.init();
  Music.enableEnvelope1();
  Music.enableEnvelope2();
  Music.getPreset(13);

  // Initializy orchestra MIDI interface
  Midi.init();

  // Wait for Music and MIDI (incl. Serial connection)
  // to be initiatlized
  if(mOrchestra->isKlockMeister()) {
    delay(INIT_DELAY+1000);
  } else {
    delay(INIT_DELAY);
  }

  // print device configuration
  mOrchestra->printConfiguration();

  // register USB MIDI interface callbacks
  usbMIDI.setHandleNoteOff(onUsbNoteOn);
  usbMIDI.setHandleNoteOn(onUsbNoteOff);
  usbMIDI.setHandleControlChange(onUsbControlChange);
  usbMIDI.setHandleRealTimeSystem(onRealTimeSystem);
  
  // configure Orchestra MIDI interface
  Midi.setChannel(mOrchestra->getChannel());
  Midi.setMidiIn(true);
  Midi.setMidiThru(false);
  Midi.setMidiOut(true);
  Midi.setMidiClockIn(true);
  Midi.setMidiClockThru(false);
  Midi.setMidiClockOut(true);

  // register Orchestra MIDI callbacks
  Midi.setNoteOnCallback(&onMidiNoteOn);
  Midi.setNoteOffCallback(&onMidiNoteOff);
  Midi.setControllerCallback(&onMidiControlChange);
  Midi.setClockTickCallback(&onMidiClockTick);
  Midi.setClockStartCallback(&onMidiClockStart);
}


void loop() {
  mSensor->update();
}


void onSensorValue(uint16_t pValue) {
  //Serial.printf("sensor value: %i\n", pValue);
}


// ------------------------------
//  USB MIDI Interface callbacks
// ------------------------------


void onUsbNoteOn(byte channel, byte note, byte velocity) {
  // decrement channel by 1 due to non zero based counting in USB MIDI interface
  channel--;
}


void onUsbNoteOff(byte channel, byte note, byte velocity) {
  // decrement channel by 1 due to non zero based counting in USB MIDI interface
  channel--;
}


void onUsbControlChange(byte channel, byte control, byte value) {
  // decrement channel by 1 due to non zero based counting in USB MIDI interface
  channel--;
}


void onRealTimeSystem(byte realtimebyte) {
  Midi.midiRealTimeHandler(realtimebyte);
}


// ------------------------------------
//  Orchestra MIDI Interface callbacks
// ------------------------------------


void onMidiNoteOn(byte channel, byte note, byte velocity) {
  
}


void onMidiNoteOff(byte channel, byte note, byte velocity) {
  
}


void onMidiControlChange(byte channel, byte control, byte value) {
  
}


void onMidiClockTick() {

}


void onMidiClockStart() {

}


