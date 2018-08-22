/** parse_utils_test.c
 *
 * Test string-parsing functions.
 */

#include <stdio.h>
#include "binary_utils.h"
#include "parse_utils.h"

void parseOp2Test() {
  Operand2 op2 = parseOperand2("#0x3FC00");
  puts("Parse #0x3FC00:");
  printf("Operand type: %s\n", op2.is_immediate ? "Immediate" : "(Shifted) Register");
  printf("Operand encoding:");
  printBinary(op2.encoding);

  op2 = parseOperand2("r1");
  puts("Parse register r1:");
  printf("Operand type: %s\n", op2.is_immediate ? "Immediate" : "(Shifted) Register");
  printf("Operand encoding:");
  printBinary(op2.encoding);

  op2 = parseOperand2("r1, lsl r2");
  puts("Parse shifted register r1, lsl r2:");
  printf("Operand type: %s\n", op2.is_immediate ? "Immediate" : "(Shifted) Register");
  printf("Operand encoding:");
  printBinary(op2.encoding);
}

void parseRegTest() {
  uint16_t reg = parseRegister("PC");
  printBinary(reg);
  reg = parseRegister("r2");
  printBinary(reg);
  reg = parseRegister("r13");
  printBinary(reg);
}

void parseNumConstTest() {
  uint32_t x = parseNumericConstant("0");
  printf("'0' == %u\n", x);
  x = parseNumericConstant("12345");
  printf("'12345' == %u\n", x);
  x = parseNumericConstant("0xABD");
  printf("'0xABD' == %u\n", x);
}

void parseShiftRegTest() {
  uint16_t x = parseShiftedRegister("r3, lsl r4");
  printBinary(x);
  x = parseShiftedRegister("r2, lsl #2");
  printBinary(x);
}

void parseEncodeShiftTest() {
  uint16_t x = encodeShiftOperation("lsl", "r4");
  printBinary(x);
  x = encodeShiftOperation("ror", "#3");
  printBinary(x);
}

void isLabelTest() {
  printf("%s: %s", "loop:", isLabel("loop:") ? "YES" : "NO");
  printf("%s: %s", "4oop:", isLabel("4oop:") ? "YES" : "NO");
  printf("%s: %s", "A:", isLabel("A:") ? "YES" : "NO");
  printf("%s: %s", "Abc", isLabel("Abc") ? "YES" : "NO");
}

void runParseTests() {
  parseOp2Test();

  parseRegTest();

  parseNumConstTest();

  parseShiftRegTest();

  parseEncodeShiftTest();

  isLabelTest();
}