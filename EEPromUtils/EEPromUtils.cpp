#include <Arduino.h>
#include "EEPROM.h"

//#define DEBUG_LEVEL DEBUG_HIGH
#include "Debug.h"

#include "EEPromUtils.h"

/*
 * FROM:
 *   - http://www.barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code
 * The width of the CRC calculation and result.
 * Modify the typedef for a 16 or 32-bit CRC standard.
 */
typedef uint8_t crc_t;

#define POLYNOMIAL 0xD8  /* 11011 followed by 0's */
#define WIDTH  (8 * sizeof(crc_t))
#define TOPBIT (1 << (WIDTH - 1))

crc_t
EEPROM_crc(uint8_t const data[], int datalen)
{
    crc_t  remainder = 0;

    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (int byte = 0; byte < datalen; ++byte)
    {
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= (data[byte] << (WIDTH - 8));

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (uint8_t bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT) {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            } else {
                remainder = (remainder << 1);
            }
        }
    }

    /*
     * The final remainder is the CRC result.
     */
    return (remainder);
}

/*
 * Write a value only after checking that it is a change
 */
boolean EEPROM_check_write(int location, uint8_t value) {
  uint8_t current = EEPROM.read(location);
  if (current != value) {
    EEPROM.write(location, value);
    return true;
  } else {
    return false;
  }
}

/*
 * Write a value to EEPROM, including a start byte and CRC byte.
 * The data written will be:
 *   1 Byte: Start
 *   1 Byte: Datalen
 *   N Byte: data
 *   1 Byte: CRC
 * Returns the address of the byte after the last one written
 */
int EEPROM_safe_write(int location, uint8_t *data, int datalen) {
  if (location + datalen + EEPROM_WRAPPER_SIZE >= EEPROM_MAX_ADDRESS) {
    DEBUG_ERR("EEPROM_safe_write: data exceeds max address");
    return -1;
  }
  
  crc_t crc = EEPROM_crc(data, datalen);

  int wrote_count = 0;

  wrote_count += EEPROM_check_write(location++, EEPROM_START_BYTE);
  wrote_count += EEPROM_check_write(location++, datalen);
  for (uint8_t i = 0; i < datalen; i++) {
    wrote_count += EEPROM_check_write(location + i, *data);
    data++;
  }
  location += datalen;
  wrote_count += EEPROM_check_write(location++, crc);

  DEBUG_VALUE(DEBUG_HIGH, "EEPROM_safe_write: addr=", start);
  DEBUG_VALUE(DEBUG_HIGH, " data=", datalen);
  DEBUG_VALUE(DEBUG_HIGH, " all=", location - start);
  DEBUG_VALUE(DEBUG_HIGH, " ret=", location);
  DEBUG_VALUELN(DEBUG_HIGH, " actual=", wrote_count);

  return location;
}

/*
 * Read data written by EEPROM_safe_write, returning error if anything
 * doesn't line up.
 *
 * Returns: address at the end of the read data
 */
int EEPROM_safe_read(int location, uint8_t *buff, int bufflen) 
{
  uint8_t val;

  DEBUG_VALUE(DEBUG_HIGH, "EEPROM_safe_read: addr=", location);

  if (location + EEPROM_WRAPPER_SIZE >= EEPROM_MAX_ADDRESS) {
    DEBUG_ERR("EEPROM_safe_read: location exceeds max address");
    return -1;
  }

  val = EEPROM.read(location++);
  if (val != EEPROM_START_BYTE) {
    DEBUG_ERR("EEPROM_safe_read: first byte not START");
    return -2;
  }

  uint8_t datalen = EEPROM.read(location++);
  if (datalen > bufflen) {
    DEBUG_ERR("EEPROM_safe_read: bufflen less than datalen");
    return -3;
  }
  if (location + datalen + EEPROM_WRAPPER_SIZE >= EEPROM_MAX_ADDRESS) {
    DEBUG_ERR("EEPROM_safe_read: data exceeds max address");
    return -4;
  }

  DEBUG_PRINT(DEBUG_HIGH, " data=");
  for (uint8_t i = 0; i < datalen; i++) {
    buff[i] = EEPROM.read(location++);
    DEBUG_HEXVAL(DEBUG_HIGH, " ", buff[i]);
  }

  crc_t crc = EEPROM.read(location++);
  if (crc != EEPROM_crc(buff, datalen)) {
    DEBUG_ERR("EEPROM_safe_read: CRC didn't match");
    return -5;
  }

  DEBUG_VALUELN(DEBUG_HIGH, " ret=", location);

  return location;
}

/*
 *  Shift all data in EEPROM starting at 'start_address', 'distance' bytes
 */
int EEPROM_shift(int start_address, int distance) {
  // E2END is the last EEPROM address, for instance 1023 on the ATMega328 */

  DEBUG_VALUE(DEBUG_LOW, "EEPROM_shift: start=", start_address);
  DEBUG_VALUE(DEBUG_LOW, " distance=", distance);
  DEBUG_VALUE(DEBUG_LOW, " E2END=", E2END);

  if (abs(distance) >= E2END) DEBUG_ERR_STATE(1);

  if (distance > 0) {
    for (int current = E2END - distance; current >= start_address; current--) {
      int write_address = current + distance;
      byte val = EEPROM.read(current);
      EEPROM.write(write_address, val);
    }
  } else if (distance < 0) {
    for (int current = start_address; current <= E2END; current++) {
      int write_address = current + distance;
      byte val = EEPROM.read(current);
      EEPROM.write(write_address, val);
    }
  }
}
