# sdr-wspr -- A WSPR receiver/decoder using libsdr.

[https://en.wikipedia.org/wiki/WSPR_(amateur_radio_software)](WSPR) (Weak Signal Propagation Reporter) is a technique (actually the name of a software) to probe the propagation conditions in various amateur radio bands. Each station participating, sends its call sign, location and output power using a very slow (about 1.5 Baud) but robust digital mode (usually called WSPR). This implementation embeds the [http://physics.princeton.edu/pulsar/K1JT/wspr.html](original Fortran code) into a [https://github.com/hmatuschek/libsdr](libsdr) processing node, allowing to assemble a WSPR receiver for various sources easily.


## WSPR and RTL2832 Dongles

First of all, TV USB dongles using the RTL2832 chip and E4000 or similar tuners are limited on frequencies above 60MHz (depending on the tuner, see http://rtlsdr.org for details). For the reception of short-wave radio, especially WSPR signals a [upconverter](http://blog.kf7lze.net/2012/09/14/round-up-of-rtlsdr-upconverter-choices) is needed.

The second problem with these dongles is their poor frequency stability. Please note, that a WPRS signal consists of 4 frequencies only about 1.5Hz apart. Hence a relatively high frequency stability is required. Although the WSPR algorithm by Joe Taylor can handle (small) linear frequency drifts of the signal during the reception, any large drift or non-linear change in frequency will render the decoding impossible.

## License

sdr-wspr -- A WSPR receiver/decoder using libsdr. Copyright (C) 2014 Hannes Matuschek.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see http://www.gnu.org/licenses/

