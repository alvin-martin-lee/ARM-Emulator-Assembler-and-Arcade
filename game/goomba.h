//
// Created by Christopher Huang on 6/18/2018.
//

#ifndef DOC_FIGHTER_GOOMBA_H
#define DOC_FIGHTER_GOOMBA_H

#include <stdint.h>
#include "game.h"
#include "mario.h"

typedef struct {
  CoreObject *core;
  SDL_Texture *texture;
} Goomba;

Goomba *Goomba_create(int x, int y);
void Goomba_destroy(Goomba *goomba);

void Goomba_update(Goomba *goomba, int level[LEVEL_H][LEVEL_W]);

void Goomba_draw(Goomba *goomba, SDL_Renderer *renderer, SDL_Texture *texture);

bool Goomba_stepped_on(Mario *mario, Goomba *goomba);


#endif
