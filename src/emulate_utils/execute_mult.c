#include <stdint.h>
#include "machine.h"

/* update_CPSR
 *
 * Updates the CPSR register according to the result of a multiply execute operation.
 *
 * Parameters
 * *s - a pointer to the machine state struct
 * result - a uint32_t of the multiplication result
 *
 * Return
 * void - no return value
 */
void update_CPSR (State *s, uint32_t result) {
  uint32_t new_cspr = 0;

  // Updates N flag to bit 32 of result
  new_cspr += (result & 1u << 31u);

  // Updates Z flag
  if (result == 0) {
    new_cspr += 1u << 30u;
  }

  s->registers[CPSR_REGISTER] = new_cspr;
}

/* execute_mult
 *
 * Performs execution of the multiply instruction type, using the current execute instruction on the provided
 * machine state. Performs the relevant calculation and updates the CPSR register (if necessary) using the
 * update_CPSR helper function.
 *
 * Parameter
 * *s - a pointer to the machine state struct
 *
 * Return
 * void - no return value
 */
void execute_mult(State *s) {
  Instruction instr = s->execute;
  
  // Multiplies Rm and Rs
  uint32_t result = s->registers[instr.rm] * s->registers[instr.rs];

  // Checks if in accumulate mode
  if (instr.a) {
    // Rn is added to the result
    result += s->registers[instr.rn];
  }

  // Checks if CPSR needs to be updated
  if (instr.s) {
    update_CPSR(s, result);
  }

  // Updates Rd with the result
  s->registers[instr.rd] = result;
}
