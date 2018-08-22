#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "emulate_utils/helpers.h"
#include "emulate_utils/execute_dp.h"
#include "emulate_utils/decode.h"
#include "emulate_utils/execute_mult.h"
#include "emulate_utils/execute_sdt.h"
#include "emulate_utils/execute_branch.h"

/* main (emulate)
 *
 * Runs the emulate program.
 *
 * Parameters
 * argc - an int of the number of parameters passed to the program
 * **argv - a pointer to the array of program parameters, of length argc
 *
 * Return
 * int - the status of the program execution (0 for success)
 */
int main(int argc, char **argv) {
  /* INITIALIZATION - before main loop executes for the first time */

  // Check that file name is specified
  if (argc != 2) {
    printf("Please specify exactly one parameter: the file to be executed\n");
    return EXIT_FAILURE;
  }

  struct state s;

  // Possibly redundant re-initialization?
  for (int i = 0; i < MEMORY_ADDRESSES; i++) {
    s.memory[i] = 0;
  }
  for (int i = 0; i < REGISTER_ADDRESSES; i++) {
    s.registers[i] = 0;
  }

  FILE *f = fopen(argv[1], "rb");
  if (!f) {
    printf("Could not open file\n");
    return EXIT_FAILURE;
  }

  // Read binary file into memory (starting from address 0)
  int mem_addr = 0;
  uint32_t temp_read;
  fread(&temp_read, 4, 1, f);
  while (feof(f) == 0) {
    s.memory[mem_addr] = swap_endian(temp_read);
    mem_addr++;
    fread(&temp_read, 4, 1, f);
  }

  fclose(f);

  // Instructions in 3 special channels are in big-endian order for easier processing
  fill_pipeline(&s);
  s.registers[PC_REGISTER] = 8;

  /* MAIN - execution loop */
  while (s.execute.type != HALT) {
    // Execute the execute command
    /* To implement - this is the juicy fun part! */
    if (check_cond(s.execute.cond, s.registers[CPSR_REGISTER]) == 0) {
      // Handle execution inside if statement -- only executed if condition code checks out

      switch (s.execute.type) {
        case PROCESSING:
          // Data Processing
          execute_dp(&s);
          break;

        case MULTIPLY:
          // Multiply
          execute_mult(&s);
          break;

        case TRANSFER:
          // Single Data Transfer
          execute_sdt(&s);
          break;

        case BRANCH:
          // Branch
          execute_branch(&s);
          continue;

        default:
          printf("Invalid instruction type: %i", s.execute.type);
          exit(1);
      }
    }

    // Shift everything in the pipeline up one
    s.execute = decode(s.decode);
    s.decode = s.fetch;
    s.registers[PC_REGISTER] += 4;
    s.fetch = swap_endian(s.memory[s.registers[PC_REGISTER] / 4]);
  }

  /* SHUTDOWN - after main loop executes for the last time */

  // Print registers and non-zero memory addresses
  printf("Registers:\n");
  for (int i = 0; i <= STANDARD_REGISTERS; i++) {
    printf("$%-3d: %10d (0x%08x)\n", i, s.registers[i], s.registers[i]);
  }
  printf("PC  : %10d (0x%08x)\n", s.registers[PC_REGISTER], s.registers[PC_REGISTER]);
  printf("CPSR: %10d (0x%08x)\n", s.registers[CPSR_REGISTER], s.registers[CPSR_REGISTER]);
  printf("Non-zero memory:\n");
  for (int i = 0; i < MEMORY_ADDRESSES; i++) {
    uint32_t mem = s.memory[i];
    if (mem != 0) {
      printf("0x%08x: 0x%08x\n", i * 4, mem);
    }
  }
  return EXIT_SUCCESS;
}
