/*
 * Written by Adam Phelps, amp@cs.stanford.edu, 2014
 *
 * This library provides routines for safely writing and reading data from
 * EEPROM, which includes object length and CRC protections, as well as only
 * writing the data that doesn't match the existing data in EEPROM in order to
 * maximize the limited write lifetime of the EEPROM.
 */
#ifndef EEPROMUTILS_H
#define EEPROMUTILS_H

#include <Arduino.h>
#include "EEPROM.h"

#define EEPROM_START_BYTE 0xAF
#define EEPROM_WRAPPER_SIZE 3


/* Error result codes */
#define EEPROM_ERROR_END_EXCEEDED -1
#define EEPROM_ERROR_NOT_START    -2
#define EEPROM_ERROR_BAD_LEN      -3
#define EEPROM_CRC_ERROR          -4

/* This returns the actual size of data written to EEPROM by this library */
#define EEPROM_SIZE(x) (EEPROM_WRAPPER_SIZE + x)

/* This returns the size of just the data portion in EEPROM */
#define EEPROM_DATA_SIZE(x) (x - EEPROM_WRAPPER_SIZE)

/*
 * Write data to EEPROM along with CRC, start, and length protections
 */
int EEPROM_safe_write(int location, uint8_t *data, int datalen);

/*
 * Read back data written by EEPROM_safe_write
 */
int EEPROM_safe_read(int location, uint8_t *buff, int bufflen);

/*
 * Check the data at the indicated location to see if it is the beginning
 * of a structure;
 */
boolean EEPROM_check_address(int location);

/*
 * Read all EEPROM entries and write the data to the serial device
 */
void EEPROM_dump(int location);

/*
 * Shift data in EEPROM forward or backware the indicated distance
 */
void EEPROM_shift(int start_address, int distance);

#endif
