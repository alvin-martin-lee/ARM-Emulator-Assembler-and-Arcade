/** encode_sdt.c
 *
 * Encode LDR and STR instructions.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "encode_sdt.h"
#include "encode_dp.h"  // ldr uses mov if numeric const. < 0xff
#include "binary_utils.h"
#include "string_utils.h"
#include "parse_utils.h"

static void encodePreIndexedAddress(uint32_t *encoding, char *address);
static void encodePostIndexedAddress(uint32_t *encoding, char *address);

/**
 * Encoding Single Data Transfer Instruction.
 *
 * <ldr/str> Rd, <address>
 *
 * This takes three additional parameters `saveValue`, `instructionAddr`, and `valueAddr`
 * to handle the case where the opcode is LDR and the address is a numerical constant.
 *
 * If <address> is a constant greater than 0xFF, it will be stored in `saveValue`,
 * later to be written to the end of the binary file.
 *
 * `instructionAddr` specifies the address of the current instruction and
 * `valueAddr` specifies where in the binary file will the value be placed,
 * to calculate the Offset.
 */
uint32_t encodeSDT(char *opname, char *args,
                   uint32_t *saveValue, uint32_t instructionAddr, uint32_t valueAddr) {

  if (!(strEqualNCS(opname, "ldr") || strEqualNCS(opname, "str"))) {
    fprintf(stderr, "Opname %s is not a SDT instruction (ldr/str)\n", opname);
    exit(EXIT_FAILURE);
  }

  uint32_t encoding = 0;
  setBitPattern(&encoding, SDT_COND, SDT_COND_U, SDT_COND_L);
  setBit(&encoding, 26);
  setBit(&encoding, SDT_UP);
  if (strEqualNCS(opname, "ldr")) {
    setBit(&encoding, SDT_LOAD);
  }

  // keep a copy of args since tokenization will modify the original (for mov)
  char *originalArgs = malloc(sizeof(char) * (strlen(args) + 1));
  strcpy(originalArgs, args);

  char **tokens = getTokens(args, SDT_ARGC);
  setBitPattern(&encoding, parseRegister(tokens[0]), SDT_RD_U, SDT_RD_L);
  char *addressStr = tokens[1];

  // Process the address
  // numeric constant
  if (strEqualNCS(opname, "ldr") && addressStr[0] == '=') {
    uint32_t numericConstant = parseNumericConstant(addressStr + 1);
    if (numericConstant <= 0xFF) {
      // MOV: "mov Rd, #x"
      // LDR: "ldr Rd, =x" (change = to #)
      char *e = strchr(originalArgs, '=');
      *e = '#';
      encoding = encodeDP("mov", originalArgs);
    } else {
      // Handle numbers > 0xFF
      // Offset: where to go to from PC
      // PC = current instruction address + 8
      uint32_t offset = valueAddr - instructionAddr - 8;
      setBitPattern(&encoding, 15, SDT_RN_U, SDT_RN_L); // Rn = PC 1111
      setBit(&encoding, SDT_PRE);
      if (offset > 0xFFF) {
        // bigger than 12 bits
        fprintf(stderr, "Numerical offset in LDR is bigger than 12 bits.");
        exit(EXIT_FAILURE);
      } else {
        *saveValue = numericConstant;
        setBitPattern(&encoding, offset, SDT_OFFSET_U, SDT_OFFSET_L);
      }
    }
  } else if (addressStr[0] == '[') {
    if (addressStr[strlen(addressStr) - 1] == ']') {
      encodePreIndexedAddress(&encoding, addressStr);
    } else {
      encodePostIndexedAddress(&encoding, addressStr);
    }
  } else {
    fprintf(stderr, "Not implemented other address formats.\n");
    exit(EXIT_FAILURE);
  }
  /************** End processing address *****************/

  free(tokens);
  free(originalArgs);
  return encoding;
}

/**
 * Set the encoding for a pre-indexed address (of the form [Rn] or [Rn, <#expr>])
 */
static void encodePreIndexedAddress(uint32_t *encoding, char *address) {
  setBit(encoding, SDT_PRE); // P(24) = 1

  // remove brackets
  address[strlen(address) - 1] = '\0';
  ++address;

  char **preIndexToks = getTokens(address, 2);
  char *expr = preIndexToks[1];
  uint8_t rn = parseRegister(preIndexToks[0]);
  setBitPattern(encoding, rn, SDT_RN_U, SDT_RN_L);
  if (*expr != '\0') {
    // shift exists
    if (*expr == '#') {
      // numeric constant (e.g. #123)
      ++expr;
      uint32_t offset;
      if (*expr == '-') {
        // negative number
        offset = parseNumericConstant(expr + 1);
        unsetBit(encoding, SDT_UP); // U = 0
      } else {
        offset = parseNumericConstant(expr);
      }
      if (offset > 0xFFF) {
        // bigger than 12 bits
        fprintf(stderr, "Numerical offset in LDR is bigger than 12 bits.");
        exit(EXIT_FAILURE);
      }
      setBitPattern(encoding, offset, SDT_OFFSET_U, SDT_OFFSET_L);
    } else if (tolower(*expr) == 'r') {
      setBitPattern(encoding, parseShiftedRegister(expr), SDT_OFFSET_U, SDT_OFFSET_L);
      setBit(encoding, SDT_IMM); // I = 1 (shiftted register)
    } else {
      // error
      fprintf(stderr, "Invalid expression after register\n");
      exit(EXIT_FAILURE);
    }
  }
  free(preIndexToks);
}

/**
 * Set the encoding for a post-indexed address (of the form [Rn],<#expression>)
 */
static void encodePostIndexedAddress(uint32_t *encoding, char *address) {
  char **tokens = getTokens(address, 2);
  char *rnStr = tokens[0];
  char *expr = tokens[1];
  rnStr[strlen(rnStr) - 1] = '\0';
  ++rnStr;
  uint8_t rn = parseRegister(rnStr);
  setBitPattern(encoding, rn, SDT_RN_U, SDT_RN_L);

  uint32_t offset;
  if (*expr == '#') {
    unsetBit(encoding, SDT_IMM);
    offset = parseNumericConstant(expr + 1);
  } else {
    setBit(encoding, SDT_IMM);
    offset = parseShiftedRegister(expr);
  }
  setBitPattern(encoding, offset, SDT_OFFSET_U, SDT_OFFSET_L);
  free(tokens);
}