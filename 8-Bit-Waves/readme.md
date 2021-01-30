Schematics for my resistor ladder DAC - based digital oscillator for Eurorack.

It uses lookup tables for V/Oct to frequency, and for the sine wavetable. Saw, triangle and square waves are all calculated in the audio interrupt loop.

If I remade this module now, I would use a MCP6000 series op amps for the analogue input buffers, and an MCP4822 or similar DAC for the output.
