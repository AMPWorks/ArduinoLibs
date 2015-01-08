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

// TODO: This needs to be set on a per-board basis
#define EEPROM_MAX_ADDRESS 1024

#define EEPROM_START_BYTE 0xAF
#define EEPROM_WRAPPER_SIZE 3

/* This returns the actual size of data written to EEPROM by this library */
#define EEPROM_SIZE(x) (EEPROM_WRAPPER_SIZE + x)

/*
 * Write data to EEPROM along with CRC, start, and length protections
 */
int EEPROM_safe_write(int location, uint8_t *data, int datalen);

/*
 * Read back data written by EEPROM_safe_write
 */
int EEPROM_safe_read(int location, uint8_t *buff, int bufflen);

/*
 * Shift data in EEPROM forward or backware the indicated distance
 */
void EEPROM_shift(int start_address, int distance);

#endif
