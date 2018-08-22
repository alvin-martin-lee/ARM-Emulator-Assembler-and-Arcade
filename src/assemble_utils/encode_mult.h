/** encode_mult.h
 *
 * Encode multiply MUL MLA instructions.
 */
#ifndef ENCODE_MULT_H
#define ENCODE_MULT_H

#define MULT_MUL_ARGC 3 // Rd, Rm, Rs
#define MULT_MLA_ARGC 4 // Rd, Rm, Rs, Rn

#define MULT_COND 14 // 1110
#define MULT_COND_U 31
#define MULT_COND_L 28
#define MULT_ACC 21
#define MULT_SET 20
#define MULT_RD_U 19
#define MULT_RD_L 16
#define MULT_RN_U 15
#define MULT_RN_L 12
#define MULT_RS_U 11
#define MULT_RS_L 8
#define MULT_RM_U 3
#define MULT_RM_L 0

uint32_t encodeMult(char *opname, char *args);

#endif
