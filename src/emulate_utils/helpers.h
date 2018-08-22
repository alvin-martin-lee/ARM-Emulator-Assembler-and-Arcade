#include "machine.h"

#ifndef ARM11_16_HELPERS_H
#define ARM11_16_HELPERS_H

typedef struct shifter_output {
  uint32_t shifted_val;
  uint8_t carry_out;
} ShifterOutput;

uint32_t swap_endian(uint32_t val);
int check_cond(uint8_t cond, uint32_t cpsr);
ShifterOutput apply_shift(uint8_t shift_type, uint32_t source_val, uint8_t shift_val);
void fill_pipeline(State *s);

#endif //ARM11_16_HELPERS_H
