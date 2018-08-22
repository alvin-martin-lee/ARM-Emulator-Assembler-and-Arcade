/** encode_branch.c
 *
 * Main program for assembling a ARM source file.
 *
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "string_utils.h"
#include "assemble_utils/binary_utils.h"
#include "hash_table.h"
#include "encode_branch.h"

static const char *branch_suffixes[] = {
    "eq", // 0000
    "ne", // 0001
    "", "", "", "", "", "", "", "", // 0010 - 1001
    "ge", // 1010
    "lt", // 1011
    "gt", // 1100
    "le", // 1101
    "al" // 1110 (note: or no suffix)
};

/**
 * Encode branch instructions - eq, ne, ge, lt, gt, le
 *
 * Requires additional parameters: symbol table and the current address*/
uint32_t encodeBranch(char *opname, char *args, HashTable *symbolTable, uint32_t currentAddress) {
  if (opname == NULL || args == NULL || symbolTable == NULL) {
    fprintf(stderr, "Invalid branching instruction.\n");
    exit(EXIT_FAILURE);
  }

  uint32_t encoding = 0;
  setBitPattern(&encoding, 10, 27, 24);

  if (opname[0] != 'b') {
    fprintf(stderr, "Not a valid branch operator %s\n", opname);
    exit(EXIT_FAILURE);
  }

  char *suffix = strEqualNCS(opname, "b") ? "al" : opname + 1;
  uint8_t cond = (uint8_t) findStringIndexNCS(suffix, branch_suffixes, BRANCH_SUFFIXES_SIZE);
  setBitPattern(&encoding, cond, BRANCH_COND_U, BRANCH_COND_L);

  // offset is shifted 2 bits to the left, taking into account the effect of the pipeline (PC is 8 bytes ahead)
  uint32_t offset = (HashTable_lookup(symbolTable, args) - currentAddress - 8) >> 2;
  setBitPattern(&encoding, offset, BRANCH_OFFSET_U, BRANCH_OFFSET_L);

  return encoding;
}
