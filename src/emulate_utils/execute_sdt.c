#include <stdio.h>
#include <stdint.h>
#include <printf.h>
#include <stdlib.h>
#include "helpers.h"

/* load_store_data
 *
 * Performs execution of the multiply instruction type, using the current execute instruction on the provided
 * machine state. Performs the relevant calculation and updates the CPSR register (if necessary) using the
 * update_CPSR helper function.
 *
 * Parameters
 * *s - a pointer to the machine state struct
 * instr - a Instruction struct representing the currently-executing instruction
 * loc - a uint32_t representing the memory address to read / write to
 *
 * Return
 * void - no return value
 */
void load_store_data (State *s, Instruction instr, uint32_t loc) {

  uint32_t val;
  uint32_t mask;
  uint32_t bit_selector = loc % 4;

  switch (bit_selector) {
    case 0:
      mask = 0xFFFFFFFFu;
      break;
    case 1:
      mask = 0x00FFFFFFu;
      break;
    case 2:
      mask = 0x0000FFFFu;
      break;
    case 3:
      mask = 0x000000FFu;
      break;
    default:
      printf("Invalid instruction\n");
      exit(1);
  }

  if (loc >= 0x20200000 && loc <= 0x20200008) {
    // Special case for GPIO configuration register
    int startpin = (loc - 0x20200000) / 4 * 10;
    printf("One GPIO pin from %i to %i has been accessed\n", startpin, startpin + 9);

    if (instr.l) {
      s->registers[instr.rd] = loc;
    }
    return;
  }

  if (loc == 0x20200028) {
    // Special case for GPIO clear register
    printf("PIN OFF\n");
    return;
  }

  if (loc == 0x2020001c) {
    // Special case for GPIO set register
    printf("PIN ON\n");
    return;
  }

  if (loc >= (MEMORY_ADDRESSES - 1) * 4) {
    printf("Error: Out of bounds memory access at address 0x%08x\n", loc);
    return;
  }

  if (instr.l) {
    uint32_t mem_loc_1 = s->memory[loc/4];
    uint32_t mem_loc_2 = s->memory[loc/4 + 1];

    val = (mem_loc_1 & mask) << 8 * bit_selector | (mem_loc_2 & ~mask) >> 8 * (4 - bit_selector);
    val = swap_endian(val);
    s->registers[instr.rd] = val;
  } else {
    // Get value to be stored from memory
    val = s->registers[instr.rd];
    val = swap_endian(val);

    if (loc % 4 == 0) {
      // Index is a multiple of 4, so can store trivially
      s->memory[loc/4] = val;
    } else {
      uint32_t mem_loc_1 = s->memory[loc/4];
      uint32_t mem_loc_2 = s->memory[loc/4 + 1];

      uint32_t val_1 = (val >> (8 * bit_selector) & mask) | (mem_loc_1 & ~mask);
      uint32_t val_2 = (val << 8 * (4 - bit_selector) & ~mask) | (mem_loc_2 & mask);

      s->memory[loc/4] = val_1;
      s->memory[loc/4 + 1] = val_2;
    }
  }
}

/* execute_sdt
 *
 * Performs execution of the single data transfer instruction type, using the current execute instruction on
 * the provided machine state. Applies a relevant shift to the operands using the apply_shift helper function
 * and computes any necessary offset, then loads or stores data using the load_store_data function.
 *
 * Parameter
 * *s - a pointer to the machine state struct
 *
 * Return
 * void - no return value
 */
void execute_sdt (State *s) {
  Instruction instr = s->execute;

  uint32_t base = s->registers[instr.rn];
  uint32_t offset = instr.offset;

  // Checks the Immediate offset flag
  if (instr.i) {
    uint8_t shift = (uint8_t) ((offset & 0x0ff0u) >> 4u);
    uint8_t rm = (uint8_t) (offset & 0x000fu);
    uint32_t rm_val = s->registers[rm];

    uint8_t shift_mode = (uint8_t) (shift & 0x01u);
    uint8_t shift_type = (uint8_t) ((shift & 0x06u) >> 1u);
    uint8_t shift_val = 0;
    if (shift_mode == 0) {
      // Shift by a constant amount
      shift_val = (uint8_t) ((shift & 0xf8u) >> 3u);
    } else {
      // Shift specified by a register
      uint8_t reg_num = (uint8_t) ((shift & 0xf0u) >> 4u);
      shift_val = (uint8_t) (s->registers[reg_num] & 0x000000ffu);
    }

    ShifterOutput so = apply_shift(shift_type, rm_val, shift_val);
    offset = so.shifted_val;
  }

  // Determines if Pre/Post indexing mode
  if (instr.p) {
    instr.u ? (base += offset) : (base -= offset);
    load_store_data(s, instr, base);
  } else {
    load_store_data(s, instr, base);
    instr.u ? (s->registers[instr.rn] += offset) : (s->registers[instr.rn] -= offset);
  }
}
