A perfboard/breadboard style PCB, to be used as a basis for any DIY module. Including a few common circuit features to speed up DIY'ing.
It's designed to connect to the rear of a module panel at 90 degrees, via the potentiometer and jack socket nuts.

- 16mm potentiometer footprints (cheapest type of potentiometer)
-- 90 degree potentiometer brackets to take the strain off the solder joints
- 3.5mm stereo jack socket footprints
- 10 pin Eurorack power connector
- 7085 voltage regulator
- voltage reference IC
- Arduino Nano footprint

The Arduino, voltage reference and power ICs are all connected via the PCB.
The PCB should also be useful for 100% analogue modules too.


**UPDATE 18th May**

- Uploaded Arduino board without 3V3 supply, with option to break out SCK and MOSI to the central rails
- Uploaded STM32 blue pill based board
- Created MIDI daughter board, fitting behind 2 jacks that can be screwed into a front panel
- Uploaded .zip files containing gerber and excellon files, that are ready for CAM
- All this is still **UNTESTED!**
