/** encode_branch.h
 *
 * Constants and prototype for branch instruction encoding
 *
 */
#ifndef ENCODE_BRANCH_H
#define ENCODE_BRANCH_H

#include <stdint.h>
#include "hash_table.h"

#define BRANCH_SUFFIXES_SIZE 15
#define BRANCH_COND_U 31
#define BRANCH_COND_L 28
#define BRANCH_OFFSET_U 23
#define BRANCH_OFFSET_L 0

uint32_t encodeBranch(char *opname, char *args, HashTable *symbolTable, uint32_t currentAddress);

#endif
