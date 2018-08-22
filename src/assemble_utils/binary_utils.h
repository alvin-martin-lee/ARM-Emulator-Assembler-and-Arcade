/** binary_utils.h
 *
 * This file contains prototypes for bit-manipulation functions.
 *
 * The byte-ordering is big-endian, i.e. the leftmost (31st) bit is the
 * greatest, the rightmost (0th) is smallest, to be consistent with the spec.
 *
 */
#ifndef BINARY_UTILS_H
#define BINARY_UTILS_H

#include <stdint.h>

void setBit(uint32_t *dest, unsigned int pos);

void unsetBit(uint32_t *dest, unsigned int pos);

void setBitPattern(uint32_t *dest, uint32_t pattern, unsigned int hi, unsigned int lo);

uint32_t bitMask(unsigned int hi, unsigned int lo);

uint16_t encodeImmConst(uint32_t value);

void printBinary(uint32_t binary);

#endif
