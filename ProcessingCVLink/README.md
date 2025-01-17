A project to output 8 channels of 12 bit CV from Processing at about 2.8kHz

The code is rough at the moment. I'm just archiving the first version I've managed to tune up to run smoothly.

The display on the Arduino is not necessary - I just included it because it's useful for debugging. So the Arduino sketch could be simplified a bit.

I haven't drawn out the circuit schematic yet. It uses 4 x MCP4822 DACs connected to SPI1 of the Pico, and 2 x MCP6004 op-amps to scale the outputs to 5V, which matches the musical scales in the Processing sketch to 1V/Oct output. A simple non inverting op amp config with fixed resistors sounds 'OK' but I plan to add trimpots to scale the output voltages accurately. Currently the op amps have 39kOhm resistors from the inverting inputs to GND, and 56kOhm resistors from the inverting inputs to the outputs.
