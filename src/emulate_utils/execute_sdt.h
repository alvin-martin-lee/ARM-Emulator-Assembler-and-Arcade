#ifndef ARM11_16_EXECUTE_SDT_H
#define ARM11_16_EXECUTE_SDT_H

void load_store_data (State *s, Instruction instr, uint32_t *base);
void execute_sdt (State *s);

#endif //ARM11_16_EXECUTE_SDT_H
