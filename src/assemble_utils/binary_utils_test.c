/** binary_utils_test.c
 *
 * Sample usage of bitwise operations.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "assemble_utils/binary_utils.h"

void setBitPatternTest(void) {
  uint32_t dest = 3715235103;
  uint32_t pattern = 2863311530;
  unsigned int hi = 21;
  unsigned int lo = 16;
  setBitPattern(&dest, pattern, hi, lo);
  printf("%u\n", dest);
}

void setUnsetBitTest() {
  uint32_t x = 0;
  setBit(&x, 0);
  setBit(&x, 31);
  setBit(&x, 24);
  printBinary(x);
  unsetBit(&x, 0);
  unsetBit(&x, 31);
  unsetBit(&x, 24);
  unsetBit(&x, 16);
  printBinary(x);
}

void bitMaskTest() {
  printBinary(bitMask(31, 0)); // all 1's
  printBinary(bitMask(31, 31));
  printBinary(bitMask(0, 0));
  printBinary(bitMask(23, 7));
}

void encodeImmConstTest() {
  printBinary(encodeImmConst(0xD7));
  printBinary(encodeImmConst(0x3FC00));
  printBinary(encodeImmConst(0xC0000034));
}

void runBinaryUtilsTests() {
  setBitPatternTest();
  setUnsetBitTest();
  bitMaskTest();
  encodeImmConstTest();
}

