EEPromUtils
===========

This library provides functions for more safely writing and reading from
EEPROM than the default Arduino EEPROM library.

When writing data to EEPROM it also adds a start byte, records the length
being written, and adds a CRC to the end of the data.  Additionally it only
writes a byte if the value to be written differs from what is present.