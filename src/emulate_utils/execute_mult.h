#ifndef ARM11_16_EXECUTE_MULT_H
#define ARM11_16_EXECUTE_MULT_H

void execute_mult(State *s);
void update_CPSR (State *s, uint32_t result);

#endif //ARM11_16_EXECUTE_MULT_H
