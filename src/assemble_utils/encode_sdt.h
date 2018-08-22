/** encode_sdt.h
 *
 * Prototypes for encoding Single Data Transfer Instructions LDR/STR
 */
#ifndef ENCODE_SDT_H
#define ENCODE_SDT_H

#include <stdint.h>

#define SDT_ARGC 2 // Rd, <address>

#define SDT_COND 14 // 1110
#define SDT_COND_U 31
#define SDT_COND_L 28
#define SDT_IMM 25
#define SDT_PRE 24
#define SDT_UP 23
#define SDT_LOAD 20
#define SDT_RN_U 19
#define SDT_RN_L 16
#define SDT_RD_U 15
#define SDT_RD_L 12
#define SDT_OFFSET_U 11
#define SDT_OFFSET_L 0

uint32_t encodeSDT(char *opname, char *args,
                   uint32_t *saveValue, uint32_t instructionAddr, uint32_t valueAddr);

#endif /* ENCODE_SDT_H */
