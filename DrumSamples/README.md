DRUM SAMPLE EURORACK MODULE

Code and schematic for an 8-bit lo-fi drum sample player for Eurorack. The samples are triggered by digital inputs, and the audio is mixed down to a single audio output. It can play several samples in parallel. The audio output is around Eurorack levels of 10Vpp.

Most of the code was written by Jan Ostman and github user https://github.com/hexagon5un : https://github.com/hexagon5un/jan_ostmans_synths

I just made a small modification to work with simple digital trigger inputs. I drew the schematic, with protection against Eurorack signal levels on the digital inputs, and a basic filter for PWM, AC balancing and x2 amplification to go from 0-5V arduino PWM to 10Vpp AC for Eurorack audio.
