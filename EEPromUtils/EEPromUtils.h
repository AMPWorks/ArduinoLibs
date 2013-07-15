#ifndef EEPROMUTILS_H
#define EEPROMUTILS_H

#include <Arduino.h>
#include "EEPROM.h"

#define EEPROM_START_BYTE 0xAF

int EEPROM_safe_write(int location, uint8_t *data, int datalen);

int EEPROM_safe_read(int location, uint8_t *buff, int bufflen);

#endif
