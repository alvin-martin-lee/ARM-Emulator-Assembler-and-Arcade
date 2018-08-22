//
// Created by Christopher Huang on 6/18/2018.
//

#include "game.h"
#include "goomba.h"
#include "collision.h"
#include "mario.h"

Goomba *Goomba_create(int x, int y) {
  Goomba *goomba = malloc(sizeof(Goomba));
  SDL_Rect size = {(x * TILE_SIZE) - GOOMBA_PADDING_W, (y * TILE_SIZE) - GOOMBA_PADDING_H, GOOMBA_GFX_W * SCREEN_SCALE, GOOMBA_GFX_H * SCREEN_SCALE};
  SDL_Rect hitbox = {x * TILE_SIZE, y * TILE_SIZE, GOOMBA_W, GOOMBA_H};
  SDL_Rect padding = {0, 0, GOOMBA_PADDING_W, GOOMBA_PADDING_H};
  goomba->core = CoreObject_create(RIGHT, size, hitbox, padding);
  goomba->texture = NULL;
  return goomba;
}

void Goomba_destroy(Goomba *goomba) {
  CoreObject_destroy(goomba->core);
  free(goomba);
}

void Goomba_update(Goomba *goomba, int level[LEVEL_H][LEVEL_W]) {
  if (goomba->core->current_direction == LEFT) {
    goomba->core->hitbox.x -= GOOMBA_MOVE_SPEED;
    if (!isOnGround(&goomba->core->hitbox, level)) {
      goomba->core->hitbox.x += GOOMBA_MOVE_SPEED;
      goomba->core->current_direction = RIGHT;
    }
    if (collideHorizontal(&goomba->core->hitbox, LEFT, level)) {
      goomba->core->current_direction = RIGHT;
    }
  } else {
    goomba->core->hitbox.x += GOOMBA_MOVE_SPEED;
    if (!isOnGround(&goomba->core->hitbox, level)) {
      goomba->core->hitbox.x -= GOOMBA_MOVE_SPEED;
      goomba->core->current_direction = LEFT;
    }
    if (collideHorizontal(&goomba->core->hitbox, RIGHT, level)) {
      goomba->core->current_direction = LEFT;
    }
  }
}

void Goomba_draw(Goomba *goomba, SDL_Renderer *renderer, SDL_Texture *texture) {
  SDL_RenderCopy(renderer, texture, NULL, &goomba->core->size);

  // Draw debug rectangles
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
  SDL_RenderDrawRect(renderer, &goomba->core->size);

  SDL_Rect debugHitBoxRect = goomba->core->hitbox;
  debugHitBoxRect.x *= SCREEN_SCALE;
  debugHitBoxRect.y *= SCREEN_SCALE;
  debugHitBoxRect.w *= SCREEN_SCALE;
  debugHitBoxRect.h *= SCREEN_SCALE;

  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);
  SDL_RenderDrawRect(renderer, &debugHitBoxRect);
}

bool Goomba_stepped_on(Mario *mario, Goomba *goomba) {
  bool is_stepped_on = false;
  int goomba_top = goomba->core->hitbox.y / TILE_SIZE;
  int goomba_left = goomba->core->hitbox.x / TILE_SIZE;
  int goomba_right = (goomba->core->hitbox.x + goomba->core->hitbox.w) / TILE_SIZE;

  int mario_bot = (mario->core->hitbox.y + mario->core->hitbox.h) / TILE_SIZE;
  int mario_left = mario->core->hitbox.x / TILE_SIZE;
  int mario_right = (mario->core->hitbox.x + mario->core->hitbox.w) / TILE_SIZE;
  for (int goomba_col = goomba_left; goomba_col <= goomba_right; goomba_col++) {
    for (int mario_col = mario_left; mario_col <= mario_right; mario_col++) {
      if (goomba_col == mario_col && goomba_top == mario_bot) {
        goomba->core->alive = false;
        is_stepped_on = true;
      }
    }
  }
  return is_stepped_on;
}

