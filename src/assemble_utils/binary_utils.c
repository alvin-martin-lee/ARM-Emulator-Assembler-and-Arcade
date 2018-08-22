/** binary_utils.c
 *
 * Implementation of the bitwise/binary-related operations.
 */
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "binary_utils.h"

/**
 * Set the bit of a 32-bit value to 1 at the given position
 */
void setBit(uint32_t *dest, unsigned int pos) {
  assert(dest != NULL && 0 <= pos && pos < 32);
  *dest |= (1U << pos);
}

/**
 * Set the bit of a 32-bit value to 0 at the given position
 */
void unsetBit(uint32_t *dest, unsigned int pos) {
  assert(dest != NULL && 0 <= pos && pos < 32);
  *dest &= ~(1U << pos);
}

/**
 * Write a bit pattern to a set of 32 bits at a particular range.
 *
 * dest: pointer to the 32-bit value to write to
 * pattern: bit pattern to write
 * high, lo: start and end positions of *dest to write the pattern to.
 *
 * Only the (hi - lo + 1) rightmost bits from `pattern` will
 * be written to dest[hi, lo].
 */
void setBitPattern(uint32_t *dest, uint32_t pattern, unsigned int hi, unsigned int lo) {
  assert(dest != NULL && 0 <= lo && lo <= hi && hi <= 31);

  // Select rightmost (hi - lo + 1) bits in pattern,
  // then shift the pattern left to the desired location
  pattern = (pattern & bitMask(hi - lo, 0)) << lo;

  // Set the target range to the pattern
  *dest = (*dest & ~bitMask(hi, lo)) | pattern;
}

/**
 * Specify a 32-bit mask given the starting and ending bit positions
 * (big-endian, i.e. leftmost bit = #31, rightmost = #0)
 * 31 >= hi >= lo >= 0
 *
 * Example: bitmask(9, 4) = 0..01111110000
 */
uint32_t bitMask(unsigned int hi, unsigned int lo) {
  assert(0 <= lo && lo <= hi && hi < 32);
  // make sequence of 32 1-bits
  // >> to get the correct number of 1-bits (b - a + 1)
  // << to get to the correct position (b)
  return (~0U >> (32 - (hi - lo + 1))) << lo;
}

/**
 * Encode 32-bit numeric constant in ARM 12-bit immediate value format if possible
 *
 * 11 - 8: number of rotate-right operations
 *  7 - 0: rotated value
 *
 * Based on https://stackoverflow.com/questions/17763582
 */
uint16_t encodeImmConst(uint32_t value) {
  uint32_t encoding = 0;
  // rotations are even (twice of what is in bits 11 to 8)
  for (int r = 0; r < 32; r += 2) {
    // check if the rotated value fits into 8 bits
    if (!(value & ~0xffU)) {
      setBitPattern(&encoding, r / 2U, 11, 8);
      setBitPattern(&encoding, value, 7, 0);
      return (uint16_t) encoding;
    }
    // perform left-rotate by 2
    value = (value << 2U) | (value >> 30U);
  }
  fprintf(stderr, "Error: value cannot be encoded to ARM immediate value format\n");
  exit(EXIT_FAILURE);
}

/**
 * Convenience function for printing 32-bit binary.
 */
void printBinary(uint32_t binary) {
  uint32_t mask = 1U << 31U;
  while (mask) {
    printf("%d", (binary & mask) != 0U);
    mask >>= 1;
  }
  printf("\n");
}
