/** encode_dp.c
 *
 * Encodes a Data-Processing Instruction.
 * Types:
 * - Computation: and, eor, sub, rsb, add, orr
 * - Move (single operand assignment): mov
 * - CPSR-setting: tst, teq, cmp
 */
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "parse_utils.h"
#include "binary_utils.h"
#include "string_utils.h"
#include "encode_dp.h"

static const char *dp_opcode_mnemonics[] = {
    "and", // 0000
    "eor", // 0001
    "sub", // 0010
    "rsb", // 0011
    "add", // 0100
    "",
    "",
    "",
    "tst", // 1000
    "teq", // 1001
    "cmp", // 1010
    "",
    "orr", // 1100
    "mov" // 1101
};

static const char *cpsr_ops[] = {"tst", "teq", "cmp"};
static const char *comp_ops[] = {"and", "eor", "sub", "rsb", "add", "orr"};

/**
 * Encode and, eor, sub, rsb, add, orr, mov, tst, teq, cmp
 *  opname: string mnemonic
 *  args: instruction operands string
 */
uint32_t encodeDP(char *opname, char *args) {
  uint32_t encoding = 0;
  uint8_t rn = 0;
  uint8_t rd = 0;
  Operand2 op2;
  char **tokens;
  uint8_t opcode = (uint8_t) findStringIndexNCS(
      opname, dp_opcode_mnemonics, sizeof(dp_opcode_mnemonics) / sizeof(char *));

  setBitPattern(&encoding, DP_COND, DP_COND_U, DP_COND_L);
  setBitPattern(&encoding, opcode, DP_OPCODE_U, DP_OPCODE_L);

  // CPSR-setting instructions
  if (findStringIndexNCS(opname, cpsr_ops, DP_CPSR_OPS_SIZE) != -1) {
    setBit(&encoding, DP_SETCOND);
    tokens = getTokens(args, DP_CPSR_ARGC);
    rn = parseRegister(tokens[0]);
    op2 = parseOperand2(tokens[1]);
  }
  // Computation instructions
  else if (findStringIndexNCS(opname, comp_ops, DP_COMP_OPS_SIZE) != -1) {
    tokens = getTokens(args, DP_COMP_ARGC);
    rd = parseRegister(tokens[0]);
    rn = parseRegister(tokens[1]);
    op2 = parseOperand2(tokens[2]);
  }
  // Move instruction
  else if (strEqualNCS(opname, "mov")) {
    tokens = getTokens(args, DP_MOV_ARGC);
    rd = parseRegister(tokens[0]);
    op2 = parseOperand2(tokens[1]);
  }
  else {
    fprintf(stderr, "Unrecognized opname %s", opname);
    exit(EXIT_FAILURE);
  }

  setBitPattern(&encoding, rn, DP_RN_U, DP_RN_L);
  setBitPattern(&encoding, rd, DP_RD_U, DP_RD_L);
  setBitPattern(&encoding, op2.encoding, DP_OP2_U, DP_OP2_L);
  if (op2.is_immediate) {
    setBit(&encoding, DP_IMM);
  }

  free(tokens);
  return encoding;
}
