# sdr-wspr -- A WSPR receiver/decoder using libsdr.

[WSPR](https://en.wikipedia.org/wiki/WSPR_(amateur_radio_software)) (Weak Signal Propagation Reporter) is a technique (actually the name of a software) to probe the propagation conditions in various amateur radio bands. Each station participating, sends its call sign, location and output power using a very slow (about 1.5 Baud) but robust digital mode (usually called WSPR). This implementation embeds the [original Fortran code](http://physics.princeton.edu/pulsar/K1JT/wspr.html) into a [libsdr](https://github.com/hmatuschek/libsdr) processing node, allowing to assemble a WSPR receiver for various sources easily.

## License

sdr-wspr -- A WSPR receiver/decoder using libsdr. Copyright (C) 2014 Hannes Matuschek.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see http://www.gnu.org/licenses/

