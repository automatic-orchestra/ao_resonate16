#ifndef MOTORMESSAGES_H
#define MOTORMESSAGES_H

#include <Arduino.h>

class MotorMessages
{
public:
  static const uint8_t ACCELERATION_DONE = 1;
  static const uint8_t DECELERATION_DONE = 2;
  static const uint8_t TURNS_DONE = 3;
  static const uint8_t TUNING_DONE = 4;
  ~MotorMessages();
private:
  MotorMessages();
};


#endif //MOTORMESSAGES_H