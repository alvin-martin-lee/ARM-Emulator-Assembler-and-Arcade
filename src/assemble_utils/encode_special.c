/** encode_special.c
 *
 * Encode ANDEQ and LSL
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "encode_special.h"
#include "binary_utils.h"
#include "string_utils.h"
#include "parse_utils.h"
#include "encode_dp.h"

/* andeq, lsl
 *
 * lsl Rn, <#expression> == mov Rn, Rn, lsl <#expression>
 * */
uint32_t encodeSpecial(char *opname, char *args) {
  if (opname == NULL || args == NULL) {
    fprintf(stderr, "Invalid special instruction.\n");
    exit(EXIT_FAILURE);
  }
  uint32_t encoding = 0;
  // andeq
  if (strEqualNCS(opname, "andeq")) {
    return encoding;
  }
  // lsl
  if (!strEqualNCS(opname, "lsl")) {
    fprintf(stderr, "Opname %s is not a special operator.\n", opname);
    exit(EXIT_FAILURE);
  }
  // equivalent to MOV
  char **tokens = getTokens(args, 2);
  uint8_t rn = parseRegister(tokens[0]);
  uint16_t shiftOp = encodeShiftOperation("lsl", tokens[1]);
  setBitPattern(&encoding, DP_COND, DP_COND_U, DP_COND_L); // sets cond 14 = 1110
  setBitPattern(&encoding, 13, DP_OPCODE_U, DP_OPCODE_L); // sets mov = 1101
  setBitPattern(&encoding, rn, DP_RD_U, DP_RD_L);
  setBitPattern(&encoding, shiftOp | rn, DP_OP2_U, DP_OP2_L);

  free(tokens);
  return encoding;
}