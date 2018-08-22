/** encode_dp.h
 *
 * Prototypes for encoding a Data-Processing Instruction.
 * Types:
 * - Computation: and, eor, sub, rsb, add, orr
 * - Move (single operand assignment): mov
 * - CPSR-setting: tst, teq, cmp
 */
#ifndef ENCODE_DP_H
#define ENCODE_DP_H

#include <stdint.h>

#define DP_COND 14  // 1110
#define DP_CPSR_OPS_SIZE 3
#define DP_CPSR_ARGC 2
#define DP_COMP_OPS_SIZE 6
#define DP_COMP_ARGC 3
#define DP_MOV_ARGC 2

#define DP_COND_U 31
#define DP_COND_L 28
#define DP_IMM 25
#define DP_OPCODE_U 24
#define DP_OPCODE_L 21
#define DP_SETCOND 20
#define DP_RN_U 19
#define DP_RN_L 16
#define DP_RD_U 15
#define DP_RD_L 12
#define DP_OP2_U 11
#define DP_OP2_L 0

uint32_t encodeDP(char *opname, char *args);

#endif
