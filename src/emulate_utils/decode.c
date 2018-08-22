#include <stdio.h>
#include <stdint.h>
#include <printf.h>
#include <stdlib.h>
#include "instruction.h"

/* decode
 *
 * Takes an instruction as a 32-bit integer in big-endian byte order and returns a decoded Instruction struct
 * representing all properties of the encoded processing instruction.
 *
 * Parameter
 * instr - a uint32_t of the raw 32 bit instruction in big-endian format.
 *
 * Return
 * Instruction - encoded Instruction struct of the instruction properties.
 */
Instruction decode(uint32_t instr) {
  Instruction decoded_instr = {HALT,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  if (instr == 0) {
    return decoded_instr;
  }

  int instr_type = (instr & 0x0c000000u) >> 26u;

  decoded_instr.cond = (uint8_t) ((instr & 0xfu << 28u) >> 28u);

  switch (instr_type) {
    case 0:
      // Data Processing or Multiply
      if ((instr & 0x02000000u) >> 25u == 1 || (instr & 0x000000f0u) >> 4u != 9) {
        // Data Processing
        decoded_instr.type = PROCESSING;
        decoded_instr.i = (uint8_t) ((instr & 0x1u << 25u) >> 25u);
        decoded_instr.opcode = (uint8_t) ((instr & 0xfu << 21u) >> 21u);
        decoded_instr.s = (uint8_t) ((instr & 0x1u << 20u) >> 20u);
        decoded_instr.rn = (uint8_t) ((instr & 0xfu << 16u) >> 16u);
        decoded_instr.rd = (uint8_t) ((instr & 0xfu << 12u) >> 12u);
        decoded_instr.operand2 = (uint16_t) (instr & 0xfffu);

      } else {
        // Multiply
        decoded_instr.type = MULTIPLY;
        decoded_instr.a = (uint8_t) ((instr & 0x1u << 21u) >> 21u);
        decoded_instr.s = (uint8_t) ((instr & 0x1u << 20u) >> 20u);
        decoded_instr.rd = (uint8_t) ((instr & 0xfu << 16u) >> 16u);
        decoded_instr.rn = (uint8_t) ((instr & 0xfu << 12u) >> 12u);
        decoded_instr.rs = (uint8_t) ((instr & 0xfu << 8u) >> 8u);
        decoded_instr.rm = (uint8_t) (instr & 0xfu);
      }
      break;

    case 1:
      // Single Data Transfer
      decoded_instr.type = TRANSFER;
      decoded_instr.i = (uint8_t) ((instr & 0x1u << 25u) >> 25u);
      decoded_instr.p = (uint8_t) ((instr & 0x1u << 24u) >> 24u);
      decoded_instr.u = (uint8_t) ((instr & 0x1u << 23u) >> 23u);
      decoded_instr.l = (uint8_t) ((instr & 0x1u << 20u) >> 20u);
      decoded_instr.rn = (uint8_t) ((instr & 0xfu << 16u) >> 16u);
      decoded_instr.rd = (uint8_t) ((instr & 0xfu << 12u) >> 12u);
      decoded_instr.offset = instr & 0xfffu;
      break;

    case 2:
      // Branch
      decoded_instr.type = BRANCH;
      decoded_instr.offset = instr & 0xffffffu;
      break;

    default:
      printf("Invalid instr: 0x%08x\n", instr);
      exit(1);
  }

  return decoded_instr;
}
