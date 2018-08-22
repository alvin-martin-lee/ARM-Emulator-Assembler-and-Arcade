//
// Created by aaron on 6/5/18.
//

#include "game.h"
#include "collision.h"
#include <stdbool.h>

/**
 * Check if two rectangles overlap.
 * also indicates what sides of A are involved in the collision. */
int collideRects(SDL_Rect *a, SDL_Rect *b) {

  int al = a->x, ar = a->x + a->w - 1;
  int at = a->y, ab = a->y + a->h - 1;
  int bl = b->x, br = b->x + b->w - 1;
  int bt = b->y, bb = b->y + b->h - 1;

  // a next to or above b (or vice versa)
  if (ar < bl || br < al || ab < bt || bb < at)
    return COLL_NONE;

  int flags = 0;
  if (ar >= bl && ar <= br) {
    flags |= COLL_RIGHT;
  }
  if (al <= br && al >= bl) {
    flags |= COLL_LEFT;
  }
  if (ab >= bt && ab <= bb) {
    flags |= COLL_BOTTOM;
  }
  if (at <= bb && at >= bt) {
    flags |= COLL_TOP;
  }
  return flags;
}

/** Check if rectangle collides with a tile in the  level and specify from what direction */
//SDL_Rect instead of Rectangle for testing, h and w instead of height and width respectively
int collideTile(SDL_Rect *rect, int tileRowIdx, int tileColIdx) {
  int flags = 0;
  int top = rect->y / TILE_SIZE;
  int bot = (rect->y + rect->h - 1) / TILE_SIZE;
  int lft = rect->x / TILE_SIZE;
  int rgt = (rect->x + rect->w - 1) / TILE_SIZE;

  if (tileColIdx == top && lft <= tileRowIdx && tileRowIdx <= rgt) {
    flags |= COLL_TOP;
  }
  if (tileColIdx == bot && lft <= tileRowIdx && tileRowIdx <= rgt) {
    flags |= COLL_BOTTOM;
  }
  if (tileRowIdx == lft && top <= tileColIdx && tileColIdx <= bot) {
    flags |= COLL_LEFT;
  }
  if (tileRowIdx == rgt && top <= tileColIdx && tileColIdx <= bot) {
    flags |= COLL_RIGHT;
  }

  return flags;
}

/** Destroy each brick tile above mario if they collide */
void marioCollideBrick(SDL_Rect *rect, int **level) {
  if (!isWithinLevel(rect)) {
    return;
  }
  int top = rect->y / TILE_SIZE;
  int lft = rect->x / TILE_SIZE;
  int rgt = (rect->x + rect->w - 1) / TILE_SIZE;

  for (int col = lft; col <= rgt; col++) {
    if (collideTile(rect, col, top) & COLL_TOP) {
      level[top][col] = EMPTY_TILE;
      // play animation of brick being destroyed?
    }
  }
}

bool isOnGround(SDL_Rect *rect, int level[LEVEL_H][LEVEL_W]) {
  if (!isWithinLevel(rect)) {
    return false;
  }

  int bot = (rect->y + rect->h) / TILE_SIZE;
  int lft = rect->x / TILE_SIZE;
  int rgt = (rect->x + rect->w - 1) / TILE_SIZE;

  bool res = false;
  for (int col = lft; col <= rgt; col++) {
    if (bot < LEVEL_H && 0 <= col && col < LEVEL_W && level[bot][col] != EMPTY_TILE) {
      res = true;
    }
  }
  return res;
}

bool collideTop(SDL_Rect *rect, int level[LEVEL_H][LEVEL_W]) {
  // allow mario to jump above screen
  if (rect->y < 0) {
    return false;
  }

  int top = rect->y / TILE_SIZE;
  int lft = rect->x / TILE_SIZE;
  int rgt = (rect->x + rect->w - 1) / TILE_SIZE;

  bool res = false;
  for (int col = lft; col <= rgt; col++) {
    if (top < LEVEL_H && 0 <= col && col < LEVEL_W && level[top][col] != EMPTY_TILE) {
      res = true;
    }
  }
  return res;
}


bool collideHorizontal(SDL_Rect *rect, DirectionH dir, int level[LEVEL_H][LEVEL_W]) {
  if (rect->x <= 0 || rect->x >= LEVEL_W * TILE_SIZE)
    return true;
  int top = rect->y / TILE_SIZE;
  int bot = (rect->y + rect->h - 1) / TILE_SIZE;
  int col = dir == LEFT ? rect->x / TILE_SIZE : (rect->x + rect->w - 1) / TILE_SIZE;
  for (int row = top; row <= bot; row++) {
    if (level[row][col] != EMPTY_TILE) {
      return true;
    }
  }
  return false;
}

bool isWithinLevel(SDL_Rect *rect) {
  int top = rect->y / TILE_SIZE;
  int bot = (rect->y + rect->h - 1) / TILE_SIZE;
  int lft = rect->x / TILE_SIZE;
  int rgt = (rect->x + rect->w - 1) / TILE_SIZE;
  return (top >= 0 && bot < LEVEL_H && lft >= 0) || rgt < LEVEL_W;
}
