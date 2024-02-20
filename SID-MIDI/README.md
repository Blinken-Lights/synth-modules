Schematic and Arduino code to integrate a SID chip with MIDI notes and CC commands for all parameters.

The schematic is intended for Eurorack, so it uses a ±12V dual supply, and boosts the SID audio output to just over ±5V, Eurorack standard levels. With some modification, this project could be used as the basis for controlling a SID in whatever context you need.

The physical controls (3 knobs and a switch) are basic, because the SID takes up a lot of pins on the Arduino, which doesn't leave many pins available for controls. But you could build a much larger controller and send data to the Arduino via MIDI, or by changing the code, you could increase the serial speed to 250000 baud and send control data even faster.
