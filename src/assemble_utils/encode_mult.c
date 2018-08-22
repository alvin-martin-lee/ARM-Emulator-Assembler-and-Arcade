/** encode_mult.c
 * 
 * Encodes a multiply (mul/mla) instruction.
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "encode_mult.h"
#include "assemble_utils/binary_utils.h"
#include "parse_utils.h"
#include "string_utils.h"

/**
 * Multiplication instructions - mul, mla
 */
uint32_t encodeMult(char *opname, char *args) {

  if (opname == NULL || strlen(opname) == 0) {
    fprintf(stderr, "encodeMult: null or empty string provided.\n");
    exit(EXIT_FAILURE);
  }

  char **tokens;
  uint32_t encoding = 0U;
  if (strEqualNCS(opname, "mul")) {
    // set Rd, Rm, Rs
    tokens = getTokens(args, MULT_MUL_ARGC);
    setBitPattern(&encoding, parseRegister(tokens[0]), MULT_RD_U, MULT_RD_L);
    setBitPattern(&encoding, parseRegister(tokens[1]), MULT_RM_U, MULT_RM_L);
    setBitPattern(&encoding, parseRegister(tokens[2]), MULT_RS_U, MULT_RS_L);
  } else if (strEqualNCS(opname, "mla")) {
    // set Rd, Rm, Rs, Rn
    tokens = getTokens(args, MULT_MLA_ARGC);
    setBit(&encoding, MULT_ACC);
    setBitPattern(&encoding, parseRegister(tokens[0]), MULT_RD_U, MULT_RD_L);
    setBitPattern(&encoding, parseRegister(tokens[1]), MULT_RM_U, MULT_RM_L);
    setBitPattern(&encoding, parseRegister(tokens[2]), MULT_RS_U, MULT_RS_L);
    setBitPattern(&encoding, parseRegister(tokens[3]), MULT_RN_U, MULT_RN_L);
  } else {
    fprintf(stderr, "Unrecognized multiply operand '%s'.\n", opname);
    exit(EXIT_FAILURE);
  }

  // Finalize non-register bits
  setBitPattern(&encoding, MULT_COND, MULT_COND_U, MULT_COND_L);
  setBitPattern(&encoding, 9, 7, 4); // 1001

  // regs has malloc'd memory
  free(tokens);
  return encoding;
}

