#ifndef ARM11_16_INSTRUCTION_H
#define ARM11_16_INSTRUCTION_H

enum instr_type {HALT, PROCESSING, MULTIPLY, TRANSFER, BRANCH};

typedef struct {
  enum instr_type type;
  uint8_t cond;
  uint8_t i;
  uint8_t opcode;
  uint8_t s;
  uint8_t rn;
  uint8_t rd;
  uint16_t operand2;
  uint8_t a;
  uint8_t rs;
  uint8_t rm;
  uint8_t p;
  uint8_t u;
  uint8_t l;
  uint32_t offset;
} Instruction;

#endif //ARM11_16_INSTRUCTION_H
