//
// Created by aaron on 6/14/18.
//

#include <time.h>
#include "block.h"

/** Return random index in the range [0, num_weights) */
int randomIndex(float weights[], int num_weights) {
  float total = 0;
  for (int i = 0; i < num_weights; i++) {
    total += weights[i];
  }
  float rand_value = (float) rand() / RAND_MAX * total;
  float csum = 0;
  for (int i = 0; i < num_weights; i++) {
    if (csum + weights[i] >= rand_value) {
      return i;
    }
    csum += weights[i];
  }
  fprintf(stderr, "randomIndex(): could not return random index.\n");
  exit(EXIT_FAILURE);
}

Block *Block_create(BlockType type, int row, int col) {
  Item items[] = {item_mushroom, item_fireflower, item_star, item_oneUp, item_coin};
  float weights[] = {0.05, 0.05, 0.025, 0.025, 0.85};

  Block *block = malloc(sizeof(Block));
  block->item = type == BLOCK_TYPE_QUESTION ? items[randomIndex(weights, 5)] : item_none;
  block->state = BLOCK_STATE_IDLE;
  block->type = type;
  block->texture = NULL;
  block->levelIdx.y = row;
  block->levelIdx.x = col;
  block->breakCountdown = BRICK_BREAK_ANIMATION_FRAMES;
  return block;
}

void Block_destroy(Block *block) {
  free(block);
}