//
// Created by aaron on 6/16/18.
//

//#include <id3/globals.h>
#include "mario.h"
#include "item.h"
#include "game.h"
#include "block.h"
#include "collision.h"
#include "linkedlist.h"
#include "projectile.h"

static void onCollectCoin(Item item, Mario *mario);
static void onCollectMushroom(Item item, Mario *mario);
static void onCollectFireFlower(Item item, Mario *mario);
static void onCollectStar(Item item, Mario *mario);
static void onCollectOneUp(Item item, Mario *mario);

Mario *Mario_create() {
  Mario *mario = malloc(sizeof(Mario));
  mario->points = 0;
  mario->lives = MAX_LIVES;
  mario->coins = 0;
  for (int i = 0; i < POWERUP_COUNT; i++) {
    mario->powerups[i] = item_none;
  }
  List_init(&mario->projectiles);
  return mario;
}

void Mario_destroy(Mario *mario) {
  CoreObject_destroy(mario->core);
  free(mario);
}

bool Mario_hasItem(Mario *mario, ItemType itemType) {
  return !Item_isNone(&mario->powerups[itemType]);
}

bool Mario_hasNoItems(Mario *mario) {
  for (int i = 0; i < POWERUP_COUNT; i++) {
    if (mario->powerups[i].type != ITEM_NONE) {
      return false;
    }
  }
  return true;
}

MarioState Mario_getState(Mario *mario) {
  MarioState state = {false};
  if (!mario->core->alive) {
    state.dead = true;
  }
  else if (Mario_hasNoItems(mario)) {
    state.small = true;
  }
  else {
    if (Mario_hasItem(mario, ITEM_MUSHROOM))
      state.big = true;
    if (Mario_hasItem(mario, ITEM_FIREFLOWER))
      state.fire = true;
    if (Mario_hasItem(mario, ITEM_STAR))
      state.invincible = true;
  }
  return state;
}

void Mario_updatePowerups(Mario *mario) {
  for (int i = 0; i < POWERUP_COUNT; i++) {
    Item *item = &mario->powerups[i];
    if (!Item_isNone(item) && item->duration != TIME_INF) {
      if (item->countdown == 0) {
        *item = item_none;
      } else {
        item->countdown -= 1;
      }
    }
  }
}

void Mario_takeDamage(Mario *mario) {
  MarioState state = Mario_getState(mario);
  // invincible -> no effect
  if (state.invincible) {
    return;
  }

  // small mario -> dead
  if (state.small) {
    mario->core->alive = false;
    if (mario->lives == 0) {
      // TODO: show game over, reset game, save statistics etc.
    } else {
      // TODO: show death animation, reset level, etc...
      --mario->lives;
    }
  }
  // big mario and/or fire mario -> small mario
  else if (state.big || state.fire) {
    for (int i = 0; i < POWERUP_COUNT; i++) {
      if (!Item_isNone(&mario->powerups[i])) {
        mario->powerups[i] = item_none;
      }
    }
  }
}

void onCollectCoin(Item item, Mario *mario) {
  printf("Collected coin.\n");
  mario->coins += 1;
}

void onCollectMushroom(Item item, Mario *mario) {
  printf("Collected mushroom.\n");
  // set mario's state to big mario
}

void onCollectFireFlower(Item item, Mario *mario) {
  printf("Collected fireflower.\n");
  // set mario state to fireball, enable shooting fireball function, change mario sprite
}

void onCollectStar(Item item, Mario *mario) {
  printf("Collected star.\n");
  // make mario invincible, play animation, change music etc.
}

void onCollectOneUp(Item item, Mario *mario) {
  printf("Collected 1-UP.\n");
  mario->lives += 1;
}

void Mario_onCollectItem(Mario *mario, Item item) {
  mario->points += item.points;
  switch (item.type) {
    case ITEM_MUSHROOM:
    case ITEM_FIREFLOWER:
    case ITEM_STAR:
      mario->powerups[item.type] = item;
      break;
    default:
      break;
  }
  switch (item.type) {
    case ITEM_COIN:
      onCollectCoin(item, mario);
      break;
    case ITEM_MUSHROOM:
      onCollectMushroom(item, mario);
      break;
    case ITEM_FIREFLOWER:
      onCollectFireFlower(item, mario);
      break;
    case ITEM_STAR:
      onCollectStar(item, mario);
      break;
    case ITEM_ONEUP:
      onCollectOneUp(item, mario);
      break;
    case ITEM_NONE:
      break;
  }
}

void Mario_onCollideBlock(Mario *mario, int blockRow, int blockCol, int level[LEVEL_H][LEVEL_W], Block ***blocks, List *gameItems) {

  // printf("Found block at (%d, %d)\n", blockRow, blockCol);
  Block *block = blocks[blockRow][blockCol];
  if (block == NULL) {
    return;
  }
  if (block->state == BLOCK_STATE_ACTIVATED) {
    return;
  }

  int i = block->levelIdx.y;
  int j = block->levelIdx.x;
  MarioState state = Mario_getState(mario);
  switch (block->type) {
    case BLOCK_TYPE_QUESTION:
      if (block->item.type != ITEM_NONE) {
        // spawn the powerup above the block
        block->item.hitbox.x = block->levelIdx.x * TILE_SIZE;
        block->item.hitbox.y = (block->levelIdx.y - 1) * TILE_SIZE;
        List_insert_front(gameItems, &block->item);
        if (block->item.type == ITEM_COIN) {
          Mario_onCollectItem(mario, block->item);
        }
      }
      block->state = BLOCK_STATE_ACTIVATED;
      // block->item = item_none;
      break;
    case BLOCK_TYPE_BRICK:
      if (state.big || state.fire) {
        level[i][j] = EMPTY_TILE;
        // free(blocks[i][j]);
        // blocks[i][j] = NULL;
        block->state = BLOCK_STATE_ACTIVATED;
      }
      break;
  }
}

void Mario_onCollideMob(Mario *mario, SDL_Rect mob, DirectionH dir, int level[LEVEL_H][LEVEL_W]) {
  int mario_top = mario->core->hitbox.y / TILE_SIZE;
  int mario_bot = (mario->core->hitbox.y + mario->core->hitbox.h) / TILE_SIZE;
  int mario_side = dir == LEFT ? mario->core->hitbox.x / TILE_SIZE : (mario->core->hitbox.x + mario->core->hitbox.w) / TILE_SIZE;
  int mob_top = mob.y / TILE_SIZE;
  int mob_bot = (mob.y + mob.h) / TILE_SIZE;
  int mob_side = dir == LEFT ? (mob.x + mob.w) / TILE_SIZE : mob.x / TILE_SIZE;
  bool collision = false;
  for (int mario_row = mario_top; mario_row <= mario_bot; mario_row++) {
    for (int mob_row = mob_top; mob_row <= mob_bot; mob_row++) {
      if (mario_row == mob_row && mario_side == mob_side) {
        collision = true;
      }
    }
  }
  if (collision) {
    Mario_takeDamage(mario);
  }
}

void Mario_draw(Mario *mario, SDL_Renderer *renderer, SDL_Texture *texture) {
  SDL_RenderCopy(renderer, texture, NULL, &mario->core->size);

  // Draw debug rectangles
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
  //SDL_RenderDrawRect(renderer, &mario->core->size);

  SDL_Rect debugHitBoxRect = mario->core->hitbox;
  debugHitBoxRect.x *= SCREEN_SCALE;
  debugHitBoxRect.y *= SCREEN_SCALE;
  debugHitBoxRect.w *= SCREEN_SCALE;
  debugHitBoxRect.h *= SCREEN_SCALE;

  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);
  //SDL_RenderDrawRect(renderer, &debugHitBoxRect);
}

void Mario_shootFireball(Mario *mario, DirectionH dir) {
  Projectile *projectile = Projectile_create(mario->core->hitbox.x, mario->core->hitbox.y + TILE_SIZE);
  projectile->duration = -1;
  projectile->vx = dir == RIGHT ? 2 : -2;
  projectile->vy = 0;
  List_insert_front(&mario->projectiles, projectile);
}