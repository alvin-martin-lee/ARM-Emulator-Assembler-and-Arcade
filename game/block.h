//
// Created by aaron on 6/14/18.
//

#ifndef DOC_FIGHTER_BLOCK_H
#define DOC_FIGHTER_BLOCK_H

#include "game.h"
#include "item.h"

#define BRICK_BREAK_ANIMATION_FRAMES 8

// ? or regular brick
typedef enum {BLOCK_TYPE_QUESTION, BLOCK_TYPE_BRICK} BlockType;

// IDLE: not touched
// ACTIVATED: ? block becomes a solid block, brick already destroyed (doesnt have to use this)
typedef enum {BLOCK_STATE_IDLE, BLOCK_STATE_ACTIVATED} BlockState;

typedef struct {
  BlockType type;
  BlockState state;
  Item item;
  SDL_Point levelIdx; // row and column indices within level
  SDL_Texture *texture;
  int breakCountdown;
} Block;

Block *Block_create(BlockType type, int row, int col);
void Block_destroy(Block *block);

#endif //DOC_FIGHTER_BLOCK_H
