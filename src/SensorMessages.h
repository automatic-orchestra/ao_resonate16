#ifndef SENSORMESSAGES_H
#define SENSORMESSAGES_H

#include <Arduino.h>


class SensorMessages
{
public:
  // start messages at 100 since motor messages start at 1
  static const uint8_t BUFFER_FULL = 100;
  static const uint8_t SENSOR_READING = 101;
  ~SensorMessages();
private:
  SensorMessages();  
};

#endif //SENSORMESSAGES_H