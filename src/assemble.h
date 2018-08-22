/* assemble.h
 *
 * Prototypes for the ARM assembly main program.
 */

#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include <stdio.h>
#include "assemble_utils/hash_table.h"

// 'bal' and 'b' repeated
#define DP_OP_COUNT 10
#define MULT_OP_COUNT 2
#define SDT_OP_COUNT 2
#define BRANCH_OP_COUNT 8
#define SPECIAL_OP_COUNT 2

#define MAX_LINE_LENGTH 512
#define MAX_LDR_CONSTS 64
#define  INSTRN_SIZE 4U
#define OPCODE_MAX_LEN 8

typedef enum OperatorType {
  DATA_PROCESSING,
  MULTIPLY,
  SINGLE_DATA_TRANSFER,
  BRANCH,
  SPECIAL
} OperatorType;

void firstPass(FILE *inputFile, HashTable **symbolTable, int *saveFileSize);

void secondPass(FILE *inputFile, HashTable *symbolTable, int fileSize, FILE *outputFile);

OperatorType getOperatorType(char *opname);

int writeWord(FILE *fp, uint32_t word);


#endif // ASSEMBLE_H
