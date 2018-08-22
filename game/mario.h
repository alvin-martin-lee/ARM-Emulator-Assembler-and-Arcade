//
// Created by aaron on 6/16/18.
//

#ifndef DOC_FIGHTER_MARIO_H
#define DOC_FIGHTER_MARIO_H

#include <stdint.h>
#include "item.h"
#include "game.h"
#include "block.h"
#include "linkedlist.h"

#define MAX_LIVES 2

typedef struct {
  bool dead;
  bool small;
  bool big;
  bool fire;
  bool invincible;
} MarioState;

typedef struct {
  int points;
  int lives;
  int coins;
  Item powerups[POWERUP_COUNT];
  CoreObject *core;
  List projectiles;
} Mario;

Mario *Mario_create();
void Mario_destroy(Mario *mario);

bool Mario_hasItem(Mario *mario, ItemType itemType);
bool Mario_hasNoItems(Mario *mario);
void Mario_onCollectItem(Mario *mario, Item item);
void Mario_updatePowerups(Mario *mario);

MarioState Mario_getState(Mario *mario);
void Mario_takeDamage(Mario *mario);

void Mario_onCollideBlock(Mario *mario, int blockRow, int blockCol, int level[LEVEL_H][LEVEL_W], Block ***blocks, List *itemList);

void Mario_onCollideMob(Mario *mario, SDL_Rect mob, DirectionH dir, int level[LEVEL_H][LEVEL_W]);

void Mario_draw(Mario *mario, SDL_Renderer *renderer, SDL_Texture *textures);

void Mario_shootFireball(Mario *mario, DirectionH dir);

#endif //DOC_FIGHTER_MARIO_H
