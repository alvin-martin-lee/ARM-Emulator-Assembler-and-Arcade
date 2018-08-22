#include <stdint.h>
#include "helpers.h"

#define SIGN_EXTENSION_FACTOR 25u

/* execute_branch
 *
 * Performs execution of the branch instruction type, using the current execute instruction on the provided
 * machine state. Updates the Program Counter register of the machine appropriately.
 *
 * Parameter
 * *s - a pointer to the machine state struct
 *
 * Return
 * void - no return value
 */
void execute_branch (State *s) {
  Instruction instr = s->execute;

  // Left shifts the offset by two
  int32_t offset = instr.offset << 2u;

  // Sign extends the 26 bit off set to 32
  uint32_t mask = 1u << (SIGN_EXTENSION_FACTOR);
  offset = ((unsigned) offset ^ mask) - mask;

  // Increment/Decrement PC by offset
  s->registers[PC_REGISTER] += offset;

  // Clear pipeline
  fill_pipeline(s);
}
