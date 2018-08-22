/** assemble.c
 *
 * Main program for assembling a ARM source file.
 *
 */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assemble_utils/parse_utils.h>

#include "assemble.h"
#include "assemble_utils/encode_dp.h"
#include "assemble_utils/encode_mult.h"
#include "assemble_utils/encode_sdt.h"
#include "assemble_utils/encode_branch.h"
#include "assemble_utils/string_utils.h"
#include "assemble_utils/encode_special.h"


int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: ./assemble <input file> <output file name>\n");
    return EXIT_FAILURE;
  }

  // Open input and output files
  FILE *inputFile, *outputFile;
  if ((inputFile = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Cannot open input file %s\n", argv[1]);
    return EXIT_FAILURE;
  }

  if ((outputFile = fopen(argv[2], "wb")) == NULL) {
    fprintf(stderr, "Cannot create/overwrite output file %s\n", argv[2]);
    return EXIT_FAILURE;
  }

  HashTable *symbolTable;
  int fileSize = 0;
  firstPass(inputFile, &symbolTable, &fileSize);
  secondPass(inputFile, symbolTable, fileSize, outputFile);

  // Cleanup
  HashTable_destroy(symbolTable);
  fclose(inputFile);
  fclose(outputFile);

  return EXIT_SUCCESS;
}

/**
 * First Pass - generate symbol table, and store total file size in bytes
 */
void firstPass(FILE *inputFile, HashTable **symbolTable, int *saveFileSize) {
  if (inputFile == NULL) {
    fprintf(stderr, "NULL file pointer argument.\n");
    exit(EXIT_FAILURE);
  }

  *symbolTable = HashTable_create();
  uint32_t address = 0;
  char line[MAX_LINE_LENGTH];

  while (fgets(line, sizeof(line), inputFile) != NULL) {

    // Remove extra newline character at the end
    line[strcspn(line, "\n")] = 0;

    // Exclude blank lines
    if (strcmp(line, "") == 0) continue;

    // Extract and check the first token
    char *label = strtok(line, " ");
    if (isLabel(label)) {
      if (!HashTable_containsKey(*symbolTable, label)) {
        // Remove the colon and add to symbol table
        label[strlen(label) - 1] = 0;
        HashTable_put(*symbolTable, label, address);
      } else {
        fprintf(stderr, "File syntax error - repeated label definition.\n");
        exit(EXIT_FAILURE);
      }
    }
    // If the label is the only token on the line, don't increment because it 'points' to the instruction below it.
    // e.g.
    // loop:
    //   add r1, r2, r4
    //   ...
    // This assumes the next line exists and is a valid instruction
    if (strtok(NULL, " ") != NULL) {
      address += INSTRN_SIZE;
    }
  }
  // save the file size
  *saveFileSize = address;
}

/**
 * Second Pass - generate binary encoding (fileSize = total bytes occupied by the file.)
 */
void secondPass(FILE *inputFile, HashTable *symbolTable, const int fileSize, FILE *outputFile) {
  if (inputFile == NULL) {
    fprintf(stderr, "NULL file pointer argument.\n");
    exit(EXIT_FAILURE);
  }

  // Move the file pointer to the start.
  fseek(inputFile, 0, SEEK_SET);

  // For the LDR numeric constant case: need to store 4-byte values at the end of the file later
  uint32_t ldrNumConsts[MAX_LDR_CONSTS]; // constants to be written to the end of file.
  uint32_t fileEnd = (uint32_t) fileSize;           // end position of the file
  uint32_t numConstIdx = 0;              // the index of the numeric constant in ldrNumConsts array

  int lineIndex = 0; // current line (ignoring empty space and label-only lines)
  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), inputFile) != NULL) {

    // Remove extra newline character at the end
    line[strcspn(line, "\n")] = 0;

    // skip blank lines
    if (strcmp(line, "") == 0) continue;

    // skip label-only line (need to make a copy since strtok modifies the original string)
    char tmp[MAX_LINE_LENGTH];
    strncpy(tmp, line, MAX_LINE_LENGTH);
    char *label = strtok(tmp, " ");
    if (isLabel(label)) {
      continue;
    }

    // split the operator from the operands
    char **tokens = getTokensGeneric(line, 2, ' ');
    char *opcode = tokens[0], *operands = tokens[1];
    OperatorType optype = getOperatorType(opcode);

    uint32_t encoding = 0;    // 32-bit instruction binary encoding
    uint32_t ldrNumConst = 0; // the LDR numerical constant > 0xFF
    switch (optype) {
      case DATA_PROCESSING:
        encoding = encodeDP(opcode, operands);
        break;
      case MULTIPLY:
        encoding = encodeMult(opcode, operands);
        break;
      case SINGLE_DATA_TRANSFER:
        encoding = encodeSDT(opcode, operands, &ldrNumConst, lineIndex * INSTRN_SIZE,
            fileEnd + numConstIdx * INSTRN_SIZE);
        if (ldrNumConst > 0xFF) {
          if (numConstIdx >= MAX_LDR_CONSTS) {
            fprintf(stderr, "Too many LDR constants.\n");
            exit(EXIT_FAILURE);
          }
          ldrNumConsts[numConstIdx++] = ldrNumConst;
        }
        ldrNumConst = 0; // reset for next iteration
        break;
      case BRANCH:
        encoding = encodeBranch(opcode, operands, symbolTable, lineIndex * INSTRN_SIZE);
        break;
      case SPECIAL:
        encoding = encodeSpecial(opcode, operands);
        break;
    }
    writeWord(outputFile, encoding);
    ++lineIndex;
    free(tokens);
  }

  // Write the numerical constants to the end of the file
  for (int i = 0; i < numConstIdx; i++) {
    writeWord(outputFile, ldrNumConsts[i]);
  }
}

/**
 * Determine the operator type given the mnemonic.
 */
OperatorType getOperatorType(char *opname) {
  // data-processing, multiply, single data transfer, branch and special
  static const char *dp_ops[] = {"and", "eor", "sub", "rsb", "add", "orr", "mov", "tst", "teq", "cmp"};
  static const char *mul_ops[] = {"mul", "mla"};
  static const char *sdt_ops[] = {"ldr", "str"};
  static const char *br_ops[] = {"beq", "bne", "bge", "blt", "bgt", "ble", "b", "bal"};
  static const char *sp_ops[] = {"andeq", "lsl"};

  if (opname == NULL) {
    fprintf(stderr, "Operator mnemonic argument is NULL.\n");
    exit(EXIT_FAILURE);
  }
  if (findStringIndexNCS(opname, dp_ops, DP_OP_COUNT) != -1)
    return DATA_PROCESSING;
  if (findStringIndexNCS(opname, mul_ops, MULT_OP_COUNT) != -1)
    return MULTIPLY;
  if (findStringIndexNCS(opname, sdt_ops, SDT_OP_COUNT) != -1)
    return SINGLE_DATA_TRANSFER;
  if (findStringIndexNCS(opname, br_ops, BRANCH_OP_COUNT) != -1)
    return BRANCH;
  if (findStringIndexNCS(opname, sp_ops, SPECIAL_OP_COUNT) != -1)
    return SPECIAL;
  // not found
  fprintf(stderr, "Operator mnemonic %s does not exist.\n", opname);
  exit(EXIT_FAILURE);
}

/**
 * Write a 32-bit word (4 bytes) to a given file pointer.
 * Returns 0 on success, -1 on failure.
 */
int writeWord(FILE *fp, uint32_t word) {
  if (fp == NULL)
    return -1;
  if (fwrite(&word, 1, sizeof(word), fp) != sizeof(word)) {
    return -1;
  }
  return 0;
}
