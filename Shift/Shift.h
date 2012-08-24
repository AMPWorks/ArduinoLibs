/*
 * Control 74HC595 shift registers
 */

#ifndef SHIFT_H
#define SHIFT_H

class Shift 
{
  public:
  Shift(byte clock, byte latch, byte data, byte registers);
  void SetBit(byte bit, boolean on);
  void Write();

  private:
  void sendLatch(void);
  
  byte _clock_pin, _latch_pin, _data_pin, _registers;
  byte _latch_value;
  byte _data[4]; // XXX: Should be generalized
};

#endif
