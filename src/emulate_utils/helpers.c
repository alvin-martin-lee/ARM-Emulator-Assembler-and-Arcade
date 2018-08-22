#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "helpers.h"
#include "decode.h"

/* swap_endian
 *
 * Converts a little-endian input into big-endian format and vice versa. This is mainly used d
 *
 * Parameter
 * val - a uint32_t of the integer requiring endian conversion
 *
 * Return
 * uint32_t - the result of applying endian conversion to val.
 */
uint32_t swap_endian(uint32_t val) {
  uint32_t swapped = 0;
  swapped |= (val & 0x000000ffu) << 24u;
  swapped |= (val & 0x0000ff00u) << 8u;
  swapped |= (val & 0x00ff0000u) >> 8u;
  swapped |= (val & 0xff000000u) >> 24u;
  return swapped;
}

/* check_cond
 *
 * Compares the 4 bit condition code of an instruction with the CPSR register to determine if the
 * instruction should execute. Returns a 0 if the instruction is allowed to execute - based on its
 * condition flags - and a 1 otherwise.
 *
 * Parameters
 * cond - a uint8_t of the 4 bit instruction condition code
 * cpsr - a uint32_t of the current status of the CPSR register
 *
 * Return
 * int - either a 0 or 1 (see description above)
 */
int check_cond(uint8_t cond, uint32_t cpsr) {
  // Could be done more efficiently, but this is easy to work with
  int n = (cpsr & 0x80000000u) >> 31u;
  int z = (cpsr & 0x40000000u) >> 30u;
  // int c = (cspr & 0x20000000u) >> 29u; -- not actually / checked anywhere ...
  int v = (cpsr & 0x10000000u) >> 28u;

  // See table at bottom of page 4 of spec
  switch (cond) {
    case 0:
      if (z == 1) {
        return 0;
      }
      break;

    case 1:
      if (z == 0) {
        return 0;
      }
      break;

    case 10:
      if (n == v) {
        return 0;
      }
      break;

    case 11:
      if (n != v) {
        return 0;
      }
      break;

    case 12:
      if (z == 0 && (n == v)) {
        return 0;
      }
      break;

    case 13:
      if (z == 1 && (n != v)) {
        return 0;
      }
      break;

    case 14:
      return 0;

    default:
      printf("Invalid instruction condition code: %i\n", cond);
      exit(1);
  }
  return 1;
}

/* apply_shift
 *
 * Computes a shift of a specified uint32_t. The types of shift that can be applied (with the shift
 * type in parenthesis) are: logical left (0), logical right (1), arithmetic right (2) and rotate
 * right (3). The source value is shifted accordingly, by an amount specified in the shift value
 * parameter, then the result of the shift and the relevant carry out bit is returned.
 *
 * Parameters
 * shift_type - a uint8_t representing the 2 bit shift type code
 * source_val - a uint32_t of the value to be shifted
 * shift_val - a uint8_t value of the amount that source_val will be shifted by
 *
 * Return
 * ShifterOutput - a struct containing the result of the shift and any carry out
 */
ShifterOutput apply_shift(uint8_t shift_type, uint32_t source_val, uint8_t shift_val) {
  uint32_t result = 0;
  uint8_t carry_out = 0;
  switch (shift_type) {
    case 0:
      // Logical left
      result = source_val << shift_val;
      carry_out = (uint8_t) (source_val << (shift_val - 1u)) >> 31u;
      break;

    case 1:
      // Logical right
      result = source_val >> shift_val;
      carry_out = (uint8_t) (source_val << (31u - shift_val)) >> 31u;
      break;

    case 2:
      // Arithmetic right
      if ((source_val & 0x80000000u) >> 31u == 1) {
        result = (source_val >> shift_val) | (0xffffffffu << (32u - shift_val));
      } else {
        result = source_val >> shift_val;
      }
      carry_out = (uint8_t) (source_val << (31u - shift_val)) >> 31u;
      break;

    case 3:
      // Rotate right
      result = (source_val >> shift_val) | (source_val << (32u - shift_val));
      carry_out = (uint8_t) (source_val << (31u - shift_val)) >> 31u;
      break;

    default:
      printf("Unknown shift type");
      exit(1);
  }

  ShifterOutput so = {result, carry_out};
  return so;
}

/* fill_pipeline
 *
 * Uses the nature of the simulation to perform the three execution cycles required to (re)fill the
 * CPU fetch / decode / execute pipeline in a single function. The three stages of the pipeline in
 * the machine state struct are filled with the correct instruction data to then be processed
 * normally in the emulate main while loop.
 *
 * Parameters
 * *s - a pointer to the machine state struct
 *
 * Return
 * void - no return value
 */
void fill_pipeline(State *s) {
  uint32_t mem_index = s->registers[PC_REGISTER] / 4;
  s->execute = decode(swap_endian(s->memory[mem_index]));
  s->decode = swap_endian(s->memory[mem_index + 1]);
  s->fetch = swap_endian(s->memory[mem_index + 2]);
  s->registers[PC_REGISTER] += 8;
}
