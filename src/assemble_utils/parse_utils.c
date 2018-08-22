/** parse_utils.c
 *
 * Functions for converting string into binary values.
 */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse_utils.h"
#include "binary_utils.h"
#include "string_utils.h"

static const char *shift_names[] = {"lsl", "lsr", "asr", "ror"};

/**
 * Need to return type (immediate or not) and encoding.
 * But for now just return 12-bit encoding.
 *
 * string could be
 * - 8-bit immediate constant, e.g. #0x58, #123
 * - shifted integer / register
 */
Operand2 parseOperand2(char *string) {
  if (string == NULL || strlen(string) == 0) {
    fprintf(stderr, "Null or empty string provided.\n");
    exit(EXIT_FAILURE);
  }

  Operand2 operand2;

  /* immediate constants */
  if (strlen(string) > 1 && string[0] == '#') {
    operand2.is_immediate = 1;

    // get string after '#' and try to convert to immediate-value encoding
    uint32_t value = parseNumericConstant(string + 1);
    operand2.encoding = (uint16_t) encodeImmConst(value);
  } else if (strlen(string) > 1 && (string[0] == 'r' || string[0] == 'R')) {
    // e.g. "r3", "r3, lsl r4"
    // If operand2 is a register:
    // 11 - 4: shift, 3 - 0 : register number
    operand2.is_immediate = 0;
    operand2.encoding = (uint16_t) parseShiftedRegister(string);
  } else {
    fprintf(stderr, "Unknown operand2 string %s\n", string);
    exit(EXIT_FAILURE);
  }
  return operand2;
}

/**
 * Convert a register string representation to binary encoding.
 */
uint8_t parseRegister(char *reg) {
  if (reg == NULL || strlen(reg) == 0) {
    fprintf(stderr, "Null or empty string token provided.\n");
    exit(EXIT_FAILURE);
  }

  // non-case-sensitive comparison
  if (strEqualNCS(reg, "pc")) {
    // program counter = r15
    return (uint8_t) 15;
  }
  if (tolower(reg[0]) == 'r' && 2 <= strlen(reg) && strlen(reg) <= 3) {
    // general purpose registers (and PC) r0 - r15
    char *end;
    unsigned long int res = strtoul(reg + 1, &end, 10);
    if (*end == '\0' && 0 <= res && res <= 15) {
      return (uint8_t) res;
    }
  }
  fprintf(stderr, "The provided token %s is not a valid register.\n", reg);
  exit(EXIT_FAILURE);
}

/**
 * Convert a string representation of an unsigned number (hex or dec) to a 32-bit value.
 */
uint32_t parseNumericConstant(char *string) {
  // either convert it to hex or dec (strtoul can tell)
  // strtoul automatically distinguishes between decimal and hex '0x'
  char *end;
  unsigned long int res = strtoul(string, &end, 0);

  // make sure it's within bounds (no bigger than 32 bits)
  // see strtoul man pages describing when the number overflows 32 bits
  errno = 0; // requires <errno.h>
  if (*end != '\0' || (res == ULONG_MAX && errno == ERANGE)) {
    fprintf(stderr, "The given number %s is out of bounds.\n", string);
    exit(EXIT_FAILURE);
  }

  return (uint32_t) res;
}

/**
 * Needed for the optional parts - parsing register arguments
 * either just a register Rx or Rx, <shift> <Ry/const>
 * E.g.
 * "sub r1, r2, r3, lsl r4"  --> encode "r3, lsl r4"
 * "str r0, [r3, r2]"        --> encode "r2"
 * "ldr r0, [r1, r2, lsl #2] --> encode "r2, lsl #2"
 */
uint16_t parseShiftedRegister(char *string) {

  if (string == NULL || strlen(string) == 0) {
    fprintf(stderr, "Null or empty string token provided.\n");
    exit(EXIT_FAILURE);
  }

  uint32_t encoding = 0;
  char **tokens = getTokens(string, 2);
  char *regStr = tokens[0];
  char *shiftStr = tokens[1];
  uint8_t rm = parseRegister(regStr);

  if (*shiftStr == '\0' && (*regStr == 'r' || *regStr == 'R')) {
    // register only: 11 - 4 = 0, 3 -0 = Rm
    encoding = rm;
  } else {
    char *shiftType = strtok(shiftStr, " ");
    char *shiftExpr = strtok(NULL, " ");
    // 11 - 4 (spec page 7)
    encoding |= encodeShiftOperation(shiftType, shiftExpr);
  }
  setBitPattern(&encoding, rm, 3, 0);
  free(tokens);
  return (uint16_t) encoding;
}

/**
 * Encode a shift operator specified by a shift type and an expression
 *
 * e.g. if the original expression is "Rn, lsl expr", this will encode the "lsl expr" part
 * encodeShiftOperation("lsl", "expr");
 *
 * Leaves bits 3 to 0 blank.
 */
uint16_t encodeShiftOperation(char *shiftType, char *shiftExpr) {
  uint32_t encoding = 0;
  int shiftIdx = findStringIndexNCS(shiftType, shift_names, 4);
  if (shiftIdx == -1) {
    fprintf(stderr, "Unrecognized shift operation %s.\n", shiftType);
    exit(EXIT_FAILURE);
  }
  setBitPattern(&encoding, (uint8_t) shiftIdx, 6, 5);

  // after the shift mnemonic, check shift operand
  if (*shiftExpr == '#') {
    // shift by constant amount
    uint32_t integer = parseNumericConstant(shiftExpr + 1);
    if (integer > 31) { // at most 5-bits 11111
      fprintf(stderr, "Shift constant %d is greater than 5 bits.\n", integer);
      exit(EXIT_FAILURE);
    }
    setBitPattern(&encoding, integer, 11, 7);
  } else if (*shiftExpr == 'r' || *shiftExpr == 'R') {
    // shift by register
    uint8_t rs = parseRegister(shiftExpr);
    setBitPattern(&encoding, rs, 11, 8);
    setBit(&encoding, 4);
  } else {
    fprintf(stderr, "Unrecognized shift operation %s %s\n", shiftType, shiftExpr);
    exit(EXIT_FAILURE);
  }
  return (uint16_t) encoding;
}

/**
 * Test if a string is a label (starts with alphabetical letter, ends with colon.
 */
int isLabel(char string[]) {
  return strlen(string) > 0 && isalpha(*string) && string[strlen(string) - 1] == ':';
}