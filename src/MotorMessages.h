#ifndef MOTORMESSAGES_H
#define MOTORMESSAGES_H

#include <Arduino.h>

class MotorMessages
{
public:
  static const uint8_t ACCELERATION_DONE = 1;
  ~MotorMessages();
private:
  MotorMessages();
};


#endif //MOTORMESSAGES_H