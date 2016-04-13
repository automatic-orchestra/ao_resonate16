# Mapping sensor readings to notes

## Readings of the arm's circumference

* we read in 1600 sensor values
* this means a 0.225 degree step distance
* Each degree step results a distance of 3.22 mm on the arc
* The calculated radius of the is 820 mm

[Calculator for the arc length based on degrees](https://www.hilfreiche-tools.de/mathematik/kreisbogen-berechnen.html)

## Translation to motor steps

* The motor has 25600 steps for a whole circle (360 degree)
* The step value is calculated like this: 200 (motor steps) * 32 (microsteps) * 4 (magic number)
* To measure 1600 values over 25600 steps results in 16 steps between each sensor reading

## Translating motor steps to duration

* The motor runs at a constant speed of 1600
* To fully go around the arc it will take 16 seconds (25600/1600)
* Calculcation for time between each step: 16 s = 16000 ms / 1600 = 10 ms => 10000 microseconds

## Mapping readings to notes

* Possible ideal note range: C3 to B7 – MIDI note values 36 to 95. (5 Octaves)

[Table of MIDI note values to musical notes](http://www.tonalsoft.com/pub/news/pitch-bend.aspx)