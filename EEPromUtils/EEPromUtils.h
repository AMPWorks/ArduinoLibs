#ifndef EEPROMUTILS_H
#define EEPROMUTILS_H

#include <Arduino.h>
#include "EEPROM.h"

#define EEPROM_MAX_ADDRESS 1024

#define EEPROM_START_BYTE 0xAF
#define EEPROM_WRAPPER_SIZE 3

int EEPROM_safe_write(int location, uint8_t *data, int datalen);

int EEPROM_safe_read(int location, uint8_t *buff, int bufflen);

#endif
