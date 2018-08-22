#include <stdio.h>
#include <stdint.h>
#include <printf.h>
#include <stdlib.h>
#include "helpers.h"

/* execute_dp
 *
 * Performs execution of the data processing instruction type, using the current execute instruction on the
 * provided machine state. Runs instructions through the shifter - using the apply_shift helper function -
 * as appropriate, then computes the correct result and (if necessary) writes this to a register.
 *
 * Parameter
 * *s - a pointer to the machine state struct
 *
 * Return
 * void - no return value
 */
void execute_dp(State *s) {
  Instruction instr = s->execute;

  uint32_t op2_val = 0;
  uint32_t rn_val = s->registers[instr.rn];
  uint32_t result = 0;
  ShifterOutput so;

  if (instr.i == 0) {
    // Operand2 is a register
    uint8_t shift = (uint8_t) ((instr.operand2 & 0x0ff0u) >> 4u);
    uint8_t rm = (uint8_t) (instr.operand2 & 0x000fu);
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

    so = apply_shift(shift_type, rm_val, shift_val);
    op2_val = so.shifted_val;
  } else {
    // Operand2 is an immediate value
    uint8_t rotate = (uint8_t) (((instr.operand2 & 0x0f00u) >> 8u) * 2u);
    uint32_t immediate = (instr.operand2 & 0x00ffu);
    // Refactor to use apply_shift helper function
    so = apply_shift(3, immediate, rotate);
    op2_val = so.shifted_val;
  }


  switch (instr.opcode) {
    case 0:
      // AND
    case 8:
      // TST
      result = rn_val & op2_val;
      break;

    case 1:
      // EOR
    case 9:
      // TEQ
      result = rn_val ^ op2_val;
      break;

    case 2:
      // SUB
    case 10:
      // CMP
      result = rn_val - op2_val;
      break;

    case 3:
      // RSB
      result = op2_val - rn_val;
      break;

    case 4:
      // ADD
      result = rn_val + op2_val;
      break;

    case 12:
      // ORR
      result = rn_val | op2_val;
      break;

    case 13:
      // MOV
      result = op2_val;
      break;

    default:
      printf("Invalid opcode: %i", instr.opcode);
      exit(1);
      break;
  }

  if (instr.s == 1) {
    uint32_t new_cpsr = 0;

    // Set V to existing value
    new_cpsr |= (instr.cond & 0x01u) << 28u;

    // Set C according to logic on spec page 6
    switch (instr.opcode) {
      case 0:
      case 1:
      case 8:
      case 9:
      case 12:
      case 13:
        // C is switcher carry out
        new_cpsr |= so.carry_out << 29u;
        break;

      case 4:
        // C is (rn + op2 < rn)
        if (rn_val + op2_val < rn_val) {
          new_cpsr |= 1u << 29u;
        }
        break;

      case 2:
      case 10:
        // C is !(op2 > rn)
        if (op2_val <= rn_val) {
          new_cpsr |= 1u << 29u;
        }
        break;

      case 3:
        // C is !(rn > op2)
        if (rn_val <= op2_val) {
          new_cpsr |= 1u << 29u;
        }
        break;

      default:
        // Should not be possible (would have exited on line 98)
        printf("Invalid opcode: %i", instr.opcode);
        exit(1);
        break;
    }

    // Set Z if result is zero
    if (result == 0) {
      new_cpsr |= 1u << 30u;
    }

    // Set N as bit 31 of result
    new_cpsr |= result & 0x80000000u;

    s->registers[CPSR_REGISTER] = new_cpsr;
  }

  switch (instr.opcode) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 12:
    case 13:
      s->registers[instr.rd] = result;
      break;

    default:
      // No processing required
      break;
  }
}
