Meter is a MIDI sequencer designed as a Eurorack module.
It can also run off USB power via the plug on the Nano - in which case the 7805 regulator and Eurorack power header can be omitted.

The schematic MIDI plugs are designed to use MIDI to TRS cables. It could be adapted to use standard MIDI connectors instead.

Note on licensing for dependency: https://github.com/olikraus/u8glib
The U8glib code (http://code.google.com/p/u8glib/) is licensed under the terms of 
the new-bsd license (two-clause bsd license).
See also: http://www.opensource.org/licenses/bsd-license.php

This library code is not included within Meter.ino, but you will need to add it to your Arduino IDE before compiling Meter.ino.
