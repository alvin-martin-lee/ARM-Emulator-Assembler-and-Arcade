/** parse_utils.h
 *
 * Functions for converting string into binary values.
 */

#include <stdint.h>

typedef struct Operand2 {
  uint16_t encoding;
  int is_immediate; // boolean
} Operand2;

Operand2 parseOperand2(char *string);

uint8_t parseRegister(char *reg);

uint32_t parseNumericConstant(char *string);

uint16_t parseShiftedRegister(char *string);

uint16_t encodeShiftOperation(char *shiftType, char *shiftExpr);

int isLabel(char string[]);
