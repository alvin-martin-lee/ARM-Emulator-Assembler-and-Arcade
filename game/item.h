//
// Created by aaron on 6/15/18.
//

#ifndef DOC_FIGHTER_ITEM_H
#define DOC_FIGHTER_ITEM_H

#include "include/SDL2/SDL.h"
#include <stdbool.h>
#include "game.h"

#define POWERUP_COUNT 3
#define FRAME_RATE 60 // temporary (fps)
#define TIME_INF (-1)
#define STAR_DURATION 10 // in frames

#define COIN_PTS 200
#define MUSHROOM_PTS 1000
#define FIREFLOWER_PTS 1000
#define STAR_PTS 1000
#define ONEUP_PTS 0
#define COIN_ANIM_MAX 20

// coin or powerup
typedef enum {ITEM_MUSHROOM, ITEM_FIREFLOWER, ITEM_STAR, ITEM_ONEUP, ITEM_COIN, ITEM_NONE} ItemType;

typedef struct Item Item;
const static struct Item {
  ItemType type;
  SDL_Texture *texture;
  int duration; // powerup countdown time
  int countdown;
  int points;
  SDL_Rect hitbox; // detect if Mario collides with this item

  // for coin only
  int animCountdown;

} item_none = {ITEM_NONE, NULL, 0, 0, 0, {0, 0, TILE_SIZE, TILE_SIZE}},
  item_coin = {ITEM_COIN, NULL, TIME_INF, TIME_INF, COIN_PTS, {0, 0, TILE_SIZE, TILE_SIZE}, COIN_ANIM_MAX},
  item_mushroom = {ITEM_MUSHROOM, NULL, TIME_INF, TIME_INF, MUSHROOM_PTS, {0, 0, TILE_SIZE, TILE_SIZE}},
  item_fireflower = {ITEM_FIREFLOWER, NULL, TIME_INF, TIME_INF, FIREFLOWER_PTS, {0, 0, TILE_SIZE, TILE_SIZE}},
  item_star = {ITEM_STAR, NULL, STAR_DURATION * FRAME_RATE, STAR_DURATION * FRAME_RATE, STAR_PTS, {0, 0, TILE_SIZE, TILE_SIZE}},
  item_oneUp = {ITEM_ONEUP, NULL, TIME_INF, TIME_INF, ONEUP_PTS, {0, 0, TILE_SIZE, TILE_SIZE}};

bool Item_isNone(Item *item);

#endif //DOC_FIGHTER_ITEM_H
