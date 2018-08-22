#include "instruction.h"

#ifndef ARM11_16_EMULATOR_MACHINE_DETAILS_H
#define ARM11_16_EMULATOR_MACHINE_DETAILS_H

#define MEMORY_ADDRESSES 16384 // Memory addresses are multiples of 4
#define REGISTER_ADDRESSES 17
#define STANDARD_REGISTERS 12
#define PC_REGISTER 15
#define CPSR_REGISTER 16

typedef struct state {
  uint32_t memory[MEMORY_ADDRESSES];
  uint32_t registers[REGISTER_ADDRESSES];
  uint32_t fetch;
  uint32_t decode;
  Instruction execute;
} State;

#endif //ARM11_16_EMULATOR_MACHINE_DETAILS_H
